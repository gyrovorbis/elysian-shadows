#ifndef ELYSIAN_RENDERABLE_HPP
#define ELYSIAN_RENDERABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_renderable_base.hpp>
namespace elysian {
	typedef RenderableBase Renderable;
}
#else
#   include <tk_renderable.h>
namespace elysian {
	typedef TkRenderable Renderable;
}
typedef TkRenderable Renderable;

#endif



#endif // ELYSIAN_RENDERABLE_HPP

