#include "elysian_combat_system.hpp"
#include "elysian_combat.hpp"
#include "elysian_area.hpp"
#include "elysian_lua_wrappers.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_camera_system.hpp"
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

	_dealCollisionDamage(combatA, combatB, contact);
}

void CombatSystem::onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLIDE) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

	_dealCollisionDamage(combatA, combatB, contact);
}

void CombatSystem::onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) {
	auto* combatA = bodyA->getEntity()->getCombat();
	if(!(combatA->getCollisionFlags() & Combat::ON_COLLISION_EXIT) || !combatA->getDmgActive()) return;
	auto* combatB = bodyB->getEntity()->getCombat();
	if(!combatB || !(combatB->getAttrib()->getTypeFlags() & combatA->getAttrib()->getTypeAgainstFlags())) return;

	_dealCollisionDamage(combatA, combatB, nullptr);
}

void CombatSystem::addCombatant(Entity& entity) {
	getCameraSys()->addEntity(&entity);
}

void CombatSystem::removeCombatant(Entity& entity) {
	getCameraSys()->removeEntity(&entity);
}

void CombatSystem::killCombatant(Entity * const /*entity*/) {

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

void CombatSystem::_dealCollisionDamage(Combat *const src, Combat *const target, CollisionContact* contact) {
	_curActionRes.contact = contact;
	resolveCombatAction(src->getActionTable(), src, target);
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


}
