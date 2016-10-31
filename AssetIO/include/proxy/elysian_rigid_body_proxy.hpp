#ifndef ELYSIAN_RIGID_BODY_PROXY_HPP
#define ELYSIAN_RIGID_BODY_PROXY_HPP

#include <crtp/elysian_rigid_body.hpp>


namespace OOLUA {
    typedef elysian::RigidBodyAttrib            RigidBodyAttrib;
    typedef elysian::ForceTarget                ForceTarget;
    typedef elysian::RigidBody                  RigidBody;
    typedef elysian::BitMask32                  BitMask32;
    typedef elysian::Vector3                    Vector3;

}

OOLUA_PROXY(RigidBodyAttrib)
    OOLUA_MEM_FUNC(void, setForceMask, const BitMask32)
    OOLUA_MEM_FUNC(BitMask32&, getForceMask)
    OOLUA_MEM_FUNC(void, setBreakable, bool)
    OOLUA_MEM_FUNC_CONST(bool, isBreakable)
    OOLUA_MEM_FUNC(void, setBaseDurability, float)
    OOLUA_MEM_FUNC_CONST(float, getBaseDurability)
    OOLUA_MEM_FUNC(void, setCanHold, const bool)
    OOLUA_MEM_FUNC_CONST(bool, getCanHold)
OOLUA_PROXY_END

OOLUA_PROXY(ForceTarget)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, applyForce, const GYVector3&)
    OOLUA_MEM_FUNC(void, applyForceToPoint, const GYVector2&, const GYVector2&)
    OOLUA_MEM_FUNC(void, clearKinetic)
    OOLUA_MEM_FUNC(Vector3, getVel)
    OOLUA_MEM_FUNC(Vector3, getAccel)
    OOLUA_MEM_FUNC(Vector3, getForceAccum)
    OOLUA_MEM_FUNC(void, setRotation, const float)
    OOLUA_MEM_FUNC(void, setAngAccel, const float)
    OOLUA_MEM_FUNC(void, setTorqueAccum, const float)
    OOLUA_MEM_FUNC_CONST(float, getRotation)
    OOLUA_MEM_FUNC_CONST(float, getAngAccel)
    OOLUA_MEM_FUNC_CONST(float, getTorqueAccum)
OOLUA_PROXY_END

OOLUA_PROXY(RigidBody, Component, ForceTarget)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<RigidBodyAttrib*>)
    OOLUA_MEM_FUNC_CONST(maybe_null<RigidBodyAttrib*>, getAttrib)
    OOLUA_MEM_FUNC(void, setDurability, float)
    OOLUA_MEM_FUNC_CONST(float, getDurability)
    OOLUA_MEM_FUNC_CONST(GYVector3, getVelocity)
    OOLUA_MEM_FUNC(void, setVelocity, const GYVector3&)
    OOLUA_MEM_FUNC_CONST(float, getTileDamping)
OOLUA_PROXY_END

#endif // ELYSIAN_RIGID_BODY_PROXY_HPP

