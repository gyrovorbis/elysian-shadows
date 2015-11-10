#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>

#include "elysian_debug.hpp"
#include "tk_lua_table_stack_model.h"
#include "elysian_engine.hpp"
#include "elysian_light_system.hpp"
#include <cmath>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QSpacerItem>
#include <utility/elysian_generic.hpp>
#include <QGridLayout>
#include <model/tk_property_item_delegate.h>
#include <tk_menu_stack.h>
#include <tk_lua_wrappers.h>

#define TK_ENTITY_CONTEXT_MENU_BEHAVIOR_NAME_SIZE   100

elysian::CollidableAttrib* TkEntity::_selColliderAttrib;
//elysian::Material* TkEntity::_selMaterial;
bool TkEntity::_staticInit = false;

TkEntity* TkEntity::create(const OOLUA::Lua_table_ref valTable) {
    TkEntity* entity = new TkEntity;
    entity->fromTable(valTable);
    return entity;
}

TkEntity::TkEntity(void): TkEntity(true) {}

TkEntity::TkEntity(const OOLUA::Lua_table_ref /*valTable*/): TkEntity(true) {
    //fromTable(valTable);
}


TkEntity::TkEntity(TkEntity* prefab): TkEntity(true, prefab) {}


TkEntity::TkEntity(const bool selectable, TkEntity* prefab):
    elysian::EntityBase(prefab),
    _selectable(selectable)
{
    setName(elysian::EntityBase::_name);

    if(selectable) {

        if(!_staticInit) {

            _selColliderAttrib = new elysian::CollidableAttrib;
            //_selColliderAttrib->setRelPos(0.0f, 0.0f, 0.0f);
            //_selColliderAttrib->setRelSize(1.0f, 1.0f, 0.0f);
            _selColliderAttrib->getFlags()->setType(elysian::BitMask32(0u));
            _selColliderAttrib->getFlags()->setTypeAgainst(0);
            _selColliderAttrib->getFlags()->getType().setValue(elysian::CollisionFlags::CURSOR);
            _selColliderAttrib->getFlags()->getTypeAgainst().setValue(elysian::CollisionFlags::CURSOR);
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
    setTreeWidgetItemState(TreeItemState::Inactive);
    setSaveFlag(_saveFlag);

}

void TkEntity::setData(int column, int role, const QVariant &value) {
    QTreeWidgetItem::setData(column, role, value);
   if(column == 0 && role == Qt::CheckStateRole) {
        if(value == Qt::Checked) _saveFlag = true;
        else if(value == Qt::Unchecked) _saveFlag = false;
    }
}

QVariant TkEntity::data(int column, int role) const {

    if(column == 0 && role == Qt::CheckStateRole) {
        return _saveFlag? Qt::Checked : Qt::Unchecked;
    }
    return QTreeWidgetItem::data(column, role);
}

TkEntity::~TkEntity(void) {
    delete _luaModel;
}

bool TkEntity::getSaveFlag(void) const {
    return _saveFlag;
}

void TkEntity::setSaveFlag(const bool val) {
    _saveFlag = val;
    //treeWidget()->dataChanged
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
    case SELECTION_NONE:
        _selSpriteAttrib->setColor(50, 50, 50, 100);
    default: break;
    }
}

bool TkEntity::activate(void) {

    if(!_active && elysian::EntityBase::activate() && _selectable) {
        _area->collidables.push_back(_selectionCollider);
        _area->renderables.push_back(_renderable);
        setTreeWidgetItemState(TreeItemState::Active);
        //Q_EMIT activated();
        return true;
    } else if(_active) {
        return true;
    }

    return false;
}

bool TkEntity::deactivate(void) {
    bool wasActive = _active;
    if(elysian::EntityBase::deactivate() && wasActive && _selectable) {
        elysian::containerEraseOnMatch(_selectionCollider, _area->collidables);
        elysian::containerEraseOnMatch(_selectionRenderable, _area->renderables);
        setTreeWidgetItemState(TreeItemState::Inactive);
        //Q_EMIT deactivated();
        return true;
    } else if(!_active) {
        return true;
    }

    return false;
}

void TkEntity::_renderRelative(void) {
    if(!gyVidShadowPass()) {

        if(_selectionState != SELECTION_NONE /* || !getRenderable()*/) {
            _selectionRenderable->beginRender();
            //if(_depthType == DEPTH_BUFFERED) transform._size.y = transform._size.z;
            _selectionRenderable->render();
            _selectionRenderable->endRender();
        }

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
    if(getRigidBody())          success &= getRigidBody()->validate(detailedText);
    if(getWarp())               success &= getWarp()->validate(detailedText);
    if(getExaminable())         success &= getExaminable()->validate(detailedText);
    if(getCharacter())          success &= getCharacter()->validate(detailedText);
    if(getLightSource())        success &= getLightSource()->validate(detailedText);
    if(getAudioSource())        success &= getAudioSource()->validate(detailedText);
    if(getParticleEmitter())    success &= getParticleEmitter()->validate(detailedText);
    if(getCombat())             success &= getCombat()->validate(detailedText);
    if(getSkeleton())           success &= getSkeleton()->validate(detailedText);

    if(!success) {
        if(!recursive) {
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setDetailedText(detailedText);
            QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            QGridLayout* layout = (QGridLayout*)msgBox.layout();
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
            msgBox.exec();
        }
    }

    return success;

}

QMenu* TkEntity::createContextMenu(void) {
    MenuStack menuStack("Entity");

    auto canAddTo = [&](const elysian::Component&& component) {
        return component.canAddTo(*this);
    };

    menuStack.insertCheckableAction("Active",
        [&](bool checked) {
            if(checked) {
                QString errorMsg;
                if(validate(errorMsg)) {
                    activate();
                } else setTreeWidgetItemState(TreeItemState::Invalid);
            }
            else {
                deactivate();
            }
        },
        isActive()
    );

    menuStack.insertAction("Delete",
         [&](bool) {
            if(isActive()) getAssets()->getCurrentArea()->removeEntity(this);
            delete this;
    });

    menuStack.push("Components");

    menuStack.insertCheckableAction("Renderable",
        [&](bool checked) {
            if(checked) addRenderable();
            else removeRenderable();
        },
        getRenderable(),
        canAddTo(elysian::Renderable()) && !isActive()
    )->setMenu(getRenderable()? getRenderable()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Collidable",
        [&](bool checked) {
            if(checked) addCollidable();
            else removeCollidable();
        },
        getCollidable(),
        canAddTo(elysian::Collidable()) && !isActive()
    )->setMenu(getCollidable()? getCollidable()->createContextMenu() : nullptr);

#if 0
    menuStack.insertCheckableAction("Animation",
        [&](bool checked) {
            if(checked) addAnimation();
            else removeAnimation();
        },
        getAnimation(),
        canAddTo(elysian::Animation())
    )->setMenu(getAnimation()? getAnimation()->createContextMenu() : nullptr);
#endif

    menuStack.insertCheckableAction("Skeleton",
        [&](bool checked=false) {
            if(checked) addSkeleton();
            else removeSkeleton();
        },
        getSkeleton(),
        canAddTo(elysian::Skeleton()) && !isActive()
    )->setMenu(getSkeleton()? getSkeleton()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Rigid Body",
        [&](bool checked) {
            if(checked) addRigidBody();
            else removeRigidBody();
        },
        getRigidBody(),
        canAddTo(elysian::RigidBody()) && !isActive()
    )->setMenu(getRigidBody()? getRigidBody()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Examinable",
        [&](bool checked) {
            if(checked) addExaminable();
            else removeExaminable();
        },
        getExaminable(),
        canAddTo(elysian::Examinable()) && !isActive()
    )->setMenu(getExaminable()? getExaminable()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Warp",
        [&](bool checked) {
            if(checked) addWarp();
            else removeWarp();
        },
        getWarp(),
        canAddTo(elysian::Warp()) && !isActive()
    )->setMenu(getWarp()? getWarp()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Character",
        [&](bool checked) {
            if(checked) addCharacter();
            else removeCharacter();
        },
        getCharacter(),
        canAddTo(elysian::Character()) && !isActive()
    )->setMenu(getCharacter()? getCharacter()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Light Source",
        [&](bool checked) {
            if(checked) addLightSource();
            else removeLightSource();
        },
        getLightSource(),
        canAddTo(elysian::LightSource()) && !isActive()
    )->setMenu(getLightSource()? getLightSource()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Audio Source",
        [&](bool checked) {
            if(checked) addAudioSource();
            else removeAudioSource();
        },
        getAudioSource(),
        canAddTo(elysian::AudioSource()) && !isActive()
    )->setMenu(getAudioSource()? getAudioSource()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Particle Emitter",
        [&](bool checked) {
            if(checked) addParticleEmitter();
            else removeParticleEmitter();
        },
        getParticleEmitter(),
        canAddTo(elysian::ParticleEmitter()) && !isActive()
    )->setMenu(getParticleEmitter()? getParticleEmitter()->createContextMenu() : nullptr);

    menuStack.insertCheckableAction("Combat",
        [&](bool checked) {
            if(checked) addCombat();
            else removeCombat();
        },
        getCombat(),
        canAddTo(elysian::Combat()) && !isActive()
    )->setMenu(getCombat()? getCombat()->createContextMenu() : nullptr);

    menuStack.pop();

    menuStack.push("Behaviors");

    unsigned behavNum = 0;
    for(auto behav : _behaviors) {
        char strBuff[TK_ENTITY_CONTEXT_MENU_BEHAVIOR_NAME_SIZE];

        sprintf(strBuff, "%d - %s", ++behavNum, Q_CSTR(static_cast<LuaBehavior*>(behav)->getScriptName()));
        menuStack.insertCheckableAction(strBuff,
            [=](bool checked) {
                if(!checked) {
                    removeLuaBehavior(behav->getBehavName());
                }
            },
            true,
            true
        );
    }
#if 0
    menuStack.insertAction("New Behavior",
                            [&](const bool checked) {
        LuaBehavior* behav = new LuaBehavior(*this);
        attachBehavior(behav);
    });
#endif
    menuStack.pop();


    return menuStack.first();
}

void TkEntity::setTreeWidgetItemState(const TreeItemState state) {
    QBrush brush;

    switch(state) {
    case TreeItemState::Active:
        brush = QBrush(Qt::black);
        break;
    case TreeItemState::Inactive:
        brush = QBrush(Qt::gray);
        break;
    case TreeItemState::Invalid:
        brush = QBrush(Qt::red);
        break;
    }

    setForeground(0, brush);
    for(int i = 0; i < childCount(); ++i) {
        static_cast<Entity*>(child(0))->setForeground(0, brush);
    }

}

void TkEntity::registerProxyMetaTable(lua_State*const vm) {

    LuaManager::stackBalanceBegin();

    getGlobal(OOLUA::Proxy_class<elysian::Entity>::class_name); //userdata metatable
    lua_newtable(*_mainState);
    lua_pushstring(vm, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
    lua_pushvalue(vm, -2);
    lua_rawset(vm, -4);

    MenuStack *menuStack = new MenuStack("Entity");

    menuStack->insertCheckableAction("Active",
        [&](bool) {
        }
    );

    auto* planeDelegate = new EnumDropdownPropertyItemDelegate({
       "RECT_PLANE_BOTTOM",
       "RECT_PLANE_FRONT",
       "RECT_PLANE_LEFT",
       "RECT_PLANE_RIGHT",
       "RECT_PLANE_FRONT_SLOPE_45",
       "RECT_PLANE_LEFT_SLOPE_45",
       "RECT_PLANE_RIGHT_SLOPE_45"
    });
    planeDelegate->setContextMenu(menuStack->first());
    OOLUA::push(*_mainState, planeDelegate);
    lua_setfield(*_mainState, -2, "planeType");

    auto* elevDelegate = new SpinboxPropertyItemDelegate(0, ELYSIAN_TILE_ENTRY_ELEV_MAX);
    OOLUA::push(*_mainState, elevDelegate);
    lua_setfield(*_mainState, -2, "elev");

    lua_pop(*_mainState, 2);

    LuaManager::stackBalanceEnd();

    elysian::EntityBase::registerProxyMetaTable(vm);
}
