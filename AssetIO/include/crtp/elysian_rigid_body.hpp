#ifndef ELYSIAN_RIGID_BODY_HPP
#define ELYSIAN_RIGID_BODY_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_rigid_body_base.hpp>
namespace elysian {
    typedef RigidBodyBase RigidBody;
}
#else
#   include <AssetIO/tk_rigid_body.h>
namespace elysian {
    typedef TkRigidBody RigidBody;
}
typedef TkRigidBody RigidBody;

#endif

#endif // ELYSIAN_RIGID_BODY_HPP

