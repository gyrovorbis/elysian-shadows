#include "elysian_lua_manager.hpp"
#include "elysian_lua_script.hpp"
#include <crtp/elysian_behavior.hpp>
#include <utility/luatables.h>
#include "elysian_debug.hpp"
#include <utility/elysian_lua_variant_base.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>

namespace elysian {

//static members
std::map<const char*, LuaScript*, ltstr>	LuaManagerBase::_scripts;
bool                                        LuaManagerBase::_serializing                                                = false;
OOLUA::Script*								LuaManagerBase::_mainState                                                  = nullptr;
int											LuaManagerBase::_errorCode                                                  = 0;
Debug::SEVERITY                             LuaManagerBase::_printSeverity                                              = Debug::LOG_VERBOSE;
int                                         LuaManagerBase::_stackBalanceTop[ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH] = { 0 };
int                                         LuaManagerBase::_stackBalanceRecursiveDepth                                 = 0;
int                                         LuaManagerBase::_traceBackDbgPushes                                         = 0;
const char									LuaManagerBase::_rPrint[] =
    "function rPrint(s, l, i) -- recursive Print (structure, limit, indent)\n"
        "l = (l) or 100; i = i or \"\";	-- default item limit, indent string\n"
        "if (l<1) then print \"LOG_ERROR: Item limit reached.\"; return l-1 end;\n"
        "local ts = type(s);\n"
        "if (ts ~= \"table\" and not(ts == \"userdata\" and getmetatable(s) and getmetatable(s).__pairs)) then print (i,ts,s); return l-1 end\n"
        "print (i,ts);           -- print \"table\"\n"
        "for k,v in pairs(s) do  -- print \"[KEY] VALUE\"\n"
            "l = rPrint(v, l, i..\"\t[\"..tostring(k)..\"]\")\n"
            "if (l < 0) then break end\n"
        "end\n"
        "return l\n"
    "end\n";

void LuaManagerBase::luaPrint(const char * const string, const Debug::SEVERITY severity) {
    Debug::SEVERITY sev = (severity == Debug::LOG_OFF)? _printSeverity : severity;
    Debug::luaPrint(string, sev);
}


//Dumps to debug log and LUA_LOG_ERROR_FILE
void LuaManagerBase::_errorprintf(const char *const string, ...) {
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_list argptr;

    va_start(argptr,string);
    vsprintf(buffer, string, argptr);
    va_end(argptr);

    Debug::luaPrint(buffer, Debug::LOG_ERROR);
}


void LuaManagerBase::_showError(void) {
    if(_errorCode == 0) return;
    const char *errorMsg = luaL_checkstring(*_mainState, -1);
    Debug::luaPrint("Error Message");
    Debug::push();
    _errorprintf((errorMsg)? errorMsg : "Lua error message not available");
    Debug::pop();
    lua_pop(*_mainState, 1); //pop error string off the stack
}

void LuaManagerBase::rPrintByIndex(const int index, const unsigned depth, const char* name) {
    //int oldTop = lua_gettop(*_mainState);
    lua_pushvalue(*_mainState, index);							//push table to print
    LuaManager::pushGlobalField("rPrint");                            //push function
    lua_pushvalue(*_mainState, -2);                             //push table to print
    if(depth) lua_pushnumber(*_mainState, depth);				//push depth if given
    else lua_pushnil(*_mainState);								//push nil for default depth if not
    lua_pushstring(*_mainState, name);							//push name of table
    lua_call(*_mainState, 3, 1);								//call rPrint
    lua_pop(*_mainState, 2);

   // oldTop = lua_gettop(*_mainState);

    //lua_pop(*_mainState, lua_gettop(*_mainState)-oldTop);									//pop return value
}

void LuaManagerBase::rPrintByRef(const int ref, const unsigned depth, const char* name) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);			//push ref onto stack
    //int index = lua_gettop(*_mainState);						//get index of ref
    rPrintByIndex(-1, depth, name);							//invoke by ref
    lua_pop(*_mainState, 1);									//pop ref from stack
}

bool LuaManagerBase::runString(const char *const string) {
    if((_errorCode = luaL_dostring(*_mainState, string)) != 0) {
        _showError();
        return false;
    }
    return true;
}


