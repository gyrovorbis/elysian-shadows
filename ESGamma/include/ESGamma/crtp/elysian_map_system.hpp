/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_MAP_SYSTEM_HPP
#define ELYSIAN_MAP_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include "elysian_map_system_base.hpp"
namespace elysian {
    typedef MapSystemBase MapSystem;
}
#else
#   include <ESToolkit/ESGamma/tk_map_system.h>
namespace elysian {
    typedef TkMapSystem MapSystem;
}
typedef TkMapSystem MapSystem;

#endif



#endif // ELYSIAN_MAP_SYSTEM_HPP
