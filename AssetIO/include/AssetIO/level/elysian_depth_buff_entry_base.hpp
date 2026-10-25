/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_DEPTH_BUFF_ENTRY_BASE_HPP
#define ELYSIAN_DEPTH_BUFF_ENTRY_BASE_HPP

#include <AssetIO/level/elysian_map_depth_buffer.hpp>

namespace elysian {

    struct MapDepthBuffEntryImplBase: public MapDepthBuffEntry {


    };

    struct EntityDepthBuffEntryImplBase: public EntityDepthBuffEntry {
        Entity* entity;
    };


}

#endif // ELYSIAN_DEPTH_BUFF_ENTRY_BASE_HPP

