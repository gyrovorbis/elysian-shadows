/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_DEBUG_HPP
#define ELYSIAN_DEBUG_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_debug_base.hpp>
namespace elysian {
    typedef DebugBase Debug;
}
#else
#   include <ESToolkit/core/tk_debug.h>
namespace elysian {
    typedef TkDebug Debug;
}
typedef TkDebug Debug;

#endif



#endif // ELYSIAN_DEBUG_H
