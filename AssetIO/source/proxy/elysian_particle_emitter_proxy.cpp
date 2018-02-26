#include <proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype ParticleEmitterAttrib
    \inherits CollidableStaticAttrib
    \since 1.6.0
    \inmodule Engine
    \brief Attributes defining characteristics of a ParticleEmitter Component.

    ParticleEmitterAttrib is used to define characteristics of a ParticleEmitter Component
    and the particles it emits such as their size, color, lifetime, textures, and more.

    \section2 Configuring Texture Maps and Materials
    By default, particles are untextured, but a texture, normal, and specular map as well as a
    \l{Material} can all be specified through methods inherited from SpriteAttrib:
    \list
    \li SpriteAttrib::setTexPath()
    \li SpriteAttrib::setNormalMapPath()
    \li SpriteAttrib::setSpecularMapPath()
    \li SpriteAttrib::setMaterial()
    \endlist

    \section2 Color and Size
    Color and size are both linearly interpolated for each particle, so they can change colors, shrink,
    or grow, as they age. The exact size and color of the particle for a frame fades from its start
    value to its end value over the lifetime of the particle, which is given by \l{maxLife}. If you
    want either of these fields to stay constant, simply set the same value as the start and end properties.

    The start and end values for both properties are as follows:
    \table
    \header
       \li Property
       \li Initial Value
       \li End Value
    \row
       \li \b{color}
       \li SpriteAttrib::color
       \li \l{endColor}
    \row
       \li \b{size}
       \li \l{startSize}
       \li \l{endSize}
    \endtable

    \section2 Sprite Animation Grid
    By default, particles will all use the entirety of a texture if one is provided. However, if a
    uniformly-spaced sprite sheet is provided, particles can be randomized to use different sprites or even
    animate between sprites from the sprite sheet.

    This kind of sprite sheet behavior is configured by creating an "Animation Grid." This is a 2D grid of
    sprite sheet indices where each column from left-to-right represents the animation timeline of a particle,
    and each row represents a different kind of particle that can be spawned.

    \section3 Simple Example - Sprite Sheet with Animation Grid Layout
    The simplest way to create an animation grid is to arrange a sprite sheet like so:
    \code
    +------+------+------+
    |Leaf1 |Leaf2 |Leaf3 |
    +------+------+------+
    |Grass1|Grass2|Grass3|
    +--------------------+
    \endcode

    The top row represents the 3 animation frames of a leaf and the bottom row represents the 3 animations of a
    blade of grass. When a particle is emitted, it is assigned a row randomly, so it can either be a leaf or a
    blade of grass. Then, as it ages, it progresses from the sprite sheet index in the first column to the index
    in the third column, giving it animation.

    The following code would be used to configure the animation grid given the above sprite sheet texture
    configuration:
    \code
    particleEmitterAttrib:setAnimGridSize(3, 2)
    \endcode
    By default, the animation grid will be assigned indices that are increasing, which matches the sheet layout.

    \section3 Complex Example - Sprite Sheet with Arbitrary Layout
    In the previous example, the sprites themselves were organized within the sheet so that it matched the animation
    grid and required no additional configuration. However, this is not a requirement, and a sheet can be organized
    completely arbitrarily. This requires the sprite sheet indices of the animation grid to be set manually.

    Consider the following sprite sheet:
    \code
    +------+------+------+
    |Grass2||Leaf3|Leaf2 |
    +------+------+------+
    |Grass3|Leaf1 |Grass1|
    +--------------------+
    \endcode
    The sprites are no longer organized in the proper order, so we have to manually set the sprite indices for the
    corresponding animation grid in Lua. Remember that sheets are split so that every sprite is given a sequential
    index moving from left to right and top to bottom.
    \code
    --Set the size of the animation grid
    particleEmitterAttrib:setAnimGridSize(3, 2)

    --Set the sprite indices of the animation grid
    particleEmitterAttrib:setAnimGridIndex(0, 0, 4) --Leaf 1 at sheet index 4
    particleEmitterAttrib:setAnimGridIndex(0, 1, 2) --Leaf 2 at sheet index 2
    particleEmitterAttrib:setAnimGridIndex(0, 2, 1) --Leaf 3 at sheet index 1

    particleEmitterAttrib:setAnimGridIndex(1, 0, 5) --Grass 1 at sheet index 5
    particleEmitterAttrib:setAnimGridIndex(1, 1, 0) --Grass 2 at sheet index 0
    particleEmitterAttrib:setAnimGridIndex(1, 2, 3) --Grass 3 at sheet index 3
    \endcode

    \section3 Looping Animations
    If the animations in each row of the animation grid should loop rather than simply start
    at the first column and end at the last column, the \l{randomStartFrame} property should
    be set. This makes each particle start at a random column and causes them to wrap back
    around and animate continuously until the particle despawns.

    \section2 Applying Forces
    \section3 Constant Acceleration
    Very simple, static, continous linear forces can be applied uniformly to particles by
    setting it as the acceleration vector through ParticleEmitterAttrib::setAcceleration().
    By default, the acceleration vector is set to simulate gravity so that particles fall down

    \section3 Force Generators
    More complex forces can be applied to particles through the use of a ForceGenerator.
    Each ForceGenerator has a force flag property signifying the type of force it
    is applying. ParticleEmitterAttrib::forceMask is used to flag the types of forces
    that can interact with a particle. For example, you may have particles that do not
    respond to gravity, yet can be magnetized with a magnetic force. The forceMask should
    have a zero in the gravity bit location and a 1 in the magnetif force location to signal
    this behavior to the engine.

    Static physics attributes for particles such as mass and moment of inertia can be configured
    through the inherited CollidableStaticAttrib properties and methods.

    \sa ParticleEmitter, ParticleSystem
