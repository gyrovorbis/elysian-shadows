#ifndef ELYSIAN_DEBUG_BASE_HPP
#define ELYSIAN_DEBUG_BASE_HPP

#include <cstring>
#include <stdio.h>
#include <libGyro/gyro_system_api.h>
#include <ESGamma/system/elysian_system.hpp>
#include <libGyro/gyro_thread_api.h>

#define ASSETS_VROOT                            "/assets"

#define ELYSIAN_DEBUG_FILE                      "engine_debug.txt"
#define ELYSIAN_LUA_DEBUG_FILE                  "lua_debug.txt"
#define ELYSIAN_DEBUG_ARG_FILE_NAME             "gamma_args.txt"
#define ELYSIAN_DEBUG_ARG_FILE                  ASSETS_VROOT "/" ELYSIAN_DEBUG_ARG_FILE_NAME
#define ELYSIAN_GLOBAL_INIT_LUA_PATH            ASSETS_VROOT "/scripts/global_init.lua"
#define ELYSIAN_DEBUG_LOG_SEVERITY_DEFAULT		Debug::LOG_VERBOSE
#define ELYSIAN_DEBUG_LIBGYRO_SEVERITY_DEFAULT  GY_DEBUG_VERBOSE
#define ELYSIAN_DEBUG_LEVEL_DEFAULT             ASSETS_VROOT "/level/bullshit/caves_test"
#define ELYSIAN_DEBUG_AREA_DEFAULT              ELYSIAN_DEBUG_LEVEL_DEFAULT "/area/caves_test01"
#define ELYSIAN_DEBUG_PREFIX_STACK              "\t"
#define ELYSIAN_DEBUG_PREFIX_LOG_ERROR          "X - "
#define ELYSIAN_DEBUG_PREFIX_LOG_WARNING        "! - "

#define ELYSIAN_DEBUG_LOG_PRINTF(level, ...)    do { \
                                                    Debug::printf(Debug::level, __VA_ARGS__); \
                                                } while(0)

#define dbgprintf(...)                          ELYSIAN_DEBUG_LOG_PRINTF(LOG_VERBOSE,   __VA_ARGS__)
#define warnprintf(...)                         ELYSIAN_DEBUG_LOG_PRINTF(LOG_WARNING,  __VA_ARGS__)
#define errprintf(...)                          ELYSIAN_DEBUG_LOG_PRINTF(LOG_ERROR,     __VA_ARGS__)

#define dbgpush()                               Debug::push()
#define dbgpop()                                Debug::pop()

#define ELYSIAN_DEBUG_PRINTF_BUFFER_SIZE        GYRO_PRINTF_BUFFER_SIZE

using namespace std;

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {

    class Engine;
    class Keyboard;
    class Mutex;

    struct DbgEvent {
        unsigned                depth = 0;
                                DbgEvent(void) = default;
                                DbgEvent(const char*const string, ...);
                                ~DbgEvent(void);
    };


    class DebugBase: public System {
	public:
        enum SOURCE             { LIBGYRO, ENGINE, LUA };
        enum SEVERITY           { LOG_VERBOSE, LOG_WARNING, LOG_ERROR, LOG_OFF };
        enum ADV_LOG_WRAP       { ADV_LOG_ALL, ADV_LOG_LUA, ADV_LOG_LOG_OFF, ADV_LOG_MAX };

    protected:
        //Taken from elysian_asset.hpp cmd-line arg list
        struct CmdLineArgs {
            char                level[100]          = {0};
            char                area[100]           = {0};
            int                 x                   = 0;
            int                 y                   = 0;
            int                 resolution          = 0;
                                CmdLineArgs(void)   = default;
        }                       _cmdLineArgs;

        static Mutex*           _logMutex;
        static GYAtomic         _stackDepth;
        static ADV_LOG_WRAP		_advLogWrap;
        static FILE*			_dbgFile;
        static FILE*			_luaDbgFile;
        static const char		_fileName[];
        bool					_nextFrame;

        //Centralized Debug Print/Log Mechanism
        static void             writeLog(const SOURCE source, const SEVERITY level, const char* const str);
        //Invoked by LibGyro (callback function)
        static void             libGyroPrintf(const GY_DEBUG_SEVERITY level, const char* const str);

    public:
        //Invoked by Engine
        static void             printf(const SEVERITY level, const char *const string, ...);
        //Invoked by Lua (always LOG_VERBOSE, not silenced with the rest)
        static void             luaPrint(const char *string, const SEVERITY lev = LOG_VERBOSE);

        static void             push(void);
        static void             pop(const unsigned count=1);
        static unsigned         getStackDepth(void);
        template<typename F>
        static void             contextBlock(const char* header, F&& fn);

    protected:

        const static
        GYDebugCallbacks        _gyDbgCallbacks;

        static Engine*          _engine;
        static SEVERITY         _level;

                                DebugBase(void);
    public:
                                ~DebugBase(void);
        //Yes, bitch. It's a fucking singleton! Our one and only.
        static Debug&           getInstance(void);

        static void             initialize(void);
        static void             setEngine(Engine &eng);
        static void             setLevel(const SEVERITY level);

        void                    update(void);
        void                    processInput(Keyboard*const input);
        void                    render(void);
        void                    renderOverlay(void);
        bool                    sandboxInit(void);

        bool                    initializeEngine(void);
        bool                    parseArguments(void);
        bool                    parseArguments(int argc, char *argv[]);

        void                    setNextFrame(const bool val=true);
        bool                    getNextFrame(void) const;

        const CmdLineArgs&      getCmdLineArgs(void) const;

        void                    _guiWidgetsTest(void);
	};





    //=========INLINEZ=======
    inline void DebugBase::push(void) { gyAtomicAdd(&_stackDepth, 1); }
    inline void DebugBase::pop(const unsigned count) {
        const int depth = gyAtomicAdd(&_stackDepth, -count);
        GY_ASSERT((int)depth - (int)count >= 0);
    }
    template<typename F>
    inline void DebugBase::contextBlock(const char* header, F&& fn) {
        DbgEvent ev(header);
        (void)ev;
        fn();
    }
    inline unsigned DebugBase::getStackDepth(void) { return gyAtomicGet(&_stackDepth); }
    inline void DebugBase::setEngine(Engine &eng) { _engine = &eng; }
    inline void DebugBase::setLevel(const SEVERITY level) { _level = level; }
    inline const DebugBase::CmdLineArgs& DebugBase::getCmdLineArgs(void) const { return _cmdLineArgs; }
    inline void DebugBase::setNextFrame(const bool val) { _nextFrame = val; }
    inline bool DebugBase::getNextFrame(void) const { return _nextFrame; }
}

#endif


