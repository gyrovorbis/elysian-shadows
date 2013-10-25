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

namespace elysian {

//======STATIC MEMBERS=====

const char Debug::_fileName[] = ELYSIAN_DEBUG_FILE;
ofstream Debug::_dbgFile;
Engine *Debug::_engine = NULL;
bool Debug::_advLogWrap = false;
unsigned int Debug::_stackDepth = 0;
Debug::LEVEL Debug::_level = ELYSIAN_DEBUG_LOG_LEVEL_DEFAULT;
    
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
	size_t length = strlen(buffer);
	strcat(buffer, "\n");
    
	_dbgFile << buffer;
	_dbgFile.flush(); //write to log file
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

		//characters that the adventure log cannot display--shouldn't it handle this? Not debug?
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
	_dbgFile.open(_fileName);
	printf(VERBOSE, "-----Debug Initialized-----");
}

Debug::~Debug(void) {
	_dbgFile.close();
}

bool Debug::sandboxInit(void) {
	//Fuck the current loading paradigm. Initialization happens here for now.
	printf(VERBOSE, "Debug::sandboxInit()");
	push();

	_advLogWrap = true;

//======================    
    
	//Base Entity Data
	Entity *player = new Entity;
	player->setName("Julien");
	//_curArea->addEntity(player);
	//player->transform.setPosition((float)_cmdLineArgs.x, (float)_cmdLineArgs.y, ELYSIAN_ZCOORD_OBJECT2+10.0f);
    player->transform.setPos(_cmdLineArgs.x, _cmdLineArgs.y, ELYSIAN_ZCOORD_OBJECT2+10.0f);
	player->transform.setSize(32.0f, 64.0f);
	player->transform.setScale(1.0f, 1.0f);
	player->transform.setOrientZ(0.0f);
	player->setDepthType(DEPTH_BUFFERED);
	
	//Renderable Component
	Renderable *render = player->addRenderable();
	render->setFrame(0);
	SpriteAttrib *sprAttrib = new SpriteAttrib;
	sprAttrib->loadTexture(gyFilePath("party/julien.png"), 256, 256, 32, 64);
	sprAttrib->setColor(255, 255, 255, 255);
	render->setSpriteAttrib(sprAttrib);

	//Animation Component
	Animation *anim = player->addAnimation();
	AnimationAttrib *attrib = new AnimationAttrib;
	AnimationSequence *seq = attrib->addSequence("WalkDown");
	seq->addFrame(0.0f, 0.0f, 0.15f, 1);
	seq->addFrame(0.0f, 0.0f, 0.15f, 2);
	seq->addFrame(0.0f, 0.0f, 0.15f, 3);
	seq->addFrame(0.0f, 0.0f, 0.15f, 4);
	seq->addFrame(0.0f, 0.0f, 0.15f, 5);
	seq->addFrame(0.0f, 0.0f, 0.15f, 6);
	seq = attrib->addSequence("WalkUp");
	seq->addFrame(0.0f, 0.0f, 0.15f, 9);
	seq->addFrame(0.0f, 0.0f, 0.15f, 10);
	seq->addFrame(0.0f, 0.0f, 0.15f, 11);
	seq->addFrame(0.0f, 0.0f, 0.15f, 12);
	seq->addFrame(0.0f, 0.0f, 0.15f, 13);
	seq->addFrame(0.0f, 0.0f, 0.15f, 14);
	seq = attrib->addSequence("WalkRight");
	seq->addFrame(0.0f, 0.0f, 0.15f, 17);
	seq->addFrame(0.0f, 0.0f, 0.15f, 18);
	seq->addFrame(0.0f, 0.0f, 0.15f, 19);
	seq->addFrame(0.0f, 0.0f, 0.15f, 20);
	seq->addFrame(0.0f, 0.0f, 0.15f, 21);
	seq->addFrame(0.0f, 0.0f, 0.15f, 22);
	seq = attrib->addSequence("WalkLeft");
	seq->addFrame(0.0f, 0.0f, 0.15f, 25);
	seq->addFrame(0.0f, 0.0f, 0.15f, 26);
	seq->addFrame(0.0f, 0.0f, 0.15f, 27);
	seq->addFrame(0.0f, 0.0f, 0.15f, 28);
	seq->addFrame(0.0f, 0.0f, 0.15f, 29);
	seq->addFrame(0.0f, 0.0f, 0.15f, 30);
	seq = attrib->addSequence("StandUp");
	seq->addFrame(0.0f, 0.0f, 0.0f, 8);
	seq = attrib->addSequence("StandDown");
	seq->addFrame(0.0f, 0.0f, 0.0f, 0);
	seq = attrib->addSequence("StandLeft");
	seq->addFrame(0.0f, 0.0f, 0.0f, 24);
	seq = attrib->addSequence("StandRight");
	seq->addFrame(0.0f, 0.0f, 0.0f, 16);
	anim->setAttrib(attrib);

	//Collidable Component
	Collidable *collid = player->addCollidable();
    CollidableAttrib *colAttrib = new CollidableAttrib;
    colAttrib->setColliderType(GY_COLLIDER_AABB);
    colAttrib->setRelPos(0.0f, 0.25f);
    colAttrib->setRelSize(1.0f, 0.4f);
    static GYStatic statAttrib = {
        /* inverseMass */ 1.0f/5000.0f,
        /* coeffOfRest */ 1.0f,
        /* damping */ 0.0f,
        /* invMomOfInertia */ 0.0f,
        /* angDamping */ 0.0f
    };
    *colAttrib->getStaticAttrib() = statAttrib;
    collid->setAttrib(colAttrib);

	//Character Component
	player->addCharacter();

	//Lua Behavior
	//LuaBehavior *behav = player->addLuaBehavior("level/new_level1/area/Falco_ZTran_Sandbox/test.lua");
	//behav->setTriggers(Behavior::ONCREATE | Behavior::ONUPDATE);

	getPartySys()->addCharacterToParty(*player);



//======================    
#if 0

	//Base Entity Data
	player = new Entity;
	_curArea->addEntity(player);
	player->setName("Angel");
	player->transform.setPos(12.0f*32.0f, 20.0f*32.0f, ELYSIAN_ZCOORD_OBJECT2+10.0f);
	player->transform.setSize(32.0f, 60.0f);
	player->transform.setScale(1.0f, 1.0f);
	player->transform.setOrientZ(0.0f);
	player->setDepthType(DEPTH_BUFFERED);

	//AudioSource Component
	/*AudioSource* audio = player->addAudioSource();
	audio->setWav(gyAudWavCreate("phone.wav"));
	audio->setLooping(true);
	//audio->setGain(0.1f);
	audio->play();*/
	
	//Renderable Component
	render = player->addRenderable();
	render->setFrame(0);
	sprAttrib = new SpriteAttrib;
	sprAttrib->loadTexture(gyFilePath("party/party_member_3.png"), 256, 256, 32, 64);
	sprAttrib->setColor(255, 255, 255, 255);
	render->setSpriteAttrib(sprAttrib);
	/*
	//Animation Component
	anim = player->addAnimation();
	attrib = new AnimationAttrib;
	seq = attrib->addSequence("WalkDown");
	seq->addFrame(0.0f, 0.0f, 0.15f, 1);
	seq->addFrame(0.0f, 0.0f, 0.15f, 2);
	seq->addFrame(0.0f, 0.0f, 0.15f, 3);
	seq->addFrame(0.0f, 0.0f, 0.15f, 4);
	seq->addFrame(0.0f, 0.0f, 0.15f, 5);
	seq->addFrame(0.0f, 0.0f, 0.15f, 6);
	seq = attrib->addSequence("WalkUp");
	seq->addFrame(0.0f, 0.0f, 0.15f, 9);
	seq->addFrame(0.0f, 0.0f, 0.15f, 10);
	seq->addFrame(0.0f, 0.0f, 0.15f, 11);
	seq->addFrame(0.0f, 0.0f, 0.15f, 12);
	seq->addFrame(0.0f, 0.0f, 0.15f, 13);
	seq->addFrame(0.0f, 0.0f, 0.15f, 14);
	seq = attrib->addSequence("WalkRight");
	seq->addFrame(0.0f, 0.0f, 0.15f, 17);
	seq->addFrame(0.0f, 0.0f, 0.15f, 18);
	seq->addFrame(0.0f, 0.0f, 0.15f, 19);
	seq->addFrame(0.0f, 0.0f, 0.15f, 20);
	seq->addFrame(0.0f, 0.0f, 0.15f, 21);
	seq->addFrame(0.0f, 0.0f, 0.15f, 22);
	seq = attrib->addSequence("WalkLeft");
	seq->addFrame(0.0f, 0.0f, 0.15f, 25);
	seq->addFrame(0.0f, 0.0f, 0.15f, 26);
	seq->addFrame(0.0f, 0.0f, 0.15f, 27);
	seq->addFrame(0.0f, 0.0f, 0.15f, 28);
	seq->addFrame(0.0f, 0.0f, 0.15f, 29);
	seq->addFrame(0.0f, 0.0f, 0.15f, 30);
	seq = attrib->addSequence("StandUp");
	seq->addFrame(0.0f, 0.0f, 0.0f, 8);
	seq = attrib->addSequence("StandDown");
	seq->addFrame(0.0f, 0.0f, 0.0f, 0);
	seq = attrib->addSequence("StandLeft");
	seq->addFrame(0.0f, 0.0f, 0.0f, 24);
	seq = attrib->addSequence("StandRight");
	seq->addFrame(0.0f, 0.0f, 0.0f, 16);
	anim->setAttrib(attrib); */

	//Collidable Component
	collid = player->addCollidable();
    colAttrib = new CollidableAttrib;
    colAttrib->setColliderType(GY_COLLIDER_AABB);
    colAttrib->setRelPos(0.0f, 0.25f);
    colAttrib->setRelSize(1.0f, 0.3f);
    static GYStatic statAttrib2 = {
        /* inverseMass */ 1.0f/3000.0f,
        /* coeffOfRest */ 1.0f,
        /* damping */ 1.0f,
        /* invMomOfInertia */ 0.0f,
        /* angDamping */ 1.0f
    };
    *colAttrib->getStaticAttrib() = statAttrib;
    collid->setAttrib(colAttrib);

	//Character Component
	//player->addCharacter();

	behav = player->addLuaBehavior("level/new_level1/area/Falco_ZTran_Sandbox/test.lua");
	behav->setTriggers(Behavior::ONCREATE | Behavior::ONUPDATE);
//=================
    //Base Entity Data
	player = new Entity;
	_curArea->addEntity(player);
	player->setName("Dreamcast");
	player->transform.setPos(16.0f*32.0f, 23.0f*32.0f, ELYSIAN_ZCOORD_OBJECT2+10.0f);
	player->transform.setSize(32.0f, 32.0f);
	player->transform.setScale(1.0f, 1.0f);
	player->transform.setOrientZ(0.0f);
	
	//Renderable Component
	render = player->addRenderable();
	render->setFrame(0);
	sprAttrib = new SpriteAttrib;
	sprAttrib->loadTexture(gyFilePath("item/drmcsat.PNG"), 32, 32, 32, 32);
	sprAttrib->setColor(255, 255, 255, 255);
	render->setSpriteAttrib(sprAttrib);
    
    //Collidable Component
	collid = player->addCollidable();
    colAttrib = new CollidableAttrib;
    colAttrib->setColliderType(GY_COLLIDER_AABB);
    colAttrib->setRelPos(0.0f, 0.0f);
    colAttrib->setRelSize(0.5f, 0.5f);
    static GYStatic statAttrib3 = {
        /* inverseMass */ 1.0f/3000.0f,
        /* coeffOfRest */ 1.0f,
        /* damping */ 1.0f,
        /* invMomOfInertia */ 0.0f,
        /* angDamping */ 1.0f
    };
    *colAttrib->getStaticAttrib() = statAttrib;
    collid->setAttrib(colAttrib);
    
    //Base Entity Data
	player = new Entity;
	_curArea->addEntity(player);
	player->setName("Star");
	player->transform.setPos(16.0f*32.0f, 25.0f*32.0f, ELYSIAN_ZCOORD_OBJECT2+10.0f);
	player->transform.setSize(32.0f, 32.0f);
	player->transform.setScale(1.0f, 1.0f);
	player->transform.setOrientZ(0.0f);
	
	//Renderable Component
	render = player->addRenderable();
	render->setFrame(0);
	sprAttrib = new SpriteAttrib;
	sprAttrib->loadTexture(gyFilePath("item/star.PNG"), 32, 32, 32, 32);
	sprAttrib->setColor(255, 255, 255, 255);
	render->setSpriteAttrib(sprAttrib);
    
    //Collidable Component
	collid = player->addCollidable();
    colAttrib = new CollidableAttrib;
    colAttrib->setColliderType(GY_COLLIDER_AABB);
    colAttrib->setRelPos(0.0f, 0.0f);
    colAttrib->setRelSize(0.5f, 0.5f);
    static GYStatic statAttrib4 = {
        /* inverseMass */ 1.0f/3000.0f,
        /* coeffOfRest */ 1.0f,
        /* damping */ 1.0f,
        /* invMomOfInertia */ 0.0f,
        /* angDamping */ 1.0f
    };
    *colAttrib->getStaticAttrib() = statAttrib;
    collid->setAttrib(colAttrib);
    
    //Base Entity Data
	player = new Entity;
	_curArea->addEntity(player);
	player->setName("Fireflower");
	player->transform.setPos(19.0f*32.0f, 21.0f*32.0f, ELYSIAN_ZCOORD_OBJECT2+10.0f);
	player->transform.setSize(32.0f, 32.0f);
	player->transform.setScale(1.0f, 1.0f);
	player->transform.setOrientZ(0.0f);
	
	//Renderable Component
	render = player->addRenderable();
	render->setFrame(0);
	sprAttrib = new SpriteAttrib;
	sprAttrib->loadTexture(gyFilePath("item/fireflower.PNG"), 32, 32, 32, 32);
	sprAttrib->setColor(255, 255, 255, 255);
	render->setSpriteAttrib(sprAttrib);
    
    //Collidable Component
	collid = player->addCollidable();
    colAttrib = new CollidableAttrib;
    colAttrib->setColliderType(GY_COLLIDER_AABB);
    colAttrib->setRelPos(0.0f, 0.0f);
    colAttrib->setRelSize(0.5f, 0.5f);
    static GYStatic statAttrib5 = {
        /* inverseMass */ 1.0f/3000.0f,
        /* coeffOfRest */ 1.0f,
        /* damping */ 1.0f,
        /* invMomOfInertia */ 0.0f,
        /* angDamping */ 1.0f
    };
    *colAttrib->getStaticAttrib() = statAttrib;
    collid->setAttrib(colAttrib);
    
    //Base Entity Data
	player = new Entity;
	_curArea->addEntity(player);
	player->setName("Block");
	player->transform.setPos(22.0f*32.0f, 24.0f*32.0f, ELYSIAN_ZCOORD_OBJECT2+10.0f);
	player->transform.setSize(32.0f, 32.0f);
	player->transform.setScale(1.0f, 1.0f);
	player->transform.setOrientZ(0.0f);
	
	//Renderable Component
	render = player->addRenderable();
	render->setFrame(0);
	sprAttrib = new SpriteAttrib;
	sprAttrib->loadTexture(gyFilePath("item/marioblock.png"), 32, 32, 32, 32);
	sprAttrib->setColor(255, 255, 255, 255);
	render->setSpriteAttrib(sprAttrib);
    
    //Collidable Component
	collid = player->addCollidable();
    colAttrib = new CollidableAttrib;
    colAttrib->setColliderType(GY_COLLIDER_AABB);
    colAttrib->setRelPos(0.0f, 0.0f);
    colAttrib->setRelSize(0.5f, 0.5f);
    static GYStatic statAttrib6 = {
        /* inverseMass */ 1.0f/3000.0f,
        /* coeffOfRest */ 1.0f,
        /* damping */ 1.0f,
        /* invMomOfInertia */ 0.0f,
        /* angDamping */ 1.0f
    };
    *colAttrib->getStaticAttrib() = statAttrib;
    collid->setAttrib(colAttrib);
#endif

	getAudioSys()->loadBgmTrack(0, "audio/music/CaveLoop1.ogg");
	getAudioSys()->loadBgmTrack(1, "audio/music/CaveLoop2.ogg");
	getAudioSys()->loadAuxTrack(0, "audio/music/RainThunder.ogg");
	getAudioSys()->playAuxTrack(0);
	//getAudioSys()->setAuxTrackGain(0, 0.5f);
	getAudioSys()->playBgmTrack();
	getAudioSys()->setBgmTrackGain(0.5f);
	getAudioSys()->setBgmTrackPitch(0.5f);

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
    if(input.getTapped(GameInput::BUT_0)) { 
        if(_engine->_guiSystem->adventureLog.isEnabled()) {
			_engine->_guiSystem->adventureLog.disable();
			printf(VERBOSE, "Disable GUI Layer");
		} else {
			_engine->_guiSystem->adventureLog.enable();
			printf(VERBOSE, "Enable GUI Layer");
		}
    }
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
	if(input.getTapped(GameInput::BUT_5)) {
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
		}
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
		getAudioSys()->fadeIntoNewAuxTrack(0, 5.0f, "audio/music/RainThunder.ogg");
		//getAudioSys()->playBgmTrack();
	}
	if(input.getTapped(GameInput::BUT_X)) {
		//getAudioSys()->morphIntoBgmTrack(0, 5.0f);
		//getAudioSys()->fadeIntoBgmTrack(0, 5.0f);
		//getAudioSys()->stopBgmTrack();
		//getAudioSys()->morphIntoNewBgmTrack(1, 5.0f, "audio/music/CaveLoop1.ogg");
		//getAudioSys()->fadeIntoNewBgmTrack(1, 5.0f, "audio/music/CaveLoop1.ogg");
		getAudioSys()->fadeOutAuxTrack(0, 5.0f);
		//gyAudWavWalkthrough();
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

	FILE *file = fopen(gyFilePath("gamma_args.txt"), "r");
	if(file) { //read cmd-line arguments from file
		printf(VERBOSE, "Debug::InitializeEngine() - initializing from text file");
		push();
		while(fscanf(file, "%s", argBuff[argc]) != EOF) {
			argv[argc+1] = argBuff[argc]; //yes, I'm really doing this.
			printf(VERBOSE, "ARG - %s", argv[argc+1]);
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

	printf(VERBOSE, "Debug::InitializeEngine(%d, char *argv[])", argc);
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
	if(_cmdLineArgs.level) _engine->_levelSystem->SwitchLevel(_cmdLineArgs.level);
	if(_cmdLineArgs.area) _engine->_levelSystem->SwitchArea(_cmdLineArgs.area);
	return true;
}

}
