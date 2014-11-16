#ifndef ELYSIAN_DEBUG_HPP
#define ELYSIAN_DEBUG_HPP

#include <cstring>
#include <stdio.h>
#include <gyro.h>
//#include <QDebug>
#include "elysian_asset.hpp"
#include "elysian_system.hpp"
#include "elysian_asset_typedefs.hpp"

#define ASSETS_VROOT						"/assets"

#define ELYSIAN_DEBUG_FILE					"gamma_debug.txt"
#define ELYSIAN_LUA_DEBUG_FILE				"lua_debug.txt"
#define ELYSIAN_DEBUG_ARG_FILE				ASSETS_VROOT "/gamma_args.txt"
#define ELYSIAN_GLOBAL_INIT_LUA_PATH		"../assets/global_init.lua"
#define ELYSIAN_DEBUG_LOG_LEVEL_DEFAULT		Debug::VERBOSE
#define ELYSIAN_DEBUG_LIBGYRO_LEVEL_DEFAULT GY_DEBUG_VERBOSE
#define ELYSIAN_DEBUG_LEVEL_DEFAULT			ASSETS_VROOT "/level/Loren_Beta2"
#define ELYSIAN_DEBUG_AREA_DEFAULT			ASSETS_VROOT "/level/Loren_Beta2/area/Uptown"

using namespace std;

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class Engine;
	class GameInput;

	class Debug: public System {
	public:
		enum SOURCE { LIBGYRO, ENGINE, ASSETIO, LUA };
		enum LEVEL { VERBOSE, CRITICAL, FATAL, OFF };
		enum ADV_LOG_WRAP { ADV_LOG_ALL, ADV_LOG_LUA, ADV_LOG_OFF, ADV_LOG_MAX };

	private:
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

		//static void setAdvLogWrap(const bool val=true) { _advLogWrap = val; }
		static void setEngine(Engine &eng) { _engine = &eng; }
		static void setLevel(const LEVEL level) { _level = level; }

		~Debug(void);

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

	class LightSource;
	extern LightSource* lightSrc;
	extern GYVector3 dirVec;

}

#endif


