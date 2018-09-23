#include <stdint.h>

#include <libGyro/gyro.h>
#include <libGyro-Kinetic/gyro_kinetic.h>

#include <AssetIO/level/elysian_zcoords.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_sprite_attrib.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESGamma/crtp/elysian_level_system.hpp>
#include <ESGamma/system/elysian_engine.hpp>
//#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", <ESGamma/tk_map_system.h>)
#include <ESGamma/crtp/elysian_map_system.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>
#include <AssetIO/crtp/elysian_character.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <ESGamma/crtp/elysian_gui_system.hpp>
#include <ESGamma/system/elysian_behavior_system.hpp>
#include <ESGamma/system/elysian_collision_system.hpp>
#include <AssetIO/crtp/elysian_level.hpp>
#include <ESGamma/gui/elysian_adventure_log.hpp>
#include <AssetIO/crtp/elysian_lua_behavior.hpp>
#include <AssetIO/crtp/elysian_audio_source.hpp>
#include <AssetIO/crtp/elysian_light_source.hpp>
#include <ESGamma/system/elysian_light_system.hpp>
#include <ESGamma/crtp/elysian_particle_system.hpp>
#include <ESGamma/system/elysian_day_system.hpp>
#include <ESGamma/system/elysian_dialog_system.hpp>
#include <ESGamma/utility/elysian_event_monitor.hpp>
#include <ESGamma/system/elysian_rigid_body_system.hpp>
#include <AssetIO/crtp/elysian_renderable.hpp>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/gui/elysian_layout.hpp>
#include <ESGamma/gui/elysian_label_widget.hpp>
#include <ESGamma/gui/elysian_entity_widget.hpp>
#include <ESGamma/system/elysian_thread_system.hpp>
#include <ESGamma/system/elysian_compute_system.hpp>


#include <ESToolkit/ui/tk_estoolkit.h>
#include <QESTkUtils/ui/widget/tk_line_chart.h>

namespace elysian {

//======STATIC MEMBERS=====

FILE*                       DebugBase::_dbgFile;
FILE*                       DebugBase::_luaDbgFile;
Engine*                     DebugBase::_engine          = nullptr;
GYAtomic                    DebugBase::_stackDepth;
Debug::SEVERITY             DebugBase::_level           = ELYSIAN_DEBUG_LOG_SEVERITY_DEFAULT;
Debug::ADV_LOG_WRAP         DebugBase::_advLogWrap      = Debug::ADV_LOG_LOG_OFF;
Mutex*                      DebugBase::_logMutex        = nullptr;

const GYDebugCallbacks      DebugBase::_gyDbgCallbacks  =  {
    &libGyroPrintf,
    &Debug::push,
    &Debug::pop
};

void DebugBase::_guiWidgetsTest(void) {
#if 0
    static bool initialized = false;
    HorizontalLayout* hLayout =  nullptr;
    EntityWidget* mainWidget = nullptr;
    LabelWidget* labelWidget = nullptr;
    Entity* entity = nullptr;
    EntityWidget* subWidget = nullptr;
    VerticalLayout* vLayout = nullptr;

    if(!initialized) {
        mainWidget = new EntityWidget();
        hLayout = new HorizontalLayout;
        mainWidget->setLayout(hLayout);
        labelWidget = new LabelWidget("WISCONSin");
        _engine->getGuiSys()->setFgWidget(mainWidget);
        entity = new Entity();
        entity->getTransform().setPos(400, 200, 51);
        entity->getTransform().setSizeX(300);
        auto* render = entity->addRenderable();
        SpriteAttrib* attrib = new SpriteAttrib;
        attrib->setColor(100, 100, 100, 100);
        render->setSpriteAttrib(attrib);
        mainWidget->setEntity(entity);

        subWidget = new EntityWidget();
        Entity* entity2 = new Entity();
        entity2->getTransform().setPos(100, 100, 52);
        entity2->getTransform().setSizeX(150);
        //entity2->getTransform().setSizeY(50);
        auto* render2 = entity2->addRenderable();
        SpriteAttrib* attrib2 = new SpriteAttrib;
        attrib2->setColor(0, 200, 100, 100);
        render2->setSpriteAttrib(attrib2);
        subWidget->setEntity(entity2);
        hLayout->addWidget(subWidget);
        hLayout->addWidget(labelWidget);

        auto* text2 = new LabelWidget("A");
        auto* text3 = new LabelWidget("B");
        Entity* entity3 = new Entity();
        entity3->getTransform().setPos(100, 100, 52);
        entity3->getTransform().setSizeX(40);
        entity3->getTransform().setSizeY(40);
        auto* render3 = entity3->addRenderable();
        SpriteAttrib* attrib3 = new SpriteAttrib;
        attrib3->setColor(0, 0, 255, 100);
        render3->setSpriteAttrib(attrib3);
        vLayout = new VerticalLayout();
        vLayout->addWidget(text2);
        vLayout->addWidget(new EntityWidget(entity3));
        vLayout->addWidget(text3);
        subWidget->setLayout(vLayout);

        initialized = true;
    }
#endif

}


Debug& DebugBase::getInstance(void) {
    static Debug instance;
    return instance;
}

DbgEvent::DbgEvent(const char*const string, ...) {
    char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr,string);
    vsprintf(buffer, string, argptr);
    va_end(argptr);
    depth = Debug::getStackDepth();
    Debug::printf(Debug::LOG_VERBOSE, buffer);
    Debug::push();
}

