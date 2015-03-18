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
//#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tkMapSystem.h")
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
#include "elysian_dialog_system.hpp"
#include "elysian_event_monitor.hpp"
#include "elysian_rigid_body_system.hpp"
#include "elysian_debug_base.hpp"

namespace elysian {

//======STATIC MEMBERS=====

FILE*				DebugBase::_dbgFile;
FILE*				DebugBase::_luaDbgFile;
Engine*				DebugBase::_engine = nullptr;
unsigned int		DebugBase::_stackDepth = 0;
Debug::LEVEL		DebugBase::_level = ELYSIAN_DEBUG_LOG_LEVEL_DEFAULT;
Debug::ADV_LOG_WRAP DebugBase::_advLogWrap = Debug::ADV_LOG_OFF;

//Invoked by LibGyro (callback function)
void DebugBase::libGyroPrintf(const GY_DEBUG_LEVEL level, const char* const str) { Debug::writeLog(LIBGYRO, (LEVEL)level, str); }
//Invoked by Lua (always VERBOSE, not silenced with the rest)
void DebugBase::luaPrint(const char *string, const LEVEL lev) { Debug::writeLog(LUA, lev, string); }

// Central endpoint for all debug log calls
void DebugBase::writeLog(const DebugBase::SOURCE source, const DebugBase::LEVEL level, const char *const str) {
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

#if !defined(ANDROID)
	fputs(buffer, _dbgFile);
	fflush(_dbgFile);
	if(source == LUA) {
		fputs(str, _luaDbgFile);
		fputc('\n', _luaDbgFile);
		fflush(_luaDbgFile);
	}
#endif
	gyPrintf(buffer); //use the "preferred" IO of each platform

    if(_advLogWrap != ADV_LOG_OFF) {
	AdventureLog::TEXT_TYPE advType;

	switch(source) {
        case LUA:
            advType = AdventureLog::TEXT_LUA_DEBUG;
            break;
        default:
            advType = AdventureLog::TEXT_NORMAL;
        }
        if(level == CRITICAL || level == FATAL) advType = AdventureLog::TEXT_ERROR;

        //Check to see if this kind of output is being wrapped to the Adventure Log
        if(level == CRITICAL || level == FATAL ||
                _advLogWrap == ADV_LOG_ALL ||
                (_advLogWrap == ADV_LOG_LUA && source == LUA))
        {
            //characters that the adventure log cannot display--shouldn't it handle this? Not debug? No shit.
            for(unsigned int i = 0; i < strlen(buffer); ++i) if(buffer[i] == '\n' || buffer[i] == '\t') buffer[i] = ' ';
            if(_engine) getGuiSys()->adventureLog.addLine(advType, buffer);
        }
    }
}

// Invoked for non AssetIO Engine logging
void DebugBase::printf(const Debug::LEVEL level, const char *const string, ...) {
	char buffer[ELYSIAN_DEBUG_BUFFER_SIZE];
	va_list argptr;
	//early-out
	if(level < _level) return;

	va_start(argptr,string);
	vsprintf(buffer, string, argptr);
	va_end(argptr);
    Debug::writeLog(ENGINE, level, buffer);
}

//=====NONSTATIC MEMBERS=====

DebugBase::DebugBase(void):
	_nextFrame(true)
{
	//Register debug callback with LibGyro
	gySysDebugEnable(ELYSIAN_DEBUG_LIBGYRO_LEVEL_DEFAULT, &libGyroPrintf);
	//Register pointer to asset debug interface with AssetIO
	Asset::setDebugStack(&_assetDebug);
#if !defined(ANDROID)
	_dbgFile = fopen(ELYSIAN_DEBUG_FILE, "w");
	_luaDbgFile = fopen(ELYSIAN_LUA_DEBUG_FILE, "w");
#endif
    //printf(VERBOSE, "-----Debug Initialized-----");
}

DebugBase::~DebugBase(void) {
#if !defined(ANDROID)
	fclose(_luaDbgFile);
	fclose(_dbgFile);
#endif
}

void DebugBase::initialize(void) {
    gyDebugInit();
    getInstance();
}

LightSource* lightSrc;
#if 1
GYVector3 dirVec = { 0.0f, 1.0f, 1.0f };
#else
GYVector3 dirVec = { 0.0f, 0.0f, -1.0f };

#endif

bool DebugBase::sandboxInit(void) {
	//Fuck the current loading paradigm. Initialization happens here for now.
    printf(VERBOSE, "LuaManager::runScriptSandbox(%s)", ELYSIAN_GLOBAL_INIT_LUA_PATH);
    Debug::push();

	LuaManager::runScriptSandbox(ELYSIAN_GLOBAL_INIT_LUA_PATH);
	Entity* leader = getPartySys()->getPartyLeader();
	if(leader) {
        leader->_transform.setPos(_cmdLineArgs.x, _cmdLineArgs.y);
        leader->_transform._size.z = 64.0f;
	}

	//LightSource* lightSrc = leader->addLightSource();

    Debug::pop();
	return true;
}


void DebugBase::renderOverlay(void) {
	//if(terrainSysDebug) terrainSystem->RenderOverlays();
	//if(cameraSysDebug) cameraSystem->RenderOverlays();
	//BLAH BLAH ETC
}

void DebugBase::render(void) {

}

void DebugBase::update(void) {

}

static LightSource* src;

static EventCallback callback;

void DebugBase::processInput(GameInput& input) {

	static float theta = 0.0f;
	static unsigned size = 3;
	static unsigned intensity = 200;
    static bool freeCam = false;

	//Camera zoom
    if(input.getDown(GameInput::BUT_W) && ! input.getDown(GameInput::BUT_LALT)){
        _engine->_cameraSystem->staticScale(0.95f);
    }
    if(input.getDown(GameInput::BUT_S) && !input.getDown(GameInput::BUT_LALT)){
        _engine->_cameraSystem->staticScale(1.05f);
	}

	if(input.getTapped(GameInput::BUT_0)) {

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
		}
	}
/*
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
    }*/
	if(input.getTapped(GameInput::BUT_4)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(3);
		printf(VERBOSE, "Render Layer 4 - %s", (render)? "enabled" : "disabled");
	}

