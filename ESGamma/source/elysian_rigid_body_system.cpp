#include "elysian_rigid_body_system.hpp"
#include "elysian_area.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_entity.hpp"

namespace elysian {

void RigidBodySystem::update(void) {

}

void RigidBodySystem::_updateContacts(void) {
	CollisionSystem* colSys = getCollisionSys();
	const unsigned contactCount = colSys->getContactCount();
	CollisionContact* curContact;

	_contactCount = 0;

	//figure out how many of these contacts are rigidBodies
	//that even give a shit about velocity resolution
	for(unsigned i = 0; i < contactCount; ++i) {
		curContact = colSys->getContact(i);

		if(curContact->getBody(0)->getEntity()->getRigidBody()) {
			_contactPool[_contactCount++] = curContact;
			assert(_contactCount <= ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE);
		}
	}

}

void RigidBodySystem::resolveContactVelocities(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	float deepest = -1.0f;
	int deepestIndex = _contactCount;
	GYVector2 velDelta[2];
	float rotDelta[2];
	GYVector2 cv;
	unsigned j;

	if(_contactCount == 0) return;

	for(unsigned i = 0; i < ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS; ++i) {

		for(j = 0; j < _contactCount; ++j) {

			//reevaluate penetration depth
			if(_contactPool[j]->closingVel < deepest) {
				deepestIndex = j;
				deepest = _contactPool[j]->closingVel;
			}
		}

		if(deepest >= -0.01f || deepestIndex == _contactCount) {
			//printf("=======NONE FOUND! BREAKING EARLY!=======\n");
			break; //EPSILAAAAWN
		}

		gyContactResolveVelocity(_contactPool[deepestIndex], velDelta, rotDelta);

		//========= STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======
		RigidBody* rbA = _contactPool[deepestIndex]->getBody(0)->getEntity()->getRigidBody();
		RigidBody* rbB = (_contactPool[deepestIndex]->getBody(1)->getEntity())?
					_contactPool[deepestIndex]->getBody(1)->getEntity()->getRigidBody() :
					nullptr;

		gyVec2AddAccum(&rbA->vel, &velDelta[0]);
		rbA->rot += rotDelta[0];

		if(rbB) {
			gyVec2AddAccum(&rbB->vel, &velDelta[1]);
			rbB->rot += rotDelta[1];
		}


		//========= / STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======

		//printf("velDelta[0] = <%f, %f> velDelta[1] = <%f, %f>\n", velDelta[0].x, velDelta[0].y, velDelta[1].x, velDelta[1].y);
		//printf("rotDelta[0] = %f, rotDelta[1] = %f\n", rotDelta[0], rotDelta[1]);

		//printf("closing vel - %f\n", _conPool[j]._closingVelocity);

		for(j = 0; j < _contactCount; ++j) {
			if(_contactPool[j]->getBody(0) == _contactPool[deepestIndex]->getBody(0)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[0], rotDelta[0]);
				gyVec2AddAccum(&cv, &velDelta[0]);
				//printf("0 - 0, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel += gyVec2DotProd(&cv, &_contactPool[j]->normal);
				//printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				//gyroContactCalculateDeltaVelocity(j, deltaTime);
				//========= STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======
				RigidBody* rb1 = _contactPool[j]->getBody(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getBody(1)->getEntity())?
							_contactPool[j]->getBody(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}
			else if(_contactPool[j]->getBody(0) == _contactPool[deepestIndex]->getBody(1)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[0], rotDelta[1]);
				gyVec2AddAccum(&cv, &velDelta[1]);
				//printf("0 - 1, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel += gyVec2DotProd(&cv, &_contactPool[j]->normal);
			//	printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				RigidBody* rb1 = _contactPool[j]->getBody(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getBody(1)->getEntity())?
							_contactPool[j]->getBody(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}

			//if(THERE IS EVEN A COLINDEX[1]) {
			if(_contactPool[j]->getBody(1) == _contactPool[deepestIndex]->getBody(0)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[1], rotDelta[0]);
				gyVec2AddAccum(&cv, &velDelta[0]);
			//	printf("1 - 0, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel -= gyVec2DotProd(&cv, &_contactPool[j]->normal);
					//	printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				RigidBody* rb1 = _contactPool[j]->getBody(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getBody(1)->getEntity())?
							_contactPool[j]->getBody(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}
			else if(_contactPool[j]->getBody(1) == _contactPool[deepestIndex]->getBody(1)) {

				gyVec2VectorCrossProd(&cv, &_contactPool[j]->objPos[1], rotDelta[1]);
				gyVec2AddAccum(&cv, &velDelta[1]);
				//printf("1 - 1, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j]->closingVel -= gyVec2DotProd(&cv, &_contactPool[j]->normal);
						//printf("closing vel - %f\n", _conPool[j]._closingVelocity);
				RigidBody* rb1 = _contactPool[j]->getBody(0)->getEntity()->getRigidBody();
				RigidBody* rb2 = (_contactPool[j]->getBody(1)->getEntity())?
							_contactPool[j]->getBody(1)->getEntity()->getRigidBody() :
							nullptr;
				gyContactUpdateDeltaVelocity(_contactPool[j], rb1, rb2, deltaTime);
			}
		}
		deepestIndex = 0;
		deepest = 0.0f;
	}

}

void RigidBodySystem::integrate(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();

	//update entity RigidBodies
	auto rigidBodies = getCurArea()->rigidBodies;
	for(auto it : rigidBodies) {
		it->integrate(deltaTime);
	}

}


}
