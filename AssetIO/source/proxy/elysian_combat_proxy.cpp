#include <AssetIO/proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype CombatAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Collection of shareable attributes contained by a Combat component.

    CombatAttrib is a collection of data contained by a Combat component and used
    by the CombatSystem for triggering and managing combat between Entities.

    \sa Combat, CombatSystem
*/
/*!
 * \qmlproperty BitMask32 CombatAttrib::type
 *
 * 32-bit integer mask representing the combat type of the Entity. Other combatant Entities perform
 * a bitwise AND operation between this mask and their own typeAgainst property to see if the two should
 * engage in combat.
 * \sa getTypeFlags(), setTypeFlags()
 */
/*!
 * \qmlproperty BitMask32 CombatAttrib::typeAgainst
 *
 * 32-bit integer mask representing the types of Entities this Entity engages in combat against. This
 * field is bitwise AND-ed against the other Entity's \l{type} property to see if the two should engage
 * in combat.
 * \sa getTypeAgainstFlags(), setTypeAgainstFlags()
 */
/*!
 * \qmlmethod void CombatAttrib::setTypeFlags(BitMask32 flags)
 *
 * Sets the \l{type} property to the value given by \a{flags}.
 * \sa type, getTypeFlags()
 */
/*!
 * \qmlmethod BitMask32* CombatAttrib::getTypeFlags()
 *
 * Returns a reference to the \l{type} property.
 * \sa type, setTypeFlags()
 */
/*!
 * \qmlmethod void CombatAttrib::setTypeAgainstFlags(BitMask32 flags)
 *
 * Sets the typeAgainst property to the value given by \a{flags}.
 * \sa typeAgainst, getTypeAgainstFlags()
 */
/*!
 * \qmlmethod BitMask32* CombatAttrib::getTypeAgainstFlags()
 *
 * Returns a reference to the \l{typeAgainst} property.
 * \sa typeAgainst, setTypeAgainstFlags()
 */
/*!
 * \qmlmethod void CombatAttrib::setParticleAttrib(ParticleEmitterAttrib* attrib)
 *
 * Stores the ParticleEmitterAttrib object given by \a{attrib} as the internal partice emitter attributes.
 * This can be used to emit blood or some other type of particle when the Entity takes damage. Ownership of
 * \a{attrib} is transferred from Lua to the engine.
 */

OOLUA_EXPORT_FUNCTIONS_CONST(CombatAttrib)
OOLUA_EXPORT_FUNCTIONS(CombatAttrib,
                       setTypeFlags,
                       setTypeAgainstFlags,
                       setParticleAttrib,
                       getTypeFlags,
                       getTypeAgainstFlags)

/*!
    \qmltype CombatStatus
    \since 1.6.0
    \inmodule Engine
    \brief Combat-related stats managed by the Engine.

    CombatStatus is a collection of the combat-related stats that the engine is aware
    of. By design, most stats are kept to Lua, so the majority of the combat system is
    versatile and easily extended. However, these few stats are used internally by the
    CombatSystem.

    \sa Combat, CombatSystem, CombatAttrib
*/
/*!
 * \qmlproperty int CombatStatus::maxHp
 *
 * Maximum hit-points for combatant.
 * \sa getMaxHp(), setMaxHp()
 */
/*!
 * \qmlproperty int CombatStatus::hp
 *
 * Current hit-points for combatant.
 * \sa getHp(), setHp()
 */
/*!
 * \qmlproperty int CombatStatus::maxMp
 *
 * Maximum magic-points for combatant.
 * \sa getMaxMp(), setMaxMp()
 */
/*!
 * \qmlproperty int CombatStatus::mp
 *
 * Current magic-points for combatant.
 * \sa getMp(), setMp()
 */
/*!
 * \qmlmethod int CombatStatus::getMaxHp()
 *
 * Retuns the maximum hit-points of the combatant.
 * \sa setMaxHp(), maxHp
 */
/*!
 * \qmlmethod int CombatStatus::getHp()
 *
 * Retuns the current hit-points of the combatant.
 * \sa setHp(), hp
 */
/*!
 * \qmlmethod int CombatStatus::getMaxMp()
 *
 * Retuns the maximum magic-points of the combatant.
 * \sa setMaxMp(), maxMp
 */
/*!
 * \qmlmethod int CombatStatus::getMp()
 *
 * Retuns the current magic-points of the combatant.
 * \sa setMp(), mp
 */
/*!
 * \qmlmethod void CombatStatus::setMaxHp(int value)
 *
 * Sets the maximum hit-points of the combatant to \a{value}.
 * \sa getMaxHp(), maxHp
 */
/*!
 * \qmlmethod void CombatStatus::setHp(int value)
 *
 * Sets the current hit-points of the combatant to \a{value}.
 * \sa getHp(), hp
 */
