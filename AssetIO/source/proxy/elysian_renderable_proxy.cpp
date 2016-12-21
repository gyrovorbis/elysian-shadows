#include <proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype Renderable
    \inherits Component
    \since 1.6.0
    \inmodule ESToolkit
    \brief Component attached to an Entity to make it a sprite or quad.

    \l{Renderable} is a \l{Component} that can be attached to an \l{Entity}, allowing
    it to be rendered as a textured or untextured sprite or quad with an optional
    normal map, specular map, and \l{Material}. This class should be the preferred method
    of rendering for an \l{Entity}, as it's fast, lightweight, and requires minimal
    vertex data. For more complex geometry, see the \l{Skeleton} Component.

    The Renderable Component's texture can be a standard image or a uniformly-spaced
    sprite sheet, which it can split into frames automatically. The sprite index can be set
    through \a{frame} and can be modified at run-time for simple animations.

    Renderable can also use arbitrary texture coordinates to draw any region of
    a texture for special effects or animation. To do this, \l{frame} must be set to
    -1, then custom UV coords can be set through the \l{uvCoords} property. The default
    texture wrap setting is repeat, so you can tile a sprite by simply oversampling to
    do special effects like a waterfall by looping over UV coordinates.

    Multiple Renderables using the same sprite sheets, sprites, and vertex colors
    should share the same SpriteAttrib or QuadSpriteAttrib objects as their
    \l{spriteAttrib} properties to prevent duplicate objects in memory and make it
    easier to manage groups of similar Entities.

    \sa SpriteAttrib, QuadSpriteAttrib, Material
*/
/*!
\qmlproperty int Renderable::frame

If the current texture is a sprite sheet, frame is the sheet index of the current sprite.
If no texture is loaded, frame is ignored. If a texture is present and frame is equal to
-1, texture coordinates are fetched from the \l{uvCoords} property.
\sa getFrame(), setFrame()
*/
/*!
\qmlproperty TEX_ORIENT_TYPE Renderable::texOrient

Enumeration representing any horizontal or vertical flipping or 90 degree rotations applied
to the sprite.
\sa gyro::TEX_ORIENT_TYPE, getTexOrient(), setTexOrient()
*/
/*!
\qmlproperty SpriteAttrib* Renderable::spriteAttrib

Reference to a SpriteAttrib OR QuadSpriteAttrib object to be used by the Renderable when
rendering or nil if neither has been specified.
\sa getSpriteAttrib(), getQuadSpriteAttrib(), setSpriteAttrib()
*/
/*!
\qmlproperty Vector2 Renderable::offset

Offset of the sprite's position relative to the Entity's position. The default value is
<0, 0> which places the sprite exactly at the Entity's position.
\sa getOffsetVec(), setOffsetVec()
*/
/*!
\qmlproperty array Renderable::uvCoords

4-element array (indices 1-4) of Vector2 objects used as custom UV coordinates when
rendering the sprite. The \l{frame} property must be set to -1 for these values to
be used, because they aren't by default.
\sa getUVCoords(), setUVCoords(), getUVCoordsCount()
*/
/*!
 * \qmlmethod int Renderable::getFrame()
 *
 * Returns the value of the \l{frame} property.
 * \sa frame, setFrame()
 */
/*!
 * \qmlmethod void Renderable::setFrame(int fr)
 *
 * Sets the value of the \l{frame} property to \a{fr}.
 * \sa frame, getFrame()
 */
/*!
 * \qmlmethod TEX_ORIENT_TYPE Renderable::getTexOrient()
 *
 * Returns the value of the \l{texOrient} property.
 * \sa gyro::TEX_ORIENT_TYPE, texOrient, setTexOrient()
 */
/*!
 * \qmlmethod void Renderable::setTexOrient(TEX_ORIENT_TYPE orient)
 *
 * Sets the value of the \l{texOrient} property to \a{orient}.
 * \sa gyro::TEX_ORIENT_TYPE, texOrient, getTexOrient()
 */
/*!
 * \qmlmethod SpriteAttrib* Renderable::getSpriteAttrib()
 *
 * Returns the value of the \l{spriteAttrib} property.
 * This works fine whether \{spriteAttrib} is of type SpriteAttrib or
 * QuadSpriteAttrib, since QuadSpriteAttrib inherits from SpriteAttrib.
 * \sa spriteAttrib, setSpriteAttrib(), getQuadSpriteAttrib
 */
/*!
 * \qmlmethod QuadSpriteAttrib* Renderable::getQuadSpriteAttrib()
 *
 * Returns the value of the \l{spriteAttrib} property as type QuadSpriteAttrib.
 * \e{WARNING: This is only valid if \l{spriteAttrib} is of type QuadSpriteAttrib
 * or nil. Calling this function with \l{spriteAttrib} of type SpriteAttrib will
 * cause undefined behavior (ie fucking crash).}
 * \sa spriteAttrib, setSpriteAttrib(), getSpriteAttrib()
 */
/*!
 * \qmlmethod void Renderable::setSpriteAttrib(SpriteAttrib* attrib)
 *
 * Sets the value of the \l{spriteAttrib} property to \a{attrib}.
 * \a{attrib} can be of type SpriteAttrib, QuadSpriteAttrib, or nil.
 * If it's not nil, ownership is transferred from Lua to the engine.
 * \sa spriteAttrib, getSpriteAttrib(), getQuadSpriteAttrib()
 */
/*!
 * \qmlmethod Vector2 Renderable::getOffsetVec()
 *
 * Returns the value of the \l{offset} property.
 * \sa offset, setOffsetVec()
 */
/*!
 * \qmlmethod void Renderable::setOffsetVec(Vector2 vec)
 *
 * Sets the value of the \l{offset} property to \a{vec}.
 * \sa offset, getOffsetVec()
 */
/*!
 * \qmlmethod Vector2 Renderable::getUVCoords(int index)
 *
 * Returns the value of the \l{uvCoords} property at the given \a{index}.
 * Valid values for \a{index} are 1-4.
 * \sa uvCoords, setUVCoords(), getUVCoordsCount()
 */
/*!
 * \qmlmethod void Renderable::setUVCoords(int index, Vector2 coords)
 *
 * Sets the value of the \l{uvCoords} property at the given \a{index} to \a{coords}.
 * Valid values for \a{index} are 1-4.
 * \sa uvCoords, getUVCoords(), getUVCoordsCount()
 */
/*!
 * \qmlmethod int Renderable::getUVCoordsCount()
 *
 * Method used internally for retrieving the length of the \l{uvCoords} array.
 * This should always return 4.
 * \sa uvCoords, getUVCoords(), setUVCoords()
 */
/*!
 * \qmlmethod void Renderable::setTexCoord(int index, float u, float v)
 *
 * Sets the value of the \l{uvCoords} property at the given \a{index} to the
 * 2D vector created by <\a{u}, \a{v}>. Valid values for \a{index} are 1-4.
 * \sa uvCoords, getUVCoords(), setUVCoords(), getUVCoordsCount()
 */


OOLUA_EXPORT_FUNCTIONS(Renderable,
                       setFrame,
                       setOffsetVec,
                       setSpriteAttrib,
                       setTexCoord,
                       setTexOrient,
                       getOffsetVec,
                       getUVCoords,
                       setUVCoords)
OOLUA_EXPORT_FUNCTIONS_CONST(Renderable,
                            getFrame,
                            getSpriteAttrib,
                            getQuadSpriteAttrib,
                            getTexOrient,
                            getUVCoordsCount)


