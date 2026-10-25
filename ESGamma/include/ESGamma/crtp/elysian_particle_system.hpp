/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_PARTICLE_SYSTEM_HPP
#define ELYSIAN_PARTICLE_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include <system/elysian_particle_system_base.hpp>
namespace elysian {
    using ParticleSystem    = ParticleSystemBase;
}
#else
#   include <ESToolkit/ESGamma/tk_particle_system.h>
namespace elysian {
    using ParticleSystem    = TkParticleSystem;
}
using ParticleSystem        = TkParticleSystem;

#endif



#endif // ELYSIAN_PARTICLE_SYSTEM_HPP
