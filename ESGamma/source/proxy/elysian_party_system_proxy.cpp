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
 *  back to Lua. Returns true if the removal was successful or false otherwise (probably wasn't actually
 *  even within the party to begin with).
 * \sa addCharacterToParty(), getCharacterPartyMember()
 */
/*!
 * \qmlmethod void setPartyLeader(Entity* entity)
 *  Sets the \l{Entity} given by \a{entity} as the leader of the Party, shifting any other party members
 * back by a slot.
 *
 * \e{NOTE: \a{entity} needs to have already been added to the party!}
 * \sa getPartyLeader()
 */
/*!
 * \qmlmethod void setFollowLogicEnabled(bool enabled)
 *  If true, party members who are not the leader automatically follow the leader. If false, members of the party do not
 *  automatically move or follow the leader.
 */
/*!
 * \qmlmethod Entity* getPartyLeader()
 * Returns a reference to the \l{Entity} at the first slot of the party or nil if the party is empty.
 * \sa setPartyLeader()
 */
/*!
 * \qmlmethod int getPartySlot(Entity* entity)
 * Returns the slot index for the party member given by \a{entity} or -1 if \a{entity} is not a valid party member.
 * \e{NOTE: The slot is determined by doing a O(n) linear search for the given \l{Entity|}
 */
/*!
 * \qmlmethod Entity* getCharacterPartyMember(unsigned slot)
 * Returns a reference to the \l{Entity} party member at the given \a{slot} index or nil if there isn't one
 * at the given slot.
 * \sa addCharacterToParty(), removeCharacterFromParty()
 */
/*!
 * \qmlmethod unsigned getPartySize()
 * Returns the number of party members currently within the party.
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
