#include <AssetIO/proxy/elysian_primitive_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(GYColor4, setA, setR, setG, setB)
OOLUA_EXPORT_FUNCTIONS_CONST(GYColor4, getA, getR, getG, getB)


/*!
    \qmltype Color4
    \since 1.6.0
    \inmodule Engine
    \brief 32-Bit Color in RGBA format.

    Color4 is a 32-bit color in RGBA format, with 8 bits for each channel.
*/
/*!
\qmlproperty byte Color4::r

Value of the 8-bit red channel. 0 is none, 255 is full.
\sa getR(), setR()
*/
/*!
\qmlproperty byte Color4::g

Value of the 8-bit green channel. 0 is none, 255 is full.
\sa getG(), setG()
*/
/*!
\qmlproperty byte Color4::b

Value of the 8-bit blue channel. 0 is none, 255 is full.
\sa getB(), setB()
*/
/*!
\qmlproperty byte Color4::a

Value of the 8-bit alpha channel. 0 fully transparent, 255 is fully opaque.
\sa getA(), setA()
*/
/*!
 * \qmlmethod byte Color4::getR()
 *
 * Returns the value of the \a{r} property.
 * \sa r, setR()
 */
/*!
 * \qmlmethod void Color4::setR(byte val)
 *
 * Sets the value of the \a{r} property to \a{val}.
 * \sa r, getR()
 */
/*!
 * \qmlmethod byte Color4::getA()
 *
 * Returns the value of the \a{a} property.
 * \sa a, setA()
 */
/*!
 * \qmlmethod void Color4::setA(byte val)
 *
 * Sets the value of the \a{a} property to \a{val}.
 * \sa a, getA()
 */
/*!
 * \qmlmethod byte Color4::getG()
 *
 * Returns the value of the \a{g} property.
 * \sa g, setG()
 */
/*!
 * \qmlmethod void Color4::setG(byte val)
 *
 * Sets the value of the \a{g} property to \a{val}.
 * \sa g, getG()
 */
/*!
 * \qmlmethod byte Color4::getB()
 *
 * Returns the value of the \a{b} property.
 * \sa b, setB()
 */
/*!
 * \qmlmethod void Color4::setB(byte val)
 *
 * Sets the value of the \a{b} property to \a{val}.
 * \sa b, getB()
 */

OOLUA_EXPORT_FUNCTIONS(Color4)
OOLUA_EXPORT_FUNCTIONS_CONST(Color4, getA, getR, getG, getB, setA, setR, setG, setB)



/*!
    \qmltype PrimAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Base class for all Renderable Primitive Attributes.

    PrimAttrib is an abstract base class inherited by all primitive attributes.
*/
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(PrimAttrib)
EXPORT_OOLUA_FUNCTIONS_0_CONST(PrimAttrib)


/*!
    \qmltype SpriteAttrib
    \inherits PrimAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Renderable attributes for sprite primitives.

    SpriteAttrib contains renderable attributes used by all sprite primitives. An Entity can optionally
    use a \l{SpriteAttrib} object through the \l{Renderable} \l{Component}.

    \e{NOTE: All texture assets for use within ESTk should be saved in the PNG format!}

    \sa Renderable
*/
/*!
\qmlproperty string SpriteAttrib::texture

Asset-relative path to a PNG to use as texture or nil if no path is set.
\sa getTexPath(), setTexPath()
*/
/*!
\qmlproperty string SpriteAttrib::normalMap

Asset-relative path to a PNG to use as a normal map or nil if no path is set.
\sa getNormalMapPath(), setNormalMapPath()
*/
/*!
\qmlproperty string SpriteAttrib::specularMap

Asset-relative path to a PNG to use as a specular map or nil if no path is set.
\sa getSpecularMapPath(), setSpecularMapPath()
*/
/*!
\qmlproperty Material* SpriteAttrib::material

Material to be used by the sprite or nil if no material is set.
\sa getMaterial(), setMaterial()
*/
/*!
\qmlproperty Color4 SpriteAttrib::color

Color to be applied to the sprite. Defaults to white and fully opaque.
\sa getColor4(), setColor4()
*/
/*!
\qmlproperty int SpriteAttrib::frame

Sprite frame when using a sprite sheet texture. Frames are numbered from
left to right and top to bottom, starting at frame 0 and increasing. If no texture is
present, frame is ignored. If a texture is present, and frame is equal to -1,
the sprite is rendered using custom texture coordinates.
\sa getFrame(), setFrame(), setTexCoord()
*/
/*!
 * \qmlmethod string SpriteAttrib::getTexPath()
 *
 * Returns the value of the \l{texture} property.
 * \sa texture, setTexPath()
 */
/*!
 * \qmlmethod void SpriteAttrib::setTexPath(string path)
 *
 * Sets the value of the \l{texture} property to \a{path}.
 * \sa texture, getTexPath()
 */
/*!
 * \qmlmethod string SpriteAttrib::getNormalMapPath()
 *
 * Returns the value of the \l{normalMap} property.
 * \sa normalMap, setNormalMapPath()
 */
