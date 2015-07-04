#include <proxy/elysian_instance_transform_proxy.hpp>


//EXPORT_OOLUA_FUNCTIONS_0_CONST(GYVector2)
//EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(GYVector2)
OOLUA_EXPORT_FUNCTIONS(GYVector2, setX, setY)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector2, getX, getY)

OOLUA_EXPORT_FUNCTIONS(Vector2)
OOLUA_EXPORT_FUNCTIONS_CONST(Vector2, getX, getY, setX, setY)

//OOLUA_EXPORT_FUNCTIONS(GYVector3)
//OOLUA_EXPORT_FUNCTIONS_CONST(GYVector3)
OOLUA_EXPORT_FUNCTIONS(GYVector3, setX, setY, setZ)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector3, getX, getY, getZ)

OOLUA_EXPORT_FUNCTIONS(Vector3)
OOLUA_EXPORT_FUNCTIONS_CONST(Vector3, getX, getY, getZ, setX, setY, setZ)

//OOLUA_EXPORT_FUNCTIONS(GYVector4)
//OOLUA_EXPORT_FUNCTIONS_CONST(GYVector4)
OOLUA_EXPORT_FUNCTIONS(GYVector4, setX, setY, setZ, setW)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector4, getX, getY, getZ, getW)

OOLUA_EXPORT_FUNCTIONS(Vector4)
OOLUA_EXPORT_FUNCTIONS_CONST(Vector4, getX, getY, getZ, getW, setX, setY, setZ, setW)

OOLUA_EXPORT_FUNCTIONS(InstanceTransform,
                                    setPos2f,
                                    setPos3f,
                                    translate2f,
                                    translate3f,
                                    setSize2f,
                                    setScale2f,
                                    setSize3f,
                                    setScale3f,
                                    setOrientZ,
                                    rotate,
                                    scale2f,
                                    getPosVec,
                                    setPosZ,
                                    setPosY,
                                    setPosX,
                                    setSizeX,
                                    setSizeY,
                                    setSizeZ,
                                    setScaleX,
                                    setScaleY,
                                    setScaleZ)
OOLUA_EXPORT_FUNCTIONS_CONST(InstanceTransform,
                                apply,
                                getPos2f,
                                getPos3f,
                                getSize2f,
                                getScale2f,
                                getOrientZ,
                                getPosZ,
                                getPosY,
                                getPosX,
                                getSizeX,
                                getSizeY,
                                getSizeZ,
                                getScaleX,
                                getScaleY,
                                getScaleZ)
