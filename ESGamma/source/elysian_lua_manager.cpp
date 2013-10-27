#include "elysian_lua_manager.hpp"
#include "elysian_lua_script.hpp"
#include "elysian_lua_wrappers.hpp"
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
	lua_pop(*_mainState, 1); //pop error string off the stack
}

bool LuaManager::runString(const char *const string) {
	if((_errorCode = luaL_dostring(*_mainState, string)) != 0) {
		showError();
		return false;
	}
	return true;
}


int LuaManager::_traceBack (lua_State *L) {
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
	lua_getglobal(L, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

void LuaManager::_stackDump (void) {
	int i;
	int top = lua_gettop(*_mainState);

	Debug::printf(Debug::CRITICAL, "LUA STACK DUMP:");
	Debug::push();

	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(*_mainState, i);
		switch (t) {
			case LUA_TSTRING:  /* strings */
				Debug::printf(Debug::CRITICAL, "[%d] = '%s'", i, lua_tostring(*_mainState, i));
				break;

			case LUA_TBOOLEAN:  /* booleans */
				Debug::printf(Debug::CRITICAL, "[%d] = %s", i, lua_toboolean(*_mainState, i) ? "true" : "false");
				break;

			case LUA_TNUMBER:  /* numbers */
				Debug::printf(Debug::CRITICAL, "[%d] = %g", i, lua_tonumber(*_mainState, i));
				break;

			default:  /* other values */
				Debug::printf(Debug::CRITICAL, "[%d] = %s", i, lua_typename(*_mainState, t));
				break;
		}
	}

	Debug::pop();
}

inline LuaScript *LuaManager::_createScriptFromExisting(const char* const file) {
	const auto it = _scripts.find(file);
	if(it != _scripts.end()) {
		//already allocated
		Debug::printf(Debug::VERBOSE, "Script %s already loaded - Creating new instance", file);
		it->second->incRefCount();
		return it->second;
	}
	return nullptr;
}

LuaBehaviorScript* LuaManager::loadBehaviorScript(const char* const file) {

	LuaBehaviorScript* behav = static_cast<LuaBehaviorScript*>(_createScriptFromExisting(file));
	if(behav) return behav;

	behav = new LuaBehaviorScript(file);

	if(!runScriptSandbox(file, behav)) {
		delete behav;
		return nullptr;
	}

	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, behav->getEnvironRef());	//push _ENV
	lua_getfield(*_mainState, -1, "setScriptAttrib");						//get _ENV.setScriptAttrib()
	lua_remove(*_mainState, -2);											//remove _ENV
	OOLUA::push(*_mainState, behav->getAttrib());							//push attribTable
	_protectedCall(1, 0, behav);											//call _ENV.setScriptAttrib(attribTable)

	if(behav->getAttrib()->hasParent()) {
		behav->setParent(loadBehaviorScript(behav->getAttrib()->getParentPath()));

		//Make _ENV inherit from parent._ENV
		lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, behav->getEnvironRef());					//_ENV
		lua_newtable(*_mainState);																//metatable
		lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, behav->getParent()->getEnvironRef());		//parent._ENV
		lua_setfield(*_mainState, -2, "__index");												//metatable[__index] = parent._ENV;
		lua_setmetatable(*_mainState, -2);														//setmetatable(_ENV, metatable);
	}

	if(behav->getAttrib()->getSupportedTriggers() & Behavior::ON_SCRIPT_CREATE) {
		lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, behav->getEnvironRef());	//push _ENV
		lua_getfield(*_mainState, -1, "onScriptCreate");						//get _ENV.onScriptCreate()
		lua_remove(*_mainState, -2);											//remove _ENV
		_protectedCall(0, 0, behav);											//call _ENV.onScriptCreate()
	}

	_scripts.insert(std::pair<const char *, LuaScript *>(behav->getPath(), behav));
	return behav;

}

LuaScript* LuaManager::loadScript(const char * const file) {
	LuaScript* script = _createScriptFromExisting(file);
	if(script) return script;

	script = new LuaScript(file);

	if(!runScriptSandbox(file, script)) {
		delete script;
		return nullptr;
	}

	_scripts.insert(std::pair<const char *, LuaScript *>(script->getPath(), script));
	return script;
}

	
bool LuaManager::runScriptSandbox(const char *const file, LuaScript* const script) {
	//Check if script has already been loaded, and return its globals table
	int ref = LUA_NOREF;

	if((_errorCode = luaL_loadfile(*_mainState, file)) != 0) {
		showError();
		if(_errorCode == LUA_ERRFILE) {
			errorprintf("Could not load script file!");
		}
		if(script) script->setError();
		lua_remove(*_mainState, 1);					//take loaded chunk off of stack
		return false;

	} else {
		
		lua_newtable(*_mainState);							//ENV
		lua_newtable(*_mainState);							//metatable
		lua_getglobal(*_mainState, "_G");					//globals table
		lua_setfield(*_mainState, -2, "__index");			//metatable[__index] = _G;
		lua_setmetatable(*_mainState, -2);					//setmetatable(ENV, metatable);
		if(script) {
			lua_pushvalue(*_mainState, -1);					//duplicate ENV
			ref = luaL_ref(*_mainState,	LUA_REGISTRYINDEX);	//store reference to ENV in Lua registry
			script->setEnvironRef(ref);						//and save key in LuaScript
		}
		lua_setupvalue(*_mainState, -2, 1);
		_protectedCall(0, 0, script);
	}

	return true;
}

bool LuaManager::unloadBehaviorScript(LuaBehaviorScript &script) {
	auto* it = &script;
	auto* nextIt = it;
	bool success = true;

	//unload scripts from child to parent,
	//by traversing up the hierarchy
	while(it) {
		nextIt = it->getParent();
		success &= unloadScript(*it);
		it = nextIt;
	}

	return success;

}
	
bool LuaManager::unloadScript(LuaScript &script) {
	for(auto it = _scripts.begin();
		it != _scripts.end();
		++it)
	{
		//We found the bitch in our cache
		if(&script == it->second) {
			//Decrease its refCount
			it->second->decRefCount();
			//No more references, delete the fucker
			if(!it->second->getRefCount()) {
				//Behaviors can clean themselves up with the onScriptDestroy() trigger
				if(it->second->getType() == LuaScript::TYPE_BEHAVIOR) {
					auto behav = static_cast<LuaBehaviorScript*>(it->second);
					if(behav->getAttrib()->getSupportedTriggers() & Behavior::ON_SCRIPT_DESTROY) {
						lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, behav->getEnvironRef());	//push _ENV
						lua_getfield(*_mainState, -1, "onScriptDestroy");							//get _ENV.onScriptDestroy()
						lua_remove(*_mainState, -2);												//remove _ENV
						_protectedCall(1, 0, behav);												//call _ENV.onScriptDestroy()
					}
				}
				//remove reference to script's ENV from Lua registry
				luaL_unref(*_mainState, LUA_REGISTRYINDEX, it->second->getEnvironRef());
				//delete script
				delete it->second;
				//remove script from cache
				_scripts.erase(it);
				//do a gc sweep
				_mainState->gc();
			}
			return true;
		}
	}
	return false;
}

bool LuaManager::initialize(void) {
	Debug::printf(Debug::VERBOSE, "LuaManager::initialize() - Initializing main Lua state.");
    Debug::push();
    
	_errorFile = fopen("lua_error.txt", "w");
	_mainState = new OOLUA::Script; 
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
