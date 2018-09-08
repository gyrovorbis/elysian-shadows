#ifndef ELYSIAN_RENDERABLE_OBJECT_HPP
#define ELYSIAN_RENDERABLE_OBJECT_HPP

#include <AssetIO/primitive/elysian_instance_transform.hpp>

namespace elysian {

    struct RenderableObject {
        InstanceTransform _transform;
        virtual void render(void) = 0;
        InstanceTransform &getTransform(void) { return _transform; }
    };
}


#endif
