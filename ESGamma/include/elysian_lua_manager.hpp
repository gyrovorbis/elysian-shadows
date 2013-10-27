#ifndef ELYSIAN_LUA_MANAGER_HPP
#define ELYSIAN_LUA_MANAGER_HPP

#include <map>
#include <oolua.h>
#include "elysian_stl_algorithm.hpp"
#include "elysian_lua_script.hpp"

#define LUA_ERROR_FILE	"lua_error.txt"

#if GYRO_PLATOFRM == GYRO_PLATFORM_x86
#	define ELYSIAN_LUA_DEBUG true
#else
#	define ELYSIAN_LUA_DEBUG false
#endif

namespace elysian {
	
	class LuaManager {
	private:

		//RegisterWrappers()?
		//OpenStdLibs()?
		
		static std::map<const char *, LuaScript *, ltstr> _scripts;
		static FILE *_errorFile;
		static int _errorCode;

		static void errorprintf(const char *const string, ...);
		static void _registerWrappers(void);
	
	protected:
		LuaManager(void);
		//static lua_State *_mainState;
		static OOLUA::Script *_mainState;

		static void showError(void);
		static int getErrorCode(void);
		
		static void _protectedCall(int nargs, int nresults, LuaScript *script);
		static void _stackDump(void);
		static int _traceBack (lua_State *L);
		static LuaScript*			_createScriptFromExisting(const char* const file);
	public:

		static bool initialize(void);
		static bool uninitialize(void);
		
		static bool					runString(const char *const string);
		static LuaScript*			loadScript(const char *const file);
		static LuaBehaviorScript*	loadBehaviorScript(const char *const file);
		static bool					unloadBehaviorScript(LuaBehaviorScript &script);
		static bool					unloadScript(LuaScript &script);
		static bool runScriptSandbox(const char *const file, LuaScript* const script=nullptr);

	};
	
	inline void LuaManager::_protectedCall(int nargs, int nresults, LuaScript *script) {
#ifdef ELYSIAN_LUA_DEBUG

		lua_pushcfunction(*_mainState, &LuaManager::_traceBack);
		lua_insert(*_mainState, 1);
		if((_errorCode = lua_pcall(*_mainState, nargs, nresults, 1)) != 0) {
			showError();
            if(script) script->setError();
			_stackDump();
		}
#else
		lua_call(*_mainState, nargs, nresults);
#endif
	}
	
	
}

#endif
