#ifndef ELYSIAN_QUAD_HPP
#define ELYSIAN_QUAD_HPP

#include "elysian_renderable_object.hpp"
#include "elysian_quad_attrib.hpp"
#include "gyro.h"

namespace elysian {
#if 0
	class Quad: public RenderableObject, public QuadAttrib {
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