*/

/*!
\qmlproperty int ParticleEmitterAttrib::maxLife

The maximum number of frames an emitted particle can exist before being removed.

\sa getMaxLife(), setMaxLife()
*/
/*!
\qmlproperty int ParticleEmitterAttrib::startSize

The initial size of particles, immediately after spawning.

\sa getStartSize(), setStartSize()
*/
/*!
\qmlproperty int ParticleEmitterAttrib::endSize

The ending size of a particle before despawning.

\sa getEndSize(), setEndSize()
*/
/*!
\qmlproperty Color4 ParticleEmitterAttrib::endColor

The ending color of a particle before despawning.

\sa getEndColor(), setEndColor()
*/
/*!
\qmlproperty float ParticleEmitterAttrib::intensityFactor

Floating point multiplier applied to intensity when emitting particles to determine
how many particles are instantaneously emitted.

\sa getIntensityFactor(), setIntensityFactor()
*/
/*!
\qmlproperty bool ParticleEmitterAttrib::randomStartFrame

If set to true, and a texture is used with animations, the starting animation frame index
of spawned particles will be random. If set to false, the starting frame defaults to 0.
This property is set to false by default.

\sa getRandomStartFrame(), setRandomStartFrame()
*/
/*!
\qmlproperty BitMask32 ParticleEmitterAttrib::forceMask

32-bit bitmask representing the forces that can affect particles emitted from a ParticleEmitter
using these attributes. When a force is applied, its mask will be ANDed with \l{forceMask} to
check whether or not particles should be effected by the force.

\sa getForceMask(), setForceMask()
*/
/*!
\qmlproperty int ParticleEmitterAttrib::animFrameDuration

When using a texture with animations, this is how many frames of engine updates each animation
frame will last before advancing to the next.

\sa getAnimFrameDuration(), setAnimFrameDuration()
*/
/*!
\qmlproperty bool ParticleEmitterAttrib::shadowVolume

Set to true when particles cast shadows when hit by dynamic lights and false otherwise. This is set
to false by default, because casting shadows with a large number of particles is extremely slow.

\sa getShadowVolume(), setShadowVolume()
*/
/*!
\qmlproperty bool ParticleEmitterAttrib::stick

Set to true if the particles should stick to the ground and linger after being emitted or false if
they should fall through floors and leave the scene. This is set to false by default. Note that even when
set to true, the particles will eventually fade out and be removed, depending on the ParticleEmitter
pool size, when they need to be recycled and reemitted later.

\sa getStick(), setStick()
*/
/*!
 * \qmlmethod int ParticleEmitterAttrib::getMaxLife()
 *
 * Returns the value of the \l{maxLife} property.
 * \sa maxLife, setMaxLife()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setMaxLife(int life)
 *
 * Sets the value of the \l{maxLife} property to \a{life}.
 * \sa maxLife, getMaxLife()
 */
