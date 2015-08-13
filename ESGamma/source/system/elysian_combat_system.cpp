#include "elysian_combat_system.hpp"
#include <crtp/elysian_combat.hpp>
#include <crtp/elysian_area.hpp>
#include "elysian_collision_system.hpp"
#include "crtp/elysian_camera_system.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_audio_system.hpp"
#include <system/elysian_party_system.hpp>
#include <system/elysian_particle_system.hpp>
#include <cmath>

namespace elysian {

void CombatSystem::initialize(void) {
	getCollisionSys()->registerContactEventMonitor(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK, this);
}

void CombatSystem::onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLISION_ENTER) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

    if(!(combatA->getDmgType() == Combat::DMG_SOURCE || combatA->getDmgType() == Combat::DMG_SOURCE_TARGET) ||
            !((combatB->getDmgType() == Combat::DMG_TARGET || combatB->getDmgType() == Combat::DMG_SOURCE_TARGET))) return;

    Debug::printf(Debug::DBG_VERBOSE, "A PREFLAGS %d", bodyA->getContactEventMonitorMask());
    Debug::printf(Debug::DBG_VERBOSE, "B PREFLAGS %d", bodyB->getContactEventMonitorMask());
	_dealCollisionDamage(combatA, combatB, bodyB, contact);
    Debug::printf(Debug::DBG_VERBOSE, "A POSTFLAGS %d", bodyA->getContactEventMonitorMask());
    Debug::printf(Debug::DBG_VERBOSE, "B POSTFLAGS %d", bodyB->getContactEventMonitorMask());
}

void CombatSystem::onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* /*contact*/) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLIDE) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

    //_dealCollisionDamage(combatA, combatB, bodyB, contact);
}

void CombatSystem::onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLISION_EXIT) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

    //_dealCollisionDamage(combatA, combatB, bodyB, nullptr);
}

void CombatSystem::addCombatant(Entity& entity) {
	getCameraSys()->addEntity(&entity);
    entity.getCombat()->setInCombat(true);
	getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_ENTER);
	if(_bgmDynamic) getAudioSys()->morphIntoBgmTrack(_bgmCombatTrack, 0.5f);
	_combatants.claimFreeEntry() = &entity;
    if(entity.getCombat()->getTargetType() != Combat::TARGET_NONE) getGuiSys()->addCombatStatsEntity(&entity);
}

void CombatSystem::removeCombatant(Entity& entity) {
    entity.getCombat()->setInCombat(false);
	getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_EXIT);
	getCameraSys()->removeEntity(&entity);
	_combatants.removeEntry(&entity);
	if(_combatants.size() == 0 && _bgmDynamic) {
		getAudioSys()->morphIntoBgmTrack(0, 0.5f);
	}
    if(entity.getCombat()->getTargetType() != Combat::TARGET_NONE) getGuiSys()->removeCombatStatsEntity(&entity);
}

void CombatSystem::killCombatant(Entity& entity) {
	getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_DEATH);
    entity.getCombat()->setInCombat(false);
	//removeCombatant(entity); //don't call that or we call the ON_EXIT trigger
	getCameraSys()->removeEntity(&entity);
	_combatants.removeEntry(&entity);
	if(_combatants.size() == 0 && _bgmDynamic) {
		getAudioSys()->morphIntoBgmTrack(0, 0.5f);
	}
    /*
    if(entity.getCombat()->getTargetType() == Combat::TARGET_ENEMY) {
        GYVector2 pos = *(GYVector2*)&entity._transform._pos;
        pos.y += 0.5f*entity._transform._size.y;

        getGuiSys()->addVictoryText(*(GYVector2*)&entity._transform._pos, "10 EXP");
    }*/
    if(entity.getCombat()->getTargetType() != Combat::TARGET_NONE) {
        getGuiSys()->removeCombatStatsEntity(&entity);
    }

	//getCurArea()->removeEntity(&entity);
	//delete &entity;
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

//MACRO THESE FUNCTION NAMES

#if 0
void CombatSystem::resolveCombatAction(OOLUA::Lua_table_ref action, Combat*const src, Combat*const target) {
	OOLUA::Lua_table_ref statMods;

	_curActionRes.src = src;
	_curActionRes.target = target;

	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, action.ref());	//push actionTable
	lua_getfield(*_mainState, -1, "calculateCombatStatMods");
	OOLUA::push(*_mainState, src);
	_protectedCall(1, 1, nullptr);
	OOLUA::pull(*_mainState, statMods);

	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, target->getSelfTable().ref()); //push combat target's self table
	lua_getfield(*_mainState, -1, "applyCombatAction");
	_callFunction<void>(nullptr, statMods);

	_processCurrentCombatAction();
}
#endif


