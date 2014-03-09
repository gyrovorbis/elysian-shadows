#include <stdint.h>

#include <gyro.h>
#include <gyro_kinetic.h>

#include "elysian_animation.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_party_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_renderable.hpp"
#include "elysian_sprite_attrib.hpp"
#include "elysian_debug.hpp"
#include "elysian_level_system.hpp"
#include "elysian_engine.hpp"
#include "elysian_game_input.hpp"
#include "elysian_map_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_character.hpp"
#include "elysian_party_system.hpp"
#include "elysian_area.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_animation.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_level.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_audio_source.hpp"
#include "elysian_light_source.hpp"
#include "elysian_light_system.hpp"
#include "elysian_particle_system.hpp"
#include "elysian_day_system.hpp"

namespace elysian {

//======STATIC MEMBERS=====

ofstream		Debug::_dbgFile;
ofstream		Debug::_luaDbgFile;
Engine*			Debug::_engine = NULL;
bool			Debug::_advLogWrap = false;
unsigned int	Debug::_stackDepth = 0;
Debug::LEVEL	Debug::_level = ELYSIAN_DEBUG_LOG_LEVEL_DEFAULT;

// Central endpoint for all debug log calls
void Debug::writeLog(const Debug::SOURCE source, const Debug::LEVEL level, const char *const str) {
	static char buffer[ELYSIAN_DEBUG_BUFFER_SIZE];
	//only log events that are as important or more important than the current level
	if(level < _level && source != LUA) return;

	buffer[0] = '\0';

	//Add tabs for each stack layer
	if(source != LUA) for(unsigned int i = 0; i < _stackDepth; ++i) strcat(buffer, ELYSIAN_DEBUG_PREFIX_STACK);

	//Add prefixes for error types
	switch(level) {
	case FATAL:
		strcat(buffer, ELYSIAN_DEBUG_PREFIX_FATAL);
		break;
	case CRITICAL:
		strcat(buffer, ELYSIAN_DEBUG_PREFIX_CRITICAL);
		break;
	case VERBOSE: //no prefix
	default:
		break;
	}

	//Append string
	strcat(buffer, str);

	//Append newline character to the end
//	size_t length = strlen(buffer);
	strcat(buffer, "\n");

	_dbgFile << buffer;
	_dbgFile.flush(); //write to log file
	if(source == LUA) {
		_luaDbgFile << str << std::endl;
		_luaDbgFile.flush(); //write to lua's log file
	}
	gyPrintf(buffer); //use the "preferred" IO of each platform

	//Wrap to AdventureLog
	if(_advLogWrap) {
		AdventureLog::TEXT_TYPE advType;

		switch(source) {
		case LUA:
			advType = AdventureLog::TEXT_LUA_DEBUG;
			break;
		default:
			advType = AdventureLog::TEXT_NORMAL;
		}
		if(level == CRITICAL || level == FATAL) advType = AdventureLog::TEXT_ERROR;

		//characters that the adventure log cannot display--shouldn't it handle this? Not debug? No shit.
		for(unsigned int i = 0; i < strlen(buffer); ++i) if(buffer[i] == '\n' || buffer[i] == '\t') buffer[i] = ' ';
		if(_engine) getGuiSys()->adventureLog.addLine(advType, buffer);
	}
}

// Invoked for non AssetIO Engine logging
void Debug::printf(const Debug::LEVEL level, const char *const string, ...) {
	char buffer[ELYSIAN_DEBUG_BUFFER_SIZE];
	va_list argptr;
	//early-out
	if(level < _level) return;

	va_start(argptr,string);
	vsprintf(buffer, string, argptr);
	va_end(argptr);
	writeLog(ENGINE, level, buffer);
}

//=====NONSTATIC MEMBERS=====

Debug::Debug(void):
	_nextFrame(true)
{
	//Register debug callback with LibGyro
	gySysDebugEnable(ELYSIAN_DEBUG_LIBGYRO_LEVEL_DEFAULT, &libGyroPrintf);
	//Register pointer to asset debug interface with AssetIO
	Asset::setDebugStack(&_assetDebug);
	_dbgFile.open(ELYSIAN_DEBUG_FILE);
	_luaDbgFile.open(ELYSIAN_LUA_DEBUG_FILE);
	printf(VERBOSE, "-----Debug Initialized-----");
}

Debug::~Debug(void) {
	_luaDbgFile.close();
	_dbgFile.close();
}

LightSource* lightSrc;
#if 1
GYVector3 dirVec = { 1.0f, 0.0f, 0.0f };
#else
GYVector3 dirVec = { 0.0f, 0.0f, -1.0f };

#endif

bool Debug::sandboxInit(void) {
	//Fuck the current loading paradigm. Initialization happens here for now.
	printf(VERBOSE, "Debug::sandboxInit()");
	push();

	_advLogWrap = true;
	LuaManager::runScriptSandbox(ELYSIAN_GLOBAL_INIT_LUA_PATH);
	Entity* leader = getPartySys()->getPartyLeader();
	if(leader) {
		leader->transform.setPos(_cmdLineArgs.x, _cmdLineArgs.y);
		leader->transform._size.z = 64.0f;
	}

	//LightSource* lightSrc = leader->addLightSource();

	pop();
	return true;
}


void Debug::renderOverlay(void) {
	//if(terrainSysDebug) terrainSystem->RenderOverlays();
	//if(cameraSysDebug) cameraSystem->RenderOverlays();
	//BLAH BLAH ETC
}

void Debug::render(void) {

}

void Debug::update(void) {

}

static LightSource* src;

void Debug::processInput(const GameInput& input) {

	static float theta = 0.0f;
	static unsigned size = 3;
	static unsigned intensity = 200;

	//Camera zoom/rotate
	if(input.getDown(GameInput::BUT_Q)){
		_engine->_cameraSystem->rotate(0.2f);
	}
	if(input.getDown(GameInput::BUT_A)){
		_engine->_cameraSystem->rotate(-0.2f);
	}
	if(input.getDown(GameInput::BUT_W)){
		_engine->_cameraSystem->scale(0.9f*0.9f);
	}
	if(input.getDown(GameInput::BUT_S)){
		_engine->_cameraSystem->scale(1.1f*1.1f);
	}
	if(input.getDown(GameInput::BUT_E)) { //Reset Transform
		_engine->_cameraSystem->setScale(1.0f);
		_engine->_cameraSystem->setOrientZ(0.0f);
	}

	//Rendering layers
	/*if(input.getDown(GameInput::BUT_0)) {
		if(_engine->_guiSystem->adventureLog.isEnabled()) {
			_engine->_guiSystem->adventureLog.disable();
			printf(VERBOSE, "Disable GUI Layer");
		} else {
			_engine->_guiSystem->adventureLog.enable();
			printf(VERBOSE, "Enable GUI Layer");
		}
	}*/
	if(input.getTapped(GameInput::BUT_1)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(0);
		printf(VERBOSE, "Render Layer 1 - %s", (render)? "enabled" : "disabled");
	}
	if(input.getTapped(GameInput::BUT_2)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(1);
		printf(VERBOSE, "Render Layer 2 - %s", (render)? "enabled" : "disabled");
	}
	if(input.getTapped(GameInput::BUT_3)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(2);
		printf(VERBOSE, "Render Layer 3 - %s", (render)? "enabled" : "disabled");
	}
	if(input.getTapped(GameInput::BUT_4)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(3);
		printf(VERBOSE, "Render Layer 4 - %s", (render)? "enabled" : "disabled");
	}

		GYVector4 tempVec;
	if(input.getTapped(GameInput::BUT_0)) {

		//GYWav* wav = gyAudWavCreate("phone`.wav");
		//getAudioSys()->playSfxChannel(0, *wav);
		//getAudioSys()->setSfxChannelLooping(0, true);

		CollisionSystem::RENDER_LAYER layer = _engine->_collisionSystem->cycleRenderLayer();
		switch(layer) {
		case CollisionSystem::RENDER_DISABLED:
			printf(VERBOSE, "Render Collision Layer - Disabled");
			break;
		case CollisionSystem::RENDER_LAYER_1:
			printf(VERBOSE, "Render Collision Layer - 1");
			break;
		case CollisionSystem::RENDER_LAYER_2:
			printf(VERBOSE, "Render Collision Layer - 2");
			break;
		case CollisionSystem::RENDER_LAYER_3:
			printf(VERBOSE, "Render Collision Layer - 3");
			break;
		case CollisionSystem::RENDER_COMBINED:
			printf(VERBOSE, "Render Collision Layer - Combined");
			break;
		default: break;
		}/*
		dirVec.x -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
*/
	}

	//if(!input.getDown(GameInput::BUT_LCTRL)) return;
	//EVERYTHING BEYOND THIS POINT IS IN FALCO'S DEBUG KEYSPACE!!!!

	if(input.getDown(GameInput::BUT_5)) {
		++intensity;
		dirVec.z += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_6)) {
		if(intensity != 0) --intensity;
		dirVec.z -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_7)) {
		++size;
		dirVec.y += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_8)) {
		if(size != 0) --size;
		dirVec.y -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_9)) {
		dirVec.x += 0.1f;
		theta += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_O)) {
		//dirVec.x -= 0.1f;
		theta -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}

	if(input.getTapped(GameInput::BUT_R)) {

		static bool renderDebug = false;
		renderDebug = !renderDebug;
		getLightSys()->setRenderDebugEnabled(renderDebug);
		printf(VERBOSE, "LightSystem::renderShadowVolumes = %d", renderDebug);
	//	getPartSys()->getAttrib(0)->size = size;
		//getPartSys()->emitFrom(0, GYVector3 { entity->transform._pos.x+5, entity->transform._pos.y, entity->transform._pos.z}, theta, intensity);

	}

	static float timeSpeed = 60.0f;

	if(input.getDown(GameInput::BUT_N)) {
		timeSpeed *= 1.1f;
		getDaySys()->setTimeDilation(timeSpeed);
	}
	if(input.getDown(GameInput::BUT_M)) {
		timeSpeed *= 0.9f;
		getDaySys()->setTimeDilation(timeSpeed);
	}

	//VRAM and Audio Stats
	if(input.getTapped(GameInput::BUT_Z)) {
		//gyVidTexWalkthrough();
		//getAudioSys()->morphIntoBgmTrack(1, 5.0f);
		//getAudioSys()->fadeIntoBgmTrack(1, 5.0f);
		//getAudioSys()->morphIntoNewBgmTrack(1, 5.0f, "audio/music/CaveLoop2.ogg");
		//getAudioSys()->fadeIntoNewBgmTrack(1, 5.0f, "audio/music/CaveLoop2.ogg");
		//getAudioSys()->fadeIntoNewAuxTrack(0, 5.0f, "audio/music/RainThunder.ogg");
		//getAudioSys()->fadeInAuxTrack(0, 5.0f);
		//getAudioSys()->fadeIntoNewAuxTrack(0, 5.0f, "audio/music/RainThunder.ogg");
		//getAudioSys()->playBgmTrack();

		Entity* entity = new Entity();
		LightSource* lightSrc = entity->addLightSource();
		src = lightSrc;
		lightSrc->setLightType(GY_LIGHT_DIRECTIONAL);
		lightSrc->setDirection(GYVector3{0.0f, -1.0f, -1.0f});
		LightAttrib* attrib = new LightAttrib;
		//attrib->setAmbientColor(GYVector4{ 0.3f, 0.3f, 0.3f, 1.0f});
		attrib->setDiffuseColor(GYVector4{ 0.4f, 0.4f, 0.4f, 0.0f });
		//attrib->setSpecularColor(GYVector4{ 0.0f, 0.0f, 1.0f, 1.0f});
		//attrib->setAmbientColor(GYVector4{ 1.0f, 1.0f, 1.0f, 1.0f});
		lightSrc->setAttrib(attrib);
		lightSrc->setShadowCaster(true);
		_curArea->addEntity(entity);

		Debug::printf(VERBOSE, "Shader Uniform Changes Last Frame - %d", getLightSys()->getShaderUniformChangesLastFrame());
		Debug::printf(VERBOSE, "Lights Onscreen - %d", getLightSys()->getOnscreenLightCount());
	}
	if(input.getTapped(GameInput::BUT_FORWARDSLASH)) {
		LuaManager::dumpStats();
	}
