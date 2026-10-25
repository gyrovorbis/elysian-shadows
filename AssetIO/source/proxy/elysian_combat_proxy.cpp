/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

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
    There are a few things that need to be taken into account to give the \l CombatSystem enough
    information to manage combat between two entities.

    The following properties need to configured properly on each combatant:
    \table
        \header
            \li \b {Property}
            \li \b {Description}
        \row
            \li Combat::dmgType
            \li Combatants capable of giving collision-based damage should be a form of a SOURCE, while
                combatants capable of sending collision-based damage should be a form of a TARGET.
       \row
            \li Combat::targetType
            \li Category of combatant falls under. At very least one Entity will need to be a Combat::TARGET_ENEMY while the
                other either Combat::TARGET_PARTY or Combat::TARGET_FRIENDLY to give the CombatSystem something to fight with.
       \row
            \li Combat::aggroDist
            \li If you intend for combat to be initiated automatically through "aggro," you must at least set the \l aggroDist of
                the enemy combatant.
       \row
            \li Combat::status
            \li For the CombatSystem to do its thing wth any combatant, they must have at least 1 HP left to be considered alive.
       \row
            \li Combat::attrib
            \li At VERY LEAST, the CombatAttrib::type and CombatAttrib::typeAgainst flags must be configured properly when expecting
                damage to be dealt as a result of collisions. These work the exact same awy as CollisionFlags with Collidables. Upon
                collision, the CombatSystem will bitwise AND the CombatAttrib::type of one combatant with the CombatAttrib::typeAgainst
                of the other to determine whether they form a valid damage-dealing collision.
    \endtable

    \e {NOTE: Each combatant needs to have its dmgActive flag set to true for the two to be able to damage each other.
              They will still be able to "engage" in combat as valid combatants in the CombatSystem, but they will not
              be able to harm each other (imagine some temporary invincibility spell for example). }
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
\endlist
*/
/*!
\qmlproperty enumeration Combat::TARGET_TYPE

Represents the different categories of Combatants, from the player's perspective.
This value is used when determining who to deal damage to and from (ie preventing
friendly-fire from player-on-player or enemy-on-enemy.

The following is a list of the valid values for Combat::TARGET_TYPE.
\table
    \header
        \li \b{Constant}
        \li \b{Description}
     \row
        \li TARGET_NONE
        \li Combatant will not participate in combat.
    \row
        \li TARGET_PARTY
        \li Combatant is a playable party member controlled by a human or party AI.
    \row
        \li TARGET_FRIENDLY
        \li Combatant is a non-party AI-drven NPC/animal/pet helping the party.
    \row
        \li TARGET_ENEMY
        \li Combatant is fighting against party members and friendly AI combatants.
\endtable
*/

/*!
 * \qmlproperty Combat::DMG_TYPE Combat::dmgType
 *
 * The Combat::DMG_TYPE associated with this combatant.
 * \sa getDmgType, setDmgType
 */
/*!
 * \qmlproperty float Combat::aggroDist
 *
 * World-space radius designating the aggro region of a combatant. Entering
 * this radius causes them to aggro.
 * \sa getAggroDist, setAggroDist
 */
/*!
 * \qmlproperty float Combat::deaggroDist
 *
 * World-space radius designating the region beyond which a combatant becomes
 * deaggroed. When another combatant exits this radius, the combatant associated
 * with this Combat Componenet will no longer be aggroed.
 * \sa getDeaggroDist, setDeaggroDist, aggroDist
 */
/*!
 * \qmlproperty Combat::TARGET_TYPE Combat::targetType
 * The Combat::TARGET_TYPE associated with this combatant.
 * \sa getTargetType, setTargetType
 */
/*!
 * \qmlproperty bool Combat::inCombat
 * Read-Only property that corresponds to whether the combatant is currently
 * engaged in combat or not.
 * \sa isInCombat
 */
/*!
 * \qmlproperty CombatStatus Combat::status
 * CombatStatus object representing the engine-relevant attributes and stats
 * of the current combatant, such as HP and MP.
 * \sa getStatus, setStatus
 */
/*!
 * \qmlproperty CombatAttrib* Combat::attrib
 * CombatAttrib object associated with the combatant.
 * \sa getAttrib, setAttrib
 */
/*!
 * \qmlmethod AggroManager* Combat::getAggroManager()
 *
 * Returns the AggroManager object for the associated combatant.
 * \sa AggroManager
 */
/*!
 * \qmlmethod Combat::DMG_TYPE Combat::getDmgType()
 *
 * Returns the Combat::DMG_TYPE associated with this combatant.
 * \sa dmgType, setDmgType
 */
/*!
 * \qmlmethod void Combat::setDmgType(Combat::DMG_TYPE dmgType)
 *
 * Sets the Combat::DMG_TYPE associated with this combatant to \a dmgType.
 * \sa dmgType, getDmgType
 */
/*!
 * \qmlmethod float Combat::getAggroDist()
 *
 * Returns the value of the aggroDist property.
 * \sa aggroDist, setAggroDist
 */
/*!
 * \qmlmethod void Combat::setAggroDist(float value)
 *
 * Sets the value of the aggroDist property to \a value.
 * \sa aggroDist, getAggroDist
 */
/*!
 * \qmlmethod float Combat::getDeaggroDist()
 *
 * Returns the value of the deaggroDist property.
 * \sa deaggroDist, setDeaggroDist
 */
/*!
 * \qmlmethod void Combat::setDeaggroDist(float value)
 *
 * Sets the value of the deaggroDist property to \a value.
 * \sa deaggroDist, getDeaggroDist
 */
/*!
 * \qmlmethod bool Combat::isInCombat()
 *
 * Returns the value of the inCombatProperty
 * \sa inCombat
 */
/*!
 * \qmlmethod Combat::TARGET_TYPE Combat::getTargetType()
 *
 * Returns the value of the targetType property.
 * \sa targetType, setTargetType
 */
/*!
 * \qmlmethod void Combat::setTargetType(Combat::TARGET_TYPE value)
 *
 * Sets the value of the targetType property to \a value.
 * \sa targetType, getTargetType
 */
/*!
 * \qmlmethod CobmatAttrib* Combat::getAttrib()
 *
 * Returns the value of the \l attrib property.
 * \sa attrib, setAttrib
 */
/*!
 * \qmlmethod void Combat::setAttrib(CombatAttrib* value)
 *
 * Sets the value of the attrib property to \a value.
 * \sa attrib, getAttrib
 */
/*!
 * \qmlmethod CombatStatus Combat::getStatus()
 *
 * Returns the value of the \l status property.
 * \sa status, setStatus
 */
/*!
 * \qmlmethod CombatStatus Combat::setStatus(CombatStatus value)
 *
 * Sets the value of the \l status property to \a value.
 * \sa status, getStatus
 */
/*!
 * \qmlmethod bool Combat::getDmgActive()
 *
 * Returns true if the combatant is currently able to send and receive damage,
 * for example, this would be falso for a second or so for temporary invulnerability
 * after taking damage.
 * \sa setDmgActive
 */
/*!
 * \qmlmethod void Combat::setDmgActive(bool value)
 *
 * Sets whether the combatant is current able to deal or take damage.
 * \sa getDmgActive
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
