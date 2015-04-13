#ifndef ELYSIAN_ANIMATION_HPP
#define ELYSIAN_ANIMATION_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_animation_base.hpp>
namespace elysian {
    typedef DebugBase Debug;
}
#else
#   include <tkAnimation.h>
namespace elysian {
    typedef TkAnimation Animation;
}
typedef TkAnimation Animation;

#endif


#endif // ELYSIAN_ANIMATION_HPP
