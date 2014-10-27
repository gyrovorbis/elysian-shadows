#ifndef ELYSIAN_QUAD_SPRITE_HPP
#define ELYSIAN_QUAD_SPRITE_HPP

#include "elysian_renderable_object.hpp"
#include "elysian_quad_sprite_attrib.hpp"
#include <gyro.h>

namespace elysian {

	class QuadSprite: public RenderableObject, public QuadSpriteAttrib {
	public:

		void render(void) {
			gyMatPush();
            _transform.apply();
			submit();
			gyMatPop();
			
		}

	};

}

#endif
