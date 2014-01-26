#include <gyro.h>

#include "elysian_engine.hpp"
#include "elysian_level_system.hpp"
#include "elysian_map_system.hpp"
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
#include "elysian_entity.hpp"
#include "elysian_light_source.hpp"
#include "glal/gyro_glal.h"

namespace elysian {

Engine::Engine(void) {
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
	_guiSystem			= new GuiSystem;
	_behaviorSystem		= new BehaviorSystem;
	_audioSystem		= new AudioSystem;
	_lightSystem		= new LightSystem;

	//register Lua dependencies
	Singletons::setGameInput(&_gameInput);
	Singletons::setDebug(&Debug::getInstance());

	memset(&_platInput, 0, sizeof(GYInput));
}

bool Engine::loadAssets(void) {
    bool success = true;
    
	Debug::printf(Debug::VERBOSE, "Engine::LoadAssets()");
	Debug::push();
	//Debug::Print(Debug::NORMAL, "PartySystem::PartySystem() - Loading Playable Characters.\n");
	//success &= partySystem->LoadPlayables();
		 //Debug::Print(Debug::ERROR_2, "\t! - Playable Character file could not be opened.\n");
	 //}
	success &= !_guiSystem->loadAssets();
	
    Debug::pop();
	return true;
}

void Engine::update(void) {
	switch(System::_engineState) {
		case System::ENGINE_NORMAL:
		case System::ENGINE_WARPING:
			_cameraSystem->update();
			_lightSystem->update();
			_audioSystem->update();
			//_collisionSystem->update();
			_timerSystem->update();
			_mapSystem->update();
			_partySystem->update();
			_characterSystem->Update();
			_animationSystem->Update();
			//warpSystem->Update();
			_guiSystem->update();
			_warpSystem->Update();

			//conversationSystem->Update();
			_behaviorSystem->callTriggers(Behavior::ON_UPDATE);
			//collisionSystem->DetectCollisions();
//
			break;
		case System::ENGINE_MODAL_CONVERSATION:
		case System::ENGINE_CONVERSATION:
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
	GameInput::Translate(_platInput, _gameInput);
	Debug::getInstance().processInput(_gameInput);

	if(!_processInput) return;

	switch(System::_engineState){
		case System::ENGINE_NORMAL:
		case System::ENGINE_CONVERSATION:
			_partySystem->processInput(_gameInput);
			//conversationSystem->ProcessInput(input);
			break;
		case System::ENGINE_MODAL_CONVERSATION:
			//conversationSystem->ProcessInput(input);
			break;
		case System::ENGINE_WARPING:
			break;
		default:
			break;
	}
}


extern "C" void glalGetLightViewMatrix(GYMatrix4 *mat, const GYLightSource*const src);
extern "C" void glalGetLightProjectionMatrix(GYMatrix4 *mat, const GYLightSource*const src);
extern "C" void glalGetPointLightProjectionMatrix(GYMatrix4 dest);
extern "C" void glalGetPointLightViewMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc, unsigned face);

void Engine::render(void) {
	GYMatrix4 lightViewMatrix;
	GYMatrix4 projectionMatrix;

	switch(System::_engineState) {
		default:
			gyVidSceneBegin();
            gyMatLoadIdentity();

			unsigned w, h;
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

			gyVidOrthoProjMatrix(projectionMatrix, pleft, pright, pbottom, ptop, pnear, pfar);
			glDepthRange(0, 1);
			gyVidProjectionSetTransform(projectionMatrix);
			_cameraSystem->PushTransform();

			_lightSystem->shadowRenderPasses();


			if(lightSrc) {

					glViewport(0, 0, 1024, 1024);
					//glDepthRange(1, 0);
					glalGetPointLightViewMatrix(lightViewMatrix, lightSrc, 4);
					glalGetPointLightProjectionMatrix(projectionMatrix);

					gyVidProjectionSetTransform(projectionMatrix);
					gyVidCameraSetTransform(lightViewMatrix);
					//glCullFace(GL_BACK);

			}

			gyVidTexBind(nullptr);

			_lightSystem->render();

			_mapSystem->render();

			 _collisionSystem->render();

			_cameraSystem->PopTransform();

			//Begin rendering overlays
			_warpSystem->Render();
			_guiSystem->renderOverlay();

			gyVidSceneEnd();
	}
}

Engine::~Engine(void) { 

	LuaManager::uninitialize();
	delete _levelSystem;
	delete _mapSystem;
	delete _cameraSystem;
	delete _animationSystem;
	delete _characterSystem;
	delete _partySystem;
	delete _timerSystem;
	delete _audioSystem;
	delete _lightSystem;
	delete _guiSystem;
	delete _behaviorSystem;
	

}

}
