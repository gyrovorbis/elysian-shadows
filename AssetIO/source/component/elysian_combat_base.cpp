#include <AssetIO/crtp/elysian_combat.hpp>
#include <AssetIO/utility/elysian_generic.hpp>
#include <AssetIO/crtp/elysian_collidable.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/crtp/elysian_timer_system.hpp>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CombatStatus)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CombatBase)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CombatAttrib)

void CombatBase::setDmgType(const DMG_TYPE type) {
	_dmgType = type;

	uint32_t mask = getEntity()->getCollidable()->getContactEventMonitorMask();

	switch(type) {
	case DMG_SOURCE:
	case DMG_SOURCE_TARGET:
		mask |= ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK;
		break;
	case DMG_TARGET:
	case DMG_NONE:
	default:
		mask &= ~ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK;
	}

	getEntity()->getCollidable()->setContactEventMonitorMask(mask);
}


void CombatBase::update(void) {
    if(_dmgRecovering) {
        _dmgRecoveryElapsed += System::getTimerSys()->getDeltaTime();
        if(_dmgRecoveryElapsed >= _dmgRecoveryTime) {
            _dmgRecovering = false;
        }
    }

}

void AggroManager::_updateTargetEntity(void) {
    Entity* newTarget = getEntityAtEmnityPriority(0);
    if(newTarget != _targetEntity) {
        //tell the old target we don't give a shit about him anymore
        if(_targetEntity) {
            _targetEntity->getCombat()->getAggroManager().removeAggroedEntity(_parentEntity);
        }
        //tell the new target we're coming to kill him
        if(newTarget) {
            newTarget->getCombat()->getAggroManager().addAggroedEntity(_parentEntity);
        }
        _targetEntity = newTarget;
    }
}


void AggroManager::addEntityEmnity(Entity *entity, int16_t emnityDelta) {
    assert(entity);

    if(!emnityDelta) return;
    bool found = false;

    //Check whether entity already has a table entry
    for(auto it = _emnityTable.begin(); it != _emnityTable.end(); ++it) {
        if((*it).entity == entity) {
            (*it).emnity += emnityDelta;
            found = true;
            if((*it).emnity <= 0) {	//remove entry if emnity < 0
                _emnityTable.erase(it);
            }
            break;
        }
    }

    if(!found) _emnityTable.push_back(EmnityEntry{entity, emnityDelta});
    _updateEmnityTable();
}

void AggroManager::setEntityEmnity(Entity *entity, int16_t emnityVal) {
    assert(entity && emnityVal >= 0);
    bool found = false;

    for(auto it = _emnityTable.begin(); it != _emnityTable.end(); ++it) {
        if((*it).entity == entity) {
            if(emnityVal) {
                (*it).emnity = emnityVal;
            } else {
                _emnityTable.erase(it);
            }
            found = true;
        }
    }

    if(!found) {
        if(!emnityVal) return; //don't have shit to do
        else {
            _emnityTable.push_back(EmnityEntry{entity, emnityVal});
        }
    }

    _updateEmnityTable();
}

}