void CombatSystem::sendStatusModifier(Combat* const target, CombatStatusModifier* const modifier) {
	//not a valid combat target
	if(target->getDmgType() != Combat::DMG_TARGET && target->getDmgType() != Combat::DMG_SOURCE_TARGET) return;

	getBehavSys()->onCombatReceiveStatusModifier(target->getEntity(), modifier);

    target->applyModifier(*modifier);

    _curActionRes.initialVel = GYVector2 { 0.0f, -3.0f };
	if(_curActionRes.contact) {
		_curActionRes.contact->getNormal(_curActionRes.initialVel);
		//gyVec2ScaleAccum(&_curActionRes.initialVel, _curActionRes.contact->getClosingVel());
		//reverse the direction if normal is from point-of-reference of the other colliding body
		//if(_curActionRes.contact->getCollidable(0) != target->getEntity()->getCollidable()) {
		//	gyVec2ScaleAccum(&_curActionRes.initialVel, -1.0f);
		//}
	}

    //Emit combat text from target
    _emitCombatModifierText(target, *modifier);

	//check for particle emission
	if(_curActionRes.contact && target->getAttrib()->getParticleAttrib()) {
		GYVector3 point;
		getPartSys()->setAttrib(2, target->getAttrib()->getParticleAttrib());
		_curActionRes.contact->getWorldPos(*(GYVector2*)&point);
        point.z = target->getEntity()->_transform._pos.z + 8.0f;
        //Emit from POC with registered color and separating vel intensity
        getPartSys()->emitFromPoint(2, point, tan(point.y/point.x), 4000);
	}

	//check if dead
	if(target->getStatus().getHp() <= 0) {
		killCombatant(*target->getEntity());
	}
}


void CombatSystem::_dealCollisionDamage(Combat *const src, Combat *const target, Collidable* bodyB, CollisionContact* contact) {
    CombatStatusModifier modifier;
	_curActionRes.contact = contact;
	getBehavSys()->onCombatTargetCollisionEnter(src->getEntity(), contact, bodyB, &modifier);
    //resolveCombatAction(src->getActionTable(), src, target);]
	sendStatusModifier(target, &modifier);
}

void CombatSystem::update(void) {
    auto* leader = getPartySys()->getPartyLeader();
    const auto& playerPos = leader->_transform._pos;


    for(auto&it : getCurrentArea()->combats) {
        if(it == leader->getCombat() || it->getTargetType() == Combat::TARGET_NONE) continue;
        const float dist = gyVec3Dist(&playerPos, &it->getEntity()->_transform._pos);

        //Check for deaggro
        if(it->isInCombat()) {
            if(dist > it->getDeaggroDist()) removeCombatant(*it->getEntity());
        //Check for aggro
        } else {
            if(dist < it->getAggroDist()) addCombatant(*it->getEntity());
        }
    }

    //Check whether we should add or remove leader (PARTY EVENTUALLY)
    if(leader && leader->getCombat() && leader->getCombat()->isInCombat() && _combatants.size() == 1) {
        removeCombatant(*leader);
    } else if(_combatants.size() > 0 && !leader->getCombat()->isInCombat()) {
        addCombatant(*leader);
    }

}

/* Needs to gracefully handle the case where there are multiple modifiers.
 * Queue them so they aren't immediately shit out ontop of each other, looking like dick.
 */
void CombatSystem::_emitCombatModifierText(const Combat*const target, const CombatStatus& modifier) {
    auto emitIntegerAttrib = [&](const int attrib, const GYColor4 posColor, const GYColor4 negColor) {
		char buff[20];

		//check if the attribute was modified
		if(attrib != 0) {
            GYVector2 pos = *(GYVector2*)&target->getEntity()->_transform._pos;
            pos.y += 0.5f*target->getEntity()->_transform._size.y;
            //pos.y -= 0.7f*target->getEntity()->_transform._pos.z;

            //GYVector2 newVel = { _curActionRes.initialVel.x * 0.5f, _curActionRes.initialVel.y *0.5f };


			sprintf(buff, "%d", abs(attrib));
            getGuiSys()->addCombatText(buff, (attrib > 0)? posColor : negColor, pos, _curActionRes.initialVel);
		}
	};

    if(target->getTargetType() == Combat::TARGET_PARTY || target->getTargetType() == Combat::TARGET_FRIENDLY) {
        emitIntegerAttrib(modifier.getHp(), GYColor4 { 100, 255, 100, 255 }, GYColor4 { 255, 100, 100, 255 });
        emitIntegerAttrib(modifier.getMp(), GYColor4 { 100, 100, 255, 255 }, GYColor4 { 200, 100, 50, 255 });
    } else {
        emitIntegerAttrib(modifier.getHp(), GYColor4 { 100, 255, 100, 255 }, GYColor4 { 255, 255, 255, 255 });
        emitIntegerAttrib(modifier.getMp(), GYColor4 { 100, 100, 255, 255 }, GYColor4 { 200, 100, 50, 255 });
    }

    //Emit icons for status ailments!
}

}
