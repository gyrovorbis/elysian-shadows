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
 * \qmlmethod void addCharacterToParty(Entity* entity)
 *  Adds the \l{Entity} given by \a{entity} to the \l{PartySystem} and assumes ownership over it.
 *  Note that \a{entity} must have a valid \l{Character} component and its necessary prerequisites.
 * \sa removeCharacterFromParty(), getCharacterPartyMember()
 */
/*!
 * \qmlmethod void removeCharacterFromParty(Entity* entity)
 *  Removes the \l{Entity} given by \a{entity} from the \l{PartySystem} and returns ownership of it
 *  back to Lua.
 * \sa addCharacterToParty(), getCharacterPartyMember()
 */

OOLUA_EXPORT_FUNCTIONS(PartySystem,
                        addCharacterToParty,
                        removeCharacterFromParty,
                        setPartyLeader,
                        addPlayable,
                        setFollowLogicEnabled)
OOLUA_EXPORT_FUNCTIONS_CONST(PartySystem,
                                getPartyLeader,
                                getCharacterPartyMember,
                                getPartySize,
                                getPartySlot,
                                getPlayable)
