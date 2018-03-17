#include <proxy/elysian_instance_transform_proxy.hpp>

/*!
    \qmltype Vector2
    \since 1.5.8
    \inmodule Engine
    \brief Represents a 2-dimensional point or vector within the Engine.

    A simple 2D vector containing an \l{x} and \l{y} component.
    Favor this type over any built-in or utility Lua vector types
    or libraries, as this type uses libGyro's hardware acceleration
    and SIMD instructions on platforms where they are available.

    \section2 Coordinate System
    In Elysian Shadows, the coordinate system (in screen space) is as follows:
    \table
        \header
            \li Position (Screen Space)
            \li Description
        \row
            \li <0, 0>
            \li Top left of screen
        \row
            \li <SCREEN_WIDTH, 0>
            \li Top Right of screen
        \row
            \li <0, SCREEN_HEIGHT>
            \li Bottom Left of screen
        \row
            \li <SCREEN_WIDTH, SCREEN_HEIGHT>
            \li Bottom right of screen
    \endtable
    The Z axis located with the positive Z direction coming out direclty towards the viewer in the 2D perspective, meaning the furthest back object starts at
    Z coordinate 0 with closer objects having increasing Z values as they approach the viewer.

    \section2 Rotations
    Any rotations and angles are applied \e counterclockwise (in radians), with the positive
    X axis being at 0 degrees/radians. Unless a direction vector is taken, rotations performed
    around the Z axis (this is always the case with Vector2).

    \section2 Overloaded Operators
    The following overloaded operators are implemented in Lua through the respective metamethods.
    \table
        \header
            \li \b Symbol
            \li \b Operation
            \li \b Metamethod
        \row
            \li \b ==
            \li Equality
            \li \l{Vector2::_eq()}
        \row
            \li \b +
            \li Addition
            \li \l{Vector2::_add()}
        \row
            \li \b -
            \li Subtraction
            \li \l{Vector2::_sub()}
        \row
            \li \b *
            \li Multiplication
            \li \l{Vector2::_mult()}
        \row
            \li \b /
            \li Division
            \li \l{Vector2::_div()}
    \endtable

    \section2 Compound Assignment Operations
    Since Lua does not support in-place compound assignment operators such as +=, -=, *=, and /=, they are implemented as normal methods.
    If you are performing an operation that can be represented by one of these operators, you should always prefer to use the corresponding
    method than to use the traditional Lua method, as it saves memory and run-time overhead by not requiring extra copies.

    For example:
    \code
    vec1 = vec1 + vec2
    \endcode
    should be avoided in favor of:
    \code
    vec1:addVec(vec2)
    \endcode

    The following compound operations are supported.
    \table
        \header
            \li \b Operator
            \li \b RValue Type
            \li \b Method
        \row
            \li \b +=
            \li Vector2
            \li \l{Vector2::addVec()}
        \row
            \li \b -=
            \li Vector2
            \li \l{Vector2::subVec()}
        \row
            \li \b *=
            \li Vector2
            \li \l{Vector2::multVec()}
        \row
            \li \b /=
            \li Vector2
            \li \l{Vector2::divVec()}
        \row
            \li \b *=
            \li float
            \li \l{Vector2::multScalar()}
        \row
            \li \b /=
            \li float
            \li \l{Vector2::divScalar()}
    \endtable

    \section2 2D Cross Products
    Since the cross-product is ambiguous in 2D (or is undefined, depending on your interpretation
    of the mathematics), yet the operation is often still needed in certain scenarios (such as physics),
    the cross-product between two 2-dimension vectors can be interpreted as either a vector or a scalar
    value.

    \table
        \header
            \li Result
            \li Method
            \li Operation
            \li Geometric Interpretation
        \row
            \li float
            \li \l scalarCrossProduct()
            \li U\sub x * V\sub y - U\sub y * V\sub x
            \li Perpendicular dot product - Dot product of the perpendicular of U with V.
        \row
            \li Vector2
            \li \l vectorCrossProduct()
            \li <U\sub y * scalar, -U\sub x * scalar>
            \li Scaled perpendicular of given vector. Generalized form, determinant of 2x2 row matrix formed by the vectors.
    \endtable

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
 * \qmlmethod Vector2::Vector2()
 *
 * Creates a new Vector2 with all elements initialized to 0.
 */
/*!
 * \qmlmethod Vector2::Vector2(float xComp, float yComp)
 *
 * Creates a new Vector2 with \l {Vector2::x} initialized to \a xComp and \l{Vector2::y} initialized to \a yComp.
 */
/*!
 * \qmlmethod Vector2::Vector2(Vector2 other)
 *
 * Copy-constructs a new Vector2 whose values are initialized to the values of \a other.
 */
/*!
 * \qmlmethod Vector2 Vector2::_add(Vector2 rhs)
 *
 * Overloaded addition operator (+) metamethod. Adds the components of \a rhs to the components of the given vector and returns the result as a new vector.
 * \sa _sub(), _div(), _mult(), _eq()
 */
/*!
 * \qmlmethod Vector2 Vector2::_sub(Vector2 rhs)
 *
 * Overloaded subtraction operator (-) metamethod. Subtracts the components of \a rhs from the components of the given vector and returns the result as a new vector.
 * \sa _add(), _div(), _mult(), _eq()
 */
/*!
 * \qmlmethod Vector2 Vector2::_mult(Vector2 rhs)
 *
 * Overloaded multiplication operator (*) metamethod. Multiplies the components of \a rhs by the components of the given vector and returns the result as a new vector.
 * \sa _add(), _div(), _sub(), _eq()
 */
/*!
 * \qmlmethod Vector2 Vector2::_div(Vector2 rhs)
 *
 * Overloaded division operator (/) metamethod. Divides the components of the current vector by the components of \a rhs and returns the result as a new vector.
 * \sa _add(), _sub(), _mult(), _eq()
 */
/*!
 * \qmlmethod Vector2 Vector2::_eq(Vector2 rhs)
 *
 * Overloaded equality comparison operator (==) metamethod. Compares the components of the current vector to the components of \a rhs and returns true if they are equal or false otherwise.
 * \sa _add(), _sub(), _mult(), _div()
 */
/*!
 * \qmlmethod void Vector2::addVec(Vector2 rhs)
 *
 * Adds the components of \a rhs to the components of the given vector.
 * \sa subVec(), multVec(), divVec()
 */
/*!
 * \qmlmethod void Vector2::subVec(Vector2 rhs)
 *
 * Subtracts the components of \a rhs from the components of the given vector.
 * \sa addVec(), multVec(), divVec()
 */
/*!
 * \qmlmethod void Vector2::multVec(Vector2 rhs)
 *
 * Multiplies the components of \a rhs by the components of the given vector.
 * \sa subVec(), addVec(), divVec()
 */
/*!
 * \qmlmethod void Vector2::divVec(Vector2 rhs)
 *
 * Divides the components of the given vector by the components of \a rhs.
 * \sa subVec(), multVec(), multVec()
 */
/*!
 * \qmlmethod void Vector2::multScalar(float scalar)
 *
 * Multiplies each component of \a rhs by \a scalar.
 * \sa divScalar()
 */
/*!
 * \qmlmethod void Vector2::divScalar(float scalar)
 *
 * Divides each component of \a rhs by \a scalar.
 * \sa multScalar()
 */

/*!
 * \qmlmethod float Vector2::getX()
 *
 * Returns the value of the vector's X component.
 * \sa {Vector2::x}, setX()
 */
/*!
 * \qmlmethod float Vector2::getY()
 *
 * Returns the value of the vector's Y component.
 * \sa {Vector2::y}, setY()
 */
