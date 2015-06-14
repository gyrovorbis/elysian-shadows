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
#define ELYSIAN_DEBUG_LOG_SEVERITY_DEFAULT		Debug::DBG_VERBOSE
#define ELYSIAN_DEBUG_LIBGYRO_SEVERITY_DEFAULT GY_DEBUG_DBG_VERBOSE
#define ELYSIAN_DEBUG_SEVERITY_DEFAULT			ASSETS_VROOT "/level/Loren_Beta2"
#define ELYSIAN_DEBUG_AREA_DEFAULT			ASSETS_VROOT "/level/Loren_Beta2/area/Uptown"
#define ELYSIAN_DEBUG_PREFIX_STACK          "\t"
#define ELYSIAN_DEBUG_PREFIX_DBG_ERROR          "X - "
#define ELYSIAN_DEBUG_PREFIX_DBG_WARNING       "! - "

#define ELYSIAN_DEBUG_LOG_PRINTF(level, ...) do { \
                                                Debug::printf(Debug::level, __VA_ARGS__); \
                                            } while(0)

#define dbgprintf(...)                      ELYSIAN_DEBUG_LOG_PRINTF(DBG_VERBOSE,   __VA_ARGS__)
#define warnprintf(...)                     ELYSIAN_DEBUG_LOG_PRINTF(DBG_WARNING,  __VA_ARGS__)
#define errprintf(...)                      ELYSIAN_DEBUG_LOG_PRINTF(DBG_ERROR,     __VA_ARGS__)

#define dbgpush()                           Debug::push()
#define dbgpop()                            Debug::pop()

using namespace std;

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {

    class Engine;
    class GameInput;

    struct DbgEvent {
        unsigned                depth = 0;
                                DbgEvent(void) = default;
                                DbgEvent(const char*const string, ...);
                                ~DbgEvent(void);
    };


    class DebugBase: public System {
	public:
        enum SOURCE             { LIBGYRO, ENGINE, LUA };
        enum SEVERITY           { DBG_VERBOSE, DBG_WARNING, DBG_ERROR, DBG_OFF };
        enum ADV_LOG_WRAP       { ADV_LOG_ALL, ADV_LOG_LUA, ADV_LOG_DBG_OFF, ADV_LOG_MAX };

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

        static unsigned int		_stackDepth;
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
        //Invoked by Lua (always DBG_VERBOSE, not silenced with the rest)
        static void             luaPrint(const char *string, const SEVERITY lev = DBG_VERBOSE);

        static void             push(void);
        static void             pop(const unsigned count=1);
        static unsigned         getStackDepth(void);

    protected:

        const static
        GYDebugCallbacks        _gyDbgCallbacks;

        static Engine*          _engine;
        static SEVERITY            _level;

                                DebugBase(void);
    public:
                                ~DebugBase(void);
        //Yes, bitch. It's a fucking singleton! Our one and only.
        static DebugBase&       getInstance(void);

        static void             initialize(void);
        static void             setEngine(Engine &eng);
        static void             setLevel(const SEVERITY level);

        void                    update(void);
        void                    processInput(GameInput &input);
        void                    render(void);
        void                    renderOverlay(void);
        bool                    sandboxInit(void);

        bool                    initializeEngine(void);
        bool                    parseArguments(void);
        bool                    parseArguments(int argc, char *argv[]);

        void                    setNextFrame(const bool val=true);
        bool                    getNextFrame(void) const;

        const CmdLineArgs&      getCmdLineArgs(void) const;
	};







    //=========INLINEZ=======
    inline void DebugBase::push(void) { ++_stackDepth; }
    inline void DebugBase::pop(const unsigned count) { _stackDepth -= count; }
    inline unsigned DebugBase::getStackDepth(void) { return _stackDepth; }
    inline DebugBase& DebugBase::getInstance(void) {
        static DebugBase instance;
        return instance;
    }
    inline void DebugBase::setEngine(Engine &eng) { _engine = &eng; }
    inline void DebugBase::setLevel(const SEVERITY level) { _level = level; }
    inline const DebugBase::CmdLineArgs& DebugBase::getCmdLineArgs(void) const { return _cmdLineArgs; }
    inline void DebugBase::setNextFrame(const bool val) { _nextFrame = val; }
    inline bool DebugBase::getNextFrame(void) const { return _nextFrame; }
}

#endif


