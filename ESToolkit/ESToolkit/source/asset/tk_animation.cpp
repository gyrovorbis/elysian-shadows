#include "tk_animation.h"
#include "elysian_entity.hpp"



TkAnimation::TkAnimation(TkEntity& parent): elysian::AnimationBase(parent) {
    connect(this, SIGNAL(entityParented(Entity*const,Entity*const)), &parent, SLOT(onEntityParented(Entity*const,Entity*const)));
}

void TkAnimation::removeChildEntity(const char *animName) {
    for(auto it = _children->begin(); it < _children->end(); ++it) {
        if((*it).getStackBottomState<elysian::ChildAnimation>() && strcmp((*it).getStackBottomState<elysian::ChildAnimation>()->getName(), animName) == 0) {
            _children->remove(it);
            (*it).entity->setParentEntity(nullptr);
            Q_EMIT entityParented((*it).entity, nullptr);
            return;
        }
    }
}

void TkAnimation::addChildEntity(Entity*const entity, const char*animName) {
    elysian::AnimationBase::addChildEntity(entity, animName);
    QTreeWidgetItem*    item = static_cast<QTreeWidgetItem*>(entity);

    if(item->parent()) {
        item->parent()->removeChild(entity);
    }
    QTreeWidget*        tree = item->treeWidget();

    if(tree) tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));

    getEntity()->addChild(entity);
}

void TkAnimation::removeChildEntity(Entity *entity) {
    elysian::AnimationBase::removeChildEntity(entity);
    static_cast<QTreeWidgetItem*>(getEntity())->removeChild(entity);
}

bool TkAnimation::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "DBG_ERROR: Animation component has no attributes.");
    //if(_attrib && _attrib->_sequences.size()) str += "DBG_WARNING: Animation attributes have no sequences.\n";

    return success;
}

