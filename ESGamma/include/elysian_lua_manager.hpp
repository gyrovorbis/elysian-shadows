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

		static bool unloadScript(LuaScript &script);
		static void showError(void);
		static int getErrorCode(void);
		
		static void _protectedCall(int nargs, int nresults, LuaScript *script);
	public:

		static bool initialize(void);
		static bool uninitialize(void);
		
		static bool runString(const char *const string);
		static LuaScript *loadScript(const char *const file, bool save=true);

	};
	
	inline void LuaManager::_protectedCall(int nargs, int nresults, LuaScript *script) {
#ifdef ELYSIAN_LUA_DEBUG
		if((_errorCode = lua_pcall(*_mainState, nargs, nresults, 0)) != 0) {
			showError();
            if(script) script->setError();
		}
#else
		lua_call(*_mainState, nargs, nresults);
#endif
	}
	
	
}

#endif
