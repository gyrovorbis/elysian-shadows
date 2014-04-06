#include "elysian_lua_manager.hpp"
#include "elysian_lua_script.hpp"
#include "elysian_lua_wrappers.hpp"
#include "elysian_debug.hpp"

namespace elysian {

//static members
std::map<const char*, LuaScript*, ltstr>	LuaManager::_scripts;
OOLUA::Script*								LuaManager::_mainState = nullptr;
int											LuaManager::_errorCode = 0;
const char									LuaManager::_rPrint[] =
	"function rPrint(s, l, i) -- recursive Print (structure, limit, indent)\n"
		"l = (l) or 100; i = i or \"\";	-- default item limit, indent string\n"
		"if (l<1) then print \"ERROR: Item limit reached.\"; return l-1 end;\n"
		"local ts = type(s);\n"
		"if (ts ~= \"table\") then print (i,ts,s); return l-1 end\n"
		"print (i,ts);           -- print \"table\"\n"
		"for k,v in pairs(s) do  -- print \"[KEY] VALUE\"\n"
			"l = rPrint(v, l, i..\"\t[\"..tostring(k)..\"]\")\n"
			"if (l < 0) then break end\n"
		"end\n"
		"return l\n"
	"end\n";

LuaManager::LuaManager(void) {}

//Dumps to debug log and LUA_ERROR_FILE
void LuaManager::_errorprintf(const char *const string, ...) {
	char buffer[ELYSIAN_DEBUG_BUFFER_SIZE]; 
	va_list argptr;  

	va_start(argptr,string); 
	vsprintf(buffer, string, argptr); 
	va_end(argptr); 

	Debug::luaPrint(buffer);
}
	
	
void LuaManager::_showError(void) {
	if(_errorCode == 0) return;
	const char *errorMsg = luaL_checkstring(*_mainState, -1);
	_errorprintf("**** LUA ERROR ****\n%s", (errorMsg)? errorMsg : "Lua error message not available");
	lua_pop(*_mainState, 1); //pop error string off the stack
}

void LuaManager::_rPrintByIndex(const int index, const unsigned depth, const char* name) {
	lua_getglobal(*_mainState, "rPrint");						//push function
	lua_pushvalue(*_mainState, index);							//push table to print
	if(depth) lua_pushnumber(*_mainState, depth);				//push depth if given
	else lua_pushnil(*_mainState);								//push nil for default depth if not
	lua_pushstring(*_mainState, name);							//push name of table
	lua_call(*_mainState, 3, 1);								//call rPrint
	lua_pop(*_mainState, 1);									//pop return value
}

void LuaManager::_rPrintByRef(const int ref, const unsigned depth, const char* name) {
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);			//push ref onto stack
	int index = lua_gettop(*_mainState);						//get index of ref
	_rPrintByIndex(index, depth, name);							//invoke by ref
	lua_pop(*_mainState, 1);									//pop ref from stack
}

bool LuaManager::runString(const char *const string) {
	if((_errorCode = luaL_dostring(*_mainState, string)) != 0) {
		_showError();
		return false;
	}
	return true;
}


int LuaManager::_traceBack(lua_State *L) {
	_errorCode = 1;

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

	_showError();
	lua_pop(*_mainState, 2); //remove debug table and original error from stack
	_runtimeStackDump();

	return 1;
}

void LuaManager::_cStackDump (void) {
	int i;
	int top = lua_gettop(*_mainState);
	char varDisplayString[200];

	Debug::luaPrint("**** LUA C STACK DUMP ****");
	Debug::push();

	for (i = 1; i <= top; ++i) {  /* repeat for each level */
		sprintf(varDisplayString, "_cStack[%d]", i);
		_rPrintByIndex(i, 0, varDisplayString);
	}

	Debug::pop();
}

unsigned LuaManager::_envDump(LuaScript *scr) {
	char envString[20] = {0};
	unsigned envCount = 1;
	LuaBehaviorScript* behav = static_cast<LuaBehaviorScript*>(scr);

	if(scr->getType() == LuaScript::TYPE_BEHAVIOR && behav->getParent()) {
		envCount += _envDump(behav->getParent()); //How about you fucking recurse at me?
	} else {
		Debug::luaPrint("**** LUA SCRIPT _ENV DUMP ****");
	}

	lua_getglobal(*_mainState, "rPrint");
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, scr->getEnvironRef());
	lua_pushnil(*_mainState);
	sprintf(envString, "_env[%d]", envCount-1);
	lua_pushstring(*_mainState, envString);
	_protectedCall(3, 1, nullptr);
	lua_pop(*_mainState, 1);

	return envCount;

}

