#include "elysian_lua_manager.hpp"
#include "elysian_lua_script.hpp"
#include "elysian_debug.hpp"

namespace elysian {

//----STATIC----

std::map<const char*, LuaScript*, ltstr> LuaManager::_scripts;
//lua_State *LuaManager::_mainState = NULL;
OOLUA::Script *LuaManager::_mainState = NULL;
FILE *LuaManager::_errorFile = NULL;
int LuaManager::_errorCode = 0;

LuaManager::LuaManager(void) {}

//Dumps to debug log and LUA_ERROR_FILE
void LuaManager::errorprintf(const char *const string, ...) {
	char buffer[ELYSIAN_DEBUG_BUFFER_SIZE]; 
	va_list argptr;  

	va_start(argptr,string); 
	vsprintf(buffer, string, argptr); 
	va_end(argptr); 

	fprintf(_errorFile, "%s\n", buffer);
	fflush(_errorFile);
	Debug::printf(Debug::CRITICAL, "%s", buffer);
}
	
	
void LuaManager::showError(void) {
	if(_errorCode == 0) return;
	const char *errorMsg = luaL_checkstring(*_mainState, -1);
	errorprintf("%s", (errorMsg)? errorMsg : "Lua error message not available");
}

bool LuaManager::runString(const char *const string) {
	if((_errorCode = luaL_dostring(*_mainState, string)) != 0) {
		showError();
		return false;
	}
	return true;
}
	
	
LuaScript *LuaManager::loadScript(const char *const file, bool save) {
	//Check if script has already been loaded, and return its globals table
	
	std::map<const char *, LuaScript*, ltstr>::iterator it = _scripts.find(file);
	if(it != _scripts.end()) {
		//already allocated
		Debug::printf(Debug::VERBOSE, "Already loaded - Creating new instance");
		it->second->incRefCount();
		return it->second;
	}
	
	//Shit hasn't been loaded yet
	LuaScript *script = (save)? new LuaScript(file) : NULL;
	int ref = 0;

	if((_errorCode = luaL_loadfile(*_mainState, file)) != 0) {
		showError();
		if(_errorCode == LUA_ERRFILE) {
			errorprintf("Could not load script file!");
		}
		if(script) script->setError();
		lua_remove(*_mainState, 1);					//take loaded chunk off of stack
	} else {
		
		lua_newtable(*_mainState);					//ENV
		lua_newtable(*_mainState);					//metatable
		lua_getglobal(*_mainState, "_G");			//globals table
		lua_setfield(*_mainState, -2, "__index");	//metatable[__index] = _G;
		lua_setmetatable(*_mainState, -2);			//setmetatable(ENV, metatable);
		lua_pushvalue(*_mainState, -1);				//duplicate ENV
		ref = luaL_ref(*_mainState,	LUA_REGISTRYINDEX);	//store reference to ENV in Lua registry 
		if(script) script->setEnvironRef(ref);		//and save key in LuaScript
		lua_setupvalue(*_mainState, 1, 1);			//loadfile.upvalue1 = ENV
		_protectedCall(0, 0, script);
	}

	if(script) {
		_scripts.insert(std::pair<const char *, LuaScript *>(script->getPath(), script));
	} else {
		luaL_unref(*_mainState, LUA_REGISTRYINDEX, ref);
	}

	return script;
}
	
bool LuaManager::unloadScript(LuaScript &script) {
	for(std::map<const char *, LuaScript *, ltstr>::iterator it = _scripts.begin();
		it != _scripts.end(); ++it)
	{
		if(&script == it->second) {
			it->second->decRefCount();
			if(!it->second->getRefCount()) {
				//remove reference to script's ENV from Lua registry
				luaL_unref(*_mainState, LUA_REGISTRYINDEX, it->second->getEnvironRef());
				delete it->second;
				_scripts.erase(it);
				_mainState->gc();
			}
			return true;
		}
	}
	return false;
}

bool LuaManager::initialize(void) {
	_errorFile = fopen("lua_error.txt", "w");
	Debug::printf(Debug::VERBOSE, "LuaManager::initialize() - Initializing main Lua state.");
    Debug::push();
    

	_mainState = new OOLUA::Script; 
	//_mainState = luaL_newstate();
	luaL_openlibs(*_mainState);

	_registerWrappers();
	
    Debug::pop();
	return true;
}

bool LuaManager::uninitialize(void) {
	delete _mainState;
	fclose(_errorFile);
	return true;
}

}