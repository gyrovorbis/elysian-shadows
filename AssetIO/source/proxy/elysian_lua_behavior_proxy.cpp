#include <proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype Behavior
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Extendable component for adding custom triggers and logic outside of the engine.

    \table
        \row
            \li Inherits:
            \li \l{Component}, \l{EventTarget}, \l{EventObserver}
    \endtable.

    A Behavior is a custom Component that can be attached to an Entity, allowing a developer to
    add their own data and logic, extending the engine's functionality.

    Behaviors can be subclassed and implemented as standard C++ objects, to extend the engine
    in a statically compiled language or it can be implemented as a \l{LuaBehavior}
    which delegates control to a Lua script. C++ Behaviors offer speed and efficiency and
    can be used when optimizing, and \l{LuaBehavior}s offer rapid development and versatility.

    \sa LuaBehavior
*/

/*!
\qmlproperty enumeration Behavior::TRIGGER_TYPE

Represents the various different types of triggers that can be used in a Behavior.

A TRIGGER_TYPE can be one of the following:
\list
\li Behavior.ON_SCRIPT_CREATE
\li Behavior.ON_SCRIPT_DESTROY
\li Behavior.ON_CREATE
\li Behavior.ON_DESTROY
\li Behavior.ON_LOAD
\li Behavior.ON_UNLOAD
\li Behavior.ON_ENTER
\li Behavior.ON_EXIT
\li Behavior.ON_UPDATE
\li Behavior.ON_RENDER
\li Behavior.ON_COLLIDE
\li Behavior.ON_COLLISION_ENTER
\li Behavior.ON_COLLISION_EXIT
\li Behavior.ON_EXAMINE
\li Behavior.ON_DIALOG
\li Behavior.ON_COMBAT_ENTER
\li Behavior.ON_COMBAT_EXIT
\li Behavior.ON_COMBAT_DEATH
\li Behavior.ON_SHATTER
\li Behavior.ON_COMBAT_RECEIVE_STATUS_MODIFIER
\li Behavior.ON_COMBAT_TARGET_COLLISION_ENTER
\li Behavior.ON_BUTTON_PRESS
\li Behavior.ON_BUTTON_RELEASE
\li Behavior.ON_TK_SELECT
\li Behavior.ON_TK_UNSELECT
\endlist
*/
/*!
 * \qmlmethod void Behavior::addTrigger(TRIGGER_TYPE type)
 *
 * Adds the trigger of \a{type} to the list of active triggers by the Behavior.
 */
/*!
 * \qmlmethod void Behavior::removeTrigger(TRIGGER_TYPE type)
 *
 * Removes the trigger of \a{type} from the list of active triggers by the Behavior.
 */
/*!
 * \qmlmethod string Behavior::getBehavName()
 *
 * Returns the name of the Behavior for a C++ Behavior or the asset-relative file path of
 * the Behavior script for a LuaBehavior.
 */
EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Behavior,
                                   addTrigger,
                                   removeTrigger)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Behavior,
                                getBehavName)


/*!
    \qmltype LuaBehavior
    \inherits Behavior
    \since 1.6.0
    \inmodule Engine
    \brief Custom component type for extending the engine through a Lua script.

    LuaBehavior is a Behavior implementation that allows a Lua script to implement the \l{Behavior}'s
    trigger methods.

    \section2 Flags

    \section2 Script Layout

    **PARAM TABLE, SELF TABLE, GLOBAL SCOPE, LOCAL SCOPE, INSTANCING***
*/

/*!
\qmlproperty string LuaBehavior::path

The asset-relative path to the Lua script file.
\sa getBehavName(), setBehavName()
*/
/*!
\qmlproperty Lua_table LuaBehavior::paramTable

Lua table passed to the onCreate() trigger providing the script with instance-specific initialization arguments.
\sa getParamTable(), setParamTable()
*/
/*!
\qmlproperty int LuaBehavior::triggers

Bitmask of Lua triggers currently active for the Behavior instance. If this mask contains triggers that are not
even implemented by the Lua script, behavior is undefined (ie: shit is probably going to crash).
\sa getTriggers(), setTriggers()
*/
/*!
\qmlproperty Lua_table LuaBehavior::selfTable

Read-only property containing a reference to a Lua_table used for storing instanced variables for LuaBehaviors.
This table is created by the engine and cannot be reassigned, but the table itself is fully modifiable by the
Lua script.

By default, the table has two fields, provided by the engine:
\list
\li \b{selfEntity} - reference to the Entity which the LuaBehavior is attached to
\li \b{selfBehav} - referece to the LuaBehavior object containing the Lua script
\endlist

\sa getSelfTable()
*/
/*!
\qmlproperty LuaBehaviorScript* LuaBehavior::script

Read-only property containing a reference to the LuaBehaviorScript object describing the the script
loaded from \l{path}.

\sa getScript()
*/
/*!
 * \qmlmethod string LuaBehavior::getBehavName()
 *
 * Returns the value of the \l{path} property.
 * \sa path, setBehavName()
 */
/*!
 * \qmlmethod void LuaBehavior::setBehavName(string filePath)
 *
 * Sets the value of the \l{path} property to \a{filePath}.
 * \sa path, getBehavName()
 */
/*!
 * \qmlmethod Lua_table LuaBehavior::getParamTable()
 *
 * Returns the value of the \l{paramTable} property.
 * \sa paramTable, setParamTable()
 */
/*!
 * \qmlmethod void LuaBehavior::setParamTable(Lua_table table)
 *
 * Sets the value of the \l{paramTable} property to \a{table}.
 * \sa paramTable, getParamTable()
 */
/*!
 * \qmlmethod int LuaBehavior::getTriggers()
 *
 * Returns the value of the \l{triggers} property.
 * \sa triggers, setTriggers()
 */
/*!
 * \qmlmethod void LuaBehavior::setTriggers(int mask)
 *
 * Sets the value of the \l{triggers} property to \a{mask}.
 * \sa triggers, getTriggers()
 */
/*!
 * \qmlmethod Lua_table LuaBehavior::getSelfTable()
 *
 * Returns the value of the \l{selfTable} property.
 * \sa selfTable
 */
/*!
 * \qmlmethod LuaBehaviorScript* LuaBehavior::getScript()
 *
 * Returns the value of the \l{script} property.
 * \sa script
 */

OOLUA_EXPORT_FUNCTIONS(LuaBehavior,
                        getSelfTable,
                        setTriggers,
                        setBehavName,
                        setParamTable,
                        getParamTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehavior,
                             getTriggers,
                             getScript)
