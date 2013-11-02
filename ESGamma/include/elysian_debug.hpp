#ifndef ELYSIAN_DEBUG_HPP
#define ELYSIAN_DEBUG_HPP

#include <fstream>
#include <cstring>
#include <gyro.h>
#include "elysian_asset.hpp"
#include "elysian_system.hpp"

#define ELYSIAN_DEBUG_FILE					"gamma_debug.txt"
#define ELYSIAN_LUA_DEBUG_FILE				"lua_debug.txt"
#define ELYSIAN_DEBUG_ARG_FILE				"gamma_args.txt"
#define ELYSIAN_DEBUG_LOG_LEVEL_DEFAULT		Debug::VERBOSE
#define ELYSIAN_DEBUG_LIBGYRO_LEVEL_DEFAULT GY_DEBUG_VERBOSE
#define ELYSIAN_DEBUG_LEVEL_DEFAULT			"level/Loren_Beta2"
#define ELYSIAN_DEBUG_AREA_DEFAULT			"level/Loren_Beta2/area/Uptown"

using namespace std;

namespace elysian {

	class Engine;
	class GameInput;
	class Entity;

	class Debug: public System {
    public:
        enum SOURCE { LIBGYRO, ENGINE, ASSETIO, LUA };
		enum LEVEL { VERBOSE, CRITICAL, FATAL, OFF };

	private: 
		//Taken from elysian_asset.hpp cmd-line arg list
		struct CmdLineArgs {
			char level[100];
			char area[100];
			int x, y;
			int resolution;

			CmdLineArgs(void):
				x(0), y(0), resolution(0)
				{
					strcpy(level, ELYSIAN_DEBUG_LEVEL_DEFAULT);
					strcpy(area, ELYSIAN_DEBUG_AREA_DEFAULT);
				}
		}_cmdLineArgs;
        
		static unsigned int _stackDepth;
		static bool _advLogWrap;
		static ofstream _dbgFile;
		static ofstream _luaDbgFile;
		static const char _fileName[];
        bool _nextFrame;

        //Centralized Debug Print/Log Mechanism
        static void writeLog(const SOURCE source, const LEVEL level, const char* const str);
        //Invoked by LibGyro (callback function)
        static void libGyroPrintf(const GY_DEBUG_LEVEL level, const char* const str) { writeLog(LIBGYRO, (LEVEL)level, str); }
        
	public:
        //Invoked by Engine
		static void printf(const LEVEL level, const char *const string, ...); 

		static void push(void) { ++_stackDepth; }
		static void pop(void) { --_stackDepth; }

        //Invoked by Lua (always VERBOSE, not silenced with the rest)
		static void luaPrint(const char *string) { writeLog(LUA, VERBOSE, string); }

	private:
        //Invoked by AssetIO 
		class AssetDebugStack: public Asset::DebugStack {
			public:
				void push(void) { Debug::push(); }
				void pop(void) { Debug::pop(); }
				void printf(const Asset::LOG_LEVEL level, const char* const str) {
					writeLog(ASSETIO, (LEVEL)level, str);
				}
		} _assetDebug;

		static Engine* _engine;
		static LEVEL _level;

		Debug(void);
	public:

		//Yes, bitch. It's a fucking singleton! Our one and only.
		static Debug &getInstance(void) {
			static Debug instance;
			return instance;
		}

		static void setAdvLogWrap(const bool val=true) { _advLogWrap = val; } 
		static void setEngine(Engine &eng) { _engine = &eng; }
		static void setLevel(const LEVEL level) { _level = level; }

		~Debug(void);

		void update(void);
		void processInput(const GameInput &input);
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


