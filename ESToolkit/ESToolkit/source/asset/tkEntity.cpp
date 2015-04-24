#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "elysian_animation.hpp"
#include "luaTableStackModel.h"
#include "elysian_lua_wrappers.hpp"
#include "elysian_renderable.hpp"
#include "elysian_engine.hpp"
#include "elysian_light_system.hpp"
#include <cmath>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QSpacerItem>
#include <elysian_generic.hpp>
#include <QGridLayout>

elysian::CollidableAttrib* TkEntity::_selColliderAttrib;
//elysian::Material* TkEntity::_selMaterial;
bool TkEntity::_staticInit = false;

TkEntity::TkEntity(void): TkEntity(true) {}

TkEntity::TkEntity(const OOLUA::Lua_table_ref valTable): TkEntity(false) {
    fromTable(valTable);
}

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
        _selectionRenderable = new Renderable(*this);
        _selSpriteAttrib = new elysian::SpriteAttrib;
        _selectionRenderable->setSpriteAttrib(_selSpriteAttrib);
    }

    setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setSaveFlag(getSaveFlag());

}

void TkEntity::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
    if(column == 0 && role == Qt::CheckStateRole) {
        _saveFlag = value.toBool();
    }
}

TkEntity::~TkEntity(void) {
    delete _luaModel;
}

bool TkEntity::getSaveFlag(void) const {
    return _saveFlag;
}

void TkEntity::setSaveFlag(const bool val) {
    setCheckState(0, val? Qt::Checked : Qt::Unchecked);
}

LuaTableStackModel* TkEntity::getModel(void) const {
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

    if(!_active && elysian::EntityBase::activate() && _selectable) {
        _area->collidables.push_back(_selectionCollider);
        _area->renderables.push_back(_renderable);
        return true;
    } else if(_active) {
        return true;
    }

    return false;
}

bool TkEntity::deactivate(void) {
    if(_active && elysian::EntityBase::deactivate() && _selectable) {
        elysian::containerEraseOnMatch(_selectionCollider, _area->collidables);
        elysian::containerEraseOnMatch(_selectionRenderable, _area->renderables);
        return true;
    } else if(!_active) {
        return true;
    }

    return false;
}

void TkEntity::_renderRelative(void) {
    if(_selectionState != SELECTION_NONE && !gyVidShadowPass()) {
        _selectionRenderable->beginRender();
        //if(_depthType == DEPTH_BUFFERED) transform._size.y = transform._size.z;
        _selectionRenderable->render();
        _selectionRenderable->endRender();
    }
    elysian::EntityBase::_renderRelative();
}

void TkEntity::onEntityParented(Entity * const child, Entity * const parent) {
    Q_EMIT entityParented(child, parent);
}

bool TkEntity::validate(QString& detailedText) const {
    QMessageBox msgBox;
    bool        success = true;
    bool        recursive = !detailedText.isNull();

    msgBox.setText("Could not activate entity!");
    msgBox.setInformativeText(QString("Entity ") + QString("'")+QString(getName())+QString("' ") + QString("is either invalid or incomplete and couldn't be added to the current area. See details for more information."));
    msgBox.setStandardButtons(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);


    detailedText += recursive? QString("Validating child entity") +  QString(getName()) + QString(".\n") :
                QString("Validating entity") + QString(getName()) + QString(".\n");

    if(getRenderable())         success &= getRenderable()->validate(detailedText);
    if(getCollidable())         success &= getCollidable()->validate(detailedText);
    if(getAnimation())          success &= getAnimation()->validate(detailedText);
    if(getRigidBody())          success &= getRigidBody()->validate(detailedText);
    if(getWarp())               success &= getWarp()->validate(detailedText);
    if(getCharacter())          success &= getCharacter()->validate(detailedText);
    if(getLightSource())        success &= getLightSource()->validate(detailedText);
    if(getAudioSource())        success &= getAudioSource()->validate(detailedText);
    if(getParticleEmitter())    success &= getParticleEmitter()->validate(detailedText);
    if(getCombat())             success &= getCombat()->validate(detailedText);


    if(getAnimation() && getAnimation()->_children && getAnimation()->_children->size()) {
        detailedText += "\n\n";
        for(auto it = getAnimation()->_children->begin();
            it != getAnimation()->_children->end();
            ++it)
        {
            success &= (*it).entity->validate(detailedText);

        }
    }

    if(!success && !recursive) {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setDetailedText(detailedText);
        QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)msgBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
        msgBox.exec();
    }

    return success;

}

