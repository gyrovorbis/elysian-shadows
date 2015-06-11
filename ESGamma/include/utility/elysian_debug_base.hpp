#ifndef ELYSIAN_DEBUG_BASE_HPP
#define ELYSIAN_DEBUG_BASE_HPP

#include <cstring>
#include <stdio.h>
#include <gyro_system_api.h>
#include <elysian_system.hpp>

#define ASSETS_VROOT						"/assets"

#define ELYSIAN_DEBUG_FILE					"gamma_debug.txt"
#define ELYSIAN_LUA_DEBUG_FILE				"lua_debug.txt"
#define ELYSIAN_DEBUG_ARG_FILE				ASSETS_VROOT "/gamma_args.txt"
#define ELYSIAN_GLOBAL_INIT_LUA_PATH		ASSETS_VROOT "/global_init.lua"
#define ELYSIAN_DEBUG_LOG_LEVEL_DEFAULT		Debug::VERBOSE
#define ELYSIAN_DEBUG_LIBGYRO_LEVEL_DEFAULT GY_DEBUG_VERBOSE
#define ELYSIAN_DEBUG_LEVEL_DEFAULT			ASSETS_VROOT "/level/Loren_Beta2"
#define ELYSIAN_DEBUG_AREA_DEFAULT			ASSETS_VROOT "/level/Loren_Beta2/area/Uptown"
#define ELYSIAN_DEBUG_PREFIX_STACK          "\t"
#define ELYSIAN_DEBUG_PREFIX_FATAL          "! - "
#define ELYSIAN_DEBUG_PREFIX_CRITICAL       "* - "

#define ELYSIAN_DEBUG_LOG_PRINTF(level, ...) do { \
                                                Debug::printf(Debug::level, __VA_ARGS__); \
                                            } while(0)

#define dbgprintf(...)                      ELYSIAN_DEBUG_LOG_PRINTF(VERBOSE,   __VA_ARGS__)
#define warnprintf(...)                     ELYSIAN_DEBUG_LOG_PRINTF(CRITICAL,  __VA_ARGS__)
#define errprintf(...)                      ELYSIAN_DEBUG_LOG_PRINTF(FATAL,     __VA_ARGS__)

#define dbgpush()                           Debug::push()
#define dbgpop()                            Debug::pop()

using namespace std;

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {

    class Engine;
    class GameInput;

    struct DbgTest {
        bool    success = true;

                DbgTest(const char*const name);
                ~DbgTest(void);
    };

    class DebugBase: public System {
	public:
        enum SOURCE { LIBGYRO, ENGINE, ASSETIO, LUA };
		enum LEVEL { VERBOSE, CRITICAL, FATAL, OFF };
		enum ADV_LOG_WRAP { ADV_LOG_ALL, ADV_LOG_LUA, ADV_LOG_OFF, ADV_LOG_MAX };

    protected:
        //Taken from elysian_asset.hpp cmd-line arg list
        struct CmdLineArgs {
            char level[100];	//	= ELYSIAN_DEBUG_LEVEL_DEFAULT;
            char area[100];		//	= ELYSIAN_DEBUG_AREA_DEFAULT;	/* You can't do this crap in Linux. */
            int x			= 0;
            int y			= 0;
            int resolution	= 0;

            CmdLineArgs(void)
                {
                //	strcpy(level, ELYSIAN_DEBUG_LEVEL_DEFAULT);
                //	strcpy(area, ELYSIAN_DEBUG_AREA_DEFAULT);
                }
        } _cmdLineArgs;

        static unsigned int		_stackDepth;
        static ADV_LOG_WRAP		_advLogWrap;
        static FILE*			_dbgFile;
        static FILE*			_luaDbgFile;
        static const char		_fileName[];
        bool					_nextFrame;

        //Centralized Debug Print/Log Mechanism
        static void writeLog(const SOURCE source, const LEVEL level, const char* const str);
        //Invoked by LibGyro (callback function)
        static void libGyroPrintf(const GY_DEBUG_LEVEL level, const char* const str);

    public:
        //Invoked by Engine
        static void printf(const LEVEL level, const char *const string, ...);
        //Invoked by Lua (always VERBOSE, not silenced with the rest)
        static void luaPrint(const char *string, const LEVEL lev = VERBOSE);

        static void push(void) { ++_stackDepth; }
        static void pop(const bool /*success=true*/) { --_stackDepth; }



	private:

        static Engine* _engine;
        static LEVEL _level;
    protected:
        DebugBase(void);
    public:

        //Yes, bitch. It's a fucking singleton! Our one and only.
        static DebugBase& getInstance(void) {
            static DebugBase instance;
            return instance;
        }

        static void initialize(void);

        //static void setAdvLogWrap(const bool val=true) { _advLogWrap = val; }
        static void setEngine(Engine &eng) { _engine = &eng; }
        static void setLevel(const LEVEL level) { _level = level; }

        ~DebugBase(void);

        void update(void);
        void processInput(GameInput &input);
        void render(void);
        void renderOverlay(void);
        bool sandboxInit(void);

        bool initializeEngine(void);
        bool parseArguments(void);
        bool parseArguments(int argc, char *argv[]);

        void setNextFrame(const bool val=true) { _nextFrame = val; }
        bool getNextFrame(void) const { return _nextFrame; }

		const CmdLineArgs& getCmdLineArgs(void) const { return _cmdLineArgs; }
	};



}

#endif


