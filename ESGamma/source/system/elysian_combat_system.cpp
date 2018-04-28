#include "elysian_combat_system.hpp"
#include <crtp/elysian_combat.hpp>
#include <crtp/elysian_area.hpp>
#include "elysian_collision_system.hpp"
#include "crtp/elysian_camera_system.hpp"
#include "elysian_gui_system_base.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_audio_system.hpp"
#include <system/elysian_party_system.hpp>
#include <crtp/elysian_particle_system.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <cmath>

namespace elysian {

void CombatSystem::initialize(void) {
  getCollisionSys()->registerContactEventMonitor(
      ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK, this);
}

void CombatSystem::onCollisionEnter(Collidable* const bodyA,
                                    Collidable* const bodyB,
                                    CollisionContact* contact) {
    uint32_t mask = 0;
  auto* combatA = bodyA->getEntity()->getCombat();
  if(!combatA) return;
  mask = (combatA->getCollisionFlags().getValue() & (uint32_t)Combat::ON_COLLISION_ENTER);
  if (!(mask) ||
      !combatA->getDmgActive())
    return;
  auto* combatB = bodyB->getEntity()->getCombat();
  if(!combatB) return;
  mask = (combatB->getAttrib()->getTypeFlags().getValue() &
          combatA->getAttrib()->getTypeAgainstFlags().getValue());
  if (!combatB ||
      !(mask) || !combatB->getDmgActive())
    return;

  if (!(combatA->getDmgType() == Combat::DMG_SOURCE ||
        combatA->getDmgType() == Combat::DMG_SOURCE_TARGET) ||
      !((combatB->getDmgType() == Combat::DMG_TARGET ||
         combatB->getDmgType() == Combat::DMG_SOURCE_TARGET)))
    return;

  // Debug::printf(Debug::DBG_VERBOSE, "A PREFLAGS %d",
  // bodyA->getContactEventMonitorMask());
  // Debug::printf(Debug::DBG_VERBOSE, "B PREFLAGS %d",
  // bodyB->getContactEventMonitorMask());
  _dealCollisionDamage(combatA, combatB, bodyB, contact);
  // Debug::printf(Debug::DBG_VERBOSE, "A POSTFLAGS %d",
  // bodyA->getContactEventMonitorMask());
  // Debug::printf(Debug::DBG_VERBOSE, "B POSTFLAGS %d",
  // bodyB->getContactEventMonitorMask());
}

void CombatSystem::onCollide(Collidable* const /*bodyA*/, Collidable* const /*bodyB*/,
                             CollisionContact* /*contact*/) {
#if 0
  auto* combatA = bodyA->getEntity()->getCombat();
  if(!combatA) return;
  if (!(combatA->getCollisionFlags().getValue() & Combat::ON_COLLIDE) ||
      !combatA->getDmgActive())
    return;
  auto* combatB = bodyB->getEntity()->getCombat();
  if (!combatB ||
      !(combatB->getAttrib()->getTypeFlags().getValue() &
        combatA->getAttrib()->getTypeAgainstFlags().getValue()))
    return;
#endif
  //_dealCollisionDamage(combatA, combatB, bodyB, contact);
}

void CombatSystem::onCollisionExit(Collidable* const /*bodyA*/,
                                   Collidable* const /*bodyB*/) {
#if 0
  auto* combatA = bodyA->getEntity()->getCombat();
  if (!(combatA->getCollisionFlags().getValue() & Combat::ON_COLLISION_EXIT) ||
      !combatA->getDmgActive())
    return;
  auto* combatB = bodyB->getEntity()->getCombat();
  if (!combatB ||
      !(combatB->getAttrib()->getTypeFlags().getValue() &
        combatA->getAttrib()->getTypeAgainstFlags().getValue()))
    return;
#endif
  //_dealCollisionDamage(combatA, combatB, bodyB, nullptr);
}

void CombatSystem::onAreaEnter(void) {
    clearCombatants();
}

void CombatSystem::clearCombatants(void) {
    std::vector<Entity*> combatCache;

    for(auto it = _combatants.begin(); it != _combatants.end(); ++it) {
        combatCache.push_back(*it);
    }
    for(auto it: combatCache)
        removeCombatant(*it);
    GY_ASSERT(!_combatants.size());
}

int CombatSystem::_getCombatantIndex(const Entity& entity) const {
    int index   = -1;
    int i       = 0;
    for(auto it = _combatants.begin(); it != _combatants.end(); ++it, ++i) {
        if(*it == &entity) {
            index = i;
            break;
        }
    }
    return index;
}

void CombatSystem::addCombatant(Entity& entity) {
    if(_getCombatantIndex(entity) != -1) return;
    auto* combat = entity.getCombat();
    GY_ASSERT(combat);

    ++_combatantTargetCounts[combat->getTargetType()];
    _combatants.claimFreeEntry() = &entity;
    if(_combatState == COMBAT_STATE_ACTIVE) _setCombatantActive(&entity, true);

    _updateCombatState();
}

void CombatSystem::removeCombatant(Entity& entity) {
    if(_getCombatantIndex(entity) == -1) return;
    auto* combat = entity.getCombat();
    GY_ASSERT(combat);

    --_combatantTargetCounts[combat->getTargetType()];
    _combatants.removeEntry(&entity);
    _setCombatantActive(&entity, false);

    _updateCombatState();
}


void CombatSystem::killCombatant(Entity& entity) {
    removeCombatant(entity);
    getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_DEATH);
}

