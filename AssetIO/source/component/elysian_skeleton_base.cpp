#include <crtp/elysian_skeleton.hpp>
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

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(SkeletonBase)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(SkeletonAttribBase)

SkeletonBase::SkeletonBase(const SkeletonBase &other) {
    *this = other;
}

SkeletonAttribBase::~SkeletonAttribBase(void) {
    spAnimationStateData_dispose(_animStateData);
    spSkeletonData_dispose(_skeletonData);
    spAtlas_dispose(_atlas);
}

SkeletonBase& SkeletonBase::operator= (const SkeletonBase& other) {
    _animTimeScale = other._animTimeScale;
    setAttrib(other._attrib);
    return *this;
}

void SkeletonAttribBase::_reloadAtlas(void) {

    if(_atlas) {
        spAtlas_dispose(_atlas);
        _atlas = nullptr;
    }

    DbgEvent event("Skeleton Attrib loading atlas image [%s]", _atlasPath);

    _atlas = spAtlas_createFromFile(_atlasPath, nullptr);
    if(!_atlas) {
        errprintf("Could not be loaded!");
    }
}

void SkeletonAttribBase::setAtlasPath(const char * const file) {
    if(strcmp(_atlasPath, file) == 0) return;
    strcpy(_atlasPath, file);
    _reloadAtlas();
    if(_atlas) _reloadJson();

}

void SkeletonAttribBase::_reloadJson(void) {
    DbgEvent event("Skeleton Attrib loading json file [%s]", _jsonPath);

    if(_skeletonData) {
        spSkeletonData_dispose(_skeletonData);
        _skeletonData = nullptr;
    }
    if(_animStateData) {
        spAnimationStateData_dispose(_animStateData);
        _animStateData = nullptr;
    }

    spSkeletonJson* skelJson = spSkeletonJson_create(_atlas);
    if(!skelJson) {
        errprintf("Could not create SkeletonJson!");

    } else {

        _skeletonData = spSkeletonJson_readSkeletonDataFile(skelJson, _jsonPath);
        if(!_skeletonData) {
            errprintf("Could not initialize SkeletonData!");
        } else {

            _animStateData = spAnimationStateData_create(_skeletonData);
            if(!_animStateData) {
                errprintf("Could not initialize AnimationStateData!");
                if(_skeletonData) {
                    spSkeletonData_dispose(_skeletonData);
                    _skeletonData = nullptr;
                }
            }
        }
        spSkeletonJson_dispose(skelJson);
    }
}

void SkeletonAttribBase::setJsonPath(const char * const json) {
    if(strcmp(_jsonPath, json) == 0) return;
    strcpy(_jsonPath, json);
    if(!_atlas) return;
    _reloadJson();
}



void SkeletonAttribBase::loadAssets(const char * const atlas, const char * const json) {
    setAtlasPath(atlas);
    setJsonPath(json);
}


SkeletonBase::~SkeletonBase(void) {
    if(_animState) spAnimationState_dispose(_animState);
    if(_skeleton) spSkeleton_dispose(_skeleton);
    /*decrement attrib reference count*/
}

void SkeletonBase::setAnimByName(const int trackIndex, const char*const animationName, const bool loop) const {
    spAnimationState_setAnimationByName(_animState, trackIndex, animationName, loop);
}


void SkeletonBase::setAttrib(SkeletonAttrib * const attrib) {
    DbgEvent event("SkeletonBase setting attributes.");
    //float x, y;
    _attrib = attrib;
    if(_skeleton) {
        spSkeleton_dispose(_skeleton);
        _skeleton = nullptr;
    }
    if(_animState) {
        spAnimationState_dispose(_animState);
        _animState = nullptr;
    }
    if(!attrib) return;

    _skeleton = attrib->_skeletonData? spSkeleton_create(attrib->_skeletonData) : nullptr;
    if(!_skeleton) {
        errprintf("Could not create SkeletonData!");
    }

    _animState = attrib->_animStateData? spAnimationState_create(attrib->_animStateData) : nullptr;
    if(!_animState) {
        errprintf("Could not create AnimationState!");
    }

    if(_skeleton) {
        _skeleton->x = _entity->_transform._pos.x;
        _skeleton->y = _entity->_transform._pos.y;
        spSkeleton_updateWorldTransform(_skeleton);
    }
}

void SkeletonBase::update(const float deltaTime) const {
    if(_skeleton && _animState) {
        spSkeleton_update(_skeleton, deltaTime);
        spAnimationState_update(_animState, deltaTime * _animTimeScale);
        spAnimationState_apply(_animState, _skeleton);
    }
}

void SkeletonBase::render(void) const {
    if(_skeleton && _animState && !_attrib->isReested()) {
        _skeleton->x = _entity->_transform._pos.x;
        _skeleton->y = _entity->_transform._pos.y - _entity->_transform._pos.z;
        spSkeleton_updateWorldTransform(_skeleton);
        gyVidSkeleton(_skeleton);
    }
}



}
