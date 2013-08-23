#ifndef ELYSIAN_MODEL_HPP
#define ELYSIAN_MODEL_HPP

#include "elysian_renderable_object.hpp"
#include "elysian_model_attrib.hpp"
#include "gyro.h"

namespace elysian {

	class Model: public RenderableObject, public ModelAttrib {
	public:

		void render(void) {
			gyMatPush();
			transform.Apply();
			submit();
			gyMatPop();
			
		}

	};

}

#endif