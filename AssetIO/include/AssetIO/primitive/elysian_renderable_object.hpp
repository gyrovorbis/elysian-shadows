/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

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
