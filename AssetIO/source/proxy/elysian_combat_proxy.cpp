#include <proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype CombatAttrib
    \since 1.6.0
    \inmodule ESToolkit
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
    \inmodule ESToolkit
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

OOLUA_EXPORT_FUNCTIONS_CONST(CombatStatusModifier)
OOLUA_EXPORT_FUNCTIONS(CombatStatusModifier,
                       addStatusEffect,
                       removeStatusEffect)

/*!
    \qmltype Combat
    \inherits Component
    \since 1.6.0
    \inmodule ESToolkit
    \brief Component attached to an Entity to give it combat stats and behaviors.

    Combat is an optional, attachable Component that can be added to an Entity to signify to the CombatSystem
    that it can participate in combat. Any Entity that can send or receive damage should have a combat component.
    This includes party members, friendly helpers, enemies, and even weapons and projectiles that can deal combat
    damage.

    \section2 Configuring an Entity for Combat

    \sa CombatStatus, CombatSystem, CombatAttrib
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
#if 0
s->register_class_static<Combat>("DMG_NONE", Combat::DMG_NONE);
s->register_class_static<Combat>("DMG_SOURCE", Combat::DMG_SOURCE);
s->register_class_static<Combat>("DMG_TARGET", Combat::DMG_TARGET);
s->register_class_static<Combat>("DMG_SOURCE_TARGET", Combat::DMG_SOURCE_TARGET);

s->register_class_static<Combat>("TARGET_NONE", Combat::TARGET_NONE);
s->register_class_static<Combat>("TARGET_PARTY", Combat::TARGET_PARTY);
s->register_class_static<Combat>("TARGET_FRIENDLY", Combat::TARGET_FRIENDLY);
s->register_class_static<Combat>("TARGET_ENEMY", Combat::TARGET_ENEMY);

s->register_class_static<Combat>("ON_COLLIDE", Combat::ON_COLLIDE);
s->register_class_static<Combat>("ON_COLLISION_ENTER", Combat::ON_COLLISION_ENTER);
s->register_class_static<Combat>("ON_COLLISION_EXIT", Combat::ON_COLLISION_EXIT);


ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_FIELD(dmgType,        getDmgType,         setDmgType)
    ELYSIAN_LUA_PROXY_FIELD(collisionFlags, getCollisionFlags,  setCollisionFlags)
    ELYSIAN_LUA_PROXY_FIELD(aggroDist,      getAggroDist,       setAggroDist)
    ELYSIAN_LUA_PROXY_FIELD(deaggroDist,    getDeaggroDist,     setDeaggroDist)
    ELYSIAN_LUA_PROXY_FIELD(targetType,     getTargetType,      setTargetType)
    ELYSIAN_LUA_PROXY_FIELD(inCombat,       isInCombat)
    ELYSIAN_LUA_PROXY_FIELD(status,         getStatus,          setStatus)
    ELYSIAN_LUA_PROXY_FIELD(attrib,         getAttrib,          setAttrib)
ELYSIAN_LUA_PROXY_FIELDS_END()

#endif

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
                       getStatus)