/*!
 * \qmlmethod int ParticleEmitterAttrib::getStartSize()
 *
 * Returns the value of the \l{startSize} property.
 * \sa startSize, setStartSize()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setStartSize(int size)
 *
 * Sets the value of the \l{startSize} property to \a{size}.
 * \sa startSize, getStartSize()
 */
/*!
 * \qmlmethod int ParticleEmitterAttrib::getEndSize()
 *
 * Returns the value of the \l{endSize} property.
 * \sa endSize, setEndSize()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setEndSize(int size)
 *
 * Sets the value of the \l{endSize} property to \a{size}.
 * \sa endSize, getEndSize()
 */
/*!
 * \qmlmethod Color4& ParticleEmitterAttrib::getEndColor()
 *
 * Returns the value of the \l{endColor} property.
 * \sa endColor, setEndColor()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setEndColor(Color4 color)
 *
 * Sets the value of the \l{endColor} property to \a{color}.
 * \sa endColor, getEndColor()
 */
/*!
 * \qmlmethod int ParticleEmitterAttrib::getIntensityFactor()
 *
 * Returns the value of the \l{intensityFactor} property.
 * \sa intensityFactor, setIntensityFactor()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setIntensityFactor(int factor)
 *
 * Sets the value of the \l{intensityFactor} property to \a{factor}.
 * \sa intensityFactor, getIntensityFactor()
 */
/*!
 * \qmlmethod bool ParticleEmitterAttrib::getRandomStartFrame()
 *
 * Returns the value of the \l{randomStartFrame} property.
 * \sa randomStartFrame, setRandomStartFrame()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setRandomStartFrame(bool val)
 *
 * Sets the value of the \l{randomStartFrame} property to \a{val}.
 * \sa randomStartFrame, getRandomStartFrame()
 */
/*!
 * \qmlmethod BitMask32& ParticleEmitterAttrib::getForceMask()
 *
 * Returns the value of the \l{forceMask} property.
 * \sa forceMask, setForceMask()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setForceMask(BitMask32 mask)
 *
 * Sets the value of the \l{forceMask} property to \a{mask}.
 * \sa forceMask, getForceMask()
 */
/*!
 * \qmlmethod int ParticleEmitterAttrib::getAnimFrameDuration()
 *
 * Returns the value of the \l{animFrameDuration} property.
 * \sa animFrameDuration, setAnimFrameDuration()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setAnimFrameDuration(int duration)
 *
 * Sets the value of the \l{animFrameDuration} property to \a{duration}.
 * \sa animFrameDuration, getAnimFrameDuration()
 */
/*!
 * \qmlmethod bool ParticleEmitterAttrib::getShadowVolume()
 *
 * Returns the value of the \l{shadowVolume} property.
 * \sa shadowVolume, setShadowVolume()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setShadowVolume(bool val)
 *
 * Sets the value of the \l{shadowVolume} property to \a{val}.
 * \sa shadowVolume, getShadowVolume()
 */
