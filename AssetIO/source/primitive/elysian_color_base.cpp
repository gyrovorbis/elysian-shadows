#include <crtp/elysian_color.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Color4Base)

    const Color4 Color4Base::white  (255,255,255,255);
    const Color4 Color4Base::black  (0,  0,  0,  255);
    const Color4 Color4Base::blue   (0,  0,  255,255);
    const Color4 Color4Base::green  (0,  255,0,  255);
    const Color4 Color4Base::red    (255,0,  0,  255);
    const Color4 Color4Base::yellow (255,255,0,  255);
    const Color4 Color4Base::orange (255,128,0,  255);
    const Color4 Color4Base::gray   (128,128,128,255);
    const Color4 Color4Base::aqua   (0,  255,255,255);

    const Color4 Color4Base::brown  (150, 80, 60,255);
    const Color4 Color4Base::purple (255,  0,255,255);

}
