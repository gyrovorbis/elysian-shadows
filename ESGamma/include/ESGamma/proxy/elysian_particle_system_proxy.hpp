/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_PARTICLE_SYSTEM_PROXY_HPP
#define ELYSIAN_PARTICLE_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/crtp/elysian_particle_system.hpp>

namespace OOLUA {
    typedef elysian::ParticleSystem     ParticleSystem;
}

OOLUA_PROXY(ParticleSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC(emitFromPoint)
    OOLUA_MFUNC_CONST(getAttrib)
OOLUA_PROXY_END



#endif // ELYSIAN_PARTICLE_SYSTEM_PROXY_HPP

