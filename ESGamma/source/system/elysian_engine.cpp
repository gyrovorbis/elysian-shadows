#include <ctime>

#include <libGyro/gyro.h>

#include <gl/gyro_radial_blur_gl.h>
#include <gl/gyro_chromatic_aberration_gl.h>

#include <AssetIO/crtp/elysian_entity.hpp>
#include <ESGamma/crtp/elysian_level_system.hpp>
#include <AssetIO/crtp/elysian_light_source.hpp>

#include <ESGamma/system/elysian_engine.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>
#include <ESGamma/system/elysian_character_system.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <ESGamma/system/elysian_timer_system_base.hpp>
#include <ESGamma/system/elysian_warp_system.hpp>
#include <ESGamma/system/elysian_collision_system.hpp>
#include <ESGamma/crtp/elysian_gui_system.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <ESGamma/lua/elysian_lua_singleton.hpp>
#include <ESGamma/crtp/elysian_lua_manager.hpp>
#include <ESGamma/system/elysian_behavior_system.hpp>
#include <ESGamma/system/elysian_light_system.hpp>=
#include <ESGamma/crtp/elysian_map_system.hpp>
#include <ESGamma/crtp/elysian_particle_system.hpp>
#include <ESGamma/system/elysian_day_system.hpp>
#include <ESGamma/system/elysian_dialog_system.hpp>
#include <ESGamma/system/elysian_examinable_system.hpp>
#include <ESGamma/system/elysian_rigid_body_system.hpp>
#include <ESGamma/system/elysian_combat_system.hpp>
#include <ESGamma/system/elysian_skeleton_system.hpp>
#include <ESGamma/system/elysian_pathfinding_system.hpp>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/system/elysian_event_system.hpp>
#include <ESGamma/utility/elysian_sort_sweep.hpp>
#include <ESGamma/system/elysian_compute_system.hpp>
#include <ESGamma/crtp/elysian_render_system.hpp>
#include <ESGamma/system/elysian_thread_system.hpp>

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

    //allocate systems
    _timerSystem		= new TimerSystem;
    _threadSystem       = new ThreadSystem;
    _eventSystem        = new EventSystem;
    _audioSystem		= new AudioSystem;
    _partySystem		= new PartySystem;
    _inputSystem        = new InputSystem;
    _levelSystem		= new LevelSystem;
    _mapSystem			= new MapSystem;
    _cameraSystem		= new CameraSystem;
    _characterSystem	= new CharacterSystem;
    _warpSystem			= new WarpSystem;
    _collisionSystem	= new CollisionSystem;
    _rigidBodySystem	= new RigidBodySystem;
    _guiSystem			= new GuiSystem;
    _behaviorSystem		= new BehaviorSystem;
    _lightSystem		= new LightSystem;
    _particleSystem		= new ParticleSystem;
    _daySystem			= new DaySystem;
    _examinableSystem	= new ExaminableSystem;
    _dialogSystem		= new DialogSystem;
    _combatSystem		= new CombatSystem;
    _skeletonSystem     = new SkeletonSystem;
    _pathfindingSystem  = new PathfindingSystem;
    _computeSystem      = new ComputeSystem;
    _renderSystem       = new RenderSystem;

    InputSystem::initialize();
    _threadSystem->initialize();
    _computeSystem->initialize();
    _renderSystem->initialize();
    _behaviorSystem->initialize();
    _warpSystem->initialize();
    _combatSystem->initialize();
    _examinableSystem->initialize();
    MapSystem::staticInitialize();

    //register Lua dependencies
    //Singletons::setGameInput(&_gameInput);
    Singletons::setDebug(&Debug::getInstance());

    renderMonitor.addChild(&shadowPassMonitor);
    renderMonitor.addChild(&lightPassMonitor);
    lightPassMonitor.addChild(&getRenderSystem()->renderParticlesMonitor);
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

    renderMonitor.addChild(&getRenderSystem()->updateHDRExposureMonitor);

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
        updateParticleMonitor.start();
        _particleSystem->update();
        updateParticleMonitor.stop();


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
        _renderSystem->populateParticleVbos();
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

            _cameraSystem->processInput(_inputSystem->getKeyboard(0));
            _guiSystem->processInput();
            _examinableSystem->processInput();
            _dialogSystem->processInput();
            _partySystem->processInput();

            Debug::getInstance().processInput(_inputSystem->getKeyboard(0));
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
  //_timerSystem->pushTaskMonitor(&_renderMonitor);
  renderMonitor.start();
    switch(System::_engineState) {
        default:
            _cameraSystem->update();
            gyVidSceneBegin();
            gyMatLoadIdentity();
            _renderSystem->updateHDRExposure();

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
            _lightSystem->addLightSourcesToScene();
            _lightSystem->shadowRenderPasses();
            shadowPassMonitor.stop();


            lightPassMonitor.start();
            _mapSystem->render();
            _renderSystem->renderParticles();
            lightPassMonitor.stop();

            //_lightSystem->render();

             _collisionSystem->render();
             _particleSystem->render();
             _renderSystem->createLuminanceHistogram();
          //  func();
           // _renderSystem->forwardPlusCullLights();
             _lightSystem->removeLightSourcesFromScene();
             //glalVidApplyRadialBlur();
            //glalVidApplyChromaticAberration();
             guiRenderMonitor.start();
             _guiSystem->render();
             guiRenderMonitor.stop();

            _cameraSystem->popViewProjMatrix();

            bloomMonitor.start();
            _lightSystem->applyBloomEffect();
            bloomMonitor.stop();

            //Begin rendering overlays
          //  func();
            guiRenderMonitor.resume();
            _dialogSystem->render();
            _warpSystem->render();
            _guiSystem->renderForeground();
            guiRenderMonitor.stop();

            gyVidSceneEnd();
    }

    renderMonitor.stop();
  //_timerSystem->popTaskMonitor(&_renderMonitor);
}

Engine::~Engine(void) {
    dbgprintf("Engine: Destroying Engine");
    dbgpush();

    //SHIT ALL NEEDS SOME WORK
    MapSystem::staticUninitialize();
    InputSystem::uninitialize();
    _examinableSystem->uninitialize();

    //gyInputUninit();
    //gyTimerUninit();

    delete _partySystem;
    delete _characterSystem;
    delete _guiSystem;
    delete _daySystem;
    LuaManager::uninitialize();
    _renderSystem->uninitialize();
    _computeSystem->uninitialize();


    delete _inputSystem;
    delete _combatSystem;
    delete _dialogSystem;
    delete _examinableSystem;
    delete _particleSystem;
    delete _levelSystem;
    delete _mapSystem;
    delete _cameraSystem;
    delete _timerSystem;
    delete _lightSystem;
    delete _rigidBodySystem;
    delete _collisionSystem;
    delete _skeletonSystem;
    delete _behaviorSystem;
    delete _pathfindingSystem;
    delete _eventSystem;
    delete _computeSystem;
    delete _audioSystem;
    _threadSystem->uninitialize();
    delete _threadSystem;

    dbgpop();

}

}
