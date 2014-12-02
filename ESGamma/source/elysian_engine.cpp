#include <gyro.h>
#include <ctime>

#include "elysian_engine.hpp"
#include "elysian_level_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_game_input.hpp"
#include "elysian_animation_system.hpp"
#include "elysian_character_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_warp_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_lua_singleton.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_light_system.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")
#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tkMapSystem.h")
#include "elysian_light_source.hpp"
#include "elysian_particle_system.hpp"
#include "elysian_day_system.hpp"
#include "elysian_dialog_system.hpp"
#include "elysian_examinable_system.hpp"
#include "elysian_rigid_body_system.hpp"
#include "elysian_combat_system.hpp"
#include "glal/gyro_glal.h"

namespace elysian {

Engine::Engine(void) {
	srand(time(nullptr));

    //gyDebugInit();
    //gySysInit();
  //  gyVidInit();
    //gyAudInit();
    gyInputInit();
   // gyTimerInit();

    LuaManager::initialize();
		
    //allocate systems
    _levelSystem		= new LevelSystem;
    _mapSystem			= new MapSystem;
    _cameraSystem		= new CameraSystem;
    _animationSystem	= new AnimationSystem;
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

    _behaviorSystem->initialize();
    _warpSystem->initialize();
    _combatSystem->initialize();

	//register Lua dependencies
	Singletons::setGameInput(&_gameInput);
	Singletons::setDebug(&Debug::getInstance());

	memset(&_platInput, 0, sizeof(GYInput));
}

bool Engine::loadAssets(void) {
    bool success = true;

    _cameraSystem->initializeProjMatrix();
    
	Debug::printf(Debug::VERBOSE, "Engine::LoadAssets()");
	Debug::push();
	//Debug::Print(Debug::NORMAL, "PartySystem::PartySystem() - Loading Playable Characters.\n");
	//success &= partySystem->LoadPlayables();
		 //Debug::Print(Debug::ERROR_2, "\t! - Playable Character file could not be opened.\n");
	 //}
	success &= !_guiSystem->loadAssets();
	success &= !_dialogSystem->loadAssets();
	
    Debug::pop();
	return true;
}

void Engine::update(void) {

	switch(System::_engineState) {
		case System::ENGINE_NORMAL:
		case System::ENGINE_WARPING:
		case System::ENGINE_MODAL_CONVERSATION:
		case System::ENGINE_CONVERSATION:
			_partySystem->update();
			_cameraSystem->update();
			_lightSystem->update();
			_audioSystem->update();

            _rigidBodySystem->applyForces();
			_rigidBodySystem->integrate();
            _collisionSystem->prepareGeometry();
            _rigidBodySystem->integrate3D();
            _collisionSystem->update();
            _rigidBodySystem->update();
            _rigidBodySystem->resolveContactVelocities();
            _rigidBodySystem->breakRigidBodies();

			_timerSystem->update();
			_mapSystem->update();
			_characterSystem->update();
			_animationSystem->update();
			_guiSystem->update();
			_warpSystem->update();
			_particleSystem->update();
			_daySystem->update();
			_dialogSystem->update();
			_combatSystem->update();

			_behaviorSystem->callTriggers(Behavior::ON_UPDATE);
			break;


#if 0
			timerSystem->Update();
			terrainSystem->Update();
			characterSystem->Update();
			animationSystem->Update();
			partySystem->Update();
			guiSystem->Update();
			conversationSystem->Update();
			behaviorSystem->Update();
			collisionSystem->DetectCollisions();
//			behaviorSystem->Update();
#endif
			break;
		default:
			break;
	}

}

void Engine::processInput(void) {

    gyInputPoll(&_platInput);
	GameInput::translate(_platInput, _gameInput);
	Debug::getInstance().processInput(_gameInput);

	//if(!_processInput) return;

	switch(System::_engineState){
		case System::ENGINE_NORMAL:
		case System::ENGINE_CONVERSATION:
			if(_processInput) _partySystem->processInput(_gameInput);
			_cameraSystem->processInput(_gameInput);
			_examinableSystem->processInput(_gameInput);
			_dialogSystem->processInput(_gameInput);
			//Debug::getInstance().processInput(_gameInput);
			//conversationSystem->ProcessInput(input);
			break;
		case System::ENGINE_MODAL_CONVERSATION:
			_dialogSystem->processInput(_gameInput);
			break;
		case System::ENGINE_WARPING:
			break;
		case System::ENGINE_CAMERA_DEBUG:
			_cameraSystem->processInput(_gameInput);
		break;
		default:
			break;
	}
}

void Engine::render(void) {

	switch(System::_engineState) {
		default:
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
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
			_lightSystem->shadowRenderPasses();
#endif
          //  _lightSystem->addLightSourcesToScene();

			_mapSystem->render();

			_lightSystem->render();

             _collisionSystem->render();
             _particleSystem->render();


           //  _lightSystem->removeLightSourcesFromScene();

             _guiSystem->render();

			_cameraSystem->popViewProjMatrix();

			//Begin rendering overlays
            _warpSystem->render();
			_guiSystem->renderForeground();
            //_dialogSystem->render();

			gyVidSceneEnd();
	}
}

Engine::~Engine(void) { 

	LuaManager::uninitialize();
	delete _combatSystem;
	delete _dialogSystem;
	delete _examinableSystem;
	delete _daySystem;
	delete _particleSystem;
	delete _levelSystem;
	delete _mapSystem;
	delete _cameraSystem;
	delete _animationSystem;
	delete _characterSystem;
	delete _partySystem;
	delete _timerSystem;
	delete _audioSystem;
	delete _lightSystem;
	delete _rigidBodySystem;
	delete _collisionSystem;
	delete _guiSystem;
	delete _behaviorSystem;
	

}

}