	//Rendering layers
	if(input.getTapped(GameInput::BUT_5)) {
        static bool isEnabled = true;
        if(isEnabled) {
            //_engine->_guiSystem->adventureLog.setEnabled(false);
            _engine->_guiSystem->fadeFgLayer(1.0f, false);
            printf(VERBOSE, "Disable GUI Layer");
            isEnabled = false;
		} else {
            //_engine->_guiSystem->adventureLog.setEnabled(true);
            _engine->_guiSystem->fadeFgLayer(1.0f, true);
			printf(VERBOSE, "Enable GUI Layer");
            isEnabled = true;
		}
        //GYVector3 disp = { 200.0f, 0.0f, 0.0f };
        //_cameraSystem->shake(disp, 30, 3.0f, true);
        //LightAttrib attrib;
        //attrib.setAmbientColor(GYVector4 { 1.0f, 0.0f, 0.0f, 1.0f});
        //attrib.setDiffuseColor(GYVector4 { 0.0f, 1.0f, 0.0f, 1.0f});
        //attrib.setSpecularColor(GYVector4 { 0.0f, 0.0f, 1.0f, 1.0f});
        //_lightSystem->interpLightAttrib(5.0f, attrib, _daySystem->getSunLightAttrib());

    }

	//EVERYTHING BEYOND THIS POINT IS IN FALCO'S DEBUG KEYSPACE!!!!
	if(!input.getDown(GameInput::BUT_LALT)) return;

    if(input.getDown(GameInput::BUT_W) && freeCam){

        _engine->_cameraSystem->translate(GYVector3{0.0f, -3.0f, 0.0f});
    }
    if(input.getDown(GameInput::BUT_S) && freeCam){
        _engine->_cameraSystem->translate(GYVector3{0.0f, 3.0f, 0.0f});
    }
    if(input.getDown(GameInput::BUT_A) && freeCam){
        _engine->_cameraSystem->translate(GYVector3{-3.0f, 0.0f, 0.0f});
    }
    if(input.getDown(GameInput::BUT_D) && freeCam){
        _engine->_cameraSystem->translate(GYVector3{3.0f, 0.0f, 0.0f});
    }

    if(input.getDown(GameInput::BUT_Q) && !freeCam){
        _engine->_cameraSystem->rotateZ(0.1f*60.0f*getTimerSys()->getDeltaTime());
	}
    if(input.getDown(GameInput::BUT_A) && !freeCam){
        _engine->_cameraSystem->rotateZ(-0.1f*60.0f*getTimerSys()->getDeltaTime());
	}