void LuaManager::_runtimeStackDump(void) {
	lua_Debug luaDebug;
	unsigned stackLevel = 1;
	int localIndex = 1;
	unsigned validStackLevel;
	const char *localVarName;
	char displayName[200];

	Debug::luaPrint("**** LUA RUNTIME STACK DUMP ****");

	//Walk up the stack
	while(true) {

		validStackLevel = lua_getstack(*_mainState, stackLevel, &luaDebug);
		if(!validStackLevel) break;

		//Walk local variables and arguments
		while(true) {

			localVarName = lua_getlocal(*_mainState, &luaDebug, localIndex);
			if(!localVarName) break;

			int stackIndex = lua_gettop(*_mainState);
			sprintf(displayName, "_local[%d][%d] [%s]", stackLevel, localIndex, localVarName);
			_rPrintByIndex(stackIndex, 0, displayName);

			lua_pop(*_mainState, 1);

			++localIndex;

		}

		//Walk variable arguments

		localIndex = 1;
		++stackLevel;
	}



}

inline LuaScript *LuaManager::_createScriptFromExisting(const char* const file) {
	const auto it = _scripts.find(file);
	if(it != _scripts.end()) {
		//already allocated
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

	_callGlobalFunction<void>("setScriptAttrib", behav, behav->getAttrib());

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
		_callGlobalFunction<void>("onScriptCreate", behav);
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
		_showError();
		if(_errorCode == LUA_ERRFILE) {
			_errorprintf("Could not load script file!");
		} else {
			lua_remove(*_mainState, 1);					//take loaded chunk off of stack
		}
		if(script) script->setError();
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
						lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, behav->getEnvironRef());		//push _ENV
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
    
	_mainState = new OOLUA::Script; 
	Debug::printf(Debug::VERBOSE, "Opening standard libs");
	luaL_openlibs(*_mainState);
	Debug::printf(Debug::VERBOSE, "Registering wrappers");
	_registerWrappers();

	runString(_rPrint);

    Debug::pop();
	return true;
}

bool LuaManager::uninitialize(void) {
	delete _mainState;
	return true;
}

unsigned LuaManager::getScriptCount(void) {
	return _scripts.size();
}

unsigned LuaManager::getScriptInstances(void) {
	unsigned count = 0;
	for(auto it : _scripts) {
		count += it.second->getRefCount();
	}
	return count;
}

unsigned LuaManager::getKbytesUsed(void) {
	return lua_gc(*_mainState, LUA_GCCOUNT, 0);
}


void LuaManager::dumpStats(void) {
	char buffer[100];
	unsigned int count = 0;
	LuaBehaviorScript* behav;
	LuaBehaviorScriptAttrib* attrib;
	uint64_t triggers;
	int kbytes;

	/*
	 *
	 *	class LuaBehaviorScriptAttrib {
	private:
		uint64_t	_triggers				= 0;
		char		_parentPath[200]		= {0};
		bool		_hasParent				= false;
	public:
		uint64_t getSupportedTriggers(void) const { return _triggers; }
		const char* getParentPath(void) const { return _parentPath; }
		void addSupportedTrigger(const Behavior::TRIGGER trigger) { _triggers |= trigger; }
		void inheritFromBehavior(const char*const path) { _hasParent = true; strcpy(_parentPath, path); }
		bool hasParent(void) const { return _hasParent; }
		*/

	kbytes = lua_gc(*_mainState, LUA_GCCOUNT, 0);


	Debug::luaPrint("**** LUAMANAGER STATS DUMP ****");
	sprintf(buffer, "\tTotal scripts loaded:\t%ld", _scripts.size());
	Debug::luaPrint(buffer);
	sprintf(buffer, "\tMemory Usage:\t%fMB", (float)kbytes/1024.0f);
	Debug::luaPrint(buffer);

	for(auto it : _scripts) {

		sprintf(buffer, "\t[%d] = %s", count, it.second->getPath());
		Debug::luaPrint(buffer);

		sprintf(buffer, "\t\trefCount = %d", it.second->getRefCount());
		Debug::luaPrint(buffer);
		sprintf(buffer, "\t\terror = %d", it.second->getError());
		Debug::luaPrint(buffer);

		switch(it.second->getType()) {
		case LuaScript::TYPE_SCRIPT:
			Debug::luaPrint("\t\ttype = SCRIPT");

			break;
		case LuaScript::TYPE_BEHAVIOR:
			behav = static_cast<LuaBehaviorScript*>(it.second);
			attrib = behav->getAttrib();
			triggers = attrib->getSupportedTriggers();

			Debug::luaPrint("\t\ttype = BEHAVIOR");

			Debug::luaPrint("\t\tattrib = ");
			sprintf(buffer, "\t\t\tparent = %s", attrib->getParentPath());
			Debug::luaPrint(buffer);

			Debug::luaPrint("\t\t\ttriggers = ");
			Debug::push();
			for(unsigned i = 0; i < 64; ++i) {
				uint64_t mask = (1<<i);
				if(mask >= Behavior::TRIGGER_MAX) break;
				if(triggers & mask) {
					sprintf(buffer, "\t\t\t\t%s", Behavior::triggerNameFromFlag(triggers&mask));
					Debug::luaPrint(buffer);
				}
			}

			break;
		}

		++count;
	}
}

void LuaManager::runGarbageCollector(void) {
	_mainState->gc();
}

}
