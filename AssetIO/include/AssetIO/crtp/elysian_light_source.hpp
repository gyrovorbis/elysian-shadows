/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LIGHT_SOURCE_HPP
#define ELYSIAN_LIGHT_SOURCE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_light_source_base.hpp>
namespace elysian {
    typedef LightSourceBase LightSource;
}
#else
#   include <ESToolkit/AssetIO/tk_light_source.h>
namespace elysian {
    typedef TkLightSource LightSource;
}
typedef TkLightSource LightSource;

#endif

#endif // ELYSIAN_LIGHT_SOURCE_HPP