/*!
 * \qmlmethod void CombatStatus::setMaxMp(int value)
 *
 * Sets the maximum magic-points of the combatant to \a{value}.
 * \sa getMaxMp(), maxMp
 */
/*!
 * \qmlmethod void CombatStatus::setMp(int value)
 *
 * Sets the current magic-points of the combatant to \a{value}.
 * \sa getMp(), mp
 */

OOLUA_EXPORT_FUNCTIONS_CONST(CombatStatus,
                             getMp,
                             getHp,
                             getMaxHp,
                             getMaxMp,
                             hasStatusEffect)
OOLUA_EXPORT_FUNCTIONS(CombatStatus,
                       setHp,
                       setMaxHp,
                       setMaxMp,
                       setMp)

/*!
    \qmltype CombatStatusModifier
    \inherits CombatStatus
    \since 1.6.0
    \inmodule Engine
    \brief Modifier to dealing/healing damage or applying/removing status effects from a combatant.

    CombatStatusModifier is the class used to represent any kind of modifications to the status
    of a combatant, whether it's HP, MP, a status effect, beneficial, or harmful.

    \sa Combat, CombatSystem, CombatAttrib, AggroManager
*/
/*!
 * \qmlmethod void CombatStatusModifier::addStatusEffect(int bitflag)
 *
 * Adds the status effect represented by the given \a{bitflag}.
 * \sa removeStatusEffect()
 */
/*!
 * \qmlmethod void CombatStatusModifier::removeStatusEffect(int bitflag)
 *
 * Removes the status effect represented by the given \a{bitflag}.
 * \sa addStatusEffect()
 */
/*!
 * \qmlmethod void CombatStatusModifier::setSourceEntityEmnity(Entity* entity, int emnity)
 *
 *  Sets the source information for the origin of the CombatStatusModifier. The \a entity
 *  parameter is the combatant causing the status modification (the attacker in the case of damage),
 *  and the \a emnity parameter is how much emnity is generated towards the source \a entity in the
 *  recipient's emnity table.
 * \sa AggroManager
 */

OOLUA_EXPORT_FUNCTIONS_CONST(CombatStatusModifier)
OOLUA_EXPORT_FUNCTIONS(CombatStatusModifier,
                       addStatusEffect,
                       removeStatusEffect,
                       setSourceEntityEmnity)

/*!
    \qmltype AggroManager
    \inherits CombatStatus
    \since 1.6.0
    \inmodule Engine
    \brief Determines which combatants to target for AI driven combat behaviors

    Any Entity with a Combat Component contains an AggroManager, which is responsible for
    maintaining two sets of data which may be used by combat behavior scripts: an emnity table
    and a list of aggroed entities. Note that the AggroManager should be used for determining
    the targets of ALL AI-controlled combatants, including non-player party members, enemies,
    and friendly allies.

    \section2 Emnity Table
    Every combat attack, skill, ability or spell should be given an integer representing the amount
    of emnity it generates. The AggroManager maintains an internal table associating each combatant
    Entity with the amount of emnity it has accumulated. The Entity with the highest amount of emnity
    is determined to be the highest priority by the AggroSystem, and combat scripts and behaviors
    should use this to determine which Entity should be targetted in combat.

    For example, each hit from a sword may add 10 emnity for the Entity swinging the sword. Once the
    swinging Entity becomes the Entity with the highest emnity, the enemy AI script attached to the
    recipient should begin targetting the Entity with the sword.

    An emnity level of 0 is the default for each combatant, meaning they have not yet generated emnity
    and will not be considered as a target. Emnity may also be subtractive, meaning there can be certain
    skills or abilities such as a Ninja's "camaflouge" which may reduce an enemy's emnity levels and cause
    them to no longer be targeted. Emnity may also be generated by non-damage moves and by moves that aren't
    even targetting the recipient. For example, a white mage casting a powerful AOE healing spell might generate
    emnity for all nearby enemies, causing them to focus on the healer, even if they weren't directly affected
    by the healing spell.

    When a party member first enter's an enemy's aggro radius and combat is initiated (based on Combat::aggroDist),
    the CombatSystem will provide the enemy's emnity table with an initial entry giving the triggering party member
    an emnity value of 1, causing it to be the initial target.

    \section3 Determining who to Target
    The emnity table is listed in descending order of emnity values, meaning the first entry in the list is the Entity
    that should be targeted with the highest emnity. A reference to this Entity can be fetched with the following
    function call:
    \code
        local entityToTarget = aiEntity:getCombat():getAggroManager():getEntityAtEmnityPriority(1)
    \endcode
    Where a priority value of '1' is the highest priority, and a priority value of '2' will be the second-highest priority
    (and thus has the second-highest emnity value)

    \section2 Aggroed List
    The AggroManager also contains a list of Entities that are currently targetting the parent Entity at any point in time.
    In other words, this is a list of Entities for which the highest entry in their Aggro table is the parent Entity.

    \sa CombatStatusModifier, Combat, CombatSystem, CombatAttrib
*/
/*!
 * \qmlmethod Entity* AggroManager::getEntityAtEmnityPriority(unsigned priority)
 *
 * Returns a reference to the Entity in the emnity table at the index given by \a priority. The emnity table is always
 * sorted in descending order with the highest emnity entry at the first index, meaning table index corresponds to priority.
 *
 * \e {NOTE: nil is returned if there is no entry with the associated index/priority.}
 * \sa getEmnityTableSize(), getEntityEmnity()
 */
