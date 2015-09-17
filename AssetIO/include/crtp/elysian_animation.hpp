#ifndef ELYSIAN_ANIMATION_HPP
#define ELYSIAN_ANIMATION_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_animation_base.hpp>
namespace elysian {
    typedef AnimationBase Animation;
}
#else
#   include <tk_animation.h>
namespace elysian {
    typedef TkAnimation Animation;
}
typedef TkAnimation Animation;

#endif


#endif // ELYSIAN_ANIMATION_HPP
