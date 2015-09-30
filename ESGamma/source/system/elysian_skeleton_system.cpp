#include <system/elysian_skeleton_system.hpp>
#include <crtp/elysian_timer_system.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_skeleton.hpp>
#include <crtp/elysian_entity.hpp>
#include <spine/Bone.h>

namespace elysian {

SkeletonSystem::SkeletonSystem(void) {
    spBone_setYDown(true);
    gyVidSkeletonAttachmentCallback(SP_ATTACHMENT_ENTITY, &_renderEntityAttachment);
}

void SkeletonSystem::_renderEntityAttachment(spAttachment *attachment) {
    EntityAttachment* casted = static_cast<EntityAttachment*>(attachment);
    casted->entity->render();
}


void SkeletonSystem::update(void) {

    const float deltaTime = getTimerSys()->getDeltaTime();

    for(auto& it : _curArea->skeletons) {
        it->update(deltaTime);
    }

}




}