DbgEvent::~DbgEvent(void) {
    Debug::pop(Debug::getStackDepth()-depth);
}

//Invoked by LibGyro (callback function)
void DebugBase::libGyroPrintf(const GY_DEBUG_SEVERITY level, const char* const str) { Debug::writeLog(LIBGYRO, (SEVERITY)level, str); }
//Invoked by Lua (always LOG_VERBOSE, not silenced with the rest)
void DebugBase::luaPrint(const char *string, const SEVERITY lev) { Debug::writeLog(LUA, lev, string); }

// Central endpoint for all debug log calls
void DebugBase::writeLog(const DebugBase::SOURCE source, const DebugBase::SEVERITY level, const char *const str) {
    char buffer[GYRO_PRINTF_BUFFER_SIZE];
	//only log events that are as important or more important than the current level
	if(level < _level && source != LUA) return;

	buffer[0] = '\0';
    const int depth = gyAtomicGet(&_stackDepth);

	//Add tabs for each stack layer
    /*if(source != LUA)*/ for(unsigned int i = 0; i < depth; ++i) strcat(buffer, ELYSIAN_DEBUG_PREFIX_STACK);

	//Add prefixes for error types
	switch(level) {
    case LOG_ERROR:
        strcat(buffer, ELYSIAN_DEBUG_PREFIX_LOG_ERROR);
		break;
    case LOG_WARNING:
        strcat(buffer, ELYSIAN_DEBUG_PREFIX_LOG_WARNING);
		break;
    case LOG_VERBOSE: //no prefix
	default:
		break;
	}

	//Append string
    //trcat(buffer, str);

	//Append newline character to the end
//	size_t length = strlen(buffer);
    //strcat(buffer, "\n");

    if(!_logMutex->acquire()) return;

#if !defined(ANDROID) && GYRO_PLATFORM != GYRO_PLATFORM_iOS
	fputs(buffer, _dbgFile);
    fputs(str, _dbgFile);
    fputc('\n', _dbgFile);
	fflush(_dbgFile);
	if(source == LUA) {
		fputs(str, _luaDbgFile);
		fputc('\n', _luaDbgFile);
		fflush(_luaDbgFile);
	}
#endif

    //Only print to standard IO if we give a shit about this kind of output
    if(level >= _level) {
        gyPrintf(buffer); //use the "preferred" IO of each platform
        gyPrintf(str);
        gyPrintf("\n");
    }

    if(_advLogWrap != ADV_LOG_LOG_OFF) {
	AdventureLog::TEXT_TYPE advType;

	switch(source) {
        case LUA:
            advType = AdventureLog::TEXT_LUA_DEBUG;
            break;
        default:
            advType = AdventureLog::TEXT_NORMAL;
        }

        if(level == LOG_WARNING || level == LOG_ERROR) advType = AdventureLog::TEXT_LOG_ERROR;

        //Check to see if this kind of output is being wrapped to the Adventure Log
        if(level == LOG_WARNING || level == LOG_ERROR ||
                _advLogWrap == ADV_LOG_ALL ||
                (_advLogWrap == ADV_LOG_LUA && source == LUA))
        {
            //characters that the adventure log cannot display--shouldn't it handle this? Not debug? No shit.
            for(unsigned int i = 0; i < strlen(buffer); ++i) if(buffer[i] == '\n' || buffer[i] == '\t') buffer[i] = ' ';
            if(_engine) getGuiSys()->adventureLog.addLine(advType, str);
        }
    }

    _logMutex->release();

}