    if(input.getDown(GameInput::BUT_E) && !freeCam) {
        _engine->_cameraSystem->rotateX(-0.1f*getTimerSys()->getDeltaTime());
	}

    if(input.getDown(GameInput::BUT_D) && !freeCam) {
        _engine->_cameraSystem->rotateX(0.1f*getTimerSys()->getDeltaTime());
	}


	if(input.getTapped(GameInput::BUT_Z)) { //Reset Transform
		_engine->_cameraSystem->setStaticScale(1.0f);
		_engine->_cameraSystem->setOrientZ(0.0f);
		_engine->_cameraSystem->setOrientX(0.0f);
	}

	if(input.getTapped(GameInput::BUT_C)) {
		static bool enabled = true;
		enabled = !enabled;
		_engine->_collisionSystem->setEnabled(enabled);
		if(enabled) Debug::printf(Debug::VERBOSE, "Collision Enabled");
		else Debug::printf(Debug::VERBOSE, "Collision Disabled");

	}

	if(input.getTapped(GameInput::BUT_L)) {
		ADV_LOG_WRAP newWrap = (_advLogWrap < ADV_LOG_MAX-1)? (ADV_LOG_WRAP)(_advLogWrap+1) : (ADV_LOG_WRAP)0;
		if(newWrap == ADV_LOG_ALL) Debug::printf(Debug::VERBOSE, "Adventure Log Wrapping Engine Debug");
		else if(newWrap == ADV_LOG_LUA) Debug::printf(Debug::VERBOSE, "Adventure Log Wrapping Lua only");
		else Debug::printf(Debug::VERBOSE, "Adventure Log Wrapping None");
		_advLogWrap = newWrap;
	}

	if(input.getTapped(GameInput::BUT_G)) {
		static bool enabled = true;
		enabled = !enabled;
		getRigidBodySys()->setGravityEnabled(enabled);
		Debug::printf(Debug::VERBOSE, "Gravity %s.", (enabled)? "enabled" : "disabled");
	}

	if(input.getDown(GameInput::BUT_5)) {
		++intensity;
		dirVec.z += 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}
	if(input.getTapped(GameInput::BUT_6)) {
		static bool enabled = false;
		enabled = !enabled;
		getGuiSys()->setDebugOverLayEnabled(enabled);

		//if(intensity != 0) --intensity;
		dirVec.z -= 0.1f;
        //gyVec3Normalize(&dirVec);
        //src->setDirection(dirVec);
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
		dirVec.x -= 0.1f;
		theta -= 0.1f;
		gyVec3Normalize(&dirVec);
		src->setDirection(dirVec);
		printf(VERBOSE, "<%f, %f, %f>\n", dirVec.x, dirVec.y, dirVec.z);
	}

	if(input.getTapped(GameInput::BUT_R)) {
        freeCam = !freeCam;
        static Entity* julien = getCurArea()->findEntity("Julien");
#if 0
		getLightSys()->setRenderDebugEnabled(renderDebug);
		printf(VERBOSE, "LightSystem::renderShadowVolumes = %d", renderDebug);
#else
        if(freeCam) getCameraSys()->setFollowEntity(nullptr);
        else getCameraSys()->setFollowEntity(julien);

#endif
	}

    if(input.getDown(GameInput::BUT_U)) {

		static bool once = true;
        static ParticleEmitter* emitter;
        static Entity* entity;

		if(once) {
            auto* attrib = new ParticleEmitterAttrib();//getPartSys()->getAttrib(0);
            entity = getCurArea()->findEntity("Julien");
            attrib->loadTexture("/assets/item/ruinsPot.png", 32, 64, 32, 32);
            emitter = entity->addParticleEmitter();
            emitter->setAttrib(attrib);
            attrib->setStartSize(32);
            attrib->setEndSize(32);
            attrib->setColor(10, 10, 255, 10);
            attrib->setEndColor(GYColor4{0, 0, 255, 100});
            attrib->setAnimGridSize(2, 2);
            attrib->setAnimGridIndex(0, 0, 0);
            attrib->setAnimGridIndex(0, 1, 1);
            attrib->setAnimGridIndex(1, 0, 1);
            attrib->setAnimGridIndex(1, 1, 0);
            attrib->setAnimFrameDuration(5);
            attrib->setMaxLife(60);
            //getPartSys()->getAttrib(0)->setSize(32);
			//getPartSys()->getAttrib(0)->setFrameRange(10, 13);
            once = false;
		}

        emitter->emitFromEntity( GYVector3 { 0.0f, 0.0f, 3.0f }, theta, 1);
        //getPartSys()->Q_EMITFromPoint(0, entity->transform._pos, theta, intensity*10);
       // void Q_EMITFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity);
		  // getPartSys()->Q_EMITFromPoint(0, GYVector3 { entity->transform._pos.x+5, entity->transform._pos.y, 500.0f}, theta, intensity);

//		getGuiSys()->addCombatText("10", GYColor4 { 255, 0, 0, 255}, GYVector2 { entity->transform._pos.x, entity->transform._pos.y}, GYVector2 { 0.5f, -5.0f});

    }

//	static float timeSpeed = 60.0f;

