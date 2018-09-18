#ifndef TK_BLEND_MODE_H
#define TK_BLEND_MODE_H

#include <AssetIO/primitive/elysian_blend_mode_base.hpp>

class TkBlendMode: public elysian::BlendModeBase {

public:

                TkBlendMode(void) = default;
                TkBlendMode(GY_BLEND_MODE srcFactor, GY_BLEND_MODE dstFactor, Color4 constColor=Color4());

    static void initializeDelegates(void);

};


//====== INLINEZ ========

inline TkBlendMode::TkBlendMode(GY_BLEND_MODE srcFactor, GY_BLEND_MODE dstFactor, Color4 constColor):
    elysian::BlendModeBase(srcFactor, dstFactor, constColor) {}


#endif // TK_BLEND_MODE_H