// Invoked for non AssetIO Engine logging
void DebugBase::printf(const Debug::SEVERITY level, const char *const string, ...) {
    char buffer[GYRO_PRINTF_BUFFER_SIZE];
	va_list argptr;
	//early-out
	if(level < _level) return;

	va_start(argptr,string);
    vsnprintf(buffer, GYRO_PRINTF_BUFFER_SIZE, string, argptr);
	va_end(argptr);
    Debug::writeLog(ENGINE, level, buffer);
}

//=====NONSTATIC MEMBERS=====

DebugBase::DebugBase(void):
	_nextFrame(true)
{
    gyAtomicSet(&_stackDepth, 0);
	//Register debug callback with LibGyro
    gySysDebugEnable(ELYSIAN_DEBUG_LIBGYRO_SEVERITY_DEFAULT, &_gyDbgCallbacks);
	//Register pointer to asset debug interface with AssetIO
#if !defined(ANDROID)
	_dbgFile = fopen(ELYSIAN_DEBUG_FILE, "w");
	_luaDbgFile = fopen(ELYSIAN_LUA_DEBUG_FILE, "w");
#endif
    //printf(LOG_VERBOSE, "-----Debug Initialized-----");

    _logMutex = new Mutex;
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
    printf(LOG_VERBOSE, "Executing global_init script [%s].", ELYSIAN_GLOBAL_INIT_LUA_PATH);
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

static EventCallback callback;

void DebugBase::processInput(Keyboard*const input) {
//	static unsigned size = 3;
//	static unsigned intensity = 200;
    static bool freeCam = false;
    static int i = 0;

    if(input->isTapped(GY_KEY_B)) {
#if 0
#if 0
        AsyncTask* task = new AsyncTask("BitchTask", [](void* data) {
            int val = (uintptr_t)data;
            dbgprintf("TROLOLO!!! %d", val);
                sleep(2);
            return -1;
        });
        int newI = i + 40;
        for(; i < newI; ++i)
            getThreadSystem()->enqueueAsyncTask(task, (void*)i);
#endif
#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
        PerformanceMonitor::print(getComputeSystem()->getPerfMonitor());
#endif
#else
        auto* line = ESToolkit::getInstance()->getLineChartWidget();

        auto* series = new QESTkUtils::FrameUpdatedValueSeries<elysian::PerformanceMonitor, qreal>(&_engine->bloomMonitor, "Bloom");
        line->addSeries(series);
        series = new QESTkUtils::FrameUpdatedValueSeries<elysian::PerformanceMonitor, qreal>(&_engine->updateParticleMonitor, "Particles");
        line->addSeries(series);
        series = new QESTkUtils::FrameUpdatedValueSeries<elysian::PerformanceMonitor, qreal>(&_engine->lightPassMonitor, "LightPass");
        line->addSeries(series);
        auto diz = [=](void) -> qreal{
            return _engine->getTimerSys()->getDeltaTime();
        };


        static qreal lol = 0.0f;
        auto* serz = new QESTkUtils::FrameUpdatedValueSeries<qreal, qreal>(&lol);
        lol += 100.0f;

        line->addSeries(serz);

    std::function<qreal(void)> funz = diz;
        //auto* serz = new QESTkUtils::FrameUpdatedValueSeries<decltype(funz), qreal, QESTkUtils::FrameUpdatedValueAdapter<qreal, decltype(funz)>>(funz, "Fucker");

        line->show();

#endif
    }

	//Camera zoom
    if(input->isPressed(GY_KEY_W) && !input->isPressed(GY_KEY_LALT)) {
        _engine->_cameraSystem->staticScale(0.95f);
    }
    if(input->isPressed(GY_KEY_S) && !input->isPressed(GY_KEY_LALT)){
        _engine->_cameraSystem->staticScale(1.05f);
	}

    if(input->isTapped(GY_KEY_0)) {

		CollisionSystem::RENDER_LAYER layer = _engine->_collisionSystem->cycleRenderLayer();
		switch(layer) {
		case CollisionSystem::RENDER_DISABLED:
            printf(LOG_VERBOSE, "Render Collision Layer - Disabled");
			break;
		case CollisionSystem::RENDER_LAYER_1:
            printf(LOG_VERBOSE, "Render Collision Layer - 1");
			break;
		case CollisionSystem::RENDER_LAYER_2:
            printf(LOG_VERBOSE, "Render Collision Layer - 2");
			break;
		case CollisionSystem::RENDER_LAYER_3:
            printf(LOG_VERBOSE, "Render Collision Layer - 3");
			break;
		case CollisionSystem::RENDER_COMBINED:
            printf(LOG_VERBOSE, "Render Collision Layer - Combined");
			break;
		default: break;
		}
	}
/*
	if(input.getTapped(GameInput::BUT_1)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(0);
        printf(LOG_VERBOSE, "Render Layer 1 - %s", (render)? "enabled" : "disabled");
	}
	if(input.getTapped(GameInput::BUT_2)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(1);
        printf(LOG_VERBOSE, "Render Layer 2 - %s", (render)? "enabled" : "disabled");
	}
	if(input.getTapped(GameInput::BUT_3)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(2);
        printf(LOG_VERBOSE, "Render Layer 3 - %s", (render)? "enabled" : "disabled");
    }*/
    if(input->isTapped(GY_KEY_4)) {
		bool render = _engine->_mapSystem->toggleRenderLayer(3);
        printf(LOG_VERBOSE, "Render Layer 4 - %s", (render)? "enabled" : "disabled");
	}

	//Rendering layers
    if(input->isTapped(GY_KEY_5)) {
        static bool isEnabled = true;
        if(isEnabled) {
            //_engine->_guiSystem->adventureLog.setEnabled(false);
            _engine->_guiSystem->fadeFgLayer(1.0f, false);
            printf(LOG_VERBOSE, "Disable GUI Layer");
            isEnabled = false;
		} else {
            //_engine->_guiSystem->adventureLog.setEnabled(true);
            _engine->_guiSystem->fadeFgLayer(1.0f, true);
            printf(LOG_VERBOSE, "Enable GUI Layer");
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

    //if(input.getPinch() != 1.0f) _engine->_cameraSystem->setStaticScale(input.getPinch());

	//EVERYTHING BEYOND THIS POINT IS IN FALCO'S DEBUG KEYSPACE!!!!
    if(!input->isPressed(GY_KEY_LALT)) return;

    if(input->isPressed(GY_KEY_W) && freeCam){

        _engine->_cameraSystem->translate(GYVector3{0.0f, -3.0f, 0.0f});
    }
    if(input->isPressed(GY_KEY_S) && freeCam){
        _engine->_cameraSystem->translate(GYVector3{0.0f, 3.0f, 0.0f});
    }
    if(input->isPressed(GY_KEY_A) && freeCam){
        _engine->_cameraSystem->translate(GYVector3{-3.0f, 0.0f, 0.0f});
    }
    if(input->isPressed(GY_KEY_D) && freeCam){
        _engine->_cameraSystem->translate(GYVector3{3.0f, 0.0f, 0.0f});
    }

    if(input->isPressed(GY_KEY_Q) && !freeCam){
        _engine->_cameraSystem->getCurrentState()->setOrient(0.0f, 0.0f, 1.0f);
        _engine->_cameraSystem->getCurrentState()->rotate(0.1f*60.0f*getTimerSys()->getDeltaTime());
	}
    if(input->isPressed(GY_KEY_A) && !freeCam){
                _engine->_cameraSystem->getCurrentState()->setOrient(0.0f, 0.0f, 1.0f);
        _engine->_cameraSystem->getCurrentState()->rotate(-0.1f*60.0f*getTimerSys()->getDeltaTime());
    }

    if(input->isPressed(GY_KEY_F) && !freeCam) {
        _engine->_cameraSystem->getCurrentState()->setOrient(1.0f, 0.0f, 0.0f);
        _engine->_cameraSystem->getCurrentState()->rotate(-0.1f*60.0f*getTimerSys()->getDeltaTime());
	}

    if(input->isPressed(GY_KEY_D) && !freeCam) {
        _engine->_cameraSystem->getCurrentState()->setOrient(1.0f, 0.0f, 0.0f);
        _engine->_cameraSystem->getCurrentState()->rotate(0.1f*60.0f*getTimerSys()->getDeltaTime());
	}

    if(input->isTapped(GY_KEY_Z)) { //Reset Transform
		_engine->_cameraSystem->setStaticScale(1.0f);
        //_engine->_cameraSystem->setOrientZ(0.0f);
        //_engine->_cameraSystem->setOrientX(0.0f);
	}

    if(input->isTapped(GY_KEY_C)) {
		static bool enabled = true;
		enabled = !enabled;
		_engine->_collisionSystem->setEnabled(enabled);
        if(enabled) Debug::printf(Debug::LOG_VERBOSE, "Collision Enabled");
        else Debug::printf(Debug::LOG_VERBOSE, "Collision Disabled");

	}
/*
    if(input->isTapped(GY_KEY_B)) {
        if(_engine->_collisionSystem->getBroadPhase()) {
            _engine->_collisionSystem->setBroadPhase(false);
            Debug::printf(Debug::LOG_VERBOSE, "Sort and Sweep Disabled!");
        } else {
            _engine->_collisionSystem->setBroadPhase(true);
            Debug::printf(Debug::LOG_VERBOSE, "Sort and Sweep Enabled!");
        }
    }
*/
    if(input->isTapped(GY_KEY_L)) {
		ADV_LOG_WRAP newWrap = (_advLogWrap < ADV_LOG_MAX-1)? (ADV_LOG_WRAP)(_advLogWrap+1) : (ADV_LOG_WRAP)0;
        if(newWrap == ADV_LOG_ALL) Debug::printf(Debug::LOG_VERBOSE, "Adventure Log Wrapping Engine Debug");
        else if(newWrap == ADV_LOG_LUA) Debug::printf(Debug::LOG_VERBOSE, "Adventure Log Wrapping Lua only");
        else Debug::printf(Debug::LOG_VERBOSE, "Adventure Log Wrapping None");
		_advLogWrap = newWrap;
	}

    if(input->isTapped(GY_KEY_G)) {
#if 1
		static bool enabled = true;
		enabled = !enabled;
		getRigidBodySys()->setGravityEnabled(enabled);
        Debug::printf(Debug::LOG_VERBOSE, "Gravity %s.", (enabled)? "enabled" : "disabled");
#else
        bool usingGPU = _engine->_collisionSystem->_sortSweepPool.getUseGPU();

        _engine->_collisionSystem->_sortSweepPool.setUseGPU(!usingGPU);
        dbgprintf("Using GPU - %d", !usingGPU);

        _engine->_timerSystem->printTaskStats();
#endif
	}

    if(input->isTapped(GY_KEY_R)) {
        //freeCam = !freeCam;
        //static Entity* julien = getCurrentArea()->findEntity("Julien");
#if 1
        static bool renderDebug = false;
        renderDebug = !renderDebug;
		getLightSys()->setRenderDebugEnabled(renderDebug);
        printf(LOG_VERBOSE, "LightSystem::renderShadowVolumes = %d", renderDebug);
#else
        if(freeCam) getCameraSys()->setFollowEntity(nullptr);
        else getCameraSys()->setFollowEntity(julien);

#endif
	}


//	static float timeSpeed = 60.0f;

    if(input->isTapped(GY_KEY_N)) {
		static bool sunEnabled = true;
		sunEnabled = !sunEnabled;
        //getDaySys()->setSunEnabled(sunEnabled);
	}
    if(input->isPressed(GY_KEY_M)) {
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

    if(input->isTapped(GY_KEY_FORWARDSLASH)) {
		LuaManager::dumpStats();
	}

	//Elevate player
    if(input->isPressed(GY_KEY_Y)) {
	/*	Dialog* dialog = new Dialog;
		auto* screen = dialog->addTextScreen("WTF!? A wild Rattata appeared!");
		screen->setNextIndex(-1);
		getDialogSys()->startDialog(dialog);*/

		Entity* leader = _engine->_partySystem->getPartyLeader();
		if(leader) {
			unsigned elev = leader->getElev();
            elev = (elev < ELYSIAN_TILE_ENTRY_ELEV_MAX)? elev+1 : 0;
			leader->setElev(elev);
            printf(LOG_VERBOSE, "Player.elevation - %d", elev);
		}
	}
}

#define ARG_LIMIT 20
bool DebugBase::parseArguments(void) {
//#if 0
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
        printf(LOG_VERBOSE, "Debug::parseArguments(void) - initializing from '%s'", ELYSIAN_DEBUG_ARG_FILE);
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
            printf(LOG_VERBOSE, "argv[%d] - %s", argc + 1, argv[argc + 1]);
			bufptr += strlen(argBuff[argc]) + 1;
			++argc;
		}
		free(buffer);
		gyFileClose(&file);
        Debug::pop();
		//Recursively reinvoke argument-based initialization
		return parseArguments(argc+1, argv);

    } else {
        //printf(LOG_WARNING, "gamma_args.txt not found! Shitting pants!");
    }
    //#endif
	return false;
}

bool DebugBase::parseArguments(int argc, char *argv[]) {
	if(argc <= 1) return parseArguments(); //default init

    printf(LOG_VERBOSE, "Debug::parseArguments(%d, char *argv[])", argc);
    Debug::push();
	for(unsigned int i = 1; i < (unsigned int)argc; ++i) {
		if(argv[i][0] != '-') {
            printf(LOG_WARNING, "! - Unrecognized command-line argument: %s", argv[i]);
			continue;
		}

		//Parse all command-line arguments and store them within internal structure
		switch(argv[i][1]) {
			case 'l':
				++i;
                strcpy(_cmdLineArgs.level, argv[i]);
                //sprintf(_cmdLineArgs.level, "/assets/%s", argv[i]);
				break;
			case 'a':
				++i;
                strcpy(_cmdLineArgs.area, argv[i]);
                //sprintf(_cmdLineArgs.area, "/assets/%s", argv[i]);
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
                printf(LOG_WARNING, "! - Unrecognized command-line argument: %s", argv[i]);
		}
	}
    Debug::pop();
	return true;
}


bool DebugBase::initializeEngine(void) {
    if(_cmdLineArgs.level[0]) _engine->_levelSystem->switchLevel(_cmdLineArgs.level);
    if(_cmdLineArgs.area[0]) _engine->_levelSystem->switchArea(_cmdLineArgs.area);
	return true;
}

}
