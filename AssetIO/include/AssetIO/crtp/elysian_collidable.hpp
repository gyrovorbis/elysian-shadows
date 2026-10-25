/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_collidable_base.hpp>
namespace elysian {
    typedef CollidableBase          Collidable;
    typedef CollidableAttribBase    CollidableAttrib;
}
#else
#   include <ESToolkit/AssetIO/tk_collidable.h>
namespace elysian {
    typedef TkCollidableAttrib  CollidableAttrib;
    typedef TkCollidable        Collidable;
}
typedef TkCollidableAttrib  CollidableAttrib;
typedef TkCollidable        Collidable;

#endif





#endif