/*!
 * \qmlmethod void SpriteAttrib::setNormalMapPath(string path)
 *
 * Sets the value of the \l{normalMap} property to \a{path}.
 * \sa normalMap, getNormalMapPath()
 */
/*!
 * \qmlmethod string SpriteAttrib::getSpecularMapPath()
 *
 * Returns the value of the \l{specularMap} property.
 * \sa specularMap, setSpecularMapPath()
 */
/*!
 * \qmlmethod void SpriteAttrib::setSpecularMapPath(string path)
 *
 * Sets the value of the \l{specularMap} property to \a{path}.
 * \sa specularMap, getSpecularMapPath()
 */
/*!
 * \qmlmethod Material* SpriteAttrib::getMaterial()
 *
 * Returns the value of the \l{material} property.
 * \sa material, setMaterial()
 */
/*!
 * \qmlmethod void SpriteAttrib::setMaterial(Material* mat)
 *
 * Sets the value of the \l{material} property to \a{mat}.
 * Ownership of \a{mat} is transferred from Lua to the engine.
 * \sa material, getMaterial()
 */
/*!
 * \qmlmethod Color4 SpriteAttrib::getColor4()
 *
 * Returns the value of the \l{color} property.
 * \sa color, setColor4()
 */
/*!
 * \qmlmethod void SpriteAttrib::setColor4(Color4 col)
 *
 * Sets the value of the \l{color} property to \a{col}.
 * \sa color, getColor4()
 */
/*!
 * \qmlmethod int SpriteAttrib::getFrame()
 *
 * Returns the value of the \l{frame} property.
 * \sa frame, setFrame()
 */
/*!
 * \qmlmethod void SpriteAttrib::setFrame(int fr)
 *
 * Sets the value of the \l{frame} property to \a{fr}.
 * \sa frame, getFrame()
 */
/*!
 * \qmlmethod void SpriteAttrib::loadTexture(string filePath, int sheetWidth, int sheetHeight, int spriteWidth, int spriteHeight)
 *
 * Loads the PNG texture from the asset-relative path given by \a{filePath} with the image dimensions of
 * [\a{sheetWidth}x\a{sheetHeight}], splitting it into frames of size [\a{spriteWidth}x\a{spriteHeight}].
 * If the texture is a standard image and not a multi-frame spritesheet, simply pass spriteWidth and spriteHeight
 * with the same dimensions as sheetWidth and sheetHeight, so the entire image will be used as the first (default) frame.
 * \sa texture, setTexPath()
 */
/*!
 * \qmlmethod void SpriteAttrib::setTexCoord(int index, float u, float v)
 *
 * Sets the texture coordinates for the vertex given by \a{index} manually to <\a{u}, \a{v}>.
 * The \l{frame} must be set to -1 to use custom texture coordinates.
 * \sa frame, setFrame()
 */
/*!
 * \qmlmethod void SpriteAttrib::setColor(int r, int g, int b, int a)
 *
 * Sets the \a{color} property from separate RGBA colors. Valid values range from 0-255.
 * \sa color, setColor4(), getColor4()
 */
/*!
 * \qmlmethod void SpriteAttrib::setAlpha(int a)
 *
 * Sets the alpha component of the \a{color} property to \a{a}.
 * \sa getAlpha(), color, setColor()
 */
/*!
 * \qmlmethod int SpriteAttrib::getAlpha()
 *
 * Returns the value of the alpha component of the \a{color} property.
 * \sa getAlpha(), color, setColor()
 */
OOLUA_EXPORT_FUNCTIONS(SpriteAttrib,
                        setFrame,
                        setColor,
                        setAlpha,
                        loadTexture,
                        loadSheet,
                        setTexCoord,
                        getColor4,
                        setColor4,
                        setTexPath,
                        setSpecularMapPath,
                        setNormalMapPath,
                        setMaterial)
OOLUA_EXPORT_FUNCTIONS_CONST(SpriteAttrib,
                            getAlpha,
                            getTexPath,
                            getFrame,
                            getNormalMapPath,
                            getSpecularMapPath,
                            getMaterial)

/*!
    \qmltype QuadSpriteAttrib
    \inherits SpriteAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Renderable attributes for gourad shaded sprite primitives.

    QuadSpriteAttrib is an extension of SpriteAttrib that allows a sprite to be rendered with
    each of the four vertices getting its own color rather than a single solid color. When different
    colors are used, the polygon is filled in with a gradient using Gourad Shading.

    The color of the first vertex is taken from the parent class: SpriteAttrib::color.

    \sa Renderable
*/
/*!
 * \qmlmethod Color4 QuadSpriteAttrib::getVertexColor4(int index)
 *
 * Returns the 32-bit RGBA color associated with the vertex given by \a{index}.
 * Valid values are 0-3.
 * \sa setVertexColor4
 */
/*!
 * \qmlmethod void QuadSpriteAttrib::setVertexColor4(int index, Color4 color)
 *
 * Sets the 32-bit RGBA color associated with the vertex given by \a{index} to \a{color}.
 * Valid values for \a{index} are 0-3.
 * \sa getVertexColor4
 */
