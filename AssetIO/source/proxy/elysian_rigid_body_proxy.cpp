#include <proxy/elysian_entity_proxy.hpp>


/*!
    \qmltype RigidBodyAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Attribute data used by the RigidBody Compoonent.

    RigidBodyAttrib is a collection of miscellaneous attributes that gives
    a \l{RigidBody} characteristics such as its durability, force mask, and
    whether or not it can be picked up and held.

    These data are contained within a separate object so that multiple similar
    \l{RigidBody} Components can share the same attribute object to prevent
    duplicate run-time data and simplify managing groups of similar Entities.

    \sa RigidBody, RigidBodySystem, ForceGenerator
*/
/*!
\qmlproperty bool RigidBodyAttrib::canHold

Returns true if the Entity can be picked up and held and false otherwise.
This defaults to true.
\sa getCanHold(), setCanHold()
*/
/*!
\qmlproperty bool RigidBodyAttrib::breakable

Returns true if the Entity can be broken by being thrown around and beaten up.
This defaults to false.
\sa isBreakable(), setBreakable()
*/
/*!
\qmlproperty float RigidBodyAttrib::durability

Floating point variable representing how easily the object can be broken.
The default value is -1, since the \l{breakable} property is false by default.
\sa getBaseDurability(), setBaseDurability()
*/
/*!
\qmlproperty BitMask32* RigidBodyAttrib::forceMask

32-bit mask used to flag which force types have an effect on the Entity.
This allows you to model certain forces such as magnetism that can only
effect certain Entity types. In this case, you would pick one of the 32
bits to represent magnetic force and set it to true in the forceMask.
By default, all bits are set to true in the mask, so all forces effect
the Entity.
\sa getForceMask(), setForceMask()
*/
/*!
 * \qmlmethod bool RigidBodyAttrib::getCanHold()
 *
 * Returns the value of the \l{canHold} property.
 * \sa canHold, setCanHold()
 */
/*!
 * \qmlmethod void RigidBodyAttrib::setCanHold(bool val)
 *
 * Sets the value of the \l{canHold} property to \a{val}.
 * \sa canHold, getCanHold()
 */
/*!
 * \qmlmethod bool RigidBodyAttrib::isBreakable()
 *
 * Returns the value of the \l{breakable} property.
 * \sa breakable, setBreakable()
 */
/*!
 * \qmlmethod void RigidBodyAttrib::setBreakable(bool val)
 *
 * Sets the value of the \l{breakable} property to \a{val}.
 * \sa breakable, isBreakable()
 */
/*!
 * \qmlmethod float RigidBodyAttrib::getBaseDurability()
 *
 * Returns the value of the \l{durability} property.
 * \sa durability, setBaseDurability()
 */
/*!
 * \qmlmethod void RigidBodyAttrib::setBaseDurability(float val)
 *
 * Sets the value of the \l{durability} property to \a{val}.
 * \sa durability, getBaseDurability()
 */
/*!
 * \qmlmethod BitMask32* RigidBodyAttrib::getForceMask()
 *
 * Returns the value of the \l{forceMask} property.
 * \sa forceMask, setForceMask()
 */
/*!
 * \qmlmethod void RigidBodyAttrib::setForceMask(BitMask32 mask)
 *
 * Sets the value of the \l{forceMask} property to \a{mask}.
 * \sa forceMask, getForceMask()
 */

OOLUA_EXPORT_FUNCTIONS_CONST(RigidBodyAttrib,
                             getCanHold,
                             isBreakable,
                             getBaseDurability)
OOLUA_EXPORT_FUNCTIONS(RigidBodyAttrib,
                       setForceMask,
                       setBreakable,
                       setBaseDurability,
                       setCanHold,
                       getForceMask)