/*!
 * \qmlmethod int AggroManager::getEntityEmnity(Entity* entity)
 * Returns the accumulated emnity value associated with the given \a entity.
 * \e {NOTE: Returns 0 if there is no table entry for \a entity, so this function is
 * always valid for any entity.}
 * \sa setEntityEmnity(), addEntityEmnity()
 */
/*!
 * \qmlmethod int AggroManager::getEmnityTableSize()
 *  Returns the number of entries in the emnity table.
 */
/*!
 * \qmlmethod void AggroManager::addEntityEmnity(Entity* source, int emnityDelta)
 * Adds \a{emnityDelta} to the emnity value associated with the \a source Entity
 * or inserts a new entry into the table with \a emnityDelta as the initial emnity
 * value for \a source if an entry does not already exist, and \a emnityDelta is positive.
 * \sa setEntityEmnity(), getEntityEmnity()
 */
/*!
 * \qmlmethod void AggroManager::setEntityEmnity(Entity* source, int emnityVal)
 *  Sets the emnity value associated with the \a source Entity to \a emnityVal,
 *  overwriting any existing values if there was a previous value.
 * \e {NOTE: An \a emnityVal of 0 can be passed in to remove the \a source Entity
 *  from the emnity table.}
 *
 * \sa addEntityEmnity(), getEntityEmnity()
 */
/*!
 * \qmlmethod int AggroManager::getAggroedCount()
 *  Returns the number of Entities currently targetting the parent Entity.
 */
/*!
 * \qmlmethod Enity* AggroManager::getAggroedEntity(unsigned index)
 *  Returns the Entity at the given \a index in the aggroed entity list (or nil
 *  if there isn't one at the given index).
 *
 * \sa getAggroedCount()
 */


OOLUA_EXPORT_FUNCTIONS_CONST(AggroManager,
                             getEntityAtEmnityPriority,
                             getEntityEmnity,
                             getEmnityTableSize,
                             //getEmnityTableEntry,
                             getAggroedCount,
                             getAggroedEntity)
OOLUA_EXPORT_FUNCTIONS(AggroManager,
                       addEntityEmnity,
                       setEntityEmnity)
/*!
    \qmltype Combat
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Component attached to an Entity to give it combat stats and behaviors.

    Combat is an optional, attachable Component that can be added to an Entity to signify to the CombatSystem
    that it can participate in combat. Any Entity that can send or receive damage should have a combat component.
    This includes party members, friendly helpers, enemies, and even weapons and projectiles that can deal combat
    damage.

    \section2 Configuring an Entity for Combat

    \sa CombatStatus, CombatSystem, CombatAttrib, AggroManager
*/
/*!
\qmlproperty enumeration Combat::DMG_TYPE

Represents the collision damage types of a combatant.

The following is a list o the different collision damage types:
\list
\li Combat.DMG_NONE             - Does not deal or receive damage from collisions
\li Combat.DMG_SOURCE           - Can deal collision damage to another combatant
\li Combat.DMG_TARGET           - Can receive collision damage from another combatant
\li Combat.DMG_SOURCE_TARGET    - Deals AND receives collision damage
\li Keyboard.KEY_D
\endlist
*/
/*!
 * \qmlmethod AggroManager* Combat::getAggroManager()
 *
 * Returns the AggroManager object for the associated combatant.
 * \sa AggroManager
 */
OOLUA_EXPORT_FUNCTIONS_CONST(Combat,
                             getAttrib,
                             getTargetType,
                             getDmgActive,
                             getDmgType,
                             getAggroDist,
                             getDeaggroDist,
                             isInCombat)
OOLUA_EXPORT_FUNCTIONS(Combat,
                       setSelfTable,
                       setActionTable,
                       getSelfTable,
                       getActionTable,
                       setAttrib,
                       setDmgType,
                       setDmgActive,
                       setCollisionFlags,
                       setStatus,
                       setTargetType,
                       setAggroDist,
                       setDeaggroDist,
                       getCollisionFlags,
                       getStatus,
                       getAggroManager)