QMenu* TkEntity::createContextMenu(void) {
    QStack<QMenu*> menuStack;

    auto insertAction = [&](const QString text, const std::function<void(bool)>& callback, const bool enabled=true) {
        auto* action = new QAction(text, nullptr);
        //motherfucking C++11 lambda function Qt SLAWTS!!!
        QObject::connect(action, &QAction::triggered, callback);
        action->setEnabled(enabled);
        menuStack.top()->addAction(action);
        return action;
    };

    auto insertCheckableAction = [&](const QString text, const std::function<void(bool)>& callback, const bool checked=false, const bool enabled=true) {
        QAction* action = insertAction(text, callback, enabled);
        action->setCheckable(true);
        action->blockSignals(true);
        action->setChecked(checked);
        action->blockSignals(false);
    };

    auto canAddTo = [&](const elysian::Component&& component) {
        return component.canAddTo(*this);
    };

    menuStack.push(new QMenu("Entity"));

    insertCheckableAction("Active",
        [&](bool checked) {
            if(checked) {
                QString errorMsg;
                if(validate(errorMsg)) {
                    getAssets()->getCurrentArea()->removeInactiveEntity(this);
                    getAssets()->getCurrentArea()->addEntity(this);
                }
            }
            else {
                getAssets()->getCurrentArea()->removeEntity(this);
                getAssets()->getCurrentArea()->addInactiveEntity(this);
            }
        },
        isActive()
    );

    insertAction("Delete",
         [&](bool) {
            if(isActive()) getAssets()->getCurrentArea()->removeEntity(this);
            delete this;
    });

    menuStack.push(menuStack.top()->addMenu("Components"));

    insertCheckableAction("Renderable",
        [&](bool checked) {
            if(checked) addRenderable();
            else removeRenderable();
        },
        getRenderable(),
        canAddTo(elysian::Renderable())
    );

    insertCheckableAction("Collidable",
        [&](bool checked) {
            if(checked) addCollidable();
            else removeCollidable();
        },
        getCollidable(),
        canAddTo(elysian::Collidable())
    );

    insertCheckableAction("Animation",
        [&](bool checked) {
            if(checked) addAnimation();
            else removeAnimation();
        },
        getAnimation(),
        canAddTo(elysian::Animation())
    );

    insertCheckableAction("Rigid Body",
        [&](bool checked) {
            if(checked) addRigidBody();
            else removeRigidBody();
        },
        getRigidBody(),
        canAddTo(elysian::RigidBody())
    );

    insertCheckableAction("Warp",
        [&](bool checked) {
            if(checked) addWarp();
            else removeWarp();
        },
        getWarp(),
        canAddTo(elysian::Warp())
    );

    insertCheckableAction("Character",
        [&](bool checked) {
            if(checked) addCharacter();
            else removeCharacter();
        },
        getCharacter(),
        canAddTo(elysian::Character())
    );

    insertCheckableAction("Light Source",
        [&](bool checked) {
            if(checked) addLightSource();
            else removeLightSource();
        },
        getLightSource(),
        canAddTo(elysian::LightSource())
    );

    insertCheckableAction("Audio Source",
        [&](bool checked) {
            if(checked) addAudioSource();
            else removeAudioSource();
        },
        getAudioSource(),
        canAddTo(elysian::AudioSource())
    );

    insertCheckableAction("Particle Emitter",
        [&](bool checked=false) {
            if(checked) addParticleEmitter();
            else removeParticleEmitter();
        },
        getParticleEmitter(),
        canAddTo(elysian::ParticleEmitter())
    );

    insertCheckableAction("Combat",
        [&](bool checked=false) {
            if(checked) addCombat();
            else removeCombat();
        },
        getCombat(),
        canAddTo(elysian::Combat())
    );


    return menuStack.first();


}
