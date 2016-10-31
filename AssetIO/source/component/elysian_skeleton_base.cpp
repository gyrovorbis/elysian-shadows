#include <crtp/elysian_skeleton.hpp>
#include "elysian_debug.hpp"
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_area.hpp>

#include <spine/Atlas.h>
#include <spine/SkeletonData.h>
#include <spine/SkeletonJson.h>
#include <spine/Skeleton.h>
#include <spine/AnimationStateData.h>
#include <spine/AnimationState.h>
#include <spine/extension.h>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AnimationTrack)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Animation)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(SkeletonBase)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(SkeletonAttribBase)

void SkeletonBase::setAnim(const char* name) {

    auto* anim = _attrib->findAnimation(name);
    if(!anim || _curAnim == anim) return;

#if 0
    unsigned oldTrackCount = _curAnim? _curAnim->_tracks.size() : 0;
    for(unsigned i = 0; i < anim->_tracks.size(); ++i) {
        if(_curAnim &&  i < _curAnim->_tracks.size() && strcmp(anim->_tracks[i]->_name, _curAnim->_tracks[i]->_name) == 0) continue;
            spAnimationState_clearTrack(_animState, i);
            setAnimByName(i, anim->_tracks[i]->_name, anim->_tracks[i]->_looping);
    }

    for(unsigned i = anim->_tracks.size(); i < oldTrackCount; ++i) {
        spAnimationState_clearTrack(_animState, i);
    }
#else

    //if(_curAnim) //for(unsigned i = 0; i < _curAnim->_tracks.size(); ++i) spAnimationState_clearTrack(_animState, i);
    spAnimationState_clearTracks(_animState);
    //spSkeleton_setToSetupPose(_skeleton);
    for(unsigned i = 0; i < anim->_tracks.size(); ++i) setAnimByName(i, anim->_tracks[i]->_name, anim->_tracks[i]->_looping);
    update(0.001f);


#endif
    _curAnim = anim;
}

