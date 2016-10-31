#include <proxy/elysian_instance_transform_proxy.hpp>



/*!
    \qmltype Vector2
    \since 1.5.8
    \inmodule ESToolkit
    \brief Represents a 2-dimensional vector.
*/

/*!
 * \qmlproperty float Vector2::x
 *
 * \sa getX(), setX()
 */
/*!
 * \qmlproperty float Vector2::y
 *
 * \sa getY(), setY()
 */

/*!
 * \qmlmethod float Vector2::getX()
 *
 * Returns the value of the vector's X component.
 */
/*!
 * \qmlmethod float Vector2::getY()
 *
 * Returns the value of the vector's Y component.
 */
/*!
 * \qmlmethod void Vector2::setX(float val)
 *
 * Sets the value of the vector's X component.
 */
/*!
 * \qmlmethod void Vector2::setY(float val)
 *
 * Sets the value of the vector's Y component.
 */



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
                                    setScaleZ,
                                    setAngle,
                                    setOrientX,
                                    setOrientY,
                                    setOrientZ)
OOLUA_EXPORT_FUNCTIONS_CONST(InstanceTransform,
                                apply,
                                getPos2f,
                                getPos3f,
                                getSize2f,
                                getScale2f,
                                getPosZ,
                                getPosY,
                                getPosX,
                                getSizeX,
                                getSizeY,
                                getSizeZ,
                                getScaleX,
                                getScaleY,
                                getScaleZ,
                                getAngle,
                                getOrientX,
                                getOrientY,
                                getOrientZ)
