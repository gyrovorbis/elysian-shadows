#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "elysian_animation.hpp"
#include "luaTableStackModel.h"
#include "elysian_lua_wrappers.hpp"
#include "elysian_renderable.hpp"
#include "elysian_engine.hpp"
#include "elysian_light_system.hpp"
#include <cmath>

elysian::CollidableAttrib* TkEntity::_selColliderAttrib;
//elysian::Material* TkEntity::_selMaterial;
bool TkEntity::_staticInit = false;

TkEntity::TkEntity(void): TkEntity(true) {}

TkEntity::TkEntity(const bool selectable):
    _selectable(selectable)
{
    setName(elysian::EntityBase::_name);

    if(selectable) {
        if(!_staticInit) {

            _selColliderAttrib = new elysian::CollidableAttrib;
            //_selColliderAttrib->setRelPos(0.0f, 0.0f, 0.0f);
            //_selColliderAttrib->setRelSize(1.0f, 1.0f, 0.0f);
            _selColliderAttrib->getFlags()->resetType();
            _selColliderAttrib->getFlags()->resetCheckAgainst();
            _selColliderAttrib->getFlags()->addType(elysian::CollisionFlags::CURSOR);
            _selColliderAttrib->getFlags()->addCheckAgainst(elysian::CollisionFlags::CURSOR);
            _staticInit = true;
        }

        _selectionCollider = new elysian::Collidable(*this);
        _selectionCollider->setAttrib(_selColliderAttrib);
        _selectionCollider->setWorldSpace(false);
        _selectionCollider->setResolvePenetration(false);
        _selectionRenderable = new elysian::Renderable(*this);
        _selSpriteAttrib = new elysian::SpriteAttrib;
        _selectionRenderable->setSpriteAttrib(_selSpriteAttrib);
    }
}

TkEntity::~TkEntity(void) {
    delete _luaModel;
}

QAbstractItemModel* TkEntity::getModel(void) const {
    if(!_luaModel) {
        _luaModel = new LuaTableStackModel(getLuaTableRef());
        _luaModel->populateModel();
    }
    return _luaModel;
}

void TkEntity::setSelectionState(const SELECTION_STATE state) {
    if(!_selectable) return;
    _selectionState = state;

    switch(state) {
    case SELECTION_SELECTED:
        _selSpriteAttrib->setColor(255, 255, 0, 100);
        break;
    case SELECTION_SELECTING:
        _selSpriteAttrib->setColor(0, 255, 255, 100);
        break;
    default: break;
    }
}

bool TkEntity::activate(void) {
    if(!_active && _selectable) {
        _selectionCollider->addToLevel();
        _selectionRenderable->addToLevel();
    }

    return elysian::EntityBase::activate();
}

bool TkEntity::deactivate(void) {
    if(_active && _selectable) {
        _selectionCollider->removeFromLevel();
        _selectionRenderable->removeFromLevel();
    }
    return elysian::EntityBase::deactivate();
}

void TkEntity::_renderRelative(void) {
   // if(_selectionState != SELECTION_NONE && !getAssets()->getEngine()->getLightSys()->isShadowPass()) {
       // _selectionRenderable->beginRender();
        //if(_depthType == DEPTH_BUFFERED) transform._size.y = transform._size.z;
       // _selectionRenderable->render();
       // _selectionRenderable->endRender();
    //}
    elysian::EntityBase::_renderRelative();
}

void TkEntity::onEntityParented(Entity * const child, Entity * const parent) {
    emit entityParented(child, parent);
}
