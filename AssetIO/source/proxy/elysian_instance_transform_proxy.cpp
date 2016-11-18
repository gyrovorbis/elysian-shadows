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


/*!
    \qmltype Vector3
    \since 1.5.8
    \inmodule ESToolkit
    \brief Represents a 3-dimensional vector.
*/

/*!
 * \qmlproperty float Vector3::x
 *
 * \sa getX(), setX()
 */
/*!
 * \qmlproperty float Vector3::y
 *
 * \sa getY(), setY()
 */
/*!
 * \qmlproperty float Vector3::z
 *
 * \sa getZ(), setZ()
 */

/*!
 * \qmlmethod float Vector3::getX()
 *
 * Returns the value of the vector's X component.
 */
/*!
 * \qmlmethod float Vector3::getY()
 *
 * Returns the value of the vector's Y component.
 */
/*!
 * \qmlmethod float Vector3::getZ()
 *
 * Returns the value of the vector's Z component.
 */
/*!
 * \qmlmethod void Vector3::setX(float val)
 *
 * Sets the value of the vector's X component.
 */
/*!
 * \qmlmethod void Vector3::setY(float val)
 *
 * Sets the value of the vector's Y component.
 */
/*!
 * \qmlmethod void Vector3::setZ(float val)
 *
 * Sets the value of the vector's Z component.
 */


/*!
    \qmltype Vector4
    \since 1.5.8
    \inmodule ESToolkit
    \brief Represents a 4-dimensional vector.
*/

/*!
 * \qmlproperty float Vector4::x
 *
 * \sa getX(), setX()
 */
/*!
 * \qmlproperty float Vector4::y
 *
 * \sa getY(), setY()
 */
/*!
 * \qmlproperty float Vector4::z
 *
 * \sa getZ(), setZ()
 */
/*!
 * \qmlproperty float Vector4::w
 *
 * \sa getW(), setW()
 */

/*!
 * \qmlmethod float Vector4::getX()
 *
 * Returns the value of the vector's X component.
 */
/*!
 * \qmlmethod float Vector4::getY()
 *
 * Returns the value of the vector's Y component.
 */
/*!
 * \qmlmethod float Vector4::getZ()
 *
 * Returns the value of the vector's Z component.
 */
/*!
 * \qmlmethod float Vector4::getW()
 *
 * Returns the value of the vector's W component.
 */
/*!
 * \qmlmethod void Vector4::setX(float val)
 *
 * Sets the value of the vector's X component.
 */
/*!
 * \qmlmethod void Vector4::setY(float val)
 *
 * Sets the value of the vector's Y component.
 */
/*!
 * \qmlmethod void Vector4::setZ(float val)
 *
 * Sets the value of the vector's Z component.
 */
/*!
 * \qmlmethod void Vector4::setW(float val)
 *
 * Sets the value of the vector's W component.
 */



/*!
    \qmltype InstanceTransform
    \since 1.5.8
    \inmodule ESToolkit
    \brief Represents a 3D instance transform with position, scale, and rotation.
*/
/*!
 * \qmlproperty Vector3 InstanceTransform::pos
 * 3D Vector representing position.
 * \sa getPosX(), getPosY(), getPosZ(), setPosX(), setPosY(), setPosZ()
 */
/*!
 * \qmlproperty Vector3 InstanceTransform::size
 *  3D vector representing size. This is multiplied by scale to determine final transform size.
 *  Prefer to use size to represent the base size of an entity.
 * \sa getSizeX(), getSizeY(), getSizeZ(), setSizeX(), setSizeY(), setSizeZ()
 */
/*!
 * \qmlproperty Vector3 InstanceTransform::scale
 * 3D vector representing scale. This is multiplied by size to deterine final transform size.
 * Prefer to use scale to represent a multiplier applied to the base size of an entity.
 * \sa getScaleX(), getScaleY(), getScaleZ(), setScaleX(), setScaleY(), setScaleZ()
 */
/*!
 * \qmlproperty Vector3 InstanceTransform::orient
 * 3D UNIT vector representing the direction of rotation.
 * \sa getOrientX(), getOrientY(), getOrientZ(), setOrientX(), setOrientY(), setOrientZ()
 */
/*!
 * \qmlproperty float InstanceTransform::angle
 *
 * \sa getAngle(), setAngle()
 */

/*!
 * \qmlmethod void InstanceTransform::setPos3f(float x, float y, float z)
 *
 * Sets the 3D position vector from 3 floats.
 */
/*!
 * \qmlmethod void InstanceTransform::setPos2f(float x, float y)
 *
 * Sets the 2D position vector from 2 floats. Z coordinate does not change.
 */
/*!
 * \qmlmethod void InstanceTransform::translate3f(float x, float y, float z)
 *
 * Adds the <x,y,z> vector to the current position.
 */
/*!
 * \qmlmethod void InstanceTransform::translate2f(float x, float y)
 *
 * Adds the <x,y> vector to the current position.
 */
/*!
 * \qmlmethod void InstanceTransform::setSize3f(float x, float y, float z)
 *
 * Sets the 3D size to the <x, y, z> vector.
 */
