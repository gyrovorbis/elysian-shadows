#include "elysian_skeleton_system.hpp"
#include "elysian_timer_system.hpp"
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_skeleton.hpp>
#include <spine/Bone.h>

namespace elysian {

SkeletonSystem::SkeletonSystem() {
   spBone_setYDown(true);
}


void SkeletonSystem::update(void) {

    const float deltaTime = getTimerSys()->getDeltaTime();

    for(auto& it : _curArea->skeletons) {
        it->update(deltaTime);
    }

}




}
