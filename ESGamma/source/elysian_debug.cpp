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
	for(unsigned int i = 0; i < _stackDepth; ++i) strcat(buffer, ELYSIAN_DEBUG_PREFIX_STACK);

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
GYVector3 dirVec = { -1.0f, 0.0f, 0.0f };
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
	//Camera zoom/rotate
	if(input.getDown(GameInput::BUT_Q)){
		_engine->_cameraSystem->transform.rotate(0.1f);
	}
	if(input.getDown(GameInput::BUT_A)){
		_engine->_cameraSystem->transform.rotate(-0.1f);
	}
	if(input.getDown(GameInput::BUT_W)){
		_engine->_cameraSystem->transform.scale(0.9f, 0.9f);
	}
	if(input.getDown(GameInput::BUT_S)){
		_engine->_cameraSystem->transform.scale(1.1f, 1.1f);
	}
	if(input.getDown(GameInput::BUT_E)) { //Reset Transform
		_engine->_cameraSystem->transform.setScale(1.0f, 1.0f);
		_engine->_cameraSystem->transform.setOrientZ(0.0f);
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
	if(input.getDown(GameInput::BUT_0)) {

		GYWav* wav = gyAudWavCreate("phone`.wav");
		getAudioSys()->playSfxChannel(0, *wav);
		//getAudioSys()->setSfxChannelLooping(0, true);

		/*CollisionSystem::RENDER_LAYER layer = _engine->_collisionSystem->cycleRenderLayer();
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
		}*/
		dirVec.x -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);

	}

	if(input.getDown(GameInput::BUT_5)) {
		dirVec.z += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_6)) {
		dirVec.z -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_7)) {
		dirVec.y += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_8)) {
		dirVec.y -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getDown(GameInput::BUT_9)) {
		dirVec.x += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
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
		lightSrc->setLightType(LightSource::LIGHT_DIRECTIONAL);
		lightSrc->setDirection(GYVector3{0.5f, -1.0f, -0.5f});
		LightAttrib* attrib = new LightAttrib;
		//attrib->setAmbientColor(GYVector4{ 0.3f, 0.3f, 0.3f, 1.0f});
		attrib->setDiffuseColor(GYVector4{ 0.4f, 0.4f, 0.4f, 0.0f });
		//attrib->setSpecularColor(GYVector4{ 0.0f, 0.0f, 1.0f, 1.0f});
		//attrib->setAmbientColor(GYVector4{ 1.0f, 1.0f, 1.0f, 1.0f});
		lightSrc->setAttrib(attrib);
		_curArea->addEntity(entity);


		entity = new Entity();
		entity->transform.setPos(500.0f*2.0f, 500.0f*2.0f-100.0f);
		entity->transform.setPosZ(32.0f);
		lightSrc = entity->addLightSource();
		lightSrc->setDirection(GYVector3{1.0f, 0.0f, -1.0f});
		attrib = new LightAttrib;
		attrib->setDiffuseColor(GYVector4{ 0.0f, 2.0f, 0.5f, 1.0f });
		attrib->setSpecularColor(GYVector4{ 0.0f, 0.0f, 1.0f, 1.0f});
		lightSrc->setAttrib(attrib);
		//lightSrc->setSpotCutoffAngle(80.0f);
		//attrib->setAttenuationFactors(GYVector3{ 0.000005f, 0.000005f, 0.000005f });
		attrib->setSpotExponent(4.0f);
		_curArea->addEntity(entity);

		entity = new Entity();
		entity->transform.setPos(500.0f*2.0f-300.0f, 500.0f*2.0f-100.0f);
		entity->transform.setPosZ(10.0f);
		lightSrc = entity->addLightSource();
		attrib = new LightAttrib;
		attrib->setDiffuseColor(GYVector4{ 0.0f, 0.0f, 2.0f, 1.0f });
		attrib->setSpecularColor(GYVector4{ 0.0f, 0.0f, 1.0f, 1.0f});
		lightSrc->setDirection(GYVector3{0.0f, 0.0f, -1.0f});
		//lightSrc->setSpotCutoffAngle(80.0f);
		//attrib->setAttenuationFactors(GYVector3{ 0.000005f, 0.000005f, 0.000005f });
		attrib->setSpotExponent(4.0f);
		lightSrc->setAttrib(attrib);
		_curArea->addEntity(entity);


	}
	if(input.getTapped(GameInput::BUT_X)) {
		static int enabled = 0;
		static int once = 0;

		if(!enabled) {
			if(!once) {
				Entity* entity = _curArea->findEntity("Julien");
				//curArea->addEntity(entity);
				//entity->transform.setPos(600, 600);
				//entity->setDepthType(DEPTH_BUFFERED);
				src = lightSrc = entity->addLightSource();
				gyVec3Normalize(&dirVec);
				lightSrc->setDirection(dirVec);
				LightAttrib* attrib = new LightAttrib;
				//attrib->setAmbientColor(GYVector4{ 0.3f, 0.3f, 0.3f, 1.0f});
				attrib->setDiffuseColor(GYVector4{ 1.4f, 0.3f, 0.3f, 1.0f });
				attrib->setSpecularColor(GYVector4{ 0.0f, 0.0f, 1.0f, 1.0f});
				//attrib->setAmbientColor(GYVector4{ 1.0f, 1.0f, 1.0f, 1.0f});
				lightSrc->setAttrib(attrib);

#if 0
				lightSrc->setLightType(LightSource::LIGHT_DIRECTIONAL);
#else
				lightSrc->setSpotCutoffAngle(80.0f);
				attrib->setAttenuationFactors(GYVector3{ 0.000005f, 0.000005f, 0.000005f });
				attrib->setSpotExponent(4.0f);
#endif

				once = 1;
			} else {
				lightSrc = src;
			}
		} else {
			lightSrc = nullptr;
		}

		enabled = !enabled;

		//getAudioSys()->morphIntoBgmTrack(0, 5.0f);
		//getAudioSys()->fadeIntoBgmTrack(0, 5.0f);
		//getAudioSys()->stopBgmTrack();
		//getAudioSys()->morphIntoNewBgmTrack(1, 5.0f, "audio/music/CaveLoop1.ogg");
		//getAudioSys()->fadeIntoNewBgmTrack(1, 5.0f, "audio/music/CaveLoop1.ogg");
		//getAudioSys()->fadeOutAuxTrack(0, 5.0f);
		//gyAudWavWalkthrough();


		/*
		Entity* entity = new Entity;
		_curArea->addEntity(entity);
		entity->transform.setPos(600, 600);
		entity->setDepthType(DEPTH_BUFFERED);
		LightSource* lightSrc = entity->addLightSource();
		lightSrc->setSpotCutoffAngle(360.0f);
		LightAttrib* attrib = new LightAttrib;
		attrib->setDiffuseColor(GYVector4{ 0.2f, 2.0f, 0.4f, 1.0f });
		attrib->setAttenuationFactors(GYVector3{ 0.0, 0.000001, 0.000015 });
		lightSrc->setAttrib(attrib);

		entity = new Entity;
		_curArea->addEntity(entity);
		entity->setDepthType(DEPTH_BUFFERED);
		entity->transform.setPos(400, 900);
		lightSrc = entity->addLightSource();
		lightSrc->setSpotCutoffAngle(360.0f);
		attrib = new LightAttrib;
		attrib->setDiffuseColor(GYVector4{ 0.4f, 0.2f, 2.0f, 1.0f });
		attrib->setAttenuationFactors(GYVector3{ 0.0, 0.000001, 0.000015 });
		lightSrc->setAttrib(attrib);

		entity = new Entity;
		_curArea->addEntity(entity);
		entity->transform.setPos(700, 900);
		entity->setDepthType(DEPTH_BUFFERED);
		lightSrc = entity->addLightSource();
		lightSrc->setSpotCutoffAngle(360.0f);
		attrib = new LightAttrib;
		attrib->setDiffuseColor(GYVector4{ 1.5f, 1.0f, 0.3f, 1.0f });
		attrib->setAttenuationFactors(GYVector3{ 0.0, 0.000001, 0.000015 });
		lightSrc->setAttrib(attrib);*/
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
	if(input.getTapped(GameInput::BUT_M)) {
		getAudioSys()->fadeInBgmTrack(5.0f);
		Entity *player1 = _engine->_partySystem->getPartyLeader();
		if(player1) {
			printf(VERBOSE, "<%d, %d>\n",  ((int)player1->transform._pos.x+16)/32, ((int)player1->transform._pos.y+16)/32);
		}
	}
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
