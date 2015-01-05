#include "tkAnimation.h"
#include "elysian_animation.hpp"
#include "elysian_entity.hpp"

TkAnimation::TkAnimation(TkEntity& parent): elysian::AnimationBase(parent) {
    connect(this, SIGNAL(entityParented(Entity*const,Entity*const)), &parent, SLOT(onEntityParented(Entity*const,Entity*const)));
}

void TkAnimation::removeChildEntity(const char *animName) {
    for(auto it = _children->begin(); it < _children->end(); ++it) {
        if((*it).getStackBottomState<elysian::ChildAnimation>() && strcmp((*it).getStackBottomState<elysian::ChildAnimation>()->getName(), animName) == 0) {
            _children->remove(it);
            (*it).entity->setParentEntity(nullptr);
            emit entityParented((*it).entity, nullptr);
            return;
        }
    }
}

void TkAnimation::addChildEntity(Entity*const entity, const char*animName) {
    elysian::AnimationBase::addChildEntity(entity, animName);

    if(static_cast<QTreeWidgetItem*>(entity)->parent()) static_cast<QTreeWidgetItem*>(entity)->parent()->removeChild(entity);
    getEntity()->addChild(entity);
}

void TkAnimation::removeChildEntity(Entity *entity) {
    elysian::AnimationBase::removeChildEntity(entity);
    static_cast<QTreeWidgetItem*>(getEntity())->removeChild(entity);
}


