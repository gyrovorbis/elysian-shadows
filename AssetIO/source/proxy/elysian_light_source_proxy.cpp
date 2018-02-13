#include <proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype LightAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Attributes describing a LightSource.

    LighAttrib contains information about a LightSource such as the type of light it is, its color,
    intensity, attenuation factors, etc. A single LightAttrib object can be shared among a group of
    similar LightSource objects to reduce memory overhead and save time configuring lighting.

    \sa LightSource, LightSystem
*/
/*!
 * \qmlproperty Vector4 LightAttrib::diffuse
 *
 * 4D RGBA vector representing the diffuse color intensity of the light. Diffuse light is standard
 * directional light that is more intense on planes perpendicular to the LightSource and less intense
 * on planes not directly facing the LightSource.
 * \sa getDiffuse(), setDiffuse()
 */
/*!
 * \qmlproperty Vector4 LightAttrib::specular
 *
 * 4D RGBA vector representing the specular color intensity of the light. Specular light is reflective
 * light. It's a function of the direction of the LightSource relative to the surface plane AND the
 * direction of the viewing vector. It's the kind of light that is reflected when a metallic object
 * is lit by the sun, and you're seeing the glare.
 * \sa getSpecular(), setSpecular()
 */
/*!
 * \qmlproperty Vector4 LightAttrib::attenuation
 *
 * 3D vector representing the coefficients for the quadratic lighting attenuation function.
 * The attenuation controls how large of a \l{radius} the light effects and how quickly it fades away
 * when moving away from its center. The x component is the quadratic coefficient, the y component
 * is the linear coefficient, and the z component is the constant coefficient. The higher the values,
 * the smaller the \l{radius} of the light.
 * \sa getAttenuation(), setAttenuation()
 */
/*!
 * \qmlproperty float LightAttrib::spotExponent
 *
 * Floatig point value used to determine the radial intensity of a spot light. A high exponent produces
 * a very sharp, circularly defined spot light while a lower exponent produces a less defined, more
 * gradually fading spot light. Think of this as the "focus" of the spot light. This value is ignored
 * for directional and point lights.
 * \sa getSpotExponent(), setSpotExponent()
 */
/*!
 * \qmlproperty float LightAttrib::radius
 *
 * Read-only property representing the approximate pixel radius lit by the LightSource. This is calculated
 * from the \l{attenuation} factors and is used by the renderer to determine which objects need to be lit
 * by the LightSource.
 * \sa getRadius()
 */
/*!
 * \qmlmethod Vector4 LightAttrib::getDiffuse()
 *
 * Returns the value of the \l{diffuse} property.
 * \sa diffuse, setDiffuse()
 */
/*!
 * \qmlmethod void LightAttrib::setDiffuse(Vector4 intensity)
 *
 * Sets the value of the \l{diffuse} property to \a{intensity}.
 * \sa diffuse, getDiffuse()
 */
/*!
 * \qmlmethod Vector4 LightAttrib::getSpecular()
 *
 * Returns the value of the \l{specular} property.
 * \sa specular, setSpecular()
 */
/*!
 * \qmlmethod void LightAttrib::setSpecular(Vector4 intensity)
 *
 * Sets the value of the \l{specular} property to \a{intensity}.
 * \sa specular, getSpecular()
 */
/*!
 * \qmlmethod Vector3 LightAttrib::getAttenuation()
 *
 * Returns the value of the \l{attenuation} property.
 * \sa attenuation, setAttenuation()
 */
/*!
 * \qmlmethod void LightAttrib::setAttenuation(Vector3 factors)
 *
 * Sets the value of the \l{attenuation} property to \a{factors}.
 * \sa attenuation, getAttenuation()
 */
/*!
 * \qmlmethod float LightAttrib::getSpotExponent()
 *
 * Returns the value of the \l{spotExponent} property.
 * \sa spotExponent, setSpotExponent()
 */
/*!
 * \qmlmethod void LightAttrib::setSpotExponent(float exp)
 *
 * Sets the value of the \l{spotExponent} property to \a{exp}.
 * \sa spotExponent, getSpotExponent()
 */
/*!
 * \qmlmethod float LightAttrib::getRadius()
 *
 * Returns the value of the \l{radius} property.
 * \sa radius
 */
/*!
 * \qmlmethod void LightAttrib::getDiffuseColor(Vector4* color)
 *
 * Sets \a{color} equal to the value of the \l{diffuse} property.
 * \sa diffuse, getDiffuse(), setDiffuse()
 */
/*!
 * \qmlmethod void LightAttrib::getSpecularColor(Vector4* color)
 *
 * Sets \a{color} equal to the value of the \l{specular} property.
 * \sa specular, getSpecular(), setSpecular()
 */
/*!
 * \qmlmethod void LightAttrib::getAttenuationFactors(Vector3* factors)
 *
 * Sets \a{factors} equal to the value of the \l{attenuation} property.
 * \sa attenuation, getAttenuation(), setAttenuation()
 */

OOLUA_EXPORT_FUNCTIONS(LightAttrib,
                       setAmbientColor,
                       setDiffuseColor,
                       setSpecularColor,
                       setAttenuationFactors,
                       setSpotExponent,
                       getAmbient,
                       getDiffuse,
                       getSpecular,
                       getAttenuation,
                       getRadius,
                       setDiffuse,
                       setSpecular,
                       setAmbient,
                       setAttenuation)
OOLUA_EXPORT_FUNCTIONS_CONST(LightAttrib,
                             getAmbientColor,
                             getDiffuseColor,
                             getSpecularColor,
                             getAttenuationFactors,
                             getSpotExponent)

/*!
    \qmltype LightSource
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Component for adding a spot, point, or directional dynamic light source.

    Attaching a LightSource Component to an Entity turns it into a dynamic light source which will
    then illuminate nearby tiles and entities within the scene. Dynamic lights can also be configured
    to optionally cast pixel-perfect shadows.

    Instance properties such as type of light, direction, relative position is stored within the
    LightSource itself, while properties describing the color and intensities of the light are
    stored as LightAttrib objects within the \l{attrib} property so that similar LightSources can
    share the same LightAttrib data.

    \section2 Types of Dynamic Lights
    \list
    \li \b{Directional} - Used to model very distant, large light sources such as the sun where only direction
    (and not position) is taken into account and the entire scene is lit uniformly.
    \li \b{Spot} - Used to model cone-shaped lighting that starts at a point then spreads out, like a flashlight.
    Spot lights have position, direction, attenuation, cutoffAngle, and spotExponents.
    \li \b{Point} - Used to model a light source that emits light evenly in all directions, such as a glowing orb.
    Since it's omnidirectional, it only has a point and not a direction. It also has attenuation and spotExponents.
    \endlist

    \section2 Performance
    \section3 Forward Rendering
    The LightSystem uses a forward renderer, meaning that extra lights require additional render passes. However,
    the shaders operate in sets of four, so up to 4 dynamic lights can be visible simultaneously with neglegible
    performance impact. Each additional light after 4 will degrade performance substantially.

    \section3 Shadow Mapping
    Shadow mapping is the most expensive part of dyanmic lighting. The light calculations themselves aren't too
    costly, but additional passes are required for every direction that the light can cast shadows from. As such,
    shadow maps for Directional and Spot lights require only a single extra pass, while Point lights require 6
    extra passes to cast shadows in every direction. Because of this, with shadows enabled, Point lights are
    significantly more costly than directional and spot lights.

    \section3 Optimizations
    It is recommended that shadows are disabled for small, insignificant light sources in the scene, when they
    don't add much visually. This will substantially increase performance.

    Spot lights can also be pointed straight down from above to emulate the effect of a point light with better
    performance, quite often.

    \sa LightAttrib, LightSystem
*/

/*!
\qmlproperty enumeration LightSource::LIGHT_TYPE

Enumeration representing the different types of dynamic lights.

The light types are:
\list
\li LightSource.LIGHT_DIRECTIONAL
\li LightSource.LIGHT_SPOT
\li LightSource.LIGHT_POINT
\endlist
See \l{Types of Dynamic Lights} for detailed descriptions of each.
*/

/*!
 * \qmlproperty bool LightSource::shadowCaster
 *
 * Boolean property set to true if the LightSource is to cast shadows and false otherwise. This disabled by
 * default, and should not be enabled for minor, small lights, as shadow mapping is a very expensive operation,
 * especially for point lights. Use sparingly.
 * \sa isShadowCaster(), setShadowCaster()
 */
/*!
 * \qmlproperty LIGHT_TYPE LightSource::type
 *
 * Property storing an enumeration value representing the type of light.
 * \sa getLightType(), setLightType()
 */
/*!
 * \qmlproperty LightAttrib* LightSource::attrib
 *
 * Reference to a LightAttrib object storing data describing the intensity and colors of the light or nil
 * if no LightAttrib object has been set.
 * \sa getAttrib(), setAttrib()
 */
/*!
 * \qmlproperty Vector3 LightSource::relPos
 *
 * 3D vector representing the positional offset of the light relative to the center of the Entity.
 * The default value is <0, 0, 0>, which is the center of the Entity. This is not used with directional
 * lights.
 * \sa getRelPos(), setRelPos()
 */
/*!
 * \qmlproperty Vector3 LightSource::dir
 *
 * 3D normalized direction vector representing the direction of the light. This is not used with
 * point lights.
 * \sa getDir()
 */
/*!
 * \qmlproperty float LightSource::spotCutoffAngle
 *
 * Floating point value representing the cutoff angle of the light. In a spot light, this is
 * the angle between the center of the cone and the hypoteneuse, or its spread. This should
 * never be greater than 90 degrees with a spot light. It is ignored for a directional light,
 * and it should always be set to 360 degrees for a point light.
 * \sa getSpotCutoffAngle(), setSpotCutoffAngle()
 */
/*!
 * \qmlmethod bool LightSource::isShadowCaster()
 *
 * Returns the value of the \l{shadowCaster} property.
 * \sa shadowCaster, setShadowCaster()
 */
/*!
 * \qmlmethod void LightSource::setShadowCaster(bool val)
 *
 * Sets the value of the \l{shadowCaster} property to \a{val}.
 * \sa shadowCaster, isShadowCaster()
 */
/*!
 * \qmlmethod LIGHT_TYPE LightSource::getLightType()
 *
 * Returns the value of the \l{type} property.
 * \sa type, setLightType()
 */
/*!
 * \qmlmethod void LightSource::setLightType(LIGHT_TYPE type)
 *
 * Sets the value of the \l{type} property to \a{type}.
 * \sa type, getLightType()
 */
/*!
 * \qmlmethod LightAttrib* LightSource::getAttrib()
 *
 * Returns the value of the \l{attrib} property.
 * \sa attrib, setAttrib()
 */
/*!
 * \qmlmethod void LightSource::setAttrib(LightAttrib* obj)
 *
 * Sets the value of the \l{attrib} property to \a{obj}.
 * Ownership of \a{obj} is transferred from Lua to the engine.
 * \sa attrib, getAttrib()
 */
/*!
 * \qmlmethod Vector3 LightSource::getRelPos()
 *
 * Returns the value of the \l{relPos} property.
 * \sa relPos, setRelPos
 */
/*!
 * \qmlmethod Vector3 LightSource::getRelPos()
 *
 * Sets the value of the \l{relPos} property.
 * \sa relPos, getRelPos
 */
/*!
 * \qmlmethod Vector3 LightSource::getDir()
 *
 * Returns the value of the \l{dir} property.
 * \sa dir
 */
/*!
 * \qmlmethod float LightSource::getSpotCutoffAngle()
 *
 * Returns the value of the \l{spotCutoffAngle} property.
 * \sa spotCutoffAngle, setSpotCutoffAngle()
 */
/*!
 * \qmlmethod void LightSource::setSpotCutoffAngle(float angle)
 *
 * Sets the value of the \l{spotCutoffAngle} property to \a{angle}.
 * \sa spotCutoffAngle, getSpotCutoffAngle()
 */

OOLUA_EXPORT_FUNCTIONS(LightSource,
                       setLightType,
                       setDirection,
                       setSpotCutoffAngle,
                       setAttrib,
                       setShadowCaster,
                     //  setRelativePos,
                       setRelPos,
                       getRelPos,
                       getDir)
OOLUA_EXPORT_FUNCTIONS_CONST(LightSource,
                             getDirection,
                             getSpotCutoffAngle,
                             getLightType,
                             getAttrib,
                             isShadowCaster,
                             getRelativePos)