/*!
    \qmltype ForceTarget
    \since 1.6.0
    \inmodule Engine
    \brief Abstraction for an object which is simulated by the \l{RigidBodySystem}.

    ForceTarget is an abstraction for a physical object that can be numerically
    integrated by the \l{RigidBodySystem} and can receive forces from \l{ForceGenerator}
    objects. A ForceTarget provides velocity, acceleration, rotation, force
    accumulators and other kinetic attributes used for rigid body simulation.

    This is an abstract parent class and cannot be instantiated from Lua.
    This class is implemented by both the \l{RigidBody} \l{Component} and
    by particles which are emitted from a \l{ParticleEmitter}.

    \section2 Force Application
    To apply a force to a ForceTarget, use one of the following methods:
    \list
    \li \l{applyForce} - Applies a force uniformly, resulting in linear motion. Good for modeling
    force fields such as gravity and wind.
    \li \l {applyForceToPoint} - Applies a force to a specific point, resulting in both linear and
    rotational motion. This is required for modeling any force that is not applied to the entire
    body evenly, such as pushing or hitting an object.
    \endlist

    \section2 Linear Motion from Forces
    Forces applied to a ForceTarget are added together and accumulated in \l{forceAccum}
    over the duration of the frame. At the end of the frame, the resultant force
    vector gets converted into a change in \l{accel} and is applied to the ForceTarget.

    The amount of acceleration resulting from a single force vector can be determined
    using the equation (derived from F=ma):
    \code
    acceleration = force / mass
    \endcode

    \section2 Angular Motion from Forces
    Only applying a force through \l{applyForceToPoint()} using a specific point can result in
    rotational motion by generating torque from a linear force.

    The amount of resulting angular acceleration from a linear force applied to a point can
    be determined by the following formulas:
    \code
    torque = DOT_PRODUCT( PERPENDICULAR(forcePositionRelativeToPlayer), linearForce )
    angAccel = torque / momentOfInertia
    \endcode

    \e {NOTE: If a force is applied to the exact center of the Entity, it's equivalent to
    applying a uniform force to the entire body, meaning no rotation is generated. You can
    see this in the formula, since the dot product between a position vector of <0, 0> and
    anything else will yield 0 for torque.}

    \sa RigidBodySystem, ForceGenerator
*/
/*!
\qmlproperty Vector3* ForceTarget::vel

3D vector representing the velocity or rate of change of the position of a ForceTarget.
\sa getVel()
*/
/*!
\qmlproperty Vector3* ForceTarget::accel

3D vector representing the acceleration or rate of change of the velocity of a ForceTarget.
\sa getAccel()
*/
/*!
\qmlproperty Vector3* ForceTarget::forceAccum

3D vector representing the total sum of all forces applied to the ForceTarget over the
current frame. This is converted to a change in the ForceTarget's acceleration and is
cleared at the end of every frame.
\sa getForceAccum(), {Linear Motion from Forces}
*/
/*!
\qmlproperty float ForceTarget::rotVel

Scalar value representing the rotational velocity or rate of change of orientation of
a ForceTarget.
\sa getRotation(), setRotation()
*/
/*!
\qmlproperty float ForceTarget::angAccel

Scalar value representing the angular acceleration or rate of change of rotational
velocity of a ForceTarget.
\sa getAngAccel(), setAngAccel()
*/
/*!
\qmlproperty float ForceTarget::torqueAccum

Scalar value representing the total sum of the rotational component of all forces
applied to the ForceTarget over the current frame. At the end of the frame,
this value is converted into a change in angular acceleration and is cleared.
\sa getTorqueAccum(), setTorqueAccum(), {Angular Motion from Forces}
*/
/*!
 * \qmlmethod Vector3 ForceTarget::getVel()
 *
 * Returns the value of the \l{vel} property.
 * \sa vel
 */
/*!
 * \qmlmethod Vector3 ForceTarget::getAccel()
 *
 * Returns the value of the \l{accel} property.
 * \sa accel
 */
/*!
 * \qmlmethod Vector3 ForceTarget::getForceAccum()
 *
 * Returns the value of the \l{forceAccum} property.
 * \sa forceAccum
 */
/*!
 * \qmlmethod float ForceTarget::getRotation()
 *
 * Returns the value of the \l{rotVel} property.
 * \sa rotVel, setRotation()
 */
/*!
 * \qmlmethod void ForceTarget::setRotation(float rot)
 *
 * Sets the value of the \l{rotVel} property to \a{rot}.
 * \sa rotVel, getRotation()
 */
/*!
 * \qmlmethod float ForceTarget::getAngAccel()
 *
 * Returns the value of the \l{angAccel} property.
 * \sa angAccel, setAngAccel()
 */
/*!
 * \qmlmethod void ForceTarget::setAngAccel(float accel)
 *
 * Sets the value of the \l{angAccel} property to \a{accel}.
 * \sa angAccel, getAngAccel()
 */
/*!
 * \qmlmethod float ForceTarget::getTorqueAccum()
 *
 * Returns the value of the \l{torqueAccum} property.
 * \sa torqueAccum, setTorqueAccum()
 */
