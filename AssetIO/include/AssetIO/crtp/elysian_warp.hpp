/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_WARP_HPP
#define ELYSIAN_WARP_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_warp_base.hpp>
namespace elysian {
    typedef WarpBase Warp;
}
#else
#   include <ESToolkit/AssetIO/tk_warp.h>
namespace elysian {
    typedef TkWarp Warp;
}
typedef TkWarp Warp;

#endif

#endif // ELYSIAN_WARP_HPP

