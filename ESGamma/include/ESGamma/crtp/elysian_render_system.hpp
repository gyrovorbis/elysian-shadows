/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_RENDER_SYSTEM_HPP
#define ELYSIAN_RENDER_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <system/elysian_render_system_base.hpp>
namespace elysian {
    using ParticleSystem = ParticleSystemBase;
}
#else

#   include <ESToolkit/ESGamma/tk_render_system.h>

namespace elysian {
    using RendererSystem = TkRenderSystem;
}
using RenderSystem = TkRenderSystem;

#endif

#endif // ELYSIAN_RENDER_SYSTEM_HPP
