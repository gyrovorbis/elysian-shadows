/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LEVEL_SYSTEM_HPP
#define ELYSIAN_LEVEL_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include <system/elysian_level_system_base.hpp>
namespace elysian {
    using LevelSystem = LevelSystemBase;
}
#else

#   include <ESToolkit/ESGamma/tk_level_system.h>

namespace elysian {
    using LevelSystem = TkLevelSystem;
}
using LevelSystem = TkLevelSystem;

#endif


#endif // ELYSIAN_LEVEL_SYSTEM_HPP
