#ifndef ELYSIAN_RENDERABLE_OBJECT_HPP
#define ELYSIAN_RENDERABLE_OBJECT_HPP

#include "elysian_instance_transform.hpp"

namespace elysian {

	struct RenderableObject {
		InstanceTransform transform;
		virtual void render(void) = 0;
		InstanceTransform &getTransform(void) { return transform; }
	};
}


#endif