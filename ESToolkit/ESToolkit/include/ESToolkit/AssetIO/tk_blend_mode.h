/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_BLEND_MODE_H
#define TK_BLEND_MODE_H

#include <AssetIO/primitive/elysian_blend_mode_base.hpp>

class TkBlendMode: public elysian::BlendModeBase {

public:

                TkBlendMode(void) = default;
                TkBlendMode(GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor, Color4 constColor=Color4());

    static void initializeDelegates(void);

};


//====== INLINEZ ========

inline TkBlendMode::TkBlendMode(GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor, Color4 constColor):
    elysian::BlendModeBase(srcFactor, dstFactor, constColor) {}


#endif // TK_BLEND_MODE_H
