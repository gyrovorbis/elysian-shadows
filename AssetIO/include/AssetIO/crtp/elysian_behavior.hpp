#ifndef ELYSIAN_BEHAVIOR_HPP
#define ELYSIAN_BEHAVIOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_behavior_base.hpp>
namespace elysian {
    typedef BehaviorBase Behavior;
}
#else
#   include <ESToolkit/AssetIO/tk_behavior.h>
namespace elysian {
    typedef TkBehavior Behavior;
}
typedef TkBehavior Behavior;

#endif

#endif // ELYSIAN_BEHAVIOR_HPP