/*	if(input.getTapped(GameInput::BUT_N)) {
		Debug::luaPrint("-----------------------------------------");
		Debug::luaPrint("One man's corruption is another man's salvation.");
		Debug::luaPrint("-----------------------------------------");
	}*/
	if(input.getTapped(GameInput::BUT_T)) {
		Entity *entity = new Entity;
		entity->setName("DICKMITE");
		entity->transform.setPos(1400.0f+80.0f+32.0f, 1060.0f);
		entity->transform.setScale(1.0f, 1.0f);
		entity->transform.setSize(128.0f, 128.0f);
		entity->setElev(0);
		entity->setDepthType(DEPTH_BUFFERED);
		Renderable* render = entity->addRenderable();
#if 1
		QuadSpriteAttrib* attrib = new QuadSpriteAttrib;
		attrib->setVertexColor(0, 255, 0, 0, 255);
		attrib->setVertexColor(1, 0, 255, 0, 100);
		attrib->setVertexColor(2, 0, 0, 255, 255);
		attrib->setVertexColor(3, 255, 255, 0, 255);
		attrib->loadTexture("item/legacyItems/jigglypuff1.PNG", 32, 32, 32, 32);
#else

		SpriteAttrib* attrib = new SpriteAttrib;
		attrib->setColor(100, 100, 100, 100);
		attrib->loadTexture("item/legacyItems/jigglypuff1.PNG", 32, 32, 32, 32);
#endif
		render->setSpriteAttrib(attrib);

		GYWav* wav = gyAudWavCreate("phone.wav");
		AudioSource* src = entity->addAudioSource();
		src->setLooping(true);
		src->playWav(wav);

		getCurArea()->addEntity(entity);

	}

	if(input.getTapped(GameInput::BUT_X)) {
		GYVector4 diffuseColors[4] = {
			{ 1.0f, 1.0f, 2.0f, 1.0f},
			{ 1.0f, 2.0f, 1.0f, 1.0f},
			{ 2.0f, 1.0f, 1.0f, 1.0f},
			{ 2.0f, 2.0f, 1.0f, 1.0f}
		};

		static int count = 0;
		char buff[100];

		if(count == 0) {

		Entity* julien = getCurArea()->findEntity("Julien");

	sprintf(buff, "LightSrc[%d]", count);

			Entity* entity = julien;
		//	entity->setName(buff);
			entity->setShadowVolume(false);
			//entity->transform.setPos(((count%8)*200)+100, 700+200*(count/8));
			//entity->transform.setPos(julien->transform._pos.x, julien->transform._pos.y);
			entity->setElev(0);
			//curArea->addEntity(entity);
			//entity->transform.setPos(600, 600);
			//entity->setDepthType(DEPTH_BUFFERED);
			src = lightSrc = entity->addLightSource();
			gyVec3Normalize(&dirVec);
			lightSrc->setDirection(dirVec);
			LightAttrib* attrib = new LightAttrib;
			//attrib->setAmbientColor(GYVector4{ 0.5f, 0.5f, 0.5f, 1.0f});
			attrib->setDiffuseColor(diffuseColors[count%4]);
			attrib->setSpecularColor(GYVector4{ 5.0f, 1.0f, 1.0f, 1.0f});
			lightSrc->setAttrib(attrib);
#if 0
			lightSrc->setLightType(LightSource::LIGHT_DIRECTIONAL);
#else
			lightSrc->setLightType(GY_LIGHT_SPOT);
			lightSrc->setSpotCutoffAngle(45.0f);
			lightSrc->setShadowCaster(true);
			attrib->setAttenuationFactors(GYVector3{ 0.0005f, 0.00005f, 0.00005f });

			Debug::printf(VERBOSE, "Adding light - %d", count);

			//getCameraSys()->setCustomCameraState(lightSrc->getCameraState());

			//if(count) {
				//getCurArea()->addEntity(entity);
			//} else {
				//lightSrc->setShadowCaster(true);
			//}

		}

		if(count%2 == 0) {
			getCameraSys()->pushCustomCameraState(src->getCameraState());

		} else {
			getCameraSys()->popState();

		}

			++count;

#endif


	}

	//Elevate player
	if(input.getTapped(GameInput::BUT_Y)) {
		getAudioSys()->fadeOutBgmTrack(5.0f);
		Entity* leader = _engine->_partySystem->getPartyLeader();
		if(leader) {
			unsigned elev = leader->getElev();
			elev = (elev < 15)? elev+1 : 0;
			leader->setElev(elev);
			printf(VERBOSE, "Player.elevation - %d", elev);
		}
	}
	//Display players' tile position
