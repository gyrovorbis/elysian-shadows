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
    \inmodule Engine
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
 * \qmlmethod void LuaBehaviorScriptAttrib::addSupportedTrigger(Behavior::TRIGGER triggerType)
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
    \inmodule Engine

    \brief Base class representing an executable Lua 5.2 script. The
    script may either be a standalone script file (ie area_init.lua)
    or it can be a LuaBehavior script, which can be attached to an \l Entity
    to give it custom logic.

    \section2 Lua5.2 Language and Features
    The Engine includes the full Lua 5.2 Interpreter (with a few minor additions) and all of the standard libraries and
    features supported by it (including the optional debug library). For the complete language documentation,
    please refer to the \l{https://www.lua.org/manual/5.2/}{Lua 5.2 Reference Manual}.

    \section2 OOLua Proxy Classes
    The Engine relies upon a heavily extended implementation of the \l{http://oolua.org/}{OOLua C++ and Lua Binding Library}
    for generating "proxy classes" which are used to give Lua access to
    the C++ classes of the Engine. Many of the script errors such as type-checking and function
    argument/return value validation are handled by the library.

    \section2 Sandboxing and Environment Table
    Every LuaScript is given its own sandbox context to run within, protecting accidental
    access to global variables and preventing namespace collisions within a script. This is
    achieved by giving each LuaScript its own unique, _ENV table, which simply falls back to
    the true global environment table, \c _G, when a global key cannot be found within it.

    \e{NOTE: Refer to the Lua 5.2 Documentation for a more detailed description of environment
    tables (\c _ENV) and the global environment (\c _G): \l{https://www.lua.org/manual/5.2/manual.html#2.2}}.

    Because each LuaScript is given its own \c _ENV table, anything declared globally within a LuaScript is
    not actually a true global value but rather is only global within the execution environment of the script.
    For example, a simple LuaScript containing
    \code
    myName = "David Bowie"
    \endcode
    is only declaring the variable "myName" as a global within the context of the script. Attempting
    to access this variable from outside the script's sandbox will result in a failure, as it is not
    actually global. The code is actually equivalent to saying
    \code
    _ENV.myName = "David Bowie"
    \endcode

    \section3 Global Environment and Variable Shadowing
    Because a LuaScript obviously needs access to the true global environment table, to access
    standard library method such as print(), setmetatable(), and even to access C++ proxy classes,
    the \c _ENV table created for each LuaScript contains an \c __index metamethod which simply falls back
    to the true global environment (\c _G) when a key cannot be found within the \c _ENV table. Because of this,
    accessing an exsiting global variable will work as intended from within the context of a script for
    read operations, since the \c _ENV table falls back to the \c _G table, while write operations will create
    and insert a new key within the \c _ENV table, "shadowing" any existing value within the \c _G table within
    the \c _ENV table.

    An example of this can be seen in the following script:
    \code
    print("Hello World!")       -- works as intended, uses _G.print

    _G.myName = "David Bowie"   -- Creates a TRUE global variable, myName
    print(_G.myName)            -- "David Bowie" (accesses variable directly through globals table)
    print(_ENV.myName)          -- "David Bowie" ("myName" does not exist in _ENV, so falls through to _G)
    print(myName)               -- "David Bowie" (same thing as saying _ENV.myName)

    myName = "Geddy Lee"        -- Shadows existing true global, by overriding it within the _ENV table
    print(_G.myName)            -- "David Bowie" (direct access, haven't modified global variable)
    print(_ENV.myName)          -- "Geddy Lee"   ("myName" found _ENV, won't fall through to _G anymore)
    print(myName)               -- "Geddy Lee"   (same thing as saying _ENV.myName)
    \endcode

    \section2 Performance Optimizations and Recommended Practices
    There are a variety of techniques that can be used to optimize run-time and memory overhead of Lua
    scripts. These are not specific to the Engine and are rather well-known idioms of the language itself.
    \section3 Local Variables
    Variables should \e{always} be declared \c local when their lifetime is limited to some specific scope.
    Remember, any variable that is not explicitly declared using the \c local keyword in Lua defaults to
    global, even if it's decalred within a function.

    Global variables hurt performance in two ways. First, referencing them requires an actual lookup into
    a Lua table, since globals are either stored within the \c _G or \c _ENV tables under the hood. This
    adds the overhead of a hash-based key lookup into the environment table (or even worse, when the \c _ENV
    table has to fall back to the \c _G table, two lookups are used), while a local variable is a direct
    index with a predefined offset.

    Secondly, global variables degrade the memory utilization characteristics and performance of a script through
    their nondeterministic lifetime. A local variable will immediately be deallocated once the containing scope
    has ended, while a global variable will linger indefinitely until the garbage collector is invoked.

    \section3 Caching Nonlocal Variables
    Because global accesses require an internal hash-based lookup into the \c _ENV or \c _G table,
    it is a common Lua idiom to cache these values within local variables when they are accessed
    repeatedly. A prime example of this is using a global function within a large loop:

    \code
    for i = 1, 1000000 do
        local x = math.sin(i)
    end
    \endcode
    The above function runs approximately 30% slower than the following implementation:
    \code
    local sin = math.sin
    for i = 1, 1000000 do
        local x = sin(i)
    end
    \endcode

    \section3 Integer vs String Table Keys
    A Lua table is internally represented by two disjoint data types:
    \list
    \li \b { 1. Array Segment }: Values with contiguous integer keys starting at index 1 and growing are stored
        within an array internally, resulting in fast, direct access to data.

    \li \b { 2. Hashtable Segment}: Values with noncontiguous or noninteger keys are stored within an internal
            hashtable, which requires the calculation of a hashkey for every lookup. This is much slower than
            accessing the array segment of the table

    \endlist

    Because of this, array-style table storage and indexing should be favored over the hash-style indexing scheme
    when table access performance is critical.

    \sa LuaBehaviorScript, LuaBehavior, \l{https://www.lua.org/manual/5.2/}{Lua 5.2 Reference Manual},
       \l{http://oolua.org/}{OOLua C++ and Lua Binding Library}

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
    \inmodule Engine
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
            LuaVariant f = LuaVariant::fromStack(-1, false);
            if(f.getType() != LUA_TFUNCTION) {
                errprintf("Expected trigger function for [%s], got [%s] instead!", LuaBehavior::triggerStrings[i], f.getTypeString());
            }
            lua_settable(state, -3);
        }
    }

    lua_pop(state, 1);

    LuaManager::stackBalanceEnd();
}



}
