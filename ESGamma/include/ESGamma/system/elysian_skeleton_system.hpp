/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_SKELETON_SYSTEM_HPP
#define ELYSIAN_SKELETON_SYSTEM_HPP

#include <ESGamma/system/elysian_system.hpp>

struct spAttachment;

namespace elysian {

    class SkeletonSystem: public System {
    protected:

        static void    _renderEntityAttachment(spAttachment* attachment);
    public:
                        SkeletonSystem(void);
        void            update(void);


    };

}

#endif // ELYSIAN_SKELETON_SYSTEM_HPP