	if(input.getTapped(GameInput::BUT_N)) {
		static bool sunEnabled = true;
		sunEnabled = !sunEnabled;
		getDaySys()->setSunEnabled(sunEnabled);
	}
	if(input.getDown(GameInput::BUT_M)) {
		getDaySys()->addSeconds(600);
	}

	/*
	if(input.getDown(GameInput::BUT_N)) {
		timeSpeed *= 1.1f;
		getDaySys()->setTimeDilation(timeSpeed);
	}
	if(input.getDown(GameInput::BUT_M)) {
		timeSpeed *= 0.9f;
		getDaySys()->setTimeDilation(timeSpeed);
	}*/

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
        entity->_transform.setPos(200.0F, 200.0f);
        entity->_transform.setScale(1.0f, 1.0f);
        entity->_transform.setSize(128.0f, 128.0f);
		entity->setElev(0);
        entity->setPlaneType(GY_RECT_PLANE_FRONT);
		Renderable* render = entity->addRenderable();
#if 1
		QuadSpriteAttrib* attrib = new QuadSpriteAttrib;
        attrib->setVertexColor(0, 255, 0,
                               0, 255);
		attrib->setVertexColor(1, 0, 255, 0, 100);
		attrib->setVertexColor(2, 0, 0, 255, 255);
		attrib->setVertexColor(3, 255, 255, 0, 255);
		attrib->loadTexture("item/legacyItems/jigglypuff1.PNG", 32, 32, 32, 32);
		attrib->setTexCoord(3, 2.0, 2.0f);
		attrib->setTexCoord(2, 0.0, 2.0f);
		attrib->setTexCoord(1, 2.0, 0.0f);
		attrib->setTexCoord(0, 0.0, 0.0f);
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
		Collidable *col = entity->addCollidable();
		CollidableAttrib* colattrib = new CollidableAttrib();
		col->setAttrib(colattrib);
		entity->addRigidBody();

