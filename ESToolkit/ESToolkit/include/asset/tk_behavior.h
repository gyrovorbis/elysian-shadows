#ifndef TKBEHAVIOR_H
#define TKBEHAVIOR_H

#include <component/elysian_behavior_base.hpp>

class TkBehavior: public elysian::BehaviorBase {
public:
    TkBehavior(Entity& entity);

};






// ====== INLINEZ ======

inline TkBehavior::TkBehavior(Entity& entity): elysian::BehaviorBase(entity) {}

#endif // TKBEHAVIOR_H