int LuaManagerBase::_traceBack(lua_State *L) {
    _errorCode = 1;

    if (!lua_isstring(L, 1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    LuaManager::pushGlobalField("debug");
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

    //if(_traceBackDbgPushes) {
      //  Debug::pop();
      //  --_traceBackDbgPushes;
    //}

    Debug::luaPrint("***** LUA LOG_ERROR *****");
    Debug::push();
    //++_traceBackDbgPushes;

    _showError();
    lua_pop(*_mainState, 2); //remove debug table and original error from stack
    _runtimeStackDump();
    return 1;
}

void LuaManagerBase::_cStackDump (void) {
    int i;
    int top = lua_gettop(*_mainState);
    char varDisplayString[200];

    luaPrint("C Stack Dump");
    Debug::push();

    for (i = 1; i <= top; ++i) {  /* repeat for each level */
        sprintf(varDisplayString, "_cStack[%d]", i);
        rPrintByIndex(i, 10, varDisplayString);
    }

    Debug::pop();
}

unsigned LuaManagerBase::_envDump(LuaScript *scr) {
    char envString[20] = {0};
    unsigned envCount = 1;
    //bool topLevel = false;
    LuaBehaviorScript* behav = static_cast<LuaBehaviorScript*>(scr);

    static int recursions = 0;
    int instance = recursions++;

    if(scr->getType() == LuaScript::TYPE_BEHAVIOR && behav->getParent()) {
        envCount += _envDump(behav->getParent()); //How about you fucking recurse at me?
    } else {
        Debug::luaPrint("Script Environment Dump");
        //topLevel = true;
        Debug::push();
    }

    LuaManager::pushGlobalField("rPrint");
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, scr->getEnvironRef());
    lua_pushnumber(*_mainState, 3);
    sprintf(envString, "_env[%d]", envCount-1);
    lua_pushstring(*_mainState, envString);
    _protectedCall(3, 1, nullptr);
    lua_pop(*_mainState, 1);


    if(instance == 0) {
        Debug::pop();
        recursions = 0;
    }

    return envCount;

}

void LuaManagerBase::_runtimeStackDump(void) {
    lua_Debug luaDebug;
    unsigned stackLevel = 1;
    int localIndex = 1;
    unsigned validStackLevel;
    const char *localVarName;
    char displayName[200];

    Debug::luaPrint("Runtime Stack Dump");
    Debug::push();

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
            rPrintByIndex(stackIndex, 0, displayName);

            lua_pop(*_mainState, 1);

            ++localIndex;

        }

        //Walk variable arguments

        localIndex = 1;
        ++stackLevel;
    }

    Debug::pop();



}

inline LuaScript *LuaManagerBase::_createScriptFromExisting(const char* const file) {
    const auto it = _scripts.find(file);
    if(it != _scripts.end()) {
        //already allocated
        it->second->incRefCount();
        return it->second;
    }
    return nullptr;
}

LuaBehaviorScript* LuaManagerBase::loadBehaviorScript(const char* const file) {

    LuaBehaviorScript* behav = static_cast<LuaBehaviorScript*>(_createScriptFromExisting(file));
    if(behav) {
        dbgprintf("Script already loaded. Returning reference from cache.");
        return behav;
    }

    behav = new LuaBehaviorScript(file);

    if(!runScriptSandbox(file, behav)) {
        delete behav;
        return nullptr;
    }

    _callGlobalFunction<void>("setScriptAttrib", behav, behav->getAttrib());
    behav->cacheTriggers(*_mainState);

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

    dbgprintf("Adding new script to cache.");
    _scripts.insert(std::pair<const char *, LuaScript *>(behav->getPath(), behav));
    return behav;

}

LuaScript* LuaManagerBase::loadScript(const char * const file) {
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


bool LuaManagerBase::runScriptSandbox(const char *const file, LuaScript* const script, const unsigned retCount) {
    //Check if script has already been loaded, and return its globals table
    int ref = LUA_NOREF;

    if((_errorCode = luaL_loadfile(*_mainState, file)) != 0) {
        if(_errorCode == LUA_ERRFILE) {
            Debug::printf(Debug::LOG_WARNING, "Could not load script file [%s]!", file);
        } else _showError(); //DO NOT LOG_ERROR OUT IF FILE ISN'T FOUND!!!
        /*else {
            lua_remove(*_mainState, 1);					//take loaded chunk off of stack
        }*/
        lua_pop(*_mainState, lua_gettop(*_mainState));
        if(script) script->setError();
        return false;

    } else {

        lua_newtable(*_mainState);                                      //ENV
        lua_newtable(*_mainState);                                      //metatable
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);  //globals table
        lua_setfield(*_mainState, -2, "__index");                       //metatable[__index] = _G;
        lua_setmetatable(*_mainState, -2);                              //setmetatable(ENV, metatable);
        if(script) {
            lua_pushvalue(*_mainState, -1);                             //duplicate ENV
            ref = luaL_ref(*_mainState,	LUA_REGISTRYINDEX);             //store reference to ENV in Lua registry
            script->setEnvironRef(ref);                                 //and save key in LuaScript
        }
        lua_setupvalue(*_mainState, -2, 1);
        _protectedCall(0, retCount, script);
    }

    return true;
}

