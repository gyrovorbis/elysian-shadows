#include "elysian_skeleton_base.hpp"
#include "elysian_debug.hpp"
#include "elysian_entity.hpp"
#include "elysian_generic.hpp"
#include <elysian_area.hpp>

#include "spine/Atlas.h"
#include "spine/SkeletonData.h"
#include "spine/SkeletonJson.h"
#include "spine/Skeleton.h"
#include "spine/AnimationStateData.h"
#include "spine/AnimationState.h"

namespace elysian {

SkeletonAttrib::~SkeletonAttrib(void) {
    spAnimationStateData_dispose(_animStateData);
    spSkeletonData_dispose(_skeletonData);
    spAtlas_dispose(_atlas);
}

bool SkeletonAttrib::loadAssets(const char * const atlas, const char * const json) {
    bool success = true;

    Debug::printf(Debug::VERBOSE, "SkeletonAttrib::loadAssets(%s, %s)", atlas, json);
    Debug::push();

    _atlas = spAtlas_createFromFile(atlas, nullptr);
    if(!_atlas) {
        Debug::printf(Debug::CRITICAL, "Could not load texture atlas!");
        success = false;
    }

    spSkeletonJson* skelJson = spSkeletonJson_create(_atlas);
    if(!skelJson) {
        Debug::printf(Debug::CRITICAL, "Could not create SkeletonJson!");
        success = false;
    }

    _skeletonData = spSkeletonJson_readSkeletonDataFile(skelJson, json);
    if(!_skeletonData) {
        Debug::printf(Debug::CRITICAL, "Could not initialize SkeletonData!");
        success = false;
    }

    _animStateData = spAnimationStateData_create(_skeletonData);
    if(!_animStateData) {
        Debug::printf(Debug::CRITICAL, "Could not initialize AnimationStateData!");
        success = false;
    }

    spSkeletonJson_dispose(skelJson);

    Debug::pop();
    return success;
}


SkeletonBase::~SkeletonBase(void) {
    spAnimationState_dispose(_animState);
    spSkeleton_dispose(_skeleton);
    /*decrement attrib reference count*/
}


void SkeletonBase::setAttrib(SkeletonAttrib * const attrib) {
    //float x, y;
    _attrib = attrib;
    _skeleton = spSkeleton_create(attrib->_skeletonData);
    _animState = spAnimationState_create(attrib->_animStateData);
    assert(_skeleton);
    assert(_animState);

    _skeleton->x = _entity->_transform._pos.x;
    _skeleton->y = _entity->_transform._pos.y;

    spSkeleton_updateWorldTransform(_skeleton);
}

void SkeletonBase::update(const float deltaTime) const {
    _skeleton->x = _entity->_transform._pos.x;
    _skeleton->y = _entity->_transform._pos.y;
    spSkeleton_update(_skeleton, deltaTime);
    spAnimationState_update(_animState, deltaTime * _animTimeScale);
    spAnimationState_apply(_animState, _skeleton);
    spSkeleton_updateWorldTransform(_skeleton);
}

void SkeletonBase::render(void) const {
    gyVidSkeleton(_skeleton);
}



}
