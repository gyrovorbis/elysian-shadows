#include "elysian_rigid_body_system.hpp"
#include "elysian_area.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_force_generator.hpp"
#include "elysian_debug.hpp"
#include "elysian_level.hpp"

namespace elysian {

void RigidBodySystem::update(void) {
	_updateContacts();
}

void RigidBodySystem::applyForces(void) {
	auto rigidBodies = getCurArea()->rigidBodies;
	const float deltaTime = getTimerSys()->getDeltaTime();

	for(auto force = _forces.begin(); force < _forces.end(); ++force) {
		for(auto rb : rigidBodies) {
			if((*force)->getForceMask() & rb->getAttrib()->getForceMask()) {
				(*force)->applyForce(rb, deltaTime);
			}
		}
	}
}

void RigidBodySystem::_updateContacts(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	CollisionSystem* colSys = getCollisionSys();
	const unsigned contactCount = colSys->getContactCount();
	CollisionContact* curContact;

	_contactCount = 0;

	//figure out how many of these contacts are rigidBodies
	//that even give a shit about velocity resolution
	for(unsigned i = 0; i < contactCount; ++i) {
		curContact = colSys->getContact(i);

		if(curContact->getCollidable(0)->getEntity()->getRigidBody()) {
			_contactPool[_contactCount++] = curContact;
			curContact->_calculateVelocityInternals(deltaTime);
			assert(_contactCount <= ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE);
		}
	}

}

void RigidBodySystem::resolveContactVelocities(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	GYVector2 velDelta[2];
	float rotDelta[2];
	GYVector2 cv;
	unsigned j;

	if(_contactCount == 0) {
		_velResIter = 0;
		return;
	}

	unsigned iter;
	for(iter = 0; iter < ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS; ++iter) {
		float max = ELYSIAN_RIGID_BODY_CLOSING_VELOCITY_EPSILON;
		int maxIndex = _contactCount;

		for(j = 0; j < _contactCount; ++j) {

			//reevaluate penetration depth
			if(_contactPool[j]->deltaVel > max) {
				maxIndex = j;
				max = _contactPool[j]->deltaVel;
			}
		}

		if(maxIndex == _contactCount) {
			//printf("=======NONE FOUND! BREAKING EARLY!=======\n");
			break; //EPSILAAAAWN
		}

		gyContactResolveVelocity(_contactPool[maxIndex], velDelta, rotDelta);

		//========= STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======
		RigidBody* rbA = _contactPool[maxIndex]->getCollidable(0)->getEntity()->getRigidBody();
		RigidBody* rbB = (_contactPool[maxIndex]->getStaticAttrib(1))?
					_contactPool[maxIndex]->getCollidable(1)->getEntity()->getRigidBody() :
					nullptr;

		gyVec2AddAccum(&rbA->vel, &velDelta[0]);
		rbA->rot += rotDelta[0];

		if(rbB) {
			gyVec2AddAccum(&rbB->vel, &velDelta[1]);
			rbB->rot += rotDelta[1];
		}

	/*	Debug::printf(Debug::VERBOSE, "=====ITER[%d]=====", iter);
		Debug::printf(Debug::VERBOSE, "deltaVel = %f", max);
		Debug::printf(Debug::VERBOSE, "vel[0] = <%f, %f>, vel[1] = <%f, %f>", velDelta[0].x, velDelta[0].y, velDelta[1].x, velDelta[1].y);
		Debug::printf(Debug::VERBOSE, "rot[0] = %f, rot[1] = %f", rotDelta[0], rotDelta[1]);*/


		//========= / STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======

		//printf("velDelta[0] = <%f, %f> velDelta[1] = <%f, %f>\n", velDelta[0].x, velDelta[0].y, velDelta[1].x, velDelta[1].y);
		//printf("rotDelta[0] = %f, rotDelta[1] = %f\n", rotDelta[0], rotDelta[1]);

		//printf("closing vel - %f\n", _conPool[j]._closingVelocity);

		for(j = 0; j < _contactCount; ++j) {
			if(_contactPool[j]->getCollidable(0) == _contactPool[maxIndex]->getCollidable(0)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[0], rotDelta[0]);
				gyVec2AddAccum(&cv, &velDelta[0]);
				//printf("0 - 0, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel += gyVec2DotProd(&cv, &_contactPool[j]->normal);
				//printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				//gyroContactCalculateDeltaVelocity(j, deltaTime);
				//========= STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======
				RigidBody* rb1 = _contactPool[j]->getCollidable(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getStaticAttrib(1))?
							_contactPool[j]->getCollidable(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}
			else if(_contactPool[j]->getCollidable(0) == _contactPool[maxIndex]->getCollidable(1)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[0], rotDelta[1]);
				gyVec2AddAccum(&cv, &velDelta[1]);
				//printf("0 - 1, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel += gyVec2DotProd(&cv, &_contactPool[j]->normal);
			//	printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				RigidBody* rb1 = _contactPool[j]->getCollidable(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getStaticAttrib(1))?
							_contactPool[j]->getCollidable(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}

			//if(THERE IS EVEN A COLINDEX[1]) {
			if(_contactPool[j]->getCollidable(1) && _contactPool[j]->getCollidable(1) == _contactPool[maxIndex]->getCollidable(0)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[1], rotDelta[0]);
				gyVec2AddAccum(&cv, &velDelta[0]);
			//	printf("1 - 0, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel -= gyVec2DotProd(&cv, &_contactPool[j]->normal);
					//	printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				RigidBody* rb1 = _contactPool[j]->getCollidable(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getStaticAttrib(1))?
							_contactPool[j]->getCollidable(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}
			else if(_contactPool[j]->getCollidable(1) && _contactPool[j]->getCollidable(1) == _contactPool[maxIndex]->getCollidable(1)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[1], rotDelta[1]);
				gyVec2AddAccum(&cv, &velDelta[1]);
				//printf("1 - 1, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel -= gyVec2DotProd(&cv, &_contactPool[j]->normal);
						//printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				RigidBody* rb1 = _contactPool[j]->getCollidable(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getStaticAttrib(1))?
							_contactPool[j]->getCollidable(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}
		}
	}

	_velResIter = iter;

	if(iter == ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS) Debug::printf(Debug::CRITICAL, "OUT OF VELOCITY ITERATIONS!", iter);

	//Debug::printf(Debug::VERBOSE, "ITER - %d", iter);

}

void RigidBodySystem::integrate(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	GYVector2 worldPos;
	float damping;

	auto accumTileDamping = [&](RigidBody* rb, TileEntry entry) {
		if(entry.getElev() == rb->getEntity()->getElev()) {
			damping *= getCurLevel()->getTile(entry.getIndex())->damping;
		}
	};

	//update entity RigidBodies
	auto rigidBodies = getCurArea()->rigidBodies;
	for(auto it : rigidBodies) {
		damping = 1.0f;

		worldPos = it->getEntity()->getCollidable()->getWorldPos();
		for(unsigned i = 0; i < 3; ++i) accumTileDamping(it, getCurArea()->getTileEntry(i, worldPos.x/32.0f, worldPos.y/32.0f));

		it->setTileDamping(damping);
		it->integrate(deltaTime);
	}

}


}
