#include "elysian_skeleton_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_area.hpp"
#include "elysian_skeleton.hpp"

namespace elysian {


void SkeletonSystem::update(void) {

    const float deltaTime = getTimerSys()->getDeltaTime();

    for(auto& it : _curArea->skeletons) {
        it->update(deltaTime);
    }

}




}
