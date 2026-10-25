/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_RENDERABLE_HPP
#define ELYSIAN_RENDERABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_renderable_base.hpp>
namespace elysian {
	typedef RenderableBase Renderable;
}
#else
#   include <ESToolkit/AssetIO/tk_renderable.h>
namespace elysian {
	typedef TkRenderable Renderable;
}
typedef TkRenderable Renderable;

#endif



#endif // ELYSIAN_RENDERABLE_HPP

