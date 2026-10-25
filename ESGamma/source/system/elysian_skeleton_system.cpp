/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/system/elysian_skeleton_system.hpp>
#include <ESGamma/crtp/elysian_timer_system.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <AssetIO/crtp/elysian_skeleton.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <spine/Bone.h>

namespace elysian {

SkeletonSystem::SkeletonSystem(void) {
    spBone_setYDown(true);
    gyVidSkeletonAttachmentCallback(SP_ATTACHMENT_ENTITY, &_renderEntityAttachment);
}

void SkeletonSystem::_renderEntityAttachment(spAttachment *attachment) {
    EntityAttachment* casted = static_cast<EntityAttachment*>(attachment);
    if(casted->entity) casted->entity->render();
}


void SkeletonSystem::update(void) {

    const float deltaTime = getTimerSys()->getDeltaTime();

    for(auto& it : _curArea->skeletons) {
        it->update(deltaTime);
    }

}




}
