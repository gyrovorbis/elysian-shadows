#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_combat_system_proxy.hpp>


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
 * \sa removeCombatant(), killCombatant()
 */
/*!
 * \qmlmethod void CombatSystem::removeCombatant(Entity* entity)
 * Removes the given \a{entity} from combat. Calls the onCombatEnxit() trigger on any \l{Behavior}s
 * attached to the \l{Entity}.
 * \sa addCombatant(), killCombatant()
 */
/*!
 * \qmlmethod void CombatSystem::killCombatant(Entity* entity)
 * Calls the onCombatDeath() trigger on any \l{Behavior}s attached to the \l{Entity}, then removes it
 * from combat.
 * \sa addCombatant(), removeCombatant()
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

OOLUA_EXPORT_FUNCTIONS_CONST(CombatSystem)
OOLUA_EXPORT_FUNCTIONS(CombatSystem,
                       addCombatant,
                       removeCombatant,
                       killCombatant,
                       //resolveCombatAction,
                       sendStatusModifier,
                       setBgmCombatTrack,
                       setBgmDynamic)