/*!
 * \qmlmethod void ForceTarget::setTorqueAccum(float accum)
 *
 * Sets the value of the \l{torqueAccum} property to \a{accum}.
 * \sa torqueAccum, getTorqueAccum()
 */
/*!
 * \qmlmethod void ForceTarget::applyForce(Vector3 force)
 *
 * Applies the given \a{force} uniformly to all points on the ForceTarget.
 * This will result in only linear motion.
 * \sa {Force Application}, applyForceToPoint()
 */
/*!
 * \qmlmethod void ForceTarget::applyForceToPoint(Vector2 pos, Vector2 force)
 *
 * Applies the given \a{force} to the world location given by \a{pos}. If \a{pos}
 * is not in the center of the Entity, the force will result in both linear
 * and rotational motion
 * \sa {Force Application}, applyForce()
 */
OOLUA_EXPORT_FUNCTIONS(ForceTarget,
                       applyForce,
                       applyForceToPoint,
                       clearKinetic,
                       getVel,
                       getAccel,
                       getForceAccum,
                       setRotation,
                       setAngAccel,
                       setTorqueAccum)
OOLUA_EXPORT_FUNCTIONS_CONST(ForceTarget,
                             getRotation,
                             getAngAccel,
                             getTorqueAccum)



/*!
    \qmltype RigidBody
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Component providing an Entity with full rigid body kinematics.

    RigidBody is a \l{Component} that can be attached to an \l{Entity} to
    give it realistic rigid body kinematics such as velocity, acceleration,
    force accumulation, angular rotation and acceleration, and torque.
    A RigidBody that collides with other \l{Collidable}s or RigidBodies
    transfers and receives momentum in a realistic manner and can be
    configured to optionally break when a certain force threshold is
    exceeded. RigidBodies can also have their motion damped by certain
    terrain to simulate slippery or sticky surfaces.

    \section2 Component Prerequesites
    The following is a Component dependency and must be attached
    to an Entity before a RigidBody can be attached.
    \list
    \li \b{\l{Collidable}} - Required for collision geometry and static attributes.
    \endlist

    \section2 RigidBodyAttrib as Attributes Property
    Every active RigidBody MUST have a corresponding RigidBodyAttrib object set as
    its \l{attrib} property. If this property is nil, behavior is undefined (aka
    there's not a chance in hell it won't crash). Multiple similiar RigidBodies should
    share the same RigidBodyAttrib object to reduce on memory overhead and make
    configuration easier for groups of RigidBodies.

    \section2 Simulation Stability
    One of the most difficult parts of a rigid body physics engine is stability. As this
    is still a work in progress, you may encounter a few scenarios where the simulation
    becomes unstable. Here are a few things to look out for:
    \list
    \li \b{Low Framerates} - Low framerates force the physics engine to integrate in much
    larger steps to keep gameplay framerate independent. This prevents "slow down," but
    if framerates become sufficiently low, RigidBodies may move right through smaller
    Colliders before a collision can be detected. Make sure to not use tiny collider
    sizes to protect against this.
    \li \b{Large Amounts of Colliding Bodies} - The physics engine attempts to iteratively
    resolve each collision in order from deepest to shallowest. In the case of large
    amounts of collisions or lots of bodies resting on each other, the physics engine
    may not be able to resolve all of the collisions before it runs out of iterations. This
    can result in unstable behavior such as walking through solids or RigidBodies going
    shooting off into space as the next frame when the \l{CollisionSystem} detects the
    collision it may be unrealisticaly deep.
    \li \b{Incorrect Moments of Inertia} - CollidableStaticAttrib::invMomOfInertia is the
    main value used to determine rotational kinetics from forces and collisions. If this
    value is unrealistically low relative to the object's mass, the simulation can become
    severely unstable, as the resulting unrealistically massive amounts of rotation can bump
    into other Entities and eject from the map. Make sure to carefully choose and tweak
    the CollidableStaticAttrib::invMomOfInertia to ensure that it's a sane value. There are
    online calculators and formulas you can use to determine the moment of inertia for a
    rectangular object.
    \endlist

    \section2 Performance and Optimizations
    While the underlying collision and mathematics driving the CollisionSystem and
    PhysicsSystem are pretty efficient and even assembly-optimized in some cases,
    physics simulations are still a relatively taxing process.

    A few things to consider:
    \list
    \li Only use the RigidBody Component for Entities that actually need it. If the Entity
    isn't even moving, just a Collidable alone has all of the information required to define
    how other RigidBodies bounce off of it and interact with it.
    \li Make sure to use the strictest masks possible for the CollisionFlags::type and
    CollisionFlags::typeAgainst fields in CollidableAttrib::flags. This can save a shitload of time
    not bothering with collision checks for objects that should never interact to begin with. For
    example, an enemy's CollisionFlags::typeAgainst property should not include checking for collision
    against warps!
    \li ALWAYS use gyro.COLLIDER_AABB rather than gyro.COLLIDER_OBB for CollidableAttrib::colliderType
    when dealing with Entities that cannot rotate. Also make sure that CollidableStaticAttrib::invMomOfInertia
    is set to 0 in this case as well. Oriented bounding boxes are significantly slower than axis-aligned
    bounding boxes, as they require additional information after every collision for the
    point-of-contact and significantly more calculations for integration and collision resolution with rotations.
    Everything is optimized so that this additional overhead is avoided for axis-aligned bounding boxes.
    \li A bunch of Entities stacked ontop of each other or colliding in a cluster is the
    absolute worst-case for scenario for performance. The physics engine will be wasting a shitload of time
    resolving and reresolving a billion microcollisions every frame. A metric shitload of Entities all
    on screen but not making contact is going to perform significantly better than a smaller group that
    is all clustered together.
    \li Clustered groups of Entities constantly making contact with each other are going to perform
    better when clustered on the X axis rather than on the Y axis due to the nature of the Prune and
    Sweep broad-phase collision algorithm the engine uses. The X axis is favored on purpose, as clustering
    on that axis is significantly more common in the case where gravity is present and all objects are being
    pulled to rest on the ground. The algorithm is better able to skip expensive collision checks for bodies
    that it knows cannot possibly be colliding.
    \li Ensure that RigidBodyAttrib::forceMask is set as conservatively as possible as with the collision masks.
    This will ensure that \l{ForceGenerator}s are not unecesssarily applying forces to bodies that should not
    be effected by them.
    \endlist

    \sa ForceTarget, RigidBodyAttrib, CollidableStaticAttrib, Collidable, RigidBodySystem
*/
/*!
\qmlproperty float RigidBody::tileDamping

Read-only property containing the damping multiplier from the \l{Tile}
the Entity currently rests on. A multiplier of 1.0 means no momentum is
lost on the \l{Tile}. A multiplier of 0.5 means half of the momentum is lost
every frame when moving through the \l{Tile}.
\sa getTileDamping(), Tile::setDamping()
*/
/*!
\qmlproperty float RigidBody::durability

Property containing the floating point durability value representing how
much "damage" the Entity can take before breaking. This is set to -1.0
by default, meaning the Entity is not breakable. The initial value
(or maximum HP before breaking) is set from RigidBodyAttrib::durability.
\sa getDurability(), setDurability(), RigidBodyAttrib::durability, RigidBodyAttrib::breakable
*/
/*!
\qmlproperty RigidBodyAttrib* RigidBody::attrib

Reference to an attribute object of type \l{RigidBodyAttrib}, giving a RigidBody additional
information required by the engine, or nil if one hasn't been set. It is nil by default.
\sa getAttrib(), setAttrib()
*/
/*!
 * \qmlmethod float RigidBody::getTileDamping()
 *
 * Returns the value of the \l{tileDamping} property.
 * \sa tileDamping
 */
/*!
 * \qmlmethod float RigidBody::getDurability()
 *
 * Returns the value of the \l{durability} property.
 * \sa durability, setDurability()
 */
/*!
 * \qmlmethod void RigidBody::setDurability(float val)
 *
 * Sets the value of the \l{durability} property to \a{val}.
 * \sa durability, getDurability()
 */
/*!
 * \qmlmethod RigidBodyAttrib* RigidBody::getAttrib()
 *
 * Returns the value of the \l{attrib} property.
 * \sa attrib, setAttrib()
 */
/*!
 * \qmlmethod void RigidBody::setAttrib(RigidBodyAttrib* attr)
 *
 * Sets the value of the \l{attrib} property to \a{attr}.
 * Ownership of \a{attr} is transferred from Lua to the engine.
 * \sa attrib, getAttrib()
 */

OOLUA_EXPORT_FUNCTIONS_CONST(RigidBody,
                             getDurability,
                             getAttrib,
                             getTileDamping)
OOLUA_EXPORT_FUNCTIONS(RigidBody,
                       setAttrib,
                       setDurability)