/*!
 * \qmlmethod void InstanceTransform::setSize2f(float x, float y)
 *
 * Sets the 2D size to the <x, y> vector. The Z size does not change.
 */
/*!
 * \qmlmethod void InstanceTransform::setScale3f(float x, float y, float z)
 *
 * Sets the 3D scale to the <x, y, z> vector.
 */
/*!
 * \qmlmethod void InstanceTransform::setScale2f(float x, float y)
 *
 * Sets the 2D scale to the <x, y> vector. The Z size does not change.
 */
/*!
 * \qmlmethod void InstanceTransform::scale3f(float x, float y, float z)
 *
 * Multiplies the 3D scale vector by <x, y, z>.
 */
/*!
 * \qmlmethod void InstanceTransform::scale2f(float x, float y)
 *
 * Multiplies the 2D scale vector by <x, y> with the z component unaffected.
 */
/*!
 * \qmlmethod void InstanceTransform::setOrient3f(float x, float y, float z)
 *
 * Sets the direction of rotation to the UNIT VECTOR <x, y, z>
 */
/*!
 * \qmlmethod void InstanceTransform::setAngle(float radians)
 *
 * Sets the transform to rotate about the orientation vector by <radians>.
 */
/*!
 * \qmlmethod float InstanceTransform::getAngle()
 *
 * Returns the number of radians the transform is rotated about the orientation vector.
 */
/*!
 * \qmlmethod GYVector3& InstanceTransform::getPos()
 *
 * Returns a reference to the 3D position vector.
 */
/*!
 * \qmlmethod GYVector3& InstanceTransform::getSize()
 *
 * Returns a reference to the 3D size vector.
 */
/*!
 * \qmlmethod GYVector3& InstanceTransform::getScale()
 *
 * Returns a reference to the 3D scale vector.
 */
/*!
 * \qmlmethod GYVector3& InstanceTransform::getOrient()
 *
 * Returns a reference to the 3D orientation vector.
 */

/*!
 * \qmlmethod float InstanceTransform::getPosX()
 *
 * Returns the position vector's X component.
 */
/*!
 * \qmlmethod float InstanceTransform::getPosY()
 *
 * Returns the position vector's Y component.
 */
/*!
 * \qmlmethod float InstanceTransform::getPosZ()
 *
 * Returns the position vector's Z component.
 */

/*!
 * \qmlmethod float InstanceTransform::getSizeX()
 *
 * Returns the size vector's X component.
 */
/*!
 * \qmlmethod float InstanceTransform::getSizeY()
 *
 * Returns the size vector's Y component.
 */
/*!
 * \qmlmethod float InstanceTransform::getSizeZ()
 *
 * Returns the size vector's Z component.
 */

/*!
 * \qmlmethod float InstanceTransform::getScaleX()
 *
 * Returns the scale vector's X component.
 */
/*!
 * \qmlmethod float InstanceTransform::getScaleY()
 *
 * Returns the scale vector's Y component.
 */
/*!
 * \qmlmethod float InstanceTransform::getScaleZ()
 *
 * Returns the scale vector's Z component.
 */

/*!
 * \qmlmethod float InstanceTransform::getOrientX()
 *
 * Returns the orient vector's X component.
 */
/*!
 * \qmlmethod float InstanceTransform::getOrientY()
 *
 * Returns the orient vector's Y component.
 */
/*!
 * \qmlmethod float InstanceTransform::getOrientZ()
 *
 * Returns the orient vector's Z component.
 */

/*!
 * \qmlmethod void InstanceTransform::setPosX(float val)
 *
 * Sets the position vector's X component.
 */
/*!
 * \qmlmethod void InstanceTransform::setPosY(float val)
 *
 * Sets the position vector's Y component.
 */
/*!
 * \qmlmethod void InstanceTransform::setPosZ(float val)
 *
 * Sets the position vector's Z component.
 */

/*!
 * \qmlmethod void InstanceTransform::setSizeX(float val)
 *
 * Sets the size vector's X component.
 */
/*!
 * \qmlmethod void InstanceTransform::setSizeY(float val)
 *
 * Sets the size vector's Y component.
 */
/*!
 * \qmlmethod void InstanceTransform::setSizeZ(float val)
 *
 * Sets the size vector's Z component.
 */

/*!
 * \qmlmethod void InstanceTransform::setScaleX(float val)
 *
 * Sets the scale vector's X component.
 */
/*!
 * \qmlmethod void InstanceTransform::setScaleY(float val)
 *
 * Sets the scale vector's Y component.
 */
/*!
 * \qmlmethod void InstanceTransform::setScaleZ(float val)
 *
 * Sets the scale vector's Z component.
 */

/*!
 * \qmlmethod void InstanceTransform::setOrientX(float val)
 *
 * Sets the orient vector's X component.
 */
/*!
 * \qmlmethod void InstanceTransform::setOrientY(float val)
 *
 * Sets the orient vector's Y component.
 */
/*!
 * \qmlmethod void InstanceTransform::setOrientZ(float val)
 *
 * Sets the orient vector's Z component.
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
                                    scale3f,
                                    getPos,
                                    getOrient,
                                    getSize,
                                    getScale,
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
