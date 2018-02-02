#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_rigid_body_system_proxy.hpp>

/*!
    \qmltype RigidBodySystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine subsystem for managing physics and RigidBody Components.

    The RigidBodySystem is the \l{Engine} subsystem for managing all \{RigidBody}s attached
    to any active \l{Entity} within the scene. It handles force application, integration (updating
    acceleration, velocity, and position), and \l{Contact} resolution between two rigid bodies where
    momentum is exchanged between the two (as opposed to them simply being pushed apart as with
    static \l{Collidable}-only contacts}.

    \section2 Retrieving the RigidBodySystem
    The RigidBodySystem object can be retrieved through the static \l{Engine} class:
    \code
    rbSys = Engine.getRigidBodySys()
    \endcode

    \section2 Force Generators
    The RigidBodySystem manages all active \l{ForceGenerator} objects,
    updates them, and is responsible for applying their resulting forces to the affected
    Entities every frame.

    \sa RigidBody, RigidBodyAttrib, ForceGenerator, CollidableAttrib, Engine
*/
/*!
  \qmlmethod void RigidBodySystem::applyForce(ForceGenerator* generator)
  Applies the force given by the \l{ForceGenerator} \a{generator} to all
  Entities within the scene containing a \l{RigidBody} componnet with a
  corresponding forceMask that matches the one cotained by \a{generator}.
  Note this is just a single, instantaneous application of the given force!
  \sa addForce
*/
 /*!
  \qmlmethod void RigidBodySystem::addForce(ForceGenerator* generator)
  Adds the \l{ForceGenerator| object given by \a{generator} to the internal
  list of active generators which will be automatically applied to all
  matching Entities every frame. This is for persistant forces such as
  wind or gravity that need to be applied every frame, rather than
  single-shot forces like knock-backs, which should use \l{applyForce}.
  \sa removeForce, clearForces, applyForce
*/
/*!
 \qmlmethod void RigidBodySystem::removeForce(ForceGenerator* generator)
 Removes the \l{ForceGenerator} object given by \l{generator} from the
 internal list of persistant forces which are automatically applied
 every frame. For example, maybe a wind effect has subsided.
 \sa addForce, clearForces, applyForce
*/
/*!
 \qmlmethod void RigidBodySystem::clearForces()
 Removes all \l{ForceGenerator} objects within the internal list
 of persistent forces to be applied every frame.
 \sa addForce, removeForce, applyForce
*/

OOLUA_EXPORT_FUNCTIONS(RigidBodySystem,
                       applyForce,
                       addForce,
                       removeForce,
                       clearForces)
OOLUA_EXPORT_FUNCTIONS_CONST(RigidBodySystem)



/*!
    \qmltype ForceGenerator
    \since 1.6.0
    \inmodule Engine
    \brief Represents a force to be applied to all Entities of a matching type

    ForceGenerator is the base for all of the different types of forces which should
    be applied to a group of Entities. Since the force being applied by a ForceGenerator
    is recalculated based upon a function, they generate far more complex behaviors than
    can be repsented by simply applying static force vectors to entities.

    \section2 Force Masks
    A ForceGenerator can be applied to all entities containing \l{RigidBody} components
    with a scene, or only to a particular subset of them. For example, an enemy using a
    wind attack that temporarily pushes the party members around should not necessarily
    have an affect on other nearby enemies. Each ForceGenerator has a corresponding
    forceMask which is matched to the forceMask contained with the \{RigidBodyAttrib}
    objects to determine whether the force should be applied to that Entity. The two
    masks are bitwise ANDed together, and if the result is not zero, this produces a match.

    \section2 Applying Force Masks
    All ForceGenerators are managed by the \l{RigibBodySystem} and should be added there.

    \sa RigidBodySystem, GravityForceGenerator, ImplosionForceGenerator
*/
/*!
 \qmlmethod void ForceGenerator::setForceMask(BitMask32 mask)
 Sets the mask used to determine which rigid bodies should be affected by this generator to \{mask}.
*/


OOLUA_EXPORT_FUNCTIONS_CONST(ForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ForceGenerator,
                       setForceMask)

/*!
    \qmltype GravityForceGenerator
    \since 1.6.0
    \inherits ForceGenerator
    \inmodule Engine
    \brief Force Generator implementation for gravity-like acceleration effects.

    The GravityForceGenerator is a force generator that simply applies a constant acceleration to all
    \l{RigidBody} objects every frame. The acceleration is applied as a force, using F = ma.

*/

/*!
 \qmlmethod void GravityForceGenerator::setAccel(GYVector3 accel)
Sets the acceleration vector to be applied to each \l{RigidBody} every frame to be \a{accel}.
*/

OOLUA_EXPORT_FUNCTIONS_CONST(GravityForceGenerator)
OOLUA_EXPORT_FUNCTIONS(GravityForceGenerator,
                       setAccel)

/*!
    \qmltype ImplosionForceGenerator
    \since 1.6.0
    \inherits ForceGenerator
    \inmodule Engine
    \brief Force Generator for simulating imploding, attraction, or suction.

    The ImplosionForceGenerator applies a suction force causing all Entities within a
    given radius to be sucked towards a center focal Entity.

    \sa RigidBodySystem, GravityForceGenerator, ExplosionForceGenerator
*/
/*!
 \qmlmethod void ImplosionForceGenerator::setFocalEntity(Entity* entity)
Sets the given \a{entity} as the epicenter of the suction force which all affected Entities will be drawn towards.
*/
/*!
 \qmlmethod void ImplosionForceGenerator::setForceMagnitude(float mag)
Sets the magnitude of the suction force to \a{mag}. The higher the magnitude, the stronger the force, and the more quickly the
Entities will be sucked towards the center.

\e{NOTE: The direction of the force vector is determined by the direction of the affected Entity relative to the focal Entity}
*/
/*!
 \qmlmethod void ImplosionForceGenerator::setRadius(float radius)
Sets the region-of-effect to a circle whose origin is given by the focal Entity's position with the given \a{radius}.
*/

OOLUA_EXPORT_FUNCTIONS_CONST(ImplosionForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ImplosionForceGenerator,
                       setFocalEntity,
                       setForceMagnitude,
                       setRadius)

/*!
    \qmltype ExplosionForceGenerator
    \inherits ForceGenerator
    \since 1.6.0
    \inmodule Engine
    \brief ForceGenerator for simulating explosions and outward force.

    \sa RigidBodySystem, GravityForceGenerator, ImplosionForceGenerator
*/
/*!
 \qmlmethod void ExplosionForceGenerator::setFocalEntity(Entity* entity)
Sets the given \a{entity} as the epicenter of the explosion force which all affected Entities will be drawn towards.
*/
/*!
 \qmlmethod void ExplosionForceGenerator::setForceMagnitude(float mag)
Sets the magnitude of the explosion force to \a{mag}. The higher the magnitude, the stronger the force, and the more quickly the
Entities will be blown away from the center.

\e{NOTE: The direction of the force vector is determined by the direction of the affected Entity relative to the focal Entity}
*/
/*!
 \qmlmethod void ExplosionForceGenerator::setRadius(float radius)
Sets the region-of-effect to a circle whose origin is given by the focal Entity's position with the given \a{radius}.
*/


OOLUA_EXPORT_FUNCTIONS_CONST(ExplosionForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ExplosionForceGenerator,
                       setFocalEntity,
                       setForceMagnitude,
                       setRadius)
