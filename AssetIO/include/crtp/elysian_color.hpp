#ifndef ELYSIAN_COLOR_HPP
#define ELYSIAN_COLOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <primitive/elysian_color_base.hpp>
namespace elysian {
    typedef Color4Base Color4;
}
#else
#   include <asset/tk_color.h>
namespace elysian {
    typedef TkColor4 Color4;
}
typedef TkColor4 Color4;

#endif


#endif // ELYSIAN_COLOR_HPP
