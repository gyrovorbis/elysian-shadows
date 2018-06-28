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
    OOLUA_TAGS (
        Equal_op,
        Add_op,
        Sub_op,
        Mul_op,
        Div_op
    )
    OOLUA_CTORS(
        OOLUA_CTOR(const Vector2&)
        OOLUA_CTOR(float, float)
    )
    OOLUA_MEM_FUNC_CONST(float, getX)
    OOLUA_MEM_FUNC_CONST(float, getY)
    OOLUA_MEM_FUNC_CONST(void, setX, float)
    OOLUA_MEM_FUNC_CONST(void, setY, float)
    OOLUA_MFUNC(addVec)
    OOLUA_MFUNC(subVec)
    OOLUA_MFUNC(multVec)
    OOLUA_MFUNC(divVec)
    OOLUA_MFUNC(multScalar)
    OOLUA_MFUNC(divScalar)
    OOLUA_MFUNC(normalize)
    OOLUA_MFUNC_CONST(getMagnitude)
    OOLUA_MFUNC_CONST(squaredDistance)
    OOLUA_MFUNC_CONST(distance)
    OOLUA_MFUNC_CONST(dotProduct)
    OOLUA_MFUNC_CONST(scalarCrossProduct)
    OOLUA_MFUNC_CONST(vectorCrossProduct)
    OOLUA_MFUNC_CONST(getUnitVector)
    OOLUA_MFUNC(rotate)
    OOLUA_MFUNC_CONST(toAngle)
    OOLUA_MFUNC_CONST(getRotatedVector)
    OOLUA_MFUNC(rotateAboutPoint)
    OOLUA_MFUNC_CONST(getRotatedVectorAboutPoint)
OOLUA_PROXY_END

OOLUA_PROXY(GYVector3)
    OOLUA_MGET_MSET(x, getX, setX)
    OOLUA_MGET_MSET(y, getY, setY)
    OOLUA_MGET_MSET(z, getZ, setZ)
OOLUA_PROXY_END

OOLUA_PROXY(Vector3)
    OOLUA_TAGS(
        Equal_op,
        Add_op,
        Sub_op,
        Mul_op,
        Div_op
     )
    OOLUA_CTORS(
        OOLUA_CTOR(const Vector2&)
        OOLUA_CTOR(const Vector3&)
        OOLUA_CTOR(const Vector2&, float)
        OOLUA_CTOR(float, float, float)
        OOLUA_CTOR(float)
    )
    OOLUA_MEM_FUNC_CONST(float, getX)
    OOLUA_MEM_FUNC_CONST(float, getY)
    OOLUA_MEM_FUNC_CONST(float, getZ)
    OOLUA_MEM_FUNC_CONST(void, setX, float)
    OOLUA_MEM_FUNC_CONST(void, setY, float)
    OOLUA_MEM_FUNC_CONST(void, setZ, float)
    OOLUA_MFUNC(addVec)
    OOLUA_MFUNC(subVec)
    OOLUA_MFUNC(multVec)
    OOLUA_MFUNC(divVec)
    OOLUA_MFUNC(multScalar)
    OOLUA_MFUNC(divScalar)
    OOLUA_MFUNC(normalize)
    OOLUA_MFUNC_CONST(getMagnitude)
    OOLUA_MFUNC_CONST(squaredDistance)
    OOLUA_MFUNC_CONST(distance)
    OOLUA_MFUNC_CONST(dotProduct)
    OOLUA_MFUNC_CONST(crossProduct)
    OOLUA_MFUNC_CONST(getUnitVector)
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
    OOLUA_MEM_FUNC_RENAME(setPos2f, void, setPos, float, float)
    OOLUA_MEM_FUNC_RENAME(setPos3f, void, setPos, float, float, float)
    OOLUA_MEM_FUNC_RENAME(translate2f, void, translate, float, float)
    OOLUA_MEM_FUNC_RENAME(translate3f, void, translate, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setSize2f, void, setSize, float, float)
    OOLUA_MEM_FUNC_RENAME(setSize3f, void, setSize, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setScale2f, void, setScale, float, float)
    OOLUA_MEM_FUNC_RENAME(setScale3f, void, setScale, float, float, float)
    OOLUA_MEM_FUNC_RENAME(scale2f, void, scale, float, float)
    OOLUA_MEM_FUNC_RENAME(scale3f, void, scale, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setOrient3f, void, setOrient, float, float, float)
    OOLUA_MEM_FUNC(void, rotate, float)
    OOLUA_MEM_FUNC(GYVector3&, getPos)
    OOLUA_MEM_FUNC(GYVector3&, getSize)
    OOLUA_MEM_FUNC(GYVector3&, getScale)
    OOLUA_MEM_FUNC(GYVector3&, getOrient)
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

