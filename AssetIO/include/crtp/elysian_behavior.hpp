#ifndef ELYSIAN_BEHAVIOR_HPP
#define ELYSIAN_BEHAVIOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_behavior_base.hpp>
namespace elysian {
    typedef BehaviorBase Behavior;
}
#else
#   include <tkBehavior.h>
namespace elysian {
    typedef TkBehavior Behavior;
}
typedef TkBehavior Behavior;

#endif

#endif // ELYSIAN_BEHAVIOR_HPP