SkeletonAttribBase::~SkeletonAttribBase(void) {
    for(auto* it: _animations) delete it;
    if(_animStateData) spAnimationStateData_dispose(_animStateData);
    if(_skeletonData) spSkeletonData_dispose(_skeletonData);
    if(_atlas) spAtlas_dispose(_atlas);
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
    if(_jsonPath[0] == '\0') return;
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
            errprintf("Could not initialize SkeletonData! [%s]", _jsonPath);
        } else {

            _animStateData = spAnimationStateData_create(_skeletonData);
            if(!_animStateData) {
                errprintf("Could not initialize AnimationStateData! [%s]", _skeletonData);
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
    _removeEntityAttachments();
    if(_animState) spAnimationState_dispose(_animState);
    if(_skeleton) spSkeleton_dispose(_skeleton);
    /*decrement attrib reference count*/
}

void SkeletonBase::setAnimByName(const int trackIndex, const char* animationName, const bool loop) {
    if(!spAnimationState_setAnimationByName(_animState, trackIndex, animationName, loop)) {
        warnprintf("SkeletonBase::setAnimByName(%d, %s, %s) - Could not find animation!", trackIndex, animationName, loop? "true" : "false");
    }
    update(0.001f);
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

    setAnim(_attrib->getDefaultAnimationName());

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
        _updateEntityAttachments();
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

void SkeletonBase::attachEntityToSlot(const char *name, Entity *entity) {
    EntityAttachment* attachment = _findEntityAttachment(name);

    if(!attachment) {
        spSlot* slot =  spSkeleton_findSlot(_skeleton, name);

        if(!slot) {
            errprintf("Could not attach %s to slot %s on %s", entity->getName(), slot, _entity->getName());
            return;
        }

        if(!slot->attachment) {
            attachment = new EntityAttachment;
            attachment->slot = slot;
            attachment->parent = _entity;
            _entityAttachments.push_back(attachment);
            spSlot_setAttachment(slot, attachment);
            //slot->attachment = attachment;
        } else {
            errprintf("Attachment %s found on entity %s, but not in skeleton component cache!", name, _entity->getName());
            attachment = static_cast<EntityAttachment*>(slot->attachment);
        }
    }

    entity->setParentEntity(getEntity());
    attachment->entity = entity;
    attachment->update();

}

void SkeletonBase::_updateEntityAttachments(void) const {
    for(auto* it: _entityAttachments) it->update();
}

void SkeletonBase::removeEntityFromSlot(const char *name) {
    EntityAttachment* attachment = _findEntityAttachment(name);
    if(!attachment) {
        errprintf("Attachment %s not found on entity %s", name, _entity->getName());
        return;
    }
    if(attachment->entity) attachment->entity->setParentEntity(nullptr);
    attachment->entity = nullptr;
}

EntityAttachment* SkeletonBase::_findEntityAttachment(const char * const name) const {
    for(auto* it: _entityAttachments) {
        if(strcmp(it->slot->data->name, name) == 0) return it;
    }
    return nullptr;
}

void SkeletonBase::_removeEntityAttachments(void) {
    for(auto* it : _entityAttachments) {
        //it->slot->attachment = nullptr;
        spSlot_setAttachment(it->slot, nullptr);
        _spAttachment_deinit(it);
        spAttachment_dispose(it);
        //delete it;
    }
    _entityAttachments.clear();
}

void SkeletonBase::setAnim(const char *name, const AnimListener &listener) {
    _listener = listener;
    setAnim(name);
    _animState->listener = &_animationStateListener;
    _animState->listenerData = this;
    update(0.001f);
}


void SkeletonBase::_animationStateListener(spAnimationState *state, int trackIndex, spEventType type, spEvent *event, int loopCount) {
    SkeletonBase* skeleton = static_cast<SkeletonBase*>(state->listenerData);
    if(skeleton->_listener(trackIndex, type, event, loopCount)) {
        skeleton->_animState->listener      = nullptr;
        skeleton->_animState->listenerData  = nullptr;
    }
}

EntityAttachment::EntityAttachment(void) {
    _spAttachment_init(this, "EntityAttachment", SP_ATTACHMENT_ENTITY, &dispose);
}

void EntityAttachment::dispose(spAttachment *attachment) {
    EntityAttachment* self = static_cast<EntityAttachment*>(attachment);
    errprintf("Spine just called vtable[dispose] on entity attachment [%s]!", self->slot->data->name);
    delete self;
}

void EntityAttachment::update(void) {
    if(!entity) return;

    auto& transform = entity->getTransform();

#if 1

    transform.setPos(GYVector3{
        parent->getTransform().getPosX() + slot->bone->worldX,
        parent->getTransform().getPosY() + slot->bone->worldY,
        parent->getTransform().getPosZ()
    });

    if(slot->bone->data->inheritRotation) {
        transform.setOrient(0.0f, 0.0f, 1.0f);
        transform.setAngle(slot->bone->worldRotation*(M_PI/180.0f));
    }

#else

    transform.setPos(GYVector3{
        parent->getTransform().getPosX() + slot->bone->x,
        parent->getTransform().getPosY() + slot->bone->y,
        parent->getTransform().getPosZ()
    });

    if(slot->bone->data->inheritRotation) {
        transform.setOrient(0.0f, 0.0f, 1.0f);
        transform.setAngle(slot->bone->rotation*(M_PI/180.0f));
    }


#endif

    if(slot->bone->data->inheritScale) {
        transform.setScale(GYVector2{
            slot->bone->worldScaleX,
            slot->bone->worldScaleY
        });
    }

    transform.setFlipX(slot->bone->flipX);
    transform.setFlipY(slot->bone->flipY);

}

void SkeletonBase::setSkin(const char *name) {
    if(!spSkeleton_setSkinByName(_skeleton, name)) {
        errprintf("Skeleton::setSkin(%s) - Skin not found on entity: %s.", name, getEntity()->getName());
    }
}

const char* SkeletonBase::getSkin(void) const {
    return (_skeleton && _skeleton->skin)? _skeleton->skin->name : nullptr;

}


}
