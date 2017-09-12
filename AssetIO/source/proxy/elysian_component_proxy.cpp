#include <proxy/elysian_entity_proxy.hpp>


/*!
    \qmltype Component
    \since 1.6.0
    \inmodule Engine
    \brief Base class for all attachable Entity components.

    Component is the base class from which all other components within the engine are inherited.
    A Component is an object that can be attached to an Entity to give it certain characteristics.
    For example, a solid Entity should have a Collidable component attached to it, and an Entity that
    uses skeletal animations should have a Skeleton component attached to it.

*/

/*!
 * \qmlmethod Entity* Component::getEntity()
 *
 * Returns a reference to the parent Entity the component is currently attached to.
 */

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Component)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Component, getEntity)
