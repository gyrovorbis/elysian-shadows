#ifndef ELYSIAN_INSTANCE_TRANSFORM_PROXY_HPP
#define ELYSIAN_INSTANCE_TRANSFORM_PROXY_HPP

#include <primitive/elysian_instance_transform.hpp>
#include <gyro_matrix_api.h>

namespace OOLUA {
    typedef ::GYVector2                 GYVector2;
    typedef elysian::Vector2            Vector2;
    typedef elysian::Vector3            Vector3;
    typedef elysian::Vector4            Vector4;
    typedef elysian::InstanceTransform  InstanceTransform;

    template<> class Proxy_class<GYVector2>;
    template<> class Proxy_class<InstanceTransform>;

}


OOLUA_PROXY(GYVector2)
    OOLUA_MGET_MSET(x, getX, setX)
    OOLUA_MGET_MSET(y, getY, setY)
OOLUA_PROXY_END

OOLUA_PROXY(Vector2)
    OOLUA_CTORS(
        OOLUA_CTOR(const Vector2&)
    )
    OOLUA_MEM_FUNC_CONST(float, getX)
    OOLUA_MEM_FUNC_CONST(float, getY)
    OOLUA_MEM_FUNC_CONST(void, setX, float)
    OOLUA_MEM_FUNC_CONST(void, setY, float)
OOLUA_PROXY_END

OOLUA_PROXY(GYVector3)
    OOLUA_MGET_MSET(x, getX, setX)
    OOLUA_MGET_MSET(y, getY, setY)
    OOLUA_MGET_MSET(z, getZ, setZ)
OOLUA_PROXY_END

OOLUA_PROXY(Vector3)
    OOLUA_CTORS(
        OOLUA_CTOR(const Vector3&)
    )
    OOLUA_MEM_FUNC_CONST(float, getX)
    OOLUA_MEM_FUNC_CONST(float, getY)
    OOLUA_MEM_FUNC_CONST(float, getZ)
    OOLUA_MEM_FUNC_CONST(void, setX, float)
    OOLUA_MEM_FUNC_CONST(void, setY, float)
    OOLUA_MEM_FUNC_CONST(void, setZ, float)
OOLUA_PROXY_END

OOLUA_PROXY(GYVector4)
    OOLUA_MGET_MSET(x, getX, setX)
    OOLUA_MGET_MSET(y, getY, setY)
    OOLUA_MGET_MSET(z, getZ, setZ)
    OOLUA_MGET_MSET(w, getW, setW)
OOLUA_PROXY_END

OOLUA_PROXY(Vector4)
    OOLUA_MEM_FUNC_CONST(float, getX)
    OOLUA_MEM_FUNC_CONST(float, getY)
    OOLUA_MEM_FUNC_CONST(float, getZ)
    OOLUA_MEM_FUNC_CONST(float, getW)
    OOLUA_MEM_FUNC_CONST(void, setX, float)
    OOLUA_MEM_FUNC_CONST(void, setY, float)
    OOLUA_MEM_FUNC_CONST(void, setZ, float)
    OOLUA_MEM_FUNC_CONST(void, setW, float)
OOLUA_PROXY_END

OOLUA_PROXY(InstanceTransform)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(void, apply)
    OOLUA_MEM_FUNC_CONST_RENAME(getPos2f, void, getPos, out_p<float&>, out_p<float&>)
    OOLUA_MEM_FUNC_CONST_RENAME(getPos3f, void, getPos, out_p<float&>, out_p<float&>, out_p<float&>)
    OOLUA_MEM_FUNC_RENAME(setPos2f, void, setPos, float, float)
    OOLUA_MEM_FUNC_RENAME(setPos3f, void, setPos, float, float, float)
    OOLUA_MEM_FUNC_RENAME(translate2f, void, translate, float, float)
    OOLUA_MEM_FUNC_RENAME(translate3f, void, translate, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setSize3f, void, setSize, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setScale3f, void, setScale, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setSize2f, void, setSize, float, float)
    OOLUA_MEM_FUNC_RENAME(setScale2f, void, setScale, float, float)
    OOLUA_MEM_FUNC(void, rotate, float)
    OOLUA_MEM_FUNC_RENAME(scale2f, void, scale, float, float)
    OOLUA_MEM_FUNC_CONST_RENAME(getSize2f, void, getSize, out_p<float&>, out_p<float&>)
    OOLUA_MEM_FUNC_CONST_RENAME(getScale2f, void, getScale, out_p<float&>, out_p<float&>)
    OOLUA_MEM_FUNC(GYVector3&, getPosVec)
    OOLUA_MEM_FUNC(GYVector3&, getSizeVec)
    OOLUA_MEM_FUNC(GYVector3&, getScaleVec)
    OOLUA_MEM_FUNC(void, setSizeVec, const OOLUA::Lua_table_ref)
    OOLUA_MEM_FUNC(void, setScaleVec, const OOLUA::Lua_table_ref)
    OOLUA_MEM_FUNC(void, setPosVec, const OOLUA::Lua_table_ref)
    OOLUA_MEM_FUNC_CONST(float, getPosZ)
    OOLUA_MEM_FUNC(void, setPosZ, const float)
    OOLUA_MEM_FUNC_CONST(float, getPosX)
    OOLUA_MEM_FUNC(void, setPosX, const float)
    OOLUA_MEM_FUNC_CONST(float, getPosY)
    OOLUA_MEM_FUNC(void, setPosY, const float)
    OOLUA_MEM_FUNC_CONST(float, getSizeZ)
    OOLUA_MEM_FUNC(void, setSizeZ, const float)
    OOLUA_MEM_FUNC_CONST(float, getSizeX)
    OOLUA_MEM_FUNC(void, setSizeX, const float)
    OOLUA_MEM_FUNC_CONST(float, getSizeY)
    OOLUA_MEM_FUNC(void, setSizeY, const float)
    OOLUA_MEM_FUNC_CONST(float, getScaleZ)
    OOLUA_MEM_FUNC(void, setScaleZ, const float)
    OOLUA_MEM_FUNC_CONST(float, getScaleX)
    OOLUA_MEM_FUNC(void, setScaleX, const float)
    OOLUA_MEM_FUNC_CONST(float, getScaleY)
    OOLUA_MEM_FUNC(void, setScaleY, const float)
    OOLUA_MEM_FUNC(void, setAngle, const float)
    OOLUA_MEM_FUNC_CONST(float, getAngle)
    OOLUA_MEM_FUNC(void, setOrientX, const float)
    OOLUA_MEM_FUNC_CONST(float, getOrientX)
    OOLUA_MEM_FUNC(void, setOrientY, const float)
    OOLUA_MEM_FUNC_CONST(float, getOrientY)
    OOLUA_MEM_FUNC(void, setOrientZ, const float)
    OOLUA_MEM_FUNC_CONST(float, getOrientZ)
    OOLUA_PROXY_END



#endif // ELYSIAN_INSTANCE_TRANSFORM_PROXY_HPP