bool LuaManagerBase::unloadBehaviorScript(LuaBehaviorScript &script) {
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

bool LuaManagerBase::unloadScript(LuaScript &script) {
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

bool LuaManagerBase::initialize(void) {
    DbgEvent event("Initializing Lua virtual machine.");

    _mainState = new OOLUA::Script;
    Debug::printf(Debug::LOG_VERBOSE, "Opening standard libs");
    luaL_openlibs(*_mainState);
    Debug::printf(Debug::LOG_VERBOSE, "Registering wrappers");
    runString(_rPrint);
    runScriptSandbox(ELYSIAN_LUA_MANAGER_SERIALIZE_SCRIPT_PATH);
    runScriptSandbox(ELYSIAN_LUA_MANAGER_TABLE_MANIP_SCRIPT_PATH);
    LuaManager::_registerWrappers();
    return true;
}

bool LuaManagerBase::uninitialize(void) {
    delete _mainState;
    _scripts.clear();
    return true;
}

unsigned LuaManagerBase::getScriptCount(void) {
    return _scripts.size();
}

unsigned LuaManagerBase::getScriptInstances(void) {
    unsigned count = 0;
    for(auto it : _scripts) {
        count += it.second->getRefCount();
    }
    return count;
}

unsigned LuaManagerBase::getKbytesUsed(void) {
    return lua_gc(*_mainState, LUA_GCCOUNT, 0);
}


void LuaManagerBase::dumpStats(void) {
    char buffer[100];
    unsigned int count = 0;
    LuaBehaviorScript* behav;
    LuaBehaviorScriptAttrib* attrib;
    uint64_t triggers;
    int kbytes;

    kbytes = lua_gc(*_mainState, LUA_GCCOUNT, 0);


    Debug::luaPrint("Virtual Machine Status Dump");
    Debug::push();
    sprintf(buffer, "Total scripts loaded:\t%d", (unsigned)_scripts.size());
    Debug::luaPrint(buffer);
    sprintf(buffer, "Memory Usage:\t%fMB", (float)kbytes/1024.0f);
    Debug::luaPrint(buffer);

    for(auto& it : _scripts) {

        sprintf(buffer, "[%d] = %s", count, it.second->getPath());
        Debug::push();
        Debug::luaPrint(buffer);

        sprintf(buffer, "refCount = %d", it.second->getRefCount());
        Debug::luaPrint(buffer);
        sprintf(buffer, "error = %d", it.second->getError());
        Debug::luaPrint(buffer);

        switch(it.second->getType()) {
        case LuaScript::TYPE_SCRIPT:
            Debug::luaPrint("type = SCRIPT");

            break;
        case LuaScript::TYPE_BEHAVIOR:
            behav = static_cast<LuaBehaviorScript*>(it.second);
            attrib = behav->getAttrib();
            triggers = attrib->getSupportedTriggers();

            Debug::luaPrint("type = BEHAVIOR");
            Debug::push();

            Debug::luaPrint("attrib = ");
            Debug::push();
            sprintf(buffer, "parent = %s", attrib->getParentPath());
            Debug::luaPrint(buffer);

            Debug::luaPrint("triggers = ");
            Debug::push();
            for(unsigned i = 0; i < 64; ++i) {
                uint64_t mask = (1<<i);
                if(mask >= Behavior::TRIGGER_MAX) break;
                if(triggers & mask) {
                    sprintf(buffer, "%s", Behavior::triggerNameFromFlag(triggers&mask));
                    Debug::luaPrint(buffer);
                }
            }

            Debug::pop(3);

            break;
        }
        Debug::pop();
        ++count;
    }
    Debug::pop();
}

void LuaManagerBase::runGarbageCollector(void) {
    _mainState->gc();
}

bool LuaManagerBase::genericIterate(const int index, std::function<void(void)> lambda) {
    //bool firstPass = true;
    stackBalanceBegin();

    int beginStack = lua_gettop(*_mainState);

    lua_pushvalue(*_mainState, index);


    //assume shit is valid?
#if 0
    int valueType = lua_type(*_mainState, -1);

    Q_ASSERT(valueType == LUA_TUSERDATA || valueType == LUA_TTABLE);

    //Populate metatable
    if((metatable = getMetatable(-1))) {
        lua_pop(*_mainState, 1);
    }

    //Check if userdata is iterable
    if(valueType == LUA_TUSERDATA && metatable) {
        getMetatable(-1);
        lua_getfield(*_mainState, -1, "__connectSignals");
        //userdata has lua table proxy
        if(!lua_isnil(*_mainState, -1)) {
            lua_pushvalue(*_mainState, -3);
            lua_pushlightuserdata(*_mainState, static_cast<QObject*>(this));
            _protectedCall(2, 0, nullptr);
            iterable = true;
        //userdata doesn't have lua table proxy
        } else {
            iterable = false;
            lua_pop(*_mainState, 1); //pop nil
        }
        lua_pop(*_mainState, 1); //pop metatable
    //Populate table
    } else if(valueType == LUA_TTABLE) {
        iterable = true;
        auto* ud = TkLuaTableUserdata::inject(*_mainState, _luaRef);
        connect(ud, SIGNAL(fieldChanged(QVariant)), this, SLOT(onFieldChanged(QVariant)));

    //userdata with no metatable
    } else iterable = false;

    //Nothing to iterate over
    if(!iterable) {
        lua_pop(*_mainState, 1);
        LuaManager::stackBalanceEnd();
        return false;
    }
#endif

    pushGlobalField("pairs");
    lua_pushvalue(*_mainState, -2);
    _protectedCall(1, 3, nullptr);

    lua_pushvalue(*_mainState, -3);
    lua_insert(*_mainState, -4);

    lua_pushvalue(*_mainState, -2);
    lua_insert(*_mainState, -4);

#if GYRO_PLATFORM == GYRO_PLATFORM_Qt
    QVariant lastKey;
    QVariant lastValue;
#endif

    do {
        if(!_protectedCall(2, 2, nullptr
#if GYRO_PLATFORM == GYRO_PLATFORM_Qt
                           ,"LuaManager::genericIterate(<%s, %s>)"
                           , Q_CSTR(lastKey.toString()),
                           Q_CSTR(lastValue.toString())
#endif
                )) {
            break;
        }
         if(lua_isnil(*_mainState, -2)) {
             break;
         }

        lua_pushvalue(*_mainState, -4);
        lua_insert(*_mainState, -5);
        lua_pushvalue(*_mainState, -3);
        lua_insert(*_mainState, -5);

        lambda();

        //STACK[-2] = KEY
        //STACK[-1] = VALUE

#if GYRO_PLATFORM == GYRO_PLATFORM_Qt
        lua_pushvalue(*_mainState, -1);
        OOLUA::pull(*_mainState, lastValue);
        lua_pushvalue(*_mainState, -2);
        OOLUA::pull(*_mainState, lastKey);
#endif
        /*
        QList<QStandardItem*> items;
        auto* labelItem = (PropertyStackModel::createReadOnlyItem(key.toString()));
        auto* valueItem = new LuaTableFieldItem(_luaRef, key, labelItem);
        items.push_back(labelItem);
        items.push_back(valueItem);
        //items.push_back(

        parentItem->appendRow(items);
        valueItem->insertingModelItem();
        _fieldItems[key] = valueItem;
*/
        lua_pop(*_mainState, 1);
        //firstPass = false;

        //Pop value, leave key
    } while(1);

    //pop key + table
    //lua_pop(*_mainState, 5);

    lua_pop(*_mainState, lua_gettop(*_mainState)-beginStack);

    stackBalanceEnd();
    return true;
}


LuaVariant LuaManagerBase::exec(const char * const str, ...) {
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_list argptr;

    va_start(argptr,str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    runString(buffer);
    return LuaVariant::fromStack();
}

LuaVariant LuaManagerBase::getPrefab(const char* const name) {
    return exec("return prefab(\"%s\")", name);
}

const char* LuaManagerBase::getPrefabName(const int index) {
    return _callGlobalFunction<const char*>("getPrefabName", nullptr, RawTableReference(index));
}

bool LuaManagerBase::saveTable(const int ref, const char * const filename) {
    bool success = true;

    dbgprintf("Serializing Lua table: [%s]", filename);
    Debug::push();

    const char* serialData = _callGlobalFunction<const char*>("serialize", nullptr, RawTableReference(ref));
    if(!serialData) {
        errprintf("Returned NULL string!");
        success = false;
    } else {

        FILE* file = fopen(filename, "w");
        if(!file) {
            errprintf("Could not open file for writing!");
            success = false;
        } else {
            fprintf(file, "%s", serialData);
            fclose(file);
        }
    }

    Debug::pop();
    return success;
}

LuaVariant LuaManagerBase::getGlobalsTable(void) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    return LuaVariant::fromStack();
}



}
