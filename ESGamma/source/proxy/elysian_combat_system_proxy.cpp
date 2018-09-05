#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <ESGamma/proxy/elysian_combat_system_proxy.hpp>


/*!
    \qmltype CombatSystem
    \since 1.5.8
    \inmodule Engine
    \brief Engine subsystem handling combat-related logic and components.

    The CombatSystem is the \l{Engine} subsystem responsible for managing
    all active \l{Combat} \l{Component}s within the current \l{Area} and performing
    all combat-related tasks such as handling combatants entering combat, death,
    and delegating damage to Lua triggers.

    \section2 Retrieving the CombatSystem
    The CombatSystem object can be retrieved through the static \l{Engine} class:
    \code
    combatSys = Engine.getCombatSys()
    \endcode
    \sa Combat, Engine
*/
/*!
 * \qmlmethod void CombatSystem::addCombatant(Entity* entity)
 * Adds the given \a{entity} to combat. Calls the onCombatEnter() trigger on any \l{Behavior}s
 * attached to the \l{Entity} and transitions the BGM track if using dynamic situational audio.
 * The \l{Entity} is also added to the list of entities the Camera attempts to keep within the
 * visible area.
 *
 * The combatant will be removed automatically if it goes out of range or if it dies, or you
 * can remove it manually.
 * \sa removeCombatant(), getCombatants(), clearCombatants()
 */
/*!
 * \qmlmethod void CombatSystem::removeCombatant(Entity* entity)
 * Removes the given \a{entity} from combat. Calls the onCombatEnxit() trigger on any \l{Behavior}s
 * attached to the \l{Entity}.
 * \sa addCombatant(), clearCombatants(), getCombatants()
 */
/*!
 * \qmlmethod void CombatSystem::killCombatant(Entity* entity)
 * Calls the onCombatDeath() trigger on any \l{Behavior}s attached to the \l{Entity}, then removes it
 * from combat.
 * \sa addCombatant(), removeCombatant(), getCombatants()
 */
/*!
 * \qmlmethod array CombatSystem::getCombatants()
 * Returns a Lua array containing references to every \l{Entity} currently engaged in combat.
 *
 * \e{NOTE: This array includes all \l{Entity} combatants: party members, friendlies, enemies, etc.}
 * \sa addCombatant(), removeCombatant(), clearCombatants()
 */
/*!
 * \qmlmethod void CombatSystem::sendStatusModifier(Combat* combat, CombatStatusModifier* modifier)
 * Sends the \l{CombatStatusModifier} given by \a{modifier} to the \l{Combat} \l{Component} given by
 * \a{combat}. The onCombatReceiveStatusModifier() trigger is called for any \l{Behavior}s attached to
 * the \l{Entity} the \a{combat} is attached to for calculating the final applied status modifier. The
 * final modifier is applied to the \l{Entity}.
 */
/*!
 * \qmlmethod void CombatSystem::setBgmCombatTrack(int trackNumber)
 * Sets the battle music BGM track to the given \a{trackNumber} (1 or 2). If setBgmDynamic() has been set to true,
 * then once combat is engaged, the CombatSystem will automatically start playing the given track number, and
 * once combat resolves, it will automatically swap back to track 0. The default value is track 1.
 * \sa setBgmDynamic()
 */
/*!
 * \qmlmethod void CombatSystem::setBgmDynamic(bool value)
 * If \a{value} is true, the CombatSystem will automatically transition to the track set by setBgmCombatTrack() when combat
 * engages, and change back to track 0 when combat resolves. If \a{value} is not true, the CombatSystem does not
 * automatically manage background music. The default is set to false.
 * \sa setBgmCombatTrack()
 */
/*!
 * \qmlmethod void CombatSystem::setCombatEnabled(bool value)
 * Enables or disables combat processing. When true, CombatSystem checks Entities for aggro, updates battle logic, etc.
 * When set to false, CombatSystem doesn't do any processing. This is useful for cutscenes or special events. Defaults to true.
 */
/*!
 * \qmlmethod COMBAT_STATE CombatSystem::getCombatState()
 * Returns an enumeration representing the current internal state of the CombatSystem.
 * \sa COMBAT_STATE, setCombatEnabled()
 */
/*!
 * \qmlmethod void CombatSystem::setCombatMgmtType(COMBAT_MGMT_TYPE type)
 * Sets the behavior for initiating combat and managing ongoing combat to \a{type}.
 * \sa COMBAT_MGMT_TYPE
 */
/*!
 * \qmlmethod void CombatSystem::clearCombatants()
 * Removes all active combatants currently engaged in combat (if any) and ends combat.
 *
 * \e{NOTE: If the \l{COMBAT_MGMT_TYPE} is currently COMBAT_MGMT_AUTO and there are
 * still enemies that will be aggroed, combat will immediately restart the next frame. For most cases,
 * you will probably want to set the management type to COMBAT_MGMT_MANUAL when
 * calling this method.}
 * \sa addCombatant(), removeCombatant(), COMBAT_MGMT_TYPE
 */


/*!
\qmlproperty enumeration CombatSystem::COMBAT_STATE

Enumerated type representing the current internal state of the \l{CombatSystem}.

COMBAT_STATE can be one of the following values:
\table
    \header
        \li Name
        \li Description
    \row
        \li COMBAT_STATE_DISABLED
        \li CombatSystem logic has been disabled (see \l{setCombatEnabled})
    \row
        \li COMBAT_STATE_INACTIVE
        \li Currently no active combat, but CombatSystem will initiate once enemies are aggroed
    \row
        \li COMBAT_STATE_ACTIVE
        \li Combat is currently ongoing between at least one party member and one enemy
\endtable
*/

/*!
\qmlproperty enumeration CombatSystem::COMBAT_MGMT_TYPE

Enumerated type representing how the CombatSystem handles initiation of new combat and management of ongoing combat.

COMBAT_MGMT_TYPE can be one of the following values:
\table
    \header
        \li Name
        \li Description
    \row
        \li COMBAT_MGMT_AUTO
        \li CombatSystem handles everything automatically. Combat will begin once a party member has aggroed an enemy,
            enemies and friendlies will be added and removed automatically, and combat will end once there are no
            more aggroed enemies. This is the default value.
    \row
        \li COMBAT_MGMT_MANUAL
        \li CombatSystem no longer manages which Entities are engaged in combat, and they must now be added and
            removed manually, giving full control to scripts. This is useful for special battles such as boss fights
            where the combatants are predetermined and aggro is not involved.
\endtable
*/


OOLUA_EXPORT_FUNCTIONS_CONST(CombatSystem,
                             getCombatState,
                             getCombatants)
OOLUA_EXPORT_FUNCTIONS(CombatSystem,
                       addCombatant,
                       removeCombatant,
                       killCombatant,
                       clearCombatants,
                       //resolveCombatAction,
                       sendStatusModifier,
                       setBgmCombatTrack,
                       setBgmDynamic,
                       setCombatMgmtType,
                       setCombatEnabled)
