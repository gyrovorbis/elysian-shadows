/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_RIGID_BODY_SYSTEM_PROXY_HPP
#define ELYSIAN_RIGID_BODY_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/system/elysian_rigid_body_system.hpp>
#include <ESGamma/utility/elysian_force_generator.hpp>

namespace OOLUA {
    typedef elysian::ForceGenerator             ForceGenerator;
    typedef elysian::GravityForceGenerator      GravityForceGenerator;
    typedef elysian::ImplosionForceGenerator    ImplosionForceGenerator;
    typedef elysian::ExplosionForceGenerator    ExplosionForceGenerator;
    typedef elysian::RigidBodySystem            RigidBodySystem;
}


OOLUA_PROXY(ForceGenerator)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setForceMask, BitMask32)
OOLUA_PROXY_END

OOLUA_PROXY(GravityForceGenerator, ForceGenerator)
    OOLUA_MEM_FUNC(void, setAccel, const GYVector3&)
OOLUA_PROXY_END

OOLUA_PROXY(ImplosionForceGenerator, ForceGenerator)
    OOLUA_MEM_FUNC(void, setFocalEntity, Entity*)
    OOLUA_MEM_FUNC(void, setForceMagnitude, float)
    OOLUA_MEM_FUNC(void, setRadius, float)
OOLUA_PROXY_END

OOLUA_PROXY(ExplosionForceGenerator, ForceGenerator)
    OOLUA_MEM_FUNC(void, setFocalEntity, Entity*)
    OOLUA_MEM_FUNC(void, setForceMagnitude, float)
    OOLUA_MEM_FUNC(void, setRadius, float)
OOLUA_PROXY_END


OOLUA_PROXY(RigidBodySystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addForce, cpp_in_p<ForceGenerator*>)
    OOLUA_MEM_FUNC(void, applyForce, ForceGenerator*)
    OOLUA_MEM_FUNC(void, removeForce, ForceGenerator*)
    OOLUA_MEM_FUNC(void, clearForces)
OOLUA_PROXY_END



#endif // ELYSIAN_RIGID_BODY_SYSTEM_HPP

