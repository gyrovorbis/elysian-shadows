#include "tk_entity_selectable.h"
#include <asset/tk_entity.h>
#include "elysian_engine.hpp"
#include "elysian_behavior_system.hpp"
#include "tk_lua_table_stack_model.h"


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

EntitySelectable::EntitySelectable(void) {
    _tableRef = LuaVariant::newTable();
}


EntitySelectable::EntitySelectable(TkEntity*const entity):
    EntitySelectable()
{
    addEntity(entity);
}

EntitySelectable::EntitySelectable(EntitySelectable *selectable):
    EntitySelectable()
{
    for(auto it : selectable->_entities) {
        EntityEntry entry;
        entry.wasActive = it.wasActive;
        entry.entity = new TkEntity(it.entity);
        addEntity(entry);
    }
}

QAbstractItemModel* EntitySelectable::getModel(void) const {
    if(!_luaModel) {
        _luaModel = new LuaTableStackModel(_tableRef);
        _luaModel->populateModel();
    }
    return _luaModel;
}

auto EntitySelectable::getType(void) const -> TYPE { return ENTITY; }

//const QVector<TkEntity*>& EntitySelectable::getEntities(void) const {
//	return _entities;
//}

void EntitySelectable::addEntity(const EntityEntry entry) {
    if(!entry.entity) return;

    LuaManager::stackBalanceBegin();

    _entities.push_back(entry);
    //insertSourceModel(entry->getModel());

    _tableRef[_entities.size()] = entry.entity;

#if 0
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaTableRef);
    lua_pushinteger(*_mainState, _entities.size());
    OOLUA::push(*_mainState, entry.entity);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);
#endif

    //if(!_baseEntity) _baseEntity = entry.entity;

    entry->getModel()->setLabelItemText("Entity");

    LuaManager::stackBalanceEnd();
}

void EntitySelectable::removeEntity(TkEntity * const entity) {
    LuaManager::stackBalanceBegin();

    for(int i = 0; i < _entities.size(); ++i) {
        if(_entities[i] == entity) {
            if(_entities[i] == _baseEntity) _baseEntity = nullptr;
            _entities.erase(_entities.begin()+i);
            //removeSourceModel(entity->getModel());
#if 0
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaTableRef);
            lua_pushinteger(*_mainState, i);
            lua_pushnil(*_mainState);
            lua_settable(*_mainState, -3);
            lua_pop(*_mainState, 1);
#else
            _tableRef[i] = nullptr;
#endif
            break;
		}
	}

    LuaManager::stackBalanceEnd();
}

void EntitySelectable::translate(const float x, const float y) {
    for(auto& it : _entities) it->getTransform().translate(x, y);
}

void EntitySelectable::setPosition(const float x, const float y) {
    const GYVector3 basePos = getBaseEntity()->_transform._pos;

    auto moveEntity = [&](TkEntity* entity) {
        const GYVector3 prevPos = entity->_transform._pos;
        GYVector3 offset;
        gyVec3Sub(&offset, &prevPos, &basePos);
        entity->_transform.setPos(x+offset.x, y+offset.y, prevPos.z);
    };

    getBaseEntity()->_transform._pos = GYVector3 { x, y+basePos.z, basePos.z };
    for(auto entity : _entities)  {

        if(entity != getBaseEntity()) {
            moveEntity(entity);
        }
    }
}


void EntitySelectable::setCurrentSelection(const bool val, const Selectable* const other) {
    QVector<EntityEntry> exclusiveEntities = _entities;


    if(other && other->getType() == Selectable::ENTITY) {
        const EntitySelectable& otherEntities = *static_cast<const EntitySelectable*>(other);
        for(auto it  = otherEntities._entities.begin(); it != otherEntities._entities.end(); ++it) {
            for(auto it2 = exclusiveEntities.begin(); it2 != exclusiveEntities.end(); ++it2) {
                if(*it == *it2) {
                    it2 = exclusiveEntities.erase(it2);
                    break;
                }
            }
        }
        _baseEntity = otherEntities._baseEntity;
    }

    for(auto& it : exclusiveEntities) {
        it->setSelectionState(val? TkEntity::SELECTION_SELECTED : TkEntity::SELECTION_NONE);
        getAssets()->getEngine()->getBehavSys()->callTrigger(it, val? elysian::Behavior::ON_TK_SELECT : elysian::Behavior::ON_TK_UNSELECT);
    }
}

void EntitySelectable::deactivate(void) {
    for(auto& it : _entities) {
        it.wasActive = it->isActive();
        if(it->getArea()) it->getArea()->removeEntity(it);
    }
}

void EntitySelectable::activate(TkArea* area) {
    for(auto& it : _entities) {
        area->addEntity(it);
        if(it.wasActive) it->activate();
        else it->deactivate();
    }

}