/*!
 * \qmlmethod void QuadSpriteAttrib::setVerticesAlpha(int a0, int a1, int a2, int a3)
 *
 * Sets the alpha values of all four vertices to the values given by \a{a0}, \a{a1}, \a{a2}, and \a{a3}.
 * Valid values are 0-255 where 0 is fully transparent and 255 is fully opaque.
 */
OOLUA_EXPORT_FUNCTIONS_CONST(QuadSpriteAttrib)
OOLUA_EXPORT_FUNCTIONS(QuadSpriteAttrib,
                        setVertexColor4,
                        setVerticesAlpha,
                        getVertexColor4)


/*!
    \qmltype Material
    \since 1.6.0
    \inmodule Engine
    \brief Describes how a polygon interacts with a LightSource to create its final color.

    A Material is used to modify how a polygon, sprite, or Entity interacts with dynamic lighting
    to produce the final colors of the object. It contains properties used as color/intensity
    multipliers within the lighting equations. For example, a Material can be used to make an object
    reflect more \l{diffuse} green light than red and blue for some alien-ass space ship effect. A
    material can be used to make a metallic object extra shiny by increasing the intensity of the
    \l{specular} component. It can also make an object still emit light even when there is no LightSource
    present within a scene through its \l{emissive} property.

    \sa LightSource, LightSystem
*/
/*!
\qmlproperty Vector4 Material::ambient

4D vector serving as a multiplier for the RGBA components of the final ambient lighting color.
\sa getAmbientColor(), setAmbientColor()
*/
/*!
\qmlproperty Vector4 Material::diffuse

4D vector serving as a multiplier for the RGBA components of the final diffuse lighting color.
\sa getDiffuseColor(), setDiffuseColor()
*/
/*!
\qmlproperty Vector4 Material::specular

4D vector serving as a multiplier for the RGBA components of the final specular lighting color.
\sa getSpecularColor(), setSpecularColor()
*/
/*!
\qmlproperty Vector4 Material::emissive

4D vector serving as an RGBA base color to which all lighting components and colors are added,
meaning this color is present with or without external light.
\sa getEmissiveColor(), setEmissiveColor()
*/
/*!
\qmlproperty float Material::shininess

Floating point scalar multiplier applied to the shininess component within the specular lighting
equations.
\sa getShininess(), setShininess()
*/
/*!
 * \qmlmethod Vector4 Material::getAmbientColor()
 *
 * Returns the value of the \l{ambient} property.
 * \sa ambient, setAmbientColor()
 */
/*!
 * \qmlmethod void Material::setAmbientColor(Vector4 color)
 *
 * Sets the value of the \l{ambient} property to \a{color}.
 * \sa ambient, getAmbientColor()
 */
/*!
 * \qmlmethod Vector4 Material::getDiffuseColor()
 *
 * Returns the value of the \l{diffuse} property.
 * \sa diffuse, setDiffuseColor()
 */
/*!
 * \qmlmethod void Material::setDiffuseColor(Vector4 color)
 *
 * Sets the value of the \l{diffuse} property to \a{color}.
 * \sa diffuse, getDiffuseColor()
 */
/*!
 * \qmlmethod Vector4 Material::getSpecularColor()
 *
 * Returns the value of the \l{specular} property.
 * \sa specular, setSpecularColor()
 */
/*!
 * \qmlmethod void Material::setSpecularColor(Vector4 color)
 *
 * Sets the value of the \l{specular} property to \a{specular}.
 * \sa specular, getSpecularColor()
 */
/*!
 * \qmlmethod Vector4 Material::getEmissiveColor()
 *
 * Returns the value of the \l{emissive} property.
 * \sa emissive, setEmissiveColor()
 */
/*!
 * \qmlmethod void Material::setEmissiveColor(Vector4 color)
 *
 * Sets the value of the \l{emissive} property to \a{color}.
 * \sa emissive, getEmissiveColor()
 */
/*!
 * \qmlmethod float Material::getShininess()
 *
 * Returns the value of the \l{shininess} property.
 * \sa shininess, setShininess()
 */
/*!
 * \qmlmethod void Material::setShininess(float factor)
 *
 * Sets the value of the \l{shininess} property to \a{factor}.
 * \sa shininess, getShininess()
 */
OOLUA_EXPORT_FUNCTIONS(Material,
                       setShininess,
                       setAmbientColor,
                       setDiffuseColor,
                       setSpecularColor,
                       setEmissiveColor,
                       getAmbient,
                       getDiffuse,
                       getEmissive,
                       getSpecular)
OOLUA_EXPORT_FUNCTIONS_CONST(Material,
                             getAmbientColor,
                             getDiffuseColor,
                             getSpecularColor,
                             getEmissiveColor,
                             getShininess)


OOLUA_EXPORT_FUNCTIONS(Recti,
                       setTop,
                       setBottom,
                       setLeft,
                       setRight)

OOLUA_EXPORT_FUNCTIONS_CONST(Recti,
                       getTop,
                       getBottom,
                       getLeft,
                       getRight)