/*!
 * \qmlmethod void Vector2::setX(float val)
 *
 * Sets the value of the vector's X component.
 * \sa {Vector2::x}, getX()
 */
/*!
 * \qmlmethod void Vector2::setY(float val)
 *
 * Sets the value of the vector's Y component.
 * \sa {Vector2::y}, getY()
 */
/*!
 * \qmlmethod float Vector2::getMagnitude()
 *
 * Returns the magnitude of the 2D vector. If the vector were to represent a point, this
 * is geometrically the distance from the origin to the point represented by the vector.
 *
 * \e{NOTE: This is not a property, so every time you call this function, the magnitude will
 * have to be calculated, which requires a square root. Do not call this function repeatedly
 * unless the Vector has actually changed. If not, cache the value rather than recalling this
 * function. }
 * \sa normalize(), getUnitVector()
 */
/*!
 * \qmlmethod void Vector2::normalize()
 *
 * Converts the given vector into a unit (direction) vector of unit length (1.0).
 * \sa getMagnitude(), getUnitVector()
 */
/*!
 * \qmlmethod float Vector2::distance(Vector2 other)
 *
 * Finds the distance between two points, represented as vectors, given the current vector
 * and \a other vector, using the distance formula (magnitude of the difference vector).
 * \e { NOTE: When you only need a relative distance, in the case of simply comparing distances
 * between points, rather than the absolute distance for a calculation, ALWAYS favor squaredDistance(),
 * because it saves on performing and unecessary square root operation.}
 * \sa squaredDistance()
 */
/*!
 * \qmlmethod float Vector2::squaredDistance(Vector2 other)
 *
 * Returns the value of the distance between the two vectors squared. Always use this function rather than
 * the distance() method when you only care about relative distances, such as when you're comparing the distances
 * between a set of vectors to see which is furthest, and you don't care about the actual exact distance value. This
 * will save on unecessary slow square root calls.
 * \sa distance()
 */
/*!
 * \qmlmethod float Vector2::dotProduct(Vector2 other)
 *
 * Returns the dot (or scalar) product between the current vector and the \a {other} vector.
 * \sa scalarCrossProduct(), vectorCrossProduct()
 */
/*!
 * \qmlmethod float Vector2::scalarCrossProduct(Vector2 other)
 *
 * Returns the cross product between the current vector and \a other vector in the form of a scalar.
 * \sa {2D Cross Products}, dotProduct(), vectorCrossProduct()
 */
/*!
 * \qmlmethod Vector2 Vector2::vectorCrossProduct(Vector2 other)
 *
 * Returns the cross product between the current vector and \a other vector in the form of a vector.
 * \sa {2D Cross Products}, scalarCrossProduct(), dotProduct()
 */
/*!
 * \qmlmethod float Vector2::toAngle()
 *
 * Returns the angle between the vector representing the positive X axis and the given vector using atan2.
 * The result is in radians.
 * \sa rotate(), getRotatedVector()
 */
/*!
 * \qmlmethod Vector2 Vector2::getUnitVector()
 *
 * Returns the angle between the vector representing the positive X axis and the given vector using atan2.
 * The result is in radians.
 * \sa normalize(), getMagnitude()
 */
/*!
 * \qmlmethod void Vector2::rotate(float radians)
 *
 * Rotates the current vector \e counterclockwise about the origin by the given \a { radians } (with the positive X axis being 0 degrees).
 *
 * \e {NOTE: The vector will be rotated about the positive Z axis.}
 *
 * \sa getRotatedVector(), rotateAboutPoint(), getRotatedVectorAboutPoint(), toAngle()
 */
/*!
 * \qmlmethod Vector2 Vector2::getRotatedVector(float radians)
 *
 * Returns a new vector as the current vector rotated \e counterclockwise about the origin by the given \a { radians } (with the positive X axis being 0 degrees).
 *
 * \e {NOTE: The vector will be rotated about the positive Z axis.}
 * \sa rotate(), rotateAboutPoint(), getRotatedVectorAboutPoint(), toAngle()
 */