		getCurArea()->addEntity(entity);
	}


	if(input.getTapped(GameInput::BUT_X)) {
/*		GYVector4 diffuseColors[4] = {
			{ 1.0f, 1.0f, 5.0f, 1.0f},
			{ 1.0f, 2.0f, 1.0f, 1.0f},
			{ 2.0f, 1.0f, 1.0f, 1.0f},
			{ 2.0f, 2.0f, 1.0f, 1.0f}
		};
*/
		static int count = 0;
		char buff[100];

		if(count == 0) {

		Entity* julien = getCurArea()->findEntity("Julien");

	sprintf(buff, "LightSrc[%d]", count);

			Entity* entity = julien;
		//	--entity->getRenderable()->getSpriteAttrib()->setAlpha(0);
		//	entity->setName(buff);
			entity->setShadowVolume(false);
			//entity->transform.setSize(1.0f, 1.0f, 1.0f);
			//entity->removeAnimation();
			//entity->removeRenderable();
			//entity->transform.setPos(((count%8)*200)+100, 700+200*(count/8));
			//entity->transform.setPos(julien->transform._pos.x, julien->transform._pos.y);
			entity->setElev(0);
			//curArea->addEntity(entity);
			//entity->transform.setPos(600, 600);
			//entity->setDepthType(DEPTH_BUFFERED);
			src = lightSrc = entity->addLightSource();
			lightSrc->setLightType(GY_LIGHT_POINT);
			//lightSrc->setRelativePos(0.0f, 0.0f, 2.0f);
			gyVec3Normalize(&dirVec);
			//lightSrc->setDirection(dirVec);
			LightAttrib* attrib = new LightAttrib;
			attrib->setAmbientColor(GYVector4{ 0.1f, 0.1f, 0.1f, 0.0f});
			attrib->setDiffuseColor(GYVector4 { 0.0f, 0.0f, 1.0f, 0.0f });
			attrib->setSpecularColor(GYVector4{ 0.0f, 1.0f, 1.0f, 0.0f});
			lightSrc->setAttrib(attrib);


			//lightSrc->setSpotCutoffAngle(45.0f);
			lightSrc->setShadowCaster(true);
			attrib->setAttenuationFactors(GYVector3{ 0.00001f, 0.00001f, 0.00001f });

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


	}

	//Elevate player
	if(input.getTapped(GameInput::BUT_Y)) {
	/*	Dialog* dialog = new Dialog;
		auto* screen = dialog->addTextScreen("WTF!? A wild Rattata appeared!");
		screen->setNextIndex(-1);
		getDialogSys()->startDialog(dialog);*/

		Entity* leader = _engine->_partySystem->getPartyLeader();
		if(leader) {
			unsigned elev = leader->getElev();
			elev = (elev < ELYSIAN_MAP_ENTRY_ELEV_MAX)? elev+1 : 0;
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

	//KILL ALL INPUT WHEN YOU'RE HOLDING ALT!!!!
	input.clear();

}

#define ARG_LIMIT 20
bool DebugBase::parseArguments(void) {
	int argc = 0;
	char argBuff[ARG_LIMIT][100];
	char *argv[ARG_LIMIT];
	char *buffer;
	char *bufptr;
	size_t buffer_len = 0;
	GYFile *file;

	//Default values for command-line args
	_cmdLineArgs.x = _cmdLineArgs.y = 32*4;
	strcpy(_cmdLineArgs.level, ELYSIAN_DEBUG_LEVEL_DEFAULT);
	strcpy(_cmdLineArgs.area, ELYSIAN_DEBUG_AREA_DEFAULT);

	if (gyFileOpen(ELYSIAN_DEBUG_ARG_FILE, "r", &file)) {
		printf(VERBOSE, "Debug::parseArguments(void) - initializing from '%s'", ELYSIAN_DEBUG_ARG_FILE);
        Debug::push();
		if (!gyFileLength(file, &buffer_len)) {
			return 0;
		}
		buffer = (char *) malloc(sizeof(char) * buffer_len + 1);
		memset(buffer, 0, sizeof(char) * buffer_len + 1);
		if (!gyFileRead(file, buffer, buffer_len, NULL)) {
			return 0;
		}
		bufptr = buffer;
		while(sscanf(bufptr, "%s", argBuff[argc]) != EOF) {
			argv[argc + 1] = argBuff[argc]; //yes, I'm really doing this.
			printf(VERBOSE, "argv[%d] - %s", argc + 1, argv[argc + 1]);
			bufptr += strlen(argBuff[argc]) + 1;
			++argc;
		}
		free(buffer);
		gyFileClose(&file);
        Debug::pop();
		//Recursively reinvoke argument-based initialization
		return parseArguments(argc+1, argv);
    } else {
        printf(CRITICAL, "gamma_args.txt not found! Shitting pants!");
    }
	return false;
}

bool DebugBase::parseArguments(int argc, char *argv[]) {
	if(argc <= 1) return parseArguments(); //default init

	printf(VERBOSE, "Debug::parseArguments(%d, char *argv[])", argc);
    Debug::push();
	for(unsigned int i = 1; i < (unsigned int)argc; ++i) {
		if(argv[i][0] != '-') {
			printf(CRITICAL, "! - Unrecognized command-line argument: %s", argv[i]);
			continue;
		}

		//Parse all command-line arguments and store them within internal structure
		switch(argv[i][1]) {
			case 'l':
				++i;
				/*strcpy(_cmdLineArgs.level, argv[i]);*/
				sprintf(_cmdLineArgs.level, "/assets/%s", argv[i]);
				break;
			case 'a':
				++i;
				/*strcpy(_cmdLineArgs.area, argv[i]);*/
				sprintf(_cmdLineArgs.area, "/assets/%s", argv[i]);
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
    Debug::pop();
	return true;
}


bool DebugBase::initializeEngine(void) {
	if(_cmdLineArgs.level) _engine->_levelSystem->switchLevel(_cmdLineArgs.level);
	if(_cmdLineArgs.area) _engine->_levelSystem->switchArea(_cmdLineArgs.area);
	return true;
}

}
