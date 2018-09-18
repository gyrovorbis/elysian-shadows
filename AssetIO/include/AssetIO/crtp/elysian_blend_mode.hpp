#ifndef ELYSIAN_BLEND_MODE_HPP
#define ELYSIAN_BLEND_MODE_HPP

#ifdef ELYSIAN_ENGINE
#   include <AssetIO/primitive/elysian_blend_mode_base.hpp>
namespace elysian {
    using BlendMode  = BlendModeBase;
}
#else
#   include <ESToolkit/AssetIO/tk_blend_mode.h>
namespace elysian {
    using BlendMode     = TkBlendMode;
}
using BlendMode         = TkBlendMode;

#endif

#endif // ELYSIAN_BLEND_MODE_HPP
