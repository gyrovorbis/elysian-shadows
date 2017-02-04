#include <primitive/elysian_color.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Color4)

    const Color4 Color4::white  (255,255,255,255);
    const Color4 Color4::black  (0,  0,  0,  255);
    const Color4 Color4::blue   (0,  0,  255,255);
    const Color4 Color4::green  (0,  255,0,  255);
    const Color4 Color4::red    (255,0,  0,  255);
    const Color4 Color4::yellow (255,255,0,  255);
    const Color4 Color4::orange (255,128,0,  255);
    const Color4 Color4::gray   (128,128,128,255);
    const Color4 Color4::aqua   (0,  255,255,255);
    const Color4 Color4::brown  (150, 80, 60,255);
    const Color4 Color4::purple (255,  0,255,255);

}
