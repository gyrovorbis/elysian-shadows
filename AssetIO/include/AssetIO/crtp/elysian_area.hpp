/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_AREA_HPP
#define ELYSIAN_AREA_HPP

#ifdef ELYSIAN_ENGINE
#   include <level/elysian_area_base.hpp>
namespace elysian {
    typedef AreaBase Area;
}
#else
#   include <ESToolkit/AssetIO/tk_area.h>
namespace elysian {
    typedef TkArea Area;
}
typedef TkArea Area;

#endif


#endif // ELYSIAN_AREA_HPP