void CombatSystem::_updateCombatState(void) {
    if(_combatantTargetCounts[Combat::TARGET_PARTY] >= 1 &&
       _combatantTargetCounts[Combat::TARGET_ENEMY] >= 1 &&
            _combatState != COMBAT_STATE_DISABLED) {
        _setCombatState(COMBAT_STATE_ACTIVE);

    } else {
        _setCombatState(COMBAT_STATE_INACTIVE);
    }
}

void CombatSystem::_setCombatState(COMBAT_STATE state) {
    if(_combatState != state) {
        switch(state) {
        case COMBAT_STATE_ACTIVE:
            if(_bgmDynamic) {
                _bgmPrevTrack = getAudioSys()->getBgmTrack();
                getAudioSys()->morphIntoBgmTrack(_bgmCombatTrack, 0.5f);
            }
            break;
        case COMBAT_STATE_INACTIVE:
        case COMBAT_STATE_DISABLED:
            if(_bgmDynamic && _combatState == COMBAT_STATE_ACTIVE) {
                getAudioSys()->morphIntoBgmTrack(_bgmPrevTrack, 0.5f);
            }
        }
        _combatState = state;
        for(auto it = _combatants.begin(); it != _combatants.end(); ++it) {
            _setCombatantActive(*it, (state == COMBAT_STATE_ACTIVE)? true: false);
        }
    }
}

void CombatSystem::_setCombatantActive(Entity *entity, bool active) const {
    auto* combat = entity->getCombat();
    GY_ASSERT(combat);// && combat->isInCombat() != active);

    if(combat->isInCombat() != active) {
        combat->setInCombat(active);
        if(active) {
            getCameraSys()->addEntity(entity);
            if(combat->getTargetType() != Combat::TARGET_NONE) getGuiSys()->addCombatStatsEntity(entity);
            getBehavSys()->callTrigger(entity, Behavior::ON_COMBAT_ENTER);
        } else {
            getCameraSys()->removeEntity(entity);
            if(combat->getTargetType() != Combat::TARGET_NONE) getGuiSys()->removeCombatStatsEntity(entity);
            getBehavSys()->callTrigger(entity, Behavior::ON_COMBAT_EXIT);

        }
    }
}

