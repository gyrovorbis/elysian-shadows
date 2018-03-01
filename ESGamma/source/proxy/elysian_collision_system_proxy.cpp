#include <proxy/elysian_collision_system_proxy.hpp>

/*!
    \qmltype CollisionSystem
    \since 1.6.1
    \inmodule Engine
    \brief Engine subsystem for detecting collisions and managing contacts

    The CollisionSystem is the \l{Engine} subsystem responsible for managing
    all active \l{Collidable} \l{Component}s within the current \l{Area}.
    It handles also detecting collisions among all active Entities, generating contacts, generating
    collision events, and resolving the penetration component of each contact.

    CollisionSystem is also responsible for checking each Entity with a collidable
    against the tiles on the map as well.

    Most of the functionality it implements is broad-phase collision checks and contact
    management, delegating most of the low-level primitive collision math to
    the libKineticGyro API.

    The majority of what the CollisionSystem has to offer is driven by the usage of the
    Collidable Component on active Entities; however, it does offer some additional
    functionality for querying geometric primitives against in-game objects to generate
    contact lists for a particular region of in-game space and other useful utility functions.

    \e{Note: The velocity component of contact resolution is handled by the \l{RigidBodySystem}}.


    \section2 Broad-Phase Collision

    \section2 Fine-Phase Collision

    \section3 CollisionType and CollisionTypeAgainst Flags

    \section3 Supported Geometry



    \section2 Retrieving the CollisionSystem
    The CharacterSystem object can be retrieved through the static Engine class:
    \code
    charSys = Engine.getCollisionSys()
    \endcode
    \sa {Collidable}, {CollisionContact}, {RigidBody}, {RigidBodySystem}, {Engine}
*/

OOLUA_EXPORT_NO_FUNCTIONS(CollisionSystem)