/*!
 * \qmlmethod void Vector2::rotateAboutPoint(Vector2 pivot, float radians)
 *
 * Rotates the current vector \e counterclockwise about the given \a pivot by the given \a radians (with the positive X axis being 0 degrees).
 *
 * \e {NOTE: The vector will be rotated about the positive Z axis.}
 * \sa getRotatedVectorAboutPoint(), rotate(), getRotatedVector(), toAngle()
 */
/*!
 * \qmlmethod void Vector2::getRotatedVectorAboutPoint(Vector2 pivot, float radians)
 *
 * Returns a new vector as the current vector rotated \e counterclockwise about the given \a pivot by the given \a radians (with the positive X axis being 0 degrees).
 *
 * \e {NOTE: The vector will be rotated about the positive Z axis.}
 * \sa rotateAboutPoint(), rotate(), getRotatedVector(), toAngle()
 */

OOLUA_EXPORT_FUNCTIONS(GYVector2, setX, setY)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector2, getX, getY)

OOLUA_EXPORT_FUNCTIONS(Vector2,
                       normalize,
                       rotate,
                       rotateAboutPoint,
                       addVec,
                       subVec,
                       multVec,
                       divVec,
                       multScalar,
                       divScalar)
OOLUA_EXPORT_FUNCTIONS_CONST(Vector2,
                             getX,
                             getY,
                             setX,
                             setY,
                             getMagnitude,
                             squaredDistance,
                             distance,
                             dotProduct,
                             scalarCrossProduct,
                             vectorCrossProduct,
                             toAngle,
                             getUnitVector,
                             getRotatedVector,
                             getRotatedVectorAboutPoint)

/*!
    \qmltype Vector3
    \since 1.5.8
    \inmodule Engine
    \brief Represents a 3-dimensional vector.
    A simple 3D vector containing an \l{x}, \l{y}, and \l{z} component.
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
    \inmodule Engine
    \brief Represents a 4-dimensional vector.
    A simple 4D vector containing an \l{x}, \l{y}, \l{z}, and \l{w} component.
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
    \inmodule Engine
    \brief Represents a 3D instance transform with position, scale, and rotation.

    InstanceTransform represents a set of transforms: scale, rotation, and translation,
    applied in that order, to take geometry from local coordinates to its final size,
    orientation, and world-coordiate position in 3D space.

    The following table describes the properties used for each transform.
    \table
    \header
        \li transform
        \li properties used
    \row
        \li translation
        \li \l{pos}
    \row
        \li rotation
        \li \l {orient} and \l{angle}
    \row
        \li scale
        \li \l{size} * \l{scale}
    \endtable
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
 * \qmlmethod Vector3& InstanceTransform::getPos()
 *
 * Returns a reference to the 3D position vector.
 */
/*!
 * \qmlmethod Vector3& InstanceTransform::getSize()
 *
 * Returns a reference to the 3D size vector.
 */
/*!
 * \qmlmethod Vector3& InstanceTransform::getScale()
 *
 * Returns a reference to the 3D scale vector.
 */
/*!
 * \qmlmethod Vector3& InstanceTransform::getOrient()
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


//OOLUA_EXPORT_FUNCTIONS(GYVector3)
//OOLUA_EXPORT_FUNCTIONS_CONST(GYVector3)
OOLUA_EXPORT_FUNCTIONS(GYVector3, setX, setY, setZ)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector3, getX, getY, getZ)

OOLUA_EXPORT_FUNCTIONS(Vector3)
OOLUA_EXPORT_FUNCTIONS_CONST(Vector3,
                             getX,
                             getY,
                             getZ,
                             setX,
                             setY,
                             setZ,
                             getMagnitude,
                             squaredDistance,
                             distance,
                             dotProduct,
                             crossProduct,
                             getUnitVector)

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
