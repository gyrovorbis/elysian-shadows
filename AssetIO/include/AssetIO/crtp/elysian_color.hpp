/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_COLOR_HPP
#define ELYSIAN_COLOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <primitive/elysian_color_base.hpp>
namespace elysian {
    typedef Color4Base Color4;
}
#else
#   include <ESToolkit/AssetIO/tk_color.h>
namespace elysian {
    typedef TkColor4 Color4;
}
typedef TkColor4 Color4;

#endif


#endif // ELYSIAN_COLOR_HPP
