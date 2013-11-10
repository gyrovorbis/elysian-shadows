#ifndef ELYSIAN_RECT_HPP
#define ELYSIAN_RECT_HPP

#include "elysian_renderable_object.hpp"
#include "elysian_rect_attrib.hpp"
#include <gyro.h>

namespace elysian {
#if 0
	class Rect: public RenderableObject, public RectAttrib {
	public:

		void render(void) {
			gyMatPush();
			transform.apply();
			submit();
			gyMatPop();
			
		}

	};

#endif

}

#endif
