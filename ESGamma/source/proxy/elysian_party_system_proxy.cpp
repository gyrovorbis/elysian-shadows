#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_party_system_proxy.hpp>

/*!
    \qmltype PartySystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine subsystem for managing the party and its members.

    The PartySystem is reponsible for managing a list of \l{Entity} objects representing
    the current party members. In addition to providing pathing logic for secondary party
    members to follow the party leader, the PartySystem is what is responsible for adding
    and removing party members to and from the current \l{Area} when it is swapped out for
    a new one. PartySystem also ensures that any items or entities held by party members
    during a warp results in them persisting into the next \l{Area}.

    \section2 Retrieving the PartySystem
    The ParticleSyste object can be retrieved through the static \l{Engine} class:
    \code
    partySys = Engine.getPartySys()
    \endcode

    \sa Engine
*/


/*!
 * \qmlmethod void CharacterSystem::walkToWayPoint(Entity* entity, float x, float y, EventMonitor* monitor)
 *  Makes the given \a{entity} walk in a straight line from its current location to the 2D location given by
 * <\a{x}, \a{y}>. The \l{EventMonitor} provied by \a{monitor} is notified once the \l{Entity} arrives. This
 * uses the \l{Character} \l{Component} to walk, so one must be attached, and animations and speed are handled
 * automatically.
 * \sa moveToWayPoint()
 */
OOLUA_EXPORT_FUNCTIONS(PartySystem,
                        addCharacterToParty,
                        removeCharacterFromParty,
                        setPartyLeader,
                        addPlayable)
OOLUA_EXPORT_FUNCTIONS_CONST(PartySystem,
                                getPartyLeader,
                                getCharacterPartyMember,
                                getPartySize,
                                getPartySlot,
                                getPlayable)