/*	if(input.getTapped(GameInput::BUT_M)) {
		Entity *player1 = _engine->_partySystem->getPartyLeader();
		if(player1) {
			printf(VERBOSE, "<%d, %d>\n",  ((int)player1->transform._pos.x+16)/32, ((int)player1->transform._pos.y+16)/32);
		}
	}*/
}

bool Debug::parseArguments(void) {
	int argc = 0;
	char argBuff[20][100];
	char *argv[20];

	//Default values for command-line args
	_cmdLineArgs.x = _cmdLineArgs.y = 32*4;
	strcpy(_cmdLineArgs.level, ELYSIAN_DEBUG_LEVEL_DEFAULT);
	strcpy(_cmdLineArgs.area, ELYSIAN_DEBUG_AREA_DEFAULT);

	FILE *file = fopen(gyFilePath(ELYSIAN_DEBUG_ARG_FILE), "r");
	if(file) { //read cmd-line arguments from file
		printf(VERBOSE, "Debug::parseArguments(void) - initializing from '%s'", ELYSIAN_DEBUG_ARG_FILE);
		push();
		while(fscanf(file, "%s", argBuff[argc]) != EOF) {
			argv[argc+1] = argBuff[argc]; //yes, I'm really doing this.
			printf(VERBOSE, "argv[%d] - %s", argc+1, argv[argc+1]);
			++argc;
		}
		fclose(file);
		pop();
		//Recursively reinvoke argument-based initialization
		return parseArguments(argc+1, argv);
	}
	return false;
}

