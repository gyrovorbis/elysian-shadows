#include "elysian_lua_script.hpp"
#include "elysian_lua_behavior.hpp"
#include <lua/lua.hpp>
#include <cstring>

OOLUA_EXPORT_FUNCTIONS(LuaBehaviorScriptAttrib,
                       inheritFromBehavior,
                       addSupportedTrigger)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehaviorScriptAttrib)


/*!
    \qmltype LuaBehaviorScriptAttrib
    \since 1.5.8
    \inmodule ESToolkit
    \brief Holds properties which may be shared among instances
    of the same LuaBehaviorScript, such as parent behaviors,
    supported triggers, and file paths.

    Every LuaBehavior must initialize the LuaBehaviorScriptAttrib
    object provided by the engine in the setScriptAttrib() method.
    This give the engine information about what is supported by
    each script.

    Example:
    \code
        function setScriptAttrib(attrib)
            attrib:inheritFromBehavior("/assets/my_parent_behavior.lua")
            attrib:addSupportedTrigger(Behavior.ON_SCRIPT_CREATE)
            attrib:addSupportedTrigger(Behavior.ON_COMBAT_ENTER)
            attrib:addSupportedTrigger(Behavior.ON_COMBAT_EXIT)
            attrib:addSupportedTrigger(Behavior.ON_COMBAT_TARGET_COLLISION_ENTER)

        end
    \endcode
*/

/*!
 * \qmlmethod void LuaBehaviorScriptAttrib::addSupportedTrigger(Behavior.TRIGGER triggerType)
 *
 * Adds triggerType to the list of triggers implemented by the LuaBehaviorScript.
 */

/*!
 * \qmlmethod void LuaBehaviorScriptAttrib::inheritFromBehavior(string path)
 *
 * Specifies that the current LuaBehaviorScript inherits from the LuaBehaviorScript at
 * the given path.
 */





namespace elysian {
	
LuaScript::LuaScript(const char *path): _environRef(LUA_NOREF) { strcpy(_path, path); }


void LuaBehaviorScript::cacheTriggers(lua_State*const state) {
    LuaManager::stackBalanceBegin();

    lua_rawgeti(state, LUA_REGISTRYINDEX, _environRef);

    for(unsigned i = 0; i < LuaBehavior::TRIGGER_MAX_INDEX; ++i) {
        if((_attrib.getSupportedTriggers()&(1ul<<i))) {
            lua_pushnumber(state, i);
            lua_getfield(state, -2, LuaBehavior::triggerStrings[i]);
            lua_settable(state, -3);
        }
    }

    lua_pop(state, 1);

    LuaManager::stackBalanceEnd();
}



}
