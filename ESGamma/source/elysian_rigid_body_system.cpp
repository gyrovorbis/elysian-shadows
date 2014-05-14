#include "elysian_rigid_body_system.hpp"
#include "elysian_area.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_force_generator.hpp"
#include "elysian_debug.hpp"
#include "elysian_level.hpp"
#include "elysian_particle_system.hpp"

namespace elysian {

RigidBodySystem::RigidBodySystem(void) {
	_gravityGenerator.setForceMask(0x1);
	addForce(&_gravityGenerator);
}

void RigidBodySystem::update(void) {
	_updateContacts();
}

void RigidBodySystem::applyForce(ForceGenerator*const force) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	auto rigidBodies = getCurArea()->rigidBodies;

	for(auto rb : rigidBodies) {
		if(force->getForceMask() & rb->getAttrib()->getForceMask()) {
			force->applyForce(rb, deltaTime);
		}
	}

	getPartSys()->applyForce(force);

}

void RigidBodySystem::applyForces(void) {

	for(auto force = _forces.begin(); force < _forces.end(); ++force) {
		applyForce(*force);
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

		if(maxIndex == (int)_contactCount) {
			//printf("=======NONE FOUND! BREAKING EARLY!=======\n");
			break; //EPSILAAAAWN
		}

		gyContactResolveVelocity(_contactPool[maxIndex], velDelta, rotDelta);

		//========= STOLEN FROM LIBGYRO VELOCITY RESOLUTION =======
		RigidBody* rbA = _contactPool[maxIndex]->getCollidable(0)->getEntity()->getRigidBody();
		RigidBody* rbB = (_contactPool[maxIndex]->getStaticAttrib(1))?
					_contactPool[maxIndex]->getCollidable(1)->getEntity()->getRigidBody() :
					nullptr;

		gyVec2AddAccum((GYVector2*)&rbA->vel, &velDelta[0]);
		rbA->rot += rotDelta[0];

		if(rbB) {
			gyVec2AddAccum((GYVector2*)&rbB->vel, &velDelta[1]);
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
	GYVector3 worldPos;
	float damping;

	auto accumTileDamping = [&](RigidBody* rb, TileEntry entry) {
		if(entry.getElev() == rb->getEntity()->getElev()) {
			damping *= getCurLevel()->getTile(entry.getIndex())->damping;
		}
	};

	//update entity RigidBodies
	auto rigidBodies = getCurArea()->rigidBodies;
	for(auto& it : rigidBodies) {
		damping = 1.0f;

		worldPos = it->getEntity()->getCollidable()->getWorldPos();
		for(unsigned i = 0; i < 3; ++i) accumTileDamping(it, getCurArea()->getTileEntry(i, (worldPos.x+16.0f)/32.0f, (worldPos.y+16.0f)/32.0f));

		it->setTileDamping(damping);
		it->integrate(deltaTime);
	}

}

void RigidBodySystem::integrate3D(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	CollisionSystem*const colSys = getCollisionSys();
	auto rigidBodies = getCurArea()->rigidBodies;

#if 0

	for(auto& it : rigidBodies) {
		auto* collidable = it->getEntity()->getCollidable();

		unsigned beforeCount = collidable->getContactCount();

		if(collidable->getEntity()->transform._pos.z < 0.0f) {
			GYVector3 vel = it->getVelocity();
			vel.z = 0.0f;
			//vel.y = 0.0f;
			it->setVelocity(vel);
			collidable->getEntity()->transform._pos.z = 0.0f;
		} else {

			GYVector3 vel = it->getVelocity();
			if(vel.z > 0.0f) continue;

			colSys->_detectEntityTileCollisionCoarse(*it->getEntity()->getCollidable(), CollisionSystem::ENTITY_TILE_CHECK_BOTTOM);
			//We are standing ontop of a solid
			if(beforeCount != collidable->getContactCount()) {
				found = true;

				//it->getAttrib()->setForceMask(0);
				//move back up by normal
				float normal =
						(int)(collidable->getEntity()->transform._pos.z+16.0f)/32*32 - collidable->getEntity()->transform._pos.z;

					collidable->getEntity()->transform._pos.z += normal;
					GYVector3 vel = it->getVelocity();
					vel.z = 0.0f;
					it->setVelocity(vel);
					Debug::printf(Debug::CRITICAL, "GOT IT");
			//	} else {
					//Debug::printf(Debug::CRITICAL, "TOUGH SHIT - %f", normal);
			//	}
			}
			//else it->getAttrib()->setForceMask(1);

		}



	}

#else


	for(auto& it : rigidBodies) {

		if(!it->_active) continue;
		unsigned i;
		TileEntry entry;
		auto* collidable = it->getEntity()->getCollidable();

		GYVector3 vel = it->getVelocity();
		if(vel.z > 0.0f) continue;

/*
		if(collidable->getEntity()->transform._pos.z <= 0.0f) {
			GYVector3 vel = it->getVelocity();
			vel.z = 0.0f;
			//vel.y = 0.0f;
			it->setVelocity(vel);
			collidable->getEntity()->transform._pos.z = 0.0f;
			return;
		}*/


	//	for(unsigned i = 0; i < 4; ++i) {
		//if(found) break;
/*
		GYVector3 worldPos = {
			it->getEntity()->getCollidable()->_collider.box.worldVert[i].x,
			it->getEntity()->getCollidable()->_collider.box.worldVert[i].y,
			it->getEntity()->getCollidable()->_collider.box.worldVert[3].z
		};*/
		GYVector3 worldPos = it->getEntity()->getCollidable()->getCenterWorldPos();
		float zTop = 0.0f;

		const unsigned elev = /*(worldPos.z == 0.0f)? 0 :*/ (worldPos.z)/32;
		worldPos.y += 32.0f; /*it->getEntity()->getCollidable()->getAttrib()->getRelSize().y*
				it->getEntity()->transform._size.y * it->getEntity()->transform._scale.y;*/


			for(i = 0; i < 3; ++i) {
				for(unsigned j = 1; j < 2; ++j) {
					//if(found) break;

				entry = getCurArea()->getTileEntry(i, (worldPos.x+16.0f)/32, (worldPos.y+16.0f)/32/*+elev*/-j/*-1*/);


				//if(!(j == 0 && entry.getNormal()) && !(j == 0 && !entry.getNormal())) {


					if(getCurLevel()->getTile(entry.getIndex())->standOn) {


						float newTop = (entry.getElev()*32 + (entry.getNormal()? 32.0f : 0.0f) );


						if(newTop > zTop && fabs(newTop-worldPos.z) <= 16.0f) {
							zTop = newTop;


						}
					}
				//}
				}

			}

		//}



		//if(found) {
				//it->getAttrib()->setForceMask(0);
			if(collidable->getEntity()->transform._pos.z <= zTop) {
				collidable->getEntity()->transform._pos.z = zTop;
				GYVector3 vel = it->getVelocity();
				vel.z = 0.0f;
				it->setVelocity(vel);
				//Debug::printf(Debug::CRITICAL, "Resting");
				collidable->getEntity()->getCollidable()->update();
				it->setOnGround(true);

				//collidable->setResolvePenetration(false);
				//it->_restingOnTile = true;
		//	}

		} else {
			//Debug::printf(Debug::CRITICAL, "Falling");
			it->getAttrib()->setForceMask(1);
			it->setOnGround(false);
			//collidable->setResolvePenetration(true);
		//	it->_restingOnTile = false;

		}
	}
#endif


}


}