bool Debug::parseArguments(int argc, char *argv[]) {
	if(argc <= 1) return parseArguments(); //default init

	printf(VERBOSE, "Debug::parseArguments(%d, char *argv[])", argc);
	push();
	for(unsigned int i = 1; i < (unsigned int)argc; ++i) {
		if(argv[i][0] != '-') {
			printf(CRITICAL, "! - Unrecognized command-line argument: %s", argv[i]);
			continue;
		}

		//Parse all command-line arguments and store them within internal structure
		switch(argv[i][1]) {
			case 'l':
				++i;
				strcpy(_cmdLineArgs.level, argv[i]);
				break;
			case 'a':
				++i;
				strcpy(_cmdLineArgs.area, argv[i]);
				break;
			case 'x':
				_cmdLineArgs.x = atoi(argv[++i]);
				break;
			case 'y':
				_cmdLineArgs.y = atoi(argv[++i]);
				break;
			case 'r':
				_cmdLineArgs.resolution = atoi(argv[++i]);
				break;
			default:
				printf(CRITICAL, "! - Unrecognized command-line argument: %s", argv[i]);
		}
	}
	pop();
	return true;
}


bool Debug::initializeEngine(void) {
	if(_cmdLineArgs.level) _engine->_levelSystem->switchLevel(_cmdLineArgs.level);
	if(_cmdLineArgs.area) _engine->_levelSystem->switchArea(_cmdLineArgs.area);
	return true;
}

}
