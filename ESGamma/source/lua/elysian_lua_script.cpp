#include <lua/elysian_lua_script.hpp>
#include <crtp/elysian_lua_behavior.hpp>
#include <lua/lua.hpp>

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


/*!
    \qmltype LuaScript
    \since 1.5.8
    \inmodule ESToolkit
    \brief Base class representing an executable Lua script. The
    script may either be a standalone script file (ie area_init.lua)
    or it can be a LuaBehavior script. All LuaScripts run within
    their own sandboxed environments, with their own separate global
    namespaces.

    The sandbox environment table uses the true global '_G' table as
    its __index metamethod, meaning it can read from it normally;
    however, writing directly to the true global namespace must be
    done explicitly (ie _G.value = 3)... Also that is absolutely
    shitty style, so don't fucking do it.

*/
/*!
 * \qmlmethod string LuaScript::getPath()
 *
 * Returns the full, asset-relative path to the source Lua script.
 */

/*!
 * \qmlmethod int LuaScript::getRefCount()
 *
 * Returns the number of instances currently instantiated from
 * a particular script or behavior. This is managed internally
 * by the engine.
 */

/*!
 * \qmlmethod bool LuaScript::getError()
 *
 * Returns true if the current Lua script contains any errors.
 * Scripts in an error will report their error, then abort
 * execution.
 */


OOLUA_EXPORT_FUNCTIONS(LuaScript)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaScript,
                             getPath,
                             getRefCount,
                             getError)




/*!
    \qmltype LuaBehaviorScript
    \since 1.5.8
    \inmodule ESToolkit
    \inherits LuaScript
    \brief Class for Lua scripts implementing "Behavior" functionality.
    A LuaBehaviorScript implements a series of "trigger" callback
    functions, which are used to extend the functionality of the engine.
    A LuaBehaviorScript may be instantiated as a LuaBehavior component
    and attached to an Entity, overriding default engine behavior
    or implementing entirely new behavior outside of the engine.

    As each LuaScript is sandboxed with its own environment, each
    LuaBehaviorScript has its own sandboxed environment table,
    which is shared among all of its LuaBehavior instances.

    The "global" namespace within a LuaBehaviorScript essentially
    emulates static class variables and members, since every instance
    shares the same environment. Because of this, instanced data is
    contained within the "self" table, which is managed by the engine
    and is passed back to each trigger when invoked by the engine.

*/
/*!
 * \qmlmethod string LuaScript::getPath()
 *
 * Returns the full, asset-relative path to the source Lua script.
 */

/*!
 * \qmlmethod int LuaScript::getRefCount()
 *
 * Returns the number of instances currently instantiated from
 * a particular script or behavior. This is managed internally
 * by the engine.
 */

/*!
 * \qmlmethod bool LuaScript::getError()
 *
 * Returns true if the current Lua script contains any errors.
 * Scripts in an error will report their error, then abort
 * execution.
 */



OOLUA_EXPORT_FUNCTIONS(LuaBehaviorScript,
                       getAttrib)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehaviorScript,
                             getParent)




namespace elysian {

LuaScript::LuaScript(const char *path): _environRef(LUA_NOREF) { strcpy(_path, path); }


void LuaBehaviorScript::cacheTriggers(lua_State*const state) {
    LuaManager::stackBalanceBegin();

    lua_rawgeti(state, LUA_REGISTRYINDEX, _environRef);

    for(unsigned i = 0; i < LuaBehavior::TRIGGER_ENGINE_MAX_INDEX; ++i) {
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