#if 0
void CombatSystem::_processCurrentCombatAction(CollisionContact*const contact, Combat*const target) {
    GYVector2 vel;
    GYVector3 pos3;

    if(contact) {
        //Calculate combatText velocity
        contact->getNormal(vel);
        gyVec2ScaleAccum(&vel, contact->getClosingVel());
        if(contact->getCollidable(1) == target->getEntity()->getCollidable())
            gyVec2ScaleAccum(&vel, -1.0f);

        //Add combat text to PoC
        //_curActionRes.contact->getWorldPos(*(GYVector2*)&pos3);
        //getGuiSys()->addCombatText(_curActionRes.receipt->getText(), _curActionRes.receipt->getTextColor(), *(GYVector2*)&pos3, vel);

        //Emit particles from PoC
        getPartSys()->getAttrib(2)->setColor(GYColor4{ 255, 0, 0, 255});
        pos3.z = target->getEntity()->getElev()*32.0f;
        const float angle = tan(vel.y/vel.x);
        getPartSys()->emitFromPoint(2, pos3, angle, contact->getClosingVel()*100.0);

    }

  //Reset everything
//	_curActionRes.contact	= nullptr;
//	_curActionRes.receipt	= nullptr;
//	_curActionRes.src		= nullptr;
//	_curActionRes.target	= nullptr;

}
#endif

// MACRO THESE FUNCTION NAMES

#if 0
void CombatSystem::resolveCombatAction(OOLUA::Lua_table_ref action, Combat*const src, Combat*const target) {
  OOLUA::Lua_table_ref statMods;

  _curActionRes.src = src;
  _curActionRes.target = target;

  lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, action.ref());	//push actionTable
  lua_getfield(*_mainState, -1, "calculateCombatStatMods");
  OOLUA::push(*_mainState, src);
  ELYSIAN_LUA_MANAGER_PROTECTED_CALL(1, 1, nullptr);
  OOLUA::pull(*_mainState, statMods);

  lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, target->getSelfTable().ref()); //push combat target's self table
  lua_getfield(*_mainState, -1, "applyCombatAction");
  _callFunction<void>(nullptr, statMods);

  _processCurrentCombatAction();
}
#endif

void CombatSystem::sendStatusModifier(Combat* const target,
                                      CombatStatusModifier* const modifier) {
  // not a valid combat target
  if (target->getDmgType() != Combat::DMG_TARGET &&
      target->getDmgType() != Combat::DMG_SOURCE_TARGET)
    return;

  getBehavSys()->onCombatReceiveStatusModifier(target->getEntity(), modifier);

  target->applyModifier(*modifier);
  target->startDmgRecoveryTimer();

  _curActionRes.initialVel = GYVector2{0.0f, -3.0f};
  if (_curActionRes.contact) {
    _curActionRes.contact->getNormal(_curActionRes.initialVel);
    // gyVec2ScaleAccum(&_curActionRes.initialVel,
    // _curActionRes.contact->getClosingVel());
    // reverse the direction if normal is from point-of-reference of the other
    // colliding body
    // if(_curActionRes.contact->getCollidable(0) !=
    // target->getEntity()->getCollidable()) {
    //	gyVec2ScaleAccum(&_curActionRes.initialVel, -1.0f);
    //}
  }

  // Emit combat text from target
  _emitCombatModifierText(target, *modifier);

  // check for particle emission
  if (_curActionRes.contact && target->getAttrib()->getParticleAttrib()) {
    GYVector3 point;
    getPartSys()->setAttrib(2, target->getAttrib()->getParticleAttrib());
    _curActionRes.contact->getWorldPos(((GYVectorPunned*)&point)->vec2);
    point.z = target->getEntity()->_transform._pos.z + 8.0f;
    // Emit from POC with registered color and separating vel intensity
    getPartSys()->emitFromPoint(2, point, tan(point.y / point.x), 4000);
  }

  // check if dead
  if (target->getStatus().getHp() <= 0) {
    killCombatant(*target->getEntity());
  }
}

void CombatSystem::_dealCollisionDamage(Combat* const src, Combat* const target,
                                        Collidable* bodyB,
                                        CollisionContact* contact) {
  CombatStatusModifier modifier;
  _curActionRes.contact = contact;
  getBehavSys()->onCombatTargetCollisionEnter(src->getEntity(), contact, bodyB,
                                              &modifier);
  // resolveCombatAction(src->getActionTable(), src, target);]
  sendStatusModifier(target, &modifier);
  target->startDmgRecoveryTimer();
}