/*!
 * \qmlmethod bool ParticleEmitterAttrib::getStick()
 *
 * Returns the value of the \l{stick} property.
 * \sa stick, setStick()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setStick(bool val)
 *
 * Sets the value of the \l{stick} property to \a{val}.
 * \sa stick, getStick()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setAcceleration(Vector3 accel)
 *
 * Sets \a{accel} as an internal acceleration vector applied to the particles every frame.
 * The default value is set to simulate gravity.
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setAnimGridSize(int w, int h)
 *
 * Sets the size of the 2D animation grid to \a{w}x\a{h}.
 * \sa setAnimGridIndex()
 */
/*!
 * \qmlmethod void ParticleEmitterAttrib::setAnimGridIndex(int row, int col, int index)
 *
 * Sets the sprite index of the 2D animation grid at location [\a{row}, \a{col}] to \a{index}.
 * \sa setAnimGridSize()
 */

OOLUA_EXPORT_FUNCTIONS(ParticleEmitterAttrib,
                       setAcceleration,
                       setMaxLife,
                       setForceMask,
                       setIntensityFactor,
                       setStartSize,
                       setEndSize,
                       setEndColor,
                       setAnimGridSize,
                       setAnimGridIndex,
                       setAnimFrameDuration,
                       setStick,
                       setRandomStartFrame,
                       setShadowVolume,
                       getEndColor,
                       getForceMask)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleEmitterAttrib,
                       getShadowVolume,
                       getRandomStartFrame,
                       getStick,
                       getAnimFrameDuration,
                       getEndSize,
                       getStartSize,
                       getIntensityFactor,
                       getMaxLife)


/*!
    \qmltype ParticleEmitter
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Component attached to an Entity allowing it to emit particles.

    ParticleEmitter is an optional component that allows an Entity to emit particles for special effects when
    attached. An active Entity with a ParticleEmitter Component requires a ParticleEmitterAttrib object to be
    set as the \l{attrib} property. If this property is nil, behavior is undefined (ie: shit will crash). Two
    similar \l{ParticleEmitter}s should share a single ParticleEmitterAttrib object to reduce on memory overhead
    and time spent configuring emitters.

    \section2 Particle Characteristics and Configuration
    See \l{ParticleEmitterAttrib}.

    \section2 Emitting Particles
    There are three different ways to emit particles from a ParticleEmitter:
    \list
    \li ParticleEmitter::emitFromPoint() - emit from a world-space position with a 2D angle
    \li ParticleEmitter::emitFromPointInDir() - emit from a world-space position in a 3D direction
    \li ParticleEmitter::emitFromEntity() - emit from a local-space position with a 2D angle
    \endlist

    \section2 Performance and Optimization
    In general, particle emission is not necessarily costly, as it's fairly optimized, but depending on the
    circumtances, particles may become more expensive, especially in larger amounts. There are a few things
    to consider when tweaking for performance.
    \list
    \li \b{Shadow Volumes} - Whether particles spawned by the emitter will cast shadows or not is determined
    by the ParticleEmitterAttrib object set as the \l{attrib} field. If the ParticleEmitterAttrib::shadowVolume
    property is set to true, all particles coming from the emitter will cast shadows, which means they will have
    to be rerendered in at least one extra pass for each visible LightSource. This is a very costly operation.
    \li \b{Pool Size} - A ParticleEmitter has a statically allocated pool of particles that is used as a ringbuffer
    internally rather than dynamically allocating them as needed. This DRASTICALLY reduces allocation and
    processing overhead in exchange for a larger memory footprint. By default, the engine gives each emitter a pool
    of size 5000 particles. If you know that a ParticleEmitter will definitely not require this large of a pool size,
    you should manually lower it with ParticleEmitter::resizePool() to free wasted RAM.
    \li \b{Textures} - By default, particles are all untextured. Texturing the particles is more expensive, but not
    by a whole lot; however, normal and specular mapping them as well starts to become more expensive.
    \li \b{Lifetime} - The ParticleEmitterAttrib::maxLife property is used to configure how long until a particle
    despawns and can be recycled. Be sure to use the smallest maxLife required for the effect you need.
    \endlist

    \sa ParticleEmitterAttrib, ParticleSystem
*/
/*!
\qmlproperty ParticleEmitterAttrib* ParticleEmitter::attrib

Contains a reference to the ParticleEmitterAttrib defining characteristics of the ParticleEmitter,
or nil if one has not been set. \e{NOTE: A non-nil \l{attrib} property is required for any ParticleEmitter
attached to an active entity, otherwise behavior is undefined (ie: it's going to fucking crash).}

\sa getAttrib(), setAttrib()
*/
/*!
 * \qmlmethod ParticleEmitterAttrib* ParticleEmitter::getAttrib()
 *
 * Returns the value of the \l{attrib} property.
 * \sa attrib, setAttrib()
 */
