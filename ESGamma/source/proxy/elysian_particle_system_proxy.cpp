#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_particle_system_proxy.hpp>


/*!
    \qmltype ParticleSystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine subsystem for creating and managing particle effects.

    The ParticleSystem is the \l{Engine} subsystem for managing all particle effects.
    It assumes ownership over and manages all \l{ParticleEmitter} \l{Component}s within
    the current \l{Area} and also acts as a separate, standalone, global particle emitter
    for managing particles that are not components associated with an \l{Entity} such as
    foreground and GUI-layer particles.

    \section2 Retrieving the ParticleSystem
    The ParticleSystem object can be retrieved through the static \l{Engine} class:
    \code
    particleSys = Engine.getPartSys()
    \endcode

    \section2 Particle Resource Pools
    Each particle emitter uses a static pool internally for particle allocation and management. While
    the size of these pools can be configured at the time of creation, if more particles are being
    emitted than are available within the pool, the older particles will simply be recycled prematurely.
    This is most often not an issue at all and is very resource-friendly, but if you notice that particle
    effects aren't lingering long enough before being recycled, try increasing the pool size.

    \section2 Emitting Particles
    \section3 ParticleEmitter Component
    Particle effects should generally be created through attaching \l{ParticleEmitter} components
    attached to entities. Handling particles this way allows the engine to manage the positioning
    of the emitters for you and allows you to give each \l{Entity} its own \l{ParticleEmitterAttrib}
    object which will automatically be shared among instances of the same \l{Prefabs}.

    Giving each \l{Entity} its own emitter will prevent them from competing for particle allocation from
    the same pool.

    \section3 Global Emitters
    For any particle effects that are not specifically associated with any particular \l{Entity} or that
    even may not take place in the game world itself, such as overlay or UI-level particle effects, the
    ParticleSystem itself maintains a set of global particle emitters that can be used for these
    general-purpose scenarios.

    The ParticleSystem internally stores 4 sets of \l{ParticleEmitterAttrib} objects which may be referenced
    when emitting a batch of particles. Notice \l{getAttrib} and \l{emitFromPoint} both take an integer index
    as the first argument. This index allows you to use or retrieve one of these four sets of attributes.

   \e{NOTE: Despite the fact that there are 4 sets of attributes, they are all spawning from one big, massive
        shared pool of particles, meaning if a first batch of particles is emitted using attrib index 1, then a
        second, much larger batch is emitted with attrib index 2, and there aren't enough free particles within the
        pool, the original particles emitted using attrib index 1 may be recycled prematurely in order to be reused for
        the second batch. This is done due to memory constraints.}

   \e{NOTE: Just because there are only 4 different sets of \l{ParticleEmitterAttrib} objects available internally doesn't
        mean you can only produce 4 different sets of particles from the global emitters at any given time. If you need more than 4,
        you can always store the attributes internally within a script then swap them in and out to and from the engine as needed.}



    \sa ParticleEmitter, ParticleEmitterAttrib, Engine
*/


/*!
 * \qmlmethod ParticleEmitterAttrib* ParticleSystem::getAttrib(int index)
 * Returns a reference to the internal \l{ParticleEmitterAttrib} object located at the given \a{index}. This reference can be modified and is used to
 * configure the attributes of the global emitter the same way they are used to configure the attributes for individual \l{ParticleEmitter} components.
 *
 * \sa ParticleEmitterAttrib
 */
        void emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity);
/*!
 * \qmlmethod void ParticleSystem::emitFromPoint(int index, Vector3 pos, float angle, int intensity)
 * Creates a batch with the number of particles given by \a{intensity} and emits them from the position given by \a{pos} in
 * the 2D direction given by \a{angle} (in radians) using the internal \l{ParticleEmitterAttrib} object specified by \a{index}
 * to define their attributes.
 * \sa getAttrib, ParticleEmitterAttrib
 */

OOLUA_EXPORT_FUNCTIONS(ParticleSystem,
                       emitFromPoint)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleSystem,
                             getAttrib)
