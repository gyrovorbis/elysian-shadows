#include "elysian_combat_system.hpp"
#include "elysian_combat.hpp"
#include "elysian_area.hpp"
#include "elysian_lua_wrappers.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_audio_system.hpp"
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

	_dealCollisionDamage(combatA, combatB, bodyB, contact);
}

void CombatSystem::onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLIDE) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

	_dealCollisionDamage(combatA, combatB, bodyB, contact);
}

void CombatSystem::onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLISION_EXIT) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

	_dealCollisionDamage(combatA, combatB, bodyB, nullptr);
}

void CombatSystem::addCombatant(Entity& entity) {
	getCameraSys()->addEntity(&entity);
	getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_ENTER);
	if(_bgmDynamic) getAudioSys()->morphIntoBgmTrack(_bgmCombatTrack, 0.5f);
	_combatants.claimFreeEntry() = &entity;
}

void CombatSystem::removeCombatant(Entity& entity) {
	getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_EXIT);
	getCameraSys()->removeEntity(&entity);
	_combatants.removeEntry(&entity);
	if(_combatants.size() == 0 && _bgmDynamic) {
		getAudioSys()->morphIntoBgmTrack(0, 0.5f);
	}
}

void CombatSystem::killCombatant(Entity& entity) {
	getBehavSys()->callTrigger(&entity, Behavior::ON_COMBAT_DEATH);
	//removeCombatant(entity); //don't call that or we call the ON_EXIT trigger
	getCameraSys()->removeEntity(&entity);
	_combatants.removeEntry(&entity);
	if(_combatants.size() == 0 && _bgmDynamic) {
		getAudioSys()->morphIntoBgmTrack(0, 0.5f);
	}
	//getCurArea()->removeEntity(&entity);
	//delete &entity;
}

void CombatSystem::_processCurrentCombatAction(void) {
	GYVector2 vel;
	GYVector3 pos3;

//	if(_curActionRes.contact) {
//		//Calculate combatText velocity
//		_curActionRes.contact->getNormal(vel);
//		gyVec2ScaleAccum(&vel, _curActionRes.contact->getClosingVel());
//		if(_curActionRes.contact->getCollidable(1) == _curActionRes.target->getEntity()->getCollidable())
//			gyVec2ScaleAccum(&vel, -1.0f);

//		//Add combat text to PoC
//		_curActionRes.contact->getWorldPos(*(GYVector2*)&pos3);
//		getGuiSys()->addCombatText(_curActionRes.receipt->getText(), _curActionRes.receipt->getTextColor(), *(GYVector2*)&pos3, vel);

//		//Emit particles from PoC
//		getPartSys()->getAttrib(2)->setColor(_curActionRes.receipt->getPartColor());
//		pos3.z = _curActionRes.target->getEntity()->getElev()*32.0f;
//		const float angle = tan(vel.y/vel.x);
//		getPartSys()->emitFromPoint(2, pos3, angle, _curActionRes.contact->getClosingVel()*_curActionRes.receipt->getPartIntensityFactor());

//	}

	//Reset everything
	_curActionRes.contact	= nullptr;
	_curActionRes.receipt	= nullptr;
	_curActionRes.src		= nullptr;
	_curActionRes.target	= nullptr;

}

//MACRO THESE FUNCTION NAMES


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


void CombatSystem::sendStatusModifier(Combat* const target, CombatStatus* const modifier) {
	//not a valid combat target
	if(target->getDmgType() != Combat::DMG_TARGET && target->getDmgType() != Combat::DMG_SOURCE_TARGET) return;

	getBehavSys()->onCombatReceiveStatusModifier(target->getEntity(), modifier);

	target->applyStatus(*modifier);

    _curActionRes.initialVel = GYVector2 { 0.0f, -3.0f };
	if(_curActionRes.contact) {
		_curActionRes.contact->getNormal(_curActionRes.initialVel);
		//gyVec2ScaleAccum(&_curActionRes.initialVel, _curActionRes.contact->getClosingVel());
		//reverse the direction if normal is from point-of-reference of the other colliding body
		//if(_curActionRes.contact->getCollidable(0) != target->getEntity()->getCollidable()) {
		//	gyVec2ScaleAccum(&_curActionRes.initialVel, -1.0f);
		//}
	}

	//emit combat text from target
	_emitCombatModifierText(target, *modifier);

	//check for particle emission
	if(_curActionRes.contact && target->getAttrib()->getParticleAttrib()) {
		GYVector3 point;
		getPartSys()->setAttrib(2, target->getAttrib()->getParticleAttrib());
		_curActionRes.contact->getWorldPos(*(GYVector2*)&point);
		point.z = target->getEntity()->transform._pos.z + 8.0f;
		//emit from POC with registered color and separating vel intensity
		getPartSys()->emitFromPoint(2, point, tan(point.y/point.x), 4000);
	}

	//check if dead
	if(target->getStatus().getHp() <= 0) {
		killCombatant(*target->getEntity());
	}
}


void CombatSystem::_dealCollisionDamage(Combat *const src, Combat *const target, Collidable* bodyB, CollisionContact* contact) {
	CombatStatus modifier;
	_curActionRes.contact = contact;
	getBehavSys()->onCombatTargetCollisionEnter(src->getEntity(), contact, bodyB, &modifier);
    //resolveCombatAction(src->getActionTable(), src, target);
	sendStatusModifier(target, &modifier);
}

void CombatSystem::update(void) {
/*	std::vector<Combat*>& combats = getCurArea()->combats;

	for(unsigned i = 0; i < combats.size(); ++i) {

		if(combats[i]->getDmgType() == Combat::DMG_SOURCE) {
			for(unsigned j = 0; j < combats.size(); ++j) {
				if(i == j) continue;

				if(combats[j]->getDmgType() == Combat::DMG_TARGET &&
						(combats[i]->getAttrib()->getTypeAgainstFlags() & combats[j]->getAttrib()->getTypeFlags())) {

					if(getCollisionSys()->checkCollision(*combats[i]->getEntity()->getCollidable(), *combats[j]->getEntity()->getCollidable()))
						_dealCollisionDamage(combats[i], combats[j]);

				}
			}

		}

	}*/
}

/* Needs to gracefully handle the case where there are multiple modifiers.
 * Queue them so they aren't immediately shit out ontop of each other, looking like dick.
 */
void CombatSystem::_emitCombatModifierText(const Combat*const target, const CombatStatus& modifier) {
	auto emitIntegerAttrib = [&](const int attrib, const GYColor4 posColor, const GYColor4 negColor) {
		char buff[20];

		//check if the attribute was modified
		if(attrib != 0) {
            GYVector2 pos = *(GYVector2*)&target->getEntity()->transform._pos;
            pos.y -= 0.7f*target->getEntity()->transform._pos.z;

			sprintf(buff, "%d", abs(attrib));
            getGuiSys()->addCombatText(buff, (attrib > 0)? posColor : negColor, pos, _curActionRes.initialVel);
		}
	};

    emitIntegerAttrib(modifier.getHp(), GYColor4 { 0, 255, 0, 255 }, GYColor4 { 255, 0, 0, 255 });
    emitIntegerAttrib(modifier.getMp(), GYColor4 { 0, 0, 255, 255 }, GYColor4 { 200, 100, 0, 255 });

	//emit icons for status ailments!
}

}