void CombatSystem::update(void) {
    const unsigned partySize = getPartySys()->getPartySize();

    for(auto& it : getCurrentArea()->combats) {
        (*it).update();
    }

    if(_combatMgmtType != COMBAT_MGMT_MANUAL && _combatState != COMBAT_STATE_DISABLED) {
        //dirty-ass hack to poll for Entities that have been removed from area
        std::vector<Entity*> combatCache;
        for(auto it = _combatants.begin(); it != _combatants.end(); ++it) {
            if(!(*it)->isActive() || (*it)->getArea() != _curArea)
                combatCache.push_back(*it);
        }
        for(auto it: combatCache)
            removeCombatant(*it);

        //dirty-ass hack to add active party members as combatants
        for(unsigned i = 0; i < getPartySys()->getPartySize(); ++i) {
            auto* partyMem = getPartySys()->getCharacterPartyMember(i);
            addCombatant(*partyMem);
        }

        for(auto& it : getCurrentArea()->combats) {
            if(it->getTargetType() == Combat::TARGET_NONE || it->getTargetType() == Combat::TARGET_PARTY)
                continue;

            float dists[partySize];

            for(unsigned i = 0; i < partySize; ++i) {
                const auto& playerPos = getPartySys()->getCharacterPartyMember(i)->_transform._pos;

                dists[i] = gyVec3Dist(&playerPos, &it->getEntity()->_transform._pos);
            }

            //check for deaggro
            if(it->isInCombat()) {
                unsigned count = 0;
                for(unsigned i = 0; i < partySize; ++i) {
                    if(dists[i] > it->getDeaggroDist())
                        ++count;
                }
                //all party members have to be outside of deaggro range to remove
                if(count == partySize) removeCombatant(*it->getEntity());

            } else { //check for aggro
                //if even one party member is within range, add
                for(unsigned i = 0; i < partySize; ++i) {
                    if(dists[i] < it->getDeaggroDist()) {
                        addCombatant(*it->getEntity());
                        break;
                    }
                }
            }
        }
    }
}

/* Needs to gracefully handle the case where there are multiple modifiers.
 * Queue them so they aren't immediately shit out ontop of each other, looking
 * like dick.
 */
void CombatSystem::_emitCombatModifierText(const Combat* const target,
                                           const CombatStatus& modifier) {
  auto emitIntegerAttrib =
      [&](const int attrib, const GYColor4 posColor, const GYColor4 negColor) {
    char buff[20];

    // check if the attribute was modified
    if (attrib != 0) {
      GYVector2 pos = {target->getEntity()->_transform._pos.x,
                       target->getEntity()->_transform._pos.y};
      pos.y += 0.5f * target->getEntity()->_transform._size.y;
      // pos.y -= 0.7f*target->getEntity()->_transform._pos.z;

      // GYVector2 newVel = { _curActionRes.initialVel.x * 0.5f,
      // _curActionRes.initialVel.y *0.5f };

      sprintf(buff, "%d", abs(attrib));
      getGuiSys()->addCombatText(buff, (attrib > 0) ? posColor : negColor, pos,
                                 _curActionRes.initialVel);
    }
  };

  if (target->getTargetType() == Combat::TARGET_PARTY ||
      target->getTargetType() == Combat::TARGET_FRIENDLY) {
    emitIntegerAttrib(modifier.getHp(), GYColor4{100, 255, 100, 255},
                      GYColor4{255, 100, 100, 255});
    emitIntegerAttrib(modifier.getMp(), GYColor4{100, 100, 255, 255},
                      GYColor4{200, 100, 50, 255});
  } else {
    emitIntegerAttrib(modifier.getHp(), GYColor4{100, 255, 100, 255},
                      GYColor4{255, 255, 255, 255});
    emitIntegerAttrib(modifier.getMp(), GYColor4{100, 100, 255, 255},
                      GYColor4{200, 100, 50, 255});
  }

  // Emit icons for status ailments!
}

LuaVariant CombatSystem::getCombatants(void) const {
   return LuaVariant::fromSequenceContainer(_combatants);
}

}