/*!
 * \qmlmethod void ParticleEmitter::setAttrib(ParticleEmitterAttrib* attr)
 *
 * Sets the value of the \l{attrib} property to \a{attr}. Ownership of \a{attr} is transferred from
 * Lua to the engine.
 * \sa attrib, getAttrib()
 */
/*!
 * \qmlmethod void ParticleEmitter::emitFromPoint(Vector3 pos, float angle, int intensity)
 *
 * Emits a group of particles at the location given by \a{pos} in the 2D direction of \a{angle} (in radians).
 * The number of particles to emit is given by \a{intensity}.
 * \sa emitFromPointInDir(), emitFromEntity()
 */
/*!
<<<<<<< Updated upstream
 * \qmlmethod void ParticleEmitter::emitFromPointInDir(Vector3 pos, Vector3 dir, int intensity, float spread)
 *
 * Emits a group of particles at the location given by \a{pos} in the direction given by the unit vector, \a{dir}.
 * The number of particles to emit is given by \a{intensity}. \a{spread} is the 2D angle representing the spread
 * of the particles to be emitted from the point, forming a cone from the point of emission.
 * This gives the particles a slightly randomized initial position and velocity and is specified in degrees.
=======
 * \qmlmethod void ParticleEmitter::emitFromPointInDir(Vector3 pos, Vector3 dir, int intensity, float spreadAngle)
 *
 * Emits a group of particles at the location given by \a{pos} in the direction given by the unit vector, \a{dir}.
 * The number of particles to emit is given by \a{intensity}. \a{spreadAngle} specifies the amount of spead given
 * at the point of emission, which adds a bit of randomness to the particles' initial positions and velocities. This
 * value is given in degrees.
>>>>>>> Stashed changes
 * \sa emitFromPoint(), emitFromEntity()
 */
/*!
 * \qmlmethod void ParticleEmitter::emitFromEntity(Vector3 pos, float angle, int intensity)
 *
 * Emits a group of particles at location given by \a{pos} relative to the Entity (\a{pos} of <0, 0, 0> would
 * emit the group from the center of the entity). The 2D direction is given by \a{angle} in radians.
 * The number of particles to emit is given by \a{intensity}.
 * \sa emitFromPoint(), emitFromPointInDir()
 */
/*!
 * \qmlmethod void ParticleEmitter::resizePool(int size)
 *
 * Low-level function for reallocating the size of the particle pool used by the emitter. A larger size allows
 * for more particles on screen and for particles to last longer before needing to be recycled. Care must be taken
 * to not rape the shit out of consoles with limited RAM such as the Dreamcast by allocatig too many large pools.
 * The default size is 5000.
 */


OOLUA_EXPORT_FUNCTIONS(ParticleEmitter,
                       emitFromPoint,
                       emitFromPointInDir,
                       emitFromEntity,
                       setAttrib,
                       resizePool)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleEmitter,
                             getAttrib)
