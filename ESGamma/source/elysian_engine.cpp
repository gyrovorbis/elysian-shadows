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



static void renderCube(float x, float y, float z,
					   float sx, float sy, float sz,
					   float rotz) {
	static const float verts[17*4] = {
		-1,  1,  1, 1.0f,
		 1,  1,  1, 1.0f,
		-1, -1,  1, 1.0f,
		 1, -1,  1, 1.0f,
		-1, -1, -1, 1.0f,
		1, -1, -1, 1.0f,
		-1,  1, -1, 1.0f,
		 1,  1, -1, 1.0f,
		 -1,  1,  1, 1.0f,
		 1,  1,  1, 1.0f,
		 1, -1,  1, 1.0f,
		 1,  1, -1, 1.0f,
		 1, -1, -1, 1.0f,
		-1, -1, -1, 1.0f,
		-1,  1, -1, 1.0f,
		-1, -1,  1, 1.0f,
		-1,  1,  1, 1.0f
	};

	GYVector4 modelViewVertices[17];

	gyMatMultVec((GYVector4*)verts, modelViewVertices, 17);


	gyMatPush();
	gyMatTranslate(x, y, z);
	gyMatScale(sx, sy, sz);
	gyMatRotateZ(rotz);
	gyVidTriangleStrip(modelViewVertices, 0, 17);

	//glDrawArrays(GL_TRIANGLES, 0, 12*3);

	gyMatPop();
}

extern "C" void glalGetLightViewMatrix(GYMatrix4 *mat, const GYLightSource*const src);
extern "C" void glalGetLightProjectionMatrix(GYMatrix4 *mat, const GYLightSource*const src);
//extern "C" GYMatrix4 projectionMatrix;

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

			gyVidOrthoProjMatrix(projectionMatrix, pleft, pright, pbottom, ptop, pnear, pfar);
			gyVidProjectionSetTransform(projectionMatrix);
			_cameraSystem->PushTransform();

			if(lightSrc) {
				glalGetLightViewMatrix(&lightViewMatrix, lightSrc);
				glalGetLightProjectionMatrix(&projectionMatrix, lightSrc);

				gyVidProjectionSetTransform(projectionMatrix);
				gyVidCameraSetTransform(lightViewMatrix);

			}

			//Begin rendering things affected by camera
			_lightSystem->render();
			while(gyVidBeginNextShadowPass()) {


			/*	for(unsigned i = 10; i < 11; ++i) {
				gyVidTexBind(NULL);
					gyMatPush();
					gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
					gyMatTranslate(400+i*10.0f, 400+i*10.0f, i*10.0f);
					gyMatScale(i*10.0f, i*10.0f, i*10.0f);
					gyVidRect(0);
					gyMatPop();
				}*/
/*
				renderCube(0.0f, 0.0f, 0.0f,
						   100.0f, 100.0f, 100.0f,
						   0.0f);*/

				renderCube(350.0f, 50.0f, 10.0f,
						   25.0f, 25.0f, 20.0f,
						   0.0f);

				renderCube(400.0f, 300.0f, 10.0f,
						   25.0f, 25.0f, 20.0f,
						   2.14f);

				renderCube(200.0f, 400.0f, 30.0f,
						   50.0f, 50.0f, 60.0f,
						   0.0f);

				renderCube(400.0f, 400.0f, 100.0f,
						   25.0f, 25.0f, 50.0f,
						   0.0f);

				renderCube(350.0f+600.0f, 150.0f+600.0f, 10.0f,
						   25.0f, 25.0f, 20.0f,
						   0.0f);

				renderCube(400.0f+600.0f, 300.0f+600.0f, 10.0f,
						   25.0f, 25.0f, 20.0f,
						   2.14f);

				renderCube(150.0f+600.0f, 750.0f, 60.0f,
						   25.0f, 25.0f, 50.0f,
						   0.0f);

				renderCube(400.0f+600.0f, 400.0f+600.0f, 100.0f,
						   25.0f, 25.0f, 50.0f,
						   0.0f);
			//_mapSystem->render();
			//_collisionSystem->render();
			}
/*
			for(unsigned i = 10; i < 11; ++i) {
			gyVidTexBind(NULL);
				gyMatPush();
				gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
				gyMatTranslate(400+i*10.0f, 400+i*10.0f, i*10.0f);
				gyMatScale(i*10.0f, i*10.0f, i*10.0f);
				gyVidRect(0);
				gyMatPop();
			}*/

		/*	renderCube(0.0f, 0.0f, 0.0f,
					   100.0f, 100.0f, 100.0f,
					   0.0f);*/

			renderCube(350.0f, 50.0f, 10.0f,
					   25.0f, 25.0f, 20.0f,
					   0.0f);

			renderCube(400.0f, 300.0f, 10.0f,
					   25.0f, 25.0f, 20.0f,
					   2.14f);

			renderCube(200.0f, 400.0f, 30.0f,
					   50.0f, 50.0f, 60.0f,
					   0.0f);

			renderCube(400.0f, 400.0f, 100.0f,
					   25.0f, 25.0f, 50.0f,
					   0.0f);

			renderCube(350.0f+600.0f, 150.0f+600.0f, 10.0f,
					   25.0f, 25.0f, 20.0f,
					   0.0f);

			renderCube(400.0f+600.0f, 300.0f+600.0f, 10.0f,
					   25.0f, 25.0f, 20.0f,
					   2.14f);

			renderCube(150.0f+600.0f, 750.0f, 60.0f,
					   25.0f, 25.0f, 50.0f,
					   0.0f);

			renderCube(400.0f+600.0f, 400.0f+600.0f, 100.0f,
					   25.0f, 25.0f, 50.0f,
					   0.0f);
			GYColor4 color = { 255, 200, 255, 100};
/*			gyVidQuadListBegin(&color);
			gyMatPush();
			gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
			gyMatTranslate(500.0f, 500.0f, 5.0f);
			gyMatScale(1000.0f, 1000.0f, 2.0f);
			gyVidRect(0);
			gyMatPop();
			gyVidQuadListEnd();
*/
			_mapSystem->render();

			 //_collisionSystem->render();

			_cameraSystem->PopTransform();

			//Begin rendering overlays
			//_warpSystem->Render();
			//_guiSystem->renderOverlay();

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
