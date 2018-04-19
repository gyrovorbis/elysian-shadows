#include <gyro.h>
#include <ctime>

#include "elysian_engine.hpp"
#include <crtp/elysian_level_system.hpp>
#include "elysian_debug.hpp"
#include "crtp/elysian_camera_system.hpp"
#include "elysian_character_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_timer_system_base.hpp"
#include "elysian_warp_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_gui_system_base.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_lua_singleton.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_light_system.hpp"
#include <crtp/elysian_entity.hpp>
#include "elysian_map_system.hpp"
#include <crtp/elysian_light_source.hpp>
#include "elysian_particle_system_base.hpp"
#include "elysian_day_system.hpp"
#include "elysian_dialog_system.hpp"
#include "elysian_examinable_system.hpp"
#include "elysian_rigid_body_system.hpp"
#include "elysian_combat_system.hpp"
#include "elysian_skeleton_system.hpp"
#include "elysian_pathfinding_system.hpp"
#include <crtp/elysian_input_system.hpp>
#include <system/elysian_event_system.hpp>
#include <utility/elysian_sort_sweep.hpp>

namespace elysian {

Engine::Engine(void):
    _updateMonitor("update"),
    _renderMonitor("render"),
    _inputMonitor("input")
{
    srand(time(nullptr));

    //gyDebugInit();
    //gySysInit();
  //  gyVidInit();
    //gyAudInit();
    System::setProcessInput();
    LuaManager::initialize();
    //initializeOpenCL();

    //allocate systems
    _eventSystem        = new EventSystem;
    _inputSystem        = new InputSystem;
    _levelSystem		= new LevelSystem;
    _mapSystem			= new MapSystem;
    _cameraSystem		= new CameraSystem;
    _characterSystem	= new CharacterSystem;
    _partySystem		= new PartySystem;
    _timerSystem		= new TimerSystem;
    _warpSystem			= new WarpSystem;
    _collisionSystem	= new CollisionSystem;
    _rigidBodySystem	= new RigidBodySystem;
    _guiSystem			= new GuiSystem;
    _behaviorSystem		= new BehaviorSystem;
    _audioSystem		= new AudioSystem;
    _lightSystem		= new LightSystem;
    _particleSystem		= new ParticleSystem;
    _daySystem			= new DaySystem;
    _examinableSystem	= new ExaminableSystem;
    _dialogSystem		= new DialogSystem;
    _combatSystem		= new CombatSystem;
    _skeletonSystem     = new SkeletonSystem;
    _pathfindingSystem  = new PathfindingSystem;

    InputSystem::initialize();

    _behaviorSystem->initialize();
    _warpSystem->initialize();
    _combatSystem->initialize();
    MapSystem::staticInitialize();

    //register Lua dependencies
    //Singletons::setGameInput(&_gameInput);
    Singletons::setDebug(&Debug::getInstance());

    renderMonitor.addChild(&shadowPassMonitor);
    renderMonitor.addChild(&lightPassMonitor);
    renderMonitor.addChild(&guiRenderMonitor);
    renderMonitor.addChild(&bloomMonitor);

    updateMonitor.addChild(&updateRigidBodyMonitor);
    updateMonitor.addChild(&updateCollisionMonitor);
    updateMonitor.addChild(&updatePartyMonitor);
    updateMonitor.addChild(&updateAudioMonitor);
    updateMonitor.addChild(&updateSkeletonMonitor);
    updateMonitor.addChild(&updateLightMonitor);
    updateMonitor.addChild(&updateGuiMonitor);
    updateMonitor.addChild(&updateMapMonitor);
    updateMonitor.addChild(&updateCharacterMonitor);
    updateMonitor.addChild(&updateTimerMonitor);
    updateMonitor.addChild(&updateWarpMonitor);
    updateMonitor.addChild(&updateParticleMonitor);
    updateMonitor.addChild(&updateDayMonitor);
    updateMonitor.addChild(&updateDialogMonitor);
    updateMonitor.addChild(&updateCombatMonitor);
    updateMonitor.addChild(&updateLuaMonitor);

   // memset(&_platInput, 0, sizeof(GYInput));
}

bool Engine::loadAssets(void) {
    bool success = true;

    _cameraSystem->initializeProjMatrix();

    Debug::printf(Debug::LOG_VERBOSE, "Engine::LoadAssets()");
    Debug::push();
    success &= !_guiSystem->loadAssets();
    success &= !_dialogSystem->loadAssets();
#ifdef ELYSIAN_COLLISION_SYSTEM_DEBUG_ENABLED
    success &= !_collisionSystem->loadDebugAssets();
#endif
    Debug::pop();
    return true;
}

void Engine::perfMonitorDump() {

}

void Engine::update(void) {

    //_updateMonitor.startNewFrame();
    updateMonitor.start();

    switch(System::_engineState) {
        case System::ENGINE_NORMAL:
        case System::ENGINE_WARPING:
        case System::ENGINE_MODAL_CONVERSATION:
        case System::ENGINE_CONVERSATION:
            //LuaManager::updateFrameStats();
            updatePartyMonitor.start();
            _partySystem->update();
            updatePartyMonitor.stop();

            updateLightMonitor.start();
            _lightSystem->update();
            updateLightMonitor.stop();

            updateAudioMonitor.start();
            _audioSystem->update();
            updateAudioMonitor.stop();

            updateRigidBodyMonitor.start();
            _rigidBodySystem->applyForces();
            _rigidBodySystem->integrate();
            //_mapSystem->update();
            updateRigidBodyMonitor.stop();

            updateCollisionMonitor.start();
            _collisionSystem->prepareGeometry();
            updateCollisionMonitor.stop();

            updateRigidBodyMonitor.resume();
            _rigidBodySystem->integrate3D();
            updateRigidBodyMonitor.stop();

            updateCollisionMonitor.resume();
            _collisionSystem->update();
            updateCollisionMonitor.stop();

            updateRigidBodyMonitor.resume();
            _rigidBodySystem->update();
            _rigidBodySystem->resolveContactVelocities();
            _rigidBodySystem->breakRigidBodies();
            updateRigidBodyMonitor.stop();

            updateTimerMonitor.start();
            _timerSystem->update();
            updateTimerMonitor.stop();

            updateMapMonitor.start();
            _mapSystem->update();
            updateMapMonitor.stop();

            updateCharacterMonitor.start();
            _characterSystem->update();
            updateCharacterMonitor.stop();

            updateSkeletonMonitor.start();
            _skeletonSystem->update();
            updateSkeletonMonitor.stop();

            updateGuiMonitor.start();
            _guiSystem->update();
            updateGuiMonitor.stop();

            updateWarpMonitor.start();
            _warpSystem->update();
            updateWarpMonitor.stop();

            updateParticleMonitor.start();
            _particleSystem->update();
            updateParticleMonitor.stop();

            updateDayMonitor.start();
            _daySystem->update();
            updateDayMonitor.stop();

            updateDialogMonitor.start();
            _dialogSystem->update();
            updateDialogMonitor.stop();

            updateCombatMonitor.start();
            _combatSystem->update();
            updateCombatMonitor.stop();

            updateLuaMonitor.start();

            _behaviorSystem->callTriggers(Behavior::ON_UPDATE);
            updateLuaMonitor.stop();
            break;

        case System::ENGINE_ESTK_STOP:
            _audioSystem->update();
             _collisionSystem->prepareGeometry();
             _collisionSystem->update();
            _lightSystem->update();
            _timerSystem->update();
            _mapSystem->update();
            _guiSystem->update();
        break;
        case System::ENGINE_MODAL_MENU:
            _guiSystem->update();
            break;
        default:
            break;
    }


 // _timerSystem->popTaskMonitor(&_updateMonitor);
    updateMonitor.stop();
}

void Engine::processInput(void) {

  _timerSystem->pushTaskMonitor(&_inputMonitor);
  processInputMonitor.start();

    _inputSystem->update();

    //gyInputPoll(&_platInput);
    //_mouseInput.poll();
    //GameInput::translate(*_platInput, _gameInput);


    //if(!_processInput) return;

    switch(System::_engineState){
        case System::ENGINE_NORMAL:
        case System::ENGINE_CONVERSATION:
            //if(_processInput) _partySystem->processInput();

            _cameraSystem->processInput(_inputSystem->getKeyboard());
            _guiSystem->processInput();
            _examinableSystem->processInput();
            _dialogSystem->processInput();

            Debug::getInstance().processInput(_inputSystem->getKeyboard());
            //conversationSystem->ProcessInput(input);
            break;
        case System::ENGINE_MODAL_CONVERSATION:
            _dialogSystem->processInput();
            break;
        case System::ENGINE_MODAL_MENU:
            _guiSystem->processInput();
            break;
        case System::ENGINE_WARPING:
            break;
        case System::ENGINE_CAMERA_DEBUG:
#if 0
            _cameraSystem->processInput(_gameInput);
#endif
        break;
        default:
            break;
    }
    processInputMonitor.stop();
  _timerSystem->popTaskMonitor(&_inputMonitor);
}

void Engine::render(void) {

  _timerSystem->pushTaskMonitor(&_renderMonitor);
    renderMonitor.start();
    for(unsigned i = 0; i < 100; ++i) {
    processInputMonitor.start();
    processInputMonitor.stop();
    processInputMonitor.resume();
    processInputMonitor.stop();
    }
    renderMonitor.stop();



    switch(System::_engineState) {
        default:
            _cameraSystem->update();
            gyVidSceneBegin();
            gyMatLoadIdentity();

        /*	unsigned w, h;
            gyVidResolution(&w, &h);

            const float pfar = 1000.0f;
            const float pnear = -999.0f;
            const float pleft = 0.0f;
            const float pright = (float)w;
            const float ptop = 0.0f;
            const float pbottom = (float)h;
            unsigned width, height;

            gyVidResolution(&width, &height);
            glViewport(0, 0, width, height);

            gyVidOrthoProjMat(projectionMatrix, pleft, pright, pbottom, ptop, pnear, pfar);
            //glDepthRange(0, 1);
            gyVidProjectionCustomMatrix(projectionMatrix);
            //gyVidProjectionDefaultMatrix(); */
            _guiSystem->renderBackground();
            _cameraSystem->pushViewProjMatrix();
            shadowPassMonitor.start();
            _lightSystem->shadowRenderPasses();
            shadowPassMonitor.stop();
            _lightSystem->addLightSourcesToScene();

            lightPassMonitor.start();
            _mapSystem->render();
            lightPassMonitor.stop();

            //_lightSystem->render();

             _collisionSystem->render();
             _particleSystem->render();
          //  func();

             _lightSystem->removeLightSourcesFromScene();

             guiRenderMonitor.start();
             _guiSystem->render();
             guiRenderMonitor.stop();

            _cameraSystem->popViewProjMatrix();

            bloomMonitor.start();
            _lightSystem->applyBloomEffect();
            bloomMonitor.stop();

            //Begin rendering overlays
          //  func();
            _dialogSystem->render();
            _warpSystem->render();
            _guiSystem->renderForeground();

            gyVidSceneEnd();
    }

   // renderMonitor.stop();
  _timerSystem->popTaskMonitor(&_renderMonitor);
}

Engine::~Engine(void) {
    //SHIT ALL NEEDS SOME WORK
    MapSystem::staticUninitialize();
    InputSystem::uninitialize();

    //gyInputUninit();
    //gyTimerUninit();

    delete _partySystem;
    delete _characterSystem;
    delete _guiSystem;
    LuaManager::uninitialize();

    delete _inputSystem;
    delete _combatSystem;
    delete _dialogSystem;
    delete _examinableSystem;
    delete _daySystem;
    delete _particleSystem;
    delete _levelSystem;
    delete _mapSystem;
    delete _cameraSystem;
    delete _timerSystem;
    delete _audioSystem;
    delete _lightSystem;
    delete _rigidBodySystem;
    delete _collisionSystem;
    delete _skeletonSystem;
    delete _behaviorSystem;
    delete _pathfindingSystem;
    delete _eventSystem;

}

}
