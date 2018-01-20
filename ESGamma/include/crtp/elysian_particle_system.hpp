#ifndef ELYSIAN_PARTICLE_SYSTEM_HPP
#define ELYSIAN_PARTICLE_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include <system/elysian_particle_system_base.hpp>
namespace elysian {
    using ParticleSystem    = ParticleSystemBase;
}
#else
#   include <tk_particle_system.h>
namespace elysian {
    using ParticleSystem    = TkParticleSystem;
}
using ParticleSystem        = TkParticleSystem;

#endif



#endif // ELYSIAN_PARTICLE_SYSTEM_HPP
