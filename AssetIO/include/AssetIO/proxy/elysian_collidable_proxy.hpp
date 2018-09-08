#ifndef ELYSIAN_COLLIDABLE_PROXY_HPP
#define ELYSIAN_COLLIDABLE_PROXY_HPP

#include <AssetIO/proxy/elysian_component_proxy.hpp>
#include <AssetIO/crtp/elysian_collidable.hpp>
#include <AssetIO/proxy/elysian_bit_mask_proxy.hpp>

namespace OOLUA {
    typedef elysian::CollisionFlags             CollisionFlags;
    typedef elysian::CollidableStaticAttrib     CollidableStaticAttrib;
    typedef elysian::CollidableAttrib           CollidableAttrib;
    typedef elysian::CollisionContact           CollisionContact;
    typedef elysian::Collidable                 Collidable;
    typedef elysian::BitMask32                  BitMask32;

    typedef elysian::Vector3                    Vector3;

    template<>                          class Proxy_class<elysian::Collidable>;
}


OOLUA_PROXY(CollisionFlags)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(BitMask32&, getType)
    OOLUA_MEM_FUNC(BitMask32&, getTypeAgainst)
    OOLUA_MEM_FUNC(void, setType, const BitMask32)
    OOLUA_MEM_FUNC(void, setTypeAgainst, const BitMask32)
OOLUA_PROXY_END

OOLUA_PROXY(CollidableStaticAttrib)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setDamping, float)
    OOLUA_MEM_FUNC(void, setZDamping, float)
    OOLUA_MEM_FUNC(void, setDamping3f, float, float, float)
    OOLUA_MEM_FUNC(Vector3, getDampingVec)
    OOLUA_MEM_FUNC(void, setDampingVec, const Vector3&)
    OOLUA_MEM_FUNC(void, setInverseMass, const float)
    OOLUA_MEM_FUNC(void, setCoeffOfRest, const float)
    OOLUA_MEM_FUNC(void, setInvMomOfInertia, const float)
    OOLUA_MEM_FUNC(void, setAngDamping, const float)
    OOLUA_MEM_FUNC(void, setZCoeffOfRest, const float)
    OOLUA_MEM_FUNC_CONST(float, getInverseMass)
    OOLUA_MEM_FUNC_CONST(float, getCoeffOfRest)
    OOLUA_MEM_FUNC_CONST(float, getInvMomOfInertia)
    OOLUA_MEM_FUNC_CONST(float, getAngDamping)
    OOLUA_MEM_FUNC_CONST(float, getZCoeffOfRest)
OOLUA_PROXY_END

OOLUA_PROXY(CollidableAttrib)
    OOLUA_TAGS(Shared)
    OOLUA_MEM_FUNC_CONST(GY_COLLIDER_TYPE, getColliderType)
    OOLUA_MEM_FUNC(void, setColliderType, GY_COLLIDER_TYPE)
    OOLUA_MEM_FUNC(void, setRelPos, float, float, float)
    OOLUA_MEM_FUNC(void, setRelSize, float, float, float)
    OOLUA_MEM_FUNC(Vector3, getRelPosVec)
    OOLUA_MEM_FUNC(Vector3, getRelSizeVec)
    OOLUA_MEM_FUNC(void, setRelPosVec, const Vector3&)
    OOLUA_MEM_FUNC(void, setRelSizeVec, const Vector3&)
    OOLUA_MEM_FUNC(CollisionFlags*, getFlags)
    OOLUA_MEM_FUNC(void, setFlags, const OOLUA::Lua_table_ref)
    OOLUA_MEM_FUNC(CollidableStaticAttrib*, getStaticAttrib)
    OOLUA_MEM_FUNC(void, setStaticAttrib, const OOLUA::Lua_table_ref)
OOLUA_PROXY_END

OOLUA_PROXY(CollisionContact)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(maybe_null<Collidable*>, getCollidable, unsigned)
    OOLUA_MEM_FUNC_CONST(void, getWorldPos, GYVector2&)
    OOLUA_MEM_FUNC_CONST(void, getNormal, GYVector2&)
    OOLUA_MEM_FUNC_CONST(void, getObjPos, unsigned, GYVector2&)
    OOLUA_MEM_FUNC_CONST(float, getSeparatingVel)
    OOLUA_MEM_FUNC_CONST(float, getClosingVel)
    OOLUA_MEM_FUNC_CONST(float, getDeltaVel)
    OOLUA_MEM_FUNC_CONST(float, getDepth)
OOLUA_PROXY_END

OOLUA_PROXY(Collidable, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(maybe_null<std::shared_ptr<CollidableAttrib>>, getAttrib)
    OOLUA_MEM_FUNC(void, setAttrib, std::shared_ptr<CollidableAttrib>)
    OOLUA_MEM_FUNC_CONST(unsigned, getContactCount)
    OOLUA_MEM_FUNC_CONST(bool, isActive)
    OOLUA_MEM_FUNC(void, setActive, bool)
    OOLUA_MEM_FUNC(void, setResolvePenetration, bool)
    OOLUA_MEM_FUNC_CONST(bool, getResolvePenetration)
    OOLUA_MEM_FUNC_CONST(bool, getWorldSpace)
    OOLUA_MEM_FUNC(void, setWorldSpace, const bool)
    OOLUA_MEM_FUNC_CONST(bool, getStackedOnGround)
    OOLUA_MEM_FUNC_CONST(bool, getRestingOnBody)
OOLUA_PROXY_END



#endif // ELYSIAN_COLLIDABLE_PROXY_HPP

