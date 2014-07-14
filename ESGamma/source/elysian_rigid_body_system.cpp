#include <cassert>
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
#include "elysian_renderable.hpp"
#include "elysian_behavior_system.hpp"

namespace elysian {

RigidBodySystem::RigidBodySystem(void) {
	_gravityGenerator.setForceMask(0x1);
	addForce(&_gravityGenerator);

    for(unsigned i = 0; i < ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE; ++i) {
        _shardPool[i].setIterator(ShardPool::iterator(&_shardPool, i));
    }
}

void RigidBodySystem::update(void) {
    const float deltaTime = getTimerSys()->getDeltaTime();
    auto &rigidBodies = getCurArea()->rigidBodies;

	_updateContacts();

    int toFree = ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE -_shardPool.size();
    toFree = (toFree >= ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_IDEAL_FREE_COUNT)? 0 :
                ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_IDEAL_FREE_COUNT - toFree;

    //update shards
    for(auto* shard = _shardHead; shard != nullptr; shard = shard->getNext()) {
        //remove shards whose time has run out

        switch(shard->update(deltaTime)) {
        case RigidBodyShard::STATE_DONE:
            _removeShard(_shardHead);
        case RigidBodyShard::STATE_FADE_OUT:
            --toFree;
            break;
        default:
            if(toFree > 0) {
                shard->startFade(deltaTime);
                --toFree;
            }
            break;
        }
    }
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

void RigidBodySystem::_resolveContactZVelocities(void) {
    const float deltaTime = getTimerSys()->getDeltaTime();

    for(unsigned i = 0; i < _contactCount; ++i) {

        if(_contactPool[i]->getResolveZ()) {
            Collidable* colTop;
            Collidable* colBot;

            float totalImpulse = 0.0f;
            bool topResolved = false;
            bool botResolved = false;

            float zDepth = _contactPool[i]->getZDepth();

            if(_contactPool[i]->getZDepth() > 0.0f) {
                colTop = _contactPool[i]->_bodyB;
                colBot = _contactPool[i]->_bodyA;
            } else {
                colTop = _contactPool[i]->_bodyA;
                colBot = _contactPool[i]->_bodyB;
            }

            if(colTop) {
                GYVector3 prevVel = colTop->getEntity()->getRigidBody()->getVelocity();
                GYVector3 vel = prevVel;

                //falling down
                if(vel.z < 0.0f) {
                    vel.z *= -1.0f * colTop->getAttrib()->getStaticAttrib()->zCoeffOfRest;
                    if(colBot) vel.z *= colBot->getAttrib()->getStaticAttrib()->zCoeffOfRest;

                    //Worst-cast, 30fps escape vel, slow as shit!
                    //const float escapeVelocity = zDepth/(1.0f/30.0f)/powf(col->getAttrib()->getStaticAttrib()->zDamping, (1.0f/30.0f));

                    //Resting contact logic
                    if(fabs(vel.z) < ELYSIAN_RIGID_BODY_RESTING_CONTACT_ESCAPE_VELOCITY) {
                        colTop->getEntity()->getRigidBody()->accel.z = 0.0f;
                        vel.z = 0.0f;

                        colTop->setRestingOnBody(true);

/*                        //Do NOT push shit out of contact! Push it to the edge, and let it chill!
                        if(zDepth > 0.0f) {
                            zDepth -= ELYSIAN_COLLISION_CONTACT_Z_DEPTH_EPSILON;
                        } else {
                            zDepth += ELYSIAN_COLLISION_CONTACT_Z_DEPTH_EPSILON;
                        }
                        colTop->getEntity()->transform._pos.z += fabs(zDepth);
                        colTop->getEntity()->transform._pos.y -= fabs(zDepth);*/
                    } else {
                        totalImpulse += fabs(prevVel.z - vel.z);
                        topResolved = true;
                    }

                    colTop->getEntity()->getRigidBody()->setVelocity(vel);
                   // if(col2 && !col2->getEntity()->getRigidBody()->getOnGround()) apply downward velocity to neighbor underneath
#if 0
                    //Check zImpulse for shattering top body
                    if(colTop->getEntity()->getRigidBody()->getAttrib()->isBreakable()) {
                        const float zImpulse = fabs(vel.z * colTop->getAttrib()->getStaticAttrib()->damping.z);
                        auto* rb = colTop->getEntity()->getRigidBody();
                        if(zImpulse >= rb->getDurability()) {
                            rb->setDurability(rb->getDurability() - zImpulse);
                            rb->setBreakContact(_contactPool[i]);
                        }
                    }
                    //Check zImpulse for shattering bottom body
                    if(colBot) {
                        if(colBot->getEntity()->getRigidBody()->getAttrib()->isBreakable()) {
                            const float zImpulse = fabs(vel.z * colBot->getAttrib()->getStaticAttrib()->damping.z);
                            auto* rb = colBot->getEntity()->getRigidBody();
                            if(zImpulse >= rb->getDurability()) {
                                rb->setDurability(rb->getDurability() - zImpulse);
                                rb->setBreakContact(_contactPool[i]);
                                GYVector3 vel2 = rb->getVelocity();
                                vel2.z += vel.z;
                                rb->setVelocity(vel2);
                            }
                        }
                    }
#endif

                //resting on body below us
                } /*else if(vel.z == 0.0f) {
                    colTop->setRestingOnBody(true);

                    //Inherit velocity from entity below you. RIDE THAT BITCH!
                    if(colBot) {
                        GYVector3 vel = colTop->getEntity()->getRigidBody()->getVelocity();
                        GYVector3 inheritedVel = colBot->getEntity()->getRigidBody()->getVelocity();
                        gyVec2ScaleAccum((GYVector2*)&inheritedVel, 1.0f - colTop->getAttrib()->getStaticAttrib()->damping.z);
                        gyVec2AddAccum((GYVector2*)&vel, (GYVector2*)&inheritedVel);
                        //col->getEntity()->getRigidBody()->setVelocity(vel);
                    }
                }*/
             }
#if 0
            if(colBot) {
                GYVector3 vel = colBot->getEntity()->getRigidBody()->getVelocity();
                GYVector3 prevVel = vel;

                //moving upwards
                if(vel.z > 0.0f) {
                    vel.z *= -1.0f * colBot->getAttrib()->getStaticAttrib()->zCoeffOfRest;
                    if(colTop) vel.z *= colTop->getAttrib()->getStaticAttrib()->zCoeffOfRest;

                /*    colBot->getEntity()->getRigidBody()->setVelocity(vel);
                    totalImpulse += fabs(vel.z - prevVel.z);

                    //Move out of penetration (completely)
                    colBot->getEntity()->transform._pos.z -= zDepth;
                    colBot->getEntity()->transform._pos.y += zDepth;*/

                    botResolved = true;

                }
            }

            //Check for shattering of top body
            if(colTop && colTop->getEntity()->getRigidBody()->getAttrib()->isBreakable()) {
                auto* rb = colTop->getEntity()->getRigidBody();
                const float colTopImpulse = totalImpulse;// * colTop->getAttrib()->getStaticAttrib()->damping.z;
                if(colTopImpulse >= rb->getDurability()) {
                    rb->setDurability(rb->getDurability() - colTopImpulse);
                    rb->setBreakContact(_contactPool[i]);

                    //If we are shattering from the bottom, compensate for not receiving a velocity change

                    if(botResolved) {
                        rb->setBreakContact(_contactPool[i]);
                        GYVector3 vel = rb->getVelocity();
                        vel.z += colTopImpulse;//colBot->getEntity()->getRigidBody()->getVelocity().z;
                        rb->setVelocity(vel);
                    }
                }
            }

            //Check for shattering of bottom body
            if(colBot && colBot->getEntity()->getRigidBody()->getAttrib()->isBreakable()) {
                auto* rb = colBot->getEntity()->getRigidBody();
                const float colBotImpulse = totalImpulse;// * colBot->getAttrib()->getStaticAttrib()->damping.z;
                if(colBotImpulse >= rb->getDurability()) {
                    rb->setDurability(rb->getDurability() - colBotImpulse);

                    //If we are shattering from the top, compensate for not receiving a velocity change
                    if(topResolved) {
                        rb->setBreakContact(_contactPool[i]);
                        GYVector3 vel = rb->getVelocity();
                        vel.z += colBotImpulse;//colTop->getEntity()->getRigidBody()->getVelocity().z;
                        rb->setVelocity(vel);
                    }
                }
            }
                    #endif

            //_contactPool[i]->deltaVel = 0.0f;
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

   // _resolveContactZVelocities();

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

        //float dampingA = powf(rbA->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->damping, deltaTime);
        //float dampingB = (rbB)? powf(rbB->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->damping, deltaTime) : 1.0f;

		gyVec2AddAccum((GYVector2*)&rbA->vel, &velDelta[0]);
		rbA->rot += rotDelta[0];

        GYVector2 dampedVel = velDelta[0];

        if(rbB) gyVec2CompProdAccum(&dampedVel, (GYVector2*)&rbB->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->damping);
        float mag = gyVec2Magnitude(&dampedVel);
		if(mag >= rbA->getDurability()) {
			rbA->setDurability(rbA->getDurability() - mag);
			rbA->setBreakContact(_contactPool[maxIndex]);
		}

		if(rbB) {
            dampedVel = velDelta[1];
			gyVec2AddAccum((GYVector2*)&rbB->vel, &velDelta[1]);
			rbB->rot += rotDelta[1];

            gyVec2CompProdAccum(&dampedVel, (GYVector2*)&rbA->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->damping);
            float mag = gyVec2Magnitude(&velDelta[1]);
			if(mag >= rbB->getDurability()) {
				rbB->setDurability(rbB->getDurability() - mag);
				rbB->setBreakContact(_contactPool[maxIndex]);
			}
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

    //if(iter == ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS) Debug::printf(Debug::CRITICAL, "OUT OF VELOCITY ITERATIONS!", iter);

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

bool RigidBodySystem::_checkEntityEntity3D(const unsigned i) {
    RigidBody* rbA = _curArea->rigidBodies[i];
/*
    for(unsigned i = 0; i < rbA->getEntity()->getCollidable()->getContactCount(); ++i) {
        Collidable* colA =

    }*/

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
        if(vel.z > 0.0f/* || it->getOnGround()*/) {
            collidable->setStackedOnGround(false);
            collidable->setRestingOnBody(false);
            continue;
        }
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
        float zOffset = worldPos.z - elev*32.0f;
        worldPos.y += 32.0f;//zOffset;
        //worldPos.y -= (worldPos.z);

        /*it->getEntity()->getCollidable()->getAttrib()->getRelSize().y*
				it->getEntity()->transform._size.y * it->getEntity()->transform._scale.y;*/


			for(i = 0; i < 3; ++i) {
                for(unsigned j = 0; j < 1; ++j) {
					//if(found) break;

                entry = getCurArea()->getTileEntry(i, (worldPos.x+16.0f)/32, (worldPos.y)/32-elev-1);


				//if(!(j == 0 && entry.getNormal()) && !(j == 0 && !entry.getNormal())) {


					if(getCurLevel()->getTile(entry.getIndex())->standOn) {

                        if(entry.getNormal() && ((int)worldPos.y+16)/32 > ((int)worldPos.y/32)) continue; //bottom half of a normal, standon


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
               float zDiff = zTop - collidable->getEntity()->transform._pos.z;
				collidable->getEntity()->transform._pos.z = zTop;

                //We have to remove the same amount of yVelocity we're removing from the Z in this perspective.
               // collidable->getEntity()->transform._pos.y -= zDiff;

				GYVector3 vel = it->getVelocity();
                vel.z *= -1.0f * collidable->getAttrib()->getStaticAttrib()->zCoeffOfRest;

                //if(vel.z <= 0.0f && !it->getOnGround()) vel.y += vel.z;

                if(fabs(vel.z) < ELYSIAN_RIGID_BODY_RESTING_CONTACT_ESCAPE_VELOCITY) {

                    vel.z = 0.0f;

                    //Debug::printf(Debug::CRITICAL, "Resting");
                    collidable->getEntity()->getCollidable()->update();
                    collidable->setStackedOnGround(true);
                    collidable->setRestingOnBody(true);

                }

                it->setVelocity(vel);

                //Check zImpulse for shattering
                const float zImpulse = fabs(vel.z * collidable->getAttrib()->getStaticAttrib()->damping.z);
                auto* rb = collidable->getEntity()->getRigidBody();
                if(zImpulse >= rb->getDurability()) {
                    it->setDurability(it->getDurability() - zImpulse);
                    it->setBreakContact(nullptr);
                }
             //   it->getAttrib()->setForceMask(0);

				//collidable->setResolvePenetration(false);
				//it->_restingOnTile = true;
		//	}

        } else { // if(!it->getOnGround()){
			//Debug::printf(Debug::CRITICAL, "Falling");
        //	it->getAttrib()->setForceMask(1);
            collidable->setStackedOnGround(false);
            collidable->setRestingOnBody(false);
			//collidable->setResolvePenetration(true);
		//	it->_restingOnTile = false;

		}
	}
#endif


}

void RigidBodySystem::breakRigidBodies(void) {
	std::vector<RigidBody*> toDelete;

	auto rigidBodies = getCurArea()->rigidBodies;
	for(auto rb : rigidBodies) {
		if(rb->getAttrib()->isBreakable() && rb->getDurability() <= 0.0f) {
			toDelete.push_back(rb);
		}
	}

	for(unsigned i = 0; i < toDelete.size(); ++i) {
		for(auto rb : rigidBodies) {
			if(rb == toDelete[i]) {
                getBehavSys()->onShatter(rb->getEntity(), rb->getBreakContact());
				breakRigidBody(rb);
			}
		}
	}

}

void RigidBodySystem::breakRigidBody(RigidBody * const rigidBody) {

	float width = rigidBody->getEntity()->getTransform()._size.x * rigidBody->getEntity()->getTransform()._scale.x;
	float height = rigidBody->getEntity()->getTransform()._size.y * rigidBody->getEntity()->getTransform()._scale.y;

	int xSize = width / 16;
	int ySize = height / 16;

	float xinc = width/(float)xSize;
	float yinc = height/(float)ySize;

	unsigned row = 0;
	unsigned col = 0;


    //CollisionContact* contact = rigidBody->getBreakContact();
    //const bool bodyA = (!contact || contact->_bodyA == rigidBody->getEntity()->getCollidable())? true : false;

	auto _createAndAddShard = [&](Entity*const entity, const float x, const float y, const float xinc, const float yinc) {
        Entity* shard = _claimShard();

		shard->transform._pos.x = entity->transform._pos.x - 0.5f*width + x + 0.5f*xinc;
		shard->transform._pos.y = entity->transform._pos.y - 0.5f*height + y + 0.5f*yinc;
        shard->transform._pos.z = entity->transform._pos.z + (ySize*yinc-y);
        //shard->setPlaneType((GY_RECT_TEX_PLANE)(rand()%2==1));
		/*if(entity->getNormalType() == NORMAL_FRONT) {
			shard->transform._pos.y = entity->transform._pos.y - 0.5f*entity->transform._size.y; + y;
			//shard->transform._pos.z = (ySize-row)*16.0f;
		} else {
			shard->transform._pos.y = entity->transform._pos.y - 0.5f*entity->transform._size.y + y;
			shard->transform._pos.z = entity->transform._pos.z;

		}*/


        shard->transform.setSize(xinc, yinc, 9.0f);

        if(!shard->getRenderable()) {
            auto* renderable = shard->addRenderable();
            renderable->setSpriteAttrib(new SpriteAttrib);
        }
		auto* attrib = entity->getRenderable()->getSpriteAttrib();
        *shard->getRenderable()->getSpriteAttrib() = *attrib;

		GYTexCoordFrame* coord = &attrib->getTexture()->_coord->texCoord[entity->getRenderable()->getFrame()];

		float frameWidth = (coord->botRight.u - coord->topLeft.u)/xSize;
		float frameHeight = (coord->botRight.v - coord->topLeft.v)/ySize;

		float baseX = coord->topLeft.u+col*frameWidth;
		float baseY = coord->topLeft.v+row*frameHeight;

		GYTexCoord2 shardCoord[4] = {
			{baseX, baseY},
			{baseX+frameWidth, baseY},
			{baseX, baseY+frameHeight},
			{baseX+frameWidth, baseY+frameHeight }
		};

		for(unsigned i = 0; i < 4; ++i) shard->getRenderable()->setTexCoord(i, shardCoord[i].u, shardCoord[i].v);

//		float u1, v1, u2, v2,
//s		attrib

		if(!shard->getCollidable()) {
			shard->addCollidable();
			shard->getCollidable()->setAttrib(new CollidableAttrib());
		}
		*shard->getCollidable()->getAttrib() = *entity->getCollidable()->getAttrib();
		shard->getCollidable()->getAttrib()->getFlags()->addCheckAgainst(CollisionFlags::ALL);
		shard->getCollidable()->getAttrib()->getFlags()->addType(CollisionFlags::ALL);

		if(!shard->getRigidBody()) {
			shard->addRigidBody();
			shard->getRigidBody()->setAttrib(new RigidBodyAttrib);
		}

		*shard->getRigidBody()->getAttrib() = *entity->getRigidBody()->getAttrib();
		shard->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass *= ySize*xSize;
        shard->getCollidable()->getAttrib()->getStaticAttrib()->invMomOfInertia = 1/5000000.0f * 16.0f;
        shard->getCollidable()->getAttrib()->getStaticAttrib()->angDamping = 0.1f;
        gyVec2ScaleAccum((GYVector2*)&shard->getCollidable()->getAttrib()->getStaticAttrib()->damping, 1.0f/(ySize * xSize));
        shard->getCollidable()->getAttrib()->getStaticAttrib()->damping.z = 0.5f;
        shard->getCollidable()->getAttrib()->getStaticAttrib()->zCoeffOfRest = 0.0f;

        shard->getCollidable()->setRestingOnBody(false);
        shard->getCollidable()->setStackedOnGround(false);
		shard->getRigidBody()->getAttrib()->setBreakable(false);
        shard->getRigidBody()->getAttrib()->setCanHold(false);
        shard->getRigidBody()->clearKinetic();

		GYVector3 velDir = entity->getRigidBody()->getVelocity();
       //gyVec3ScaleAccum(&velDir, -1.0f);
        //Z AXIS STAYS NEGATIVE!!!!
        //if(bodyA) gyVec2ScaleAccum((GYVector2*)&velDir, -1.0f);
		//gyVec3Normalize(&velDir);
		//float mag = gyVec3Magnitude(&entity->getRigidBody()->getVelocity());
		//gyVec3Scale(&velDir, &velDir, xSize*ySize*mag);


        auto getContactNormal = [&]() {
            auto* contact = entity->getRigidBody()->getBreakContact();
            if(contact) {
                GYVector3 normal = { 0.0f, 0.0f, 0.0f };
                contact->getNormal(*(GYVector2*)&normal);

                if(contact->_bodyB == entity->getCollidable()) {
                    gyVec3ScaleAccum(&normal, -1.0f);
                }

                if(contact->_zDepth <= ELYSIAN_COLLISION_CONTACT_Z_DEPTH_MAX) {
                    gyVec2ScaleAccum((GYVector2*)&normal, contact->depth);
                    normal.z = contact->_zDepth;
                    if(contact->_bodyB == entity->getCollidable()) normal.z *= -1.0f;
                    gyVec3Normalize(&normal);
                }


                return normal;
            } else {
                GYVector3 vel = entity->getRigidBody()->getVelocity();
                gyVec3Normalize(&vel);
                gyVec2ScaleAccum((GYVector2*)&vel, -1.0f);
                return vel;
            }
        };

       // GYVector3 normal = getContactNormal();

#if 1
        GYVector3 localPos;
        gyVec3Sub(&localPos, &shard->transform._pos, &entity->transform._pos);
     //   gyVec3ScaleAccum(&localPos, -1.0f);
        //gyVec3Sub(&localPos, &entity->transform._pos, &shard->transform._pos);
       //gyVec3Normalize(&velDir);
        //float crossProd = gyVec3DotProd(&localPos, &velDir);

        float maxDist = powf(entity->transform._size.x*entity->transform._scale.x*0.5f, 2.0f);
        maxDist += powf(entity->transform._size.y*entity->transform._scale.y*0.5f, 2.0f);
        float maxDist3 = maxDist + powf(entity->transform._size.y*entity->transform._scale.y*0.5f, 2.0f);
       // maxDist += powf(entity->transform._size.y*entity->transform._scale.y, 2.0f); //z axis
        maxDist = gySqrt(maxDist);
        maxDist3 = gySqrt(maxDist3);

        float posMag = gyVec2Magnitude((GYVector2*)&localPos);
        float posMag3 = gyVec3Magnitude(&localPos);
        //
       float velMag = gyVec3Magnitude(&velDir);

        GYVector3 normal = getContactNormal();

        GYVector3 localPosDir = localPos;

        gyVec3Normalize(&localPosDir);

        float rot = gyVec2ScalarCrossProd((GYVector2*)&localPosDir, (GYVector2*)&normal);
        shard->getRigidBody()->setRotation(-rot*(posMag/maxDist)*velMag);

        float baseDurability = entity->getRigidBody()->getAttrib()->getBaseDurability();
        float durability = entity->getRigidBody()->getDurability();




        //gyVec3ScaleAccum(&localPosDir, -1.0f);

        GYVector3 velDirNorm = velDir;
        float velmag = gyVec3MagAndNormalize(&velDirNorm);


        float totalMag = /*(posMag3/maxDist3) */  gyVec3DotProd((GYVector3*)&normal, (GYVector3*)&localPosDir) * velmag; /* gyVec3DotProd(&localPosDir, &normal);*/  /* gyVec3DotProd(&velDirNorm, &normal) */ /* gyVec3DotProd(&localPosDir, &velDirNorm) * */
               // gyVec3CrossProd((GYVector2*)&localPos, (GYVector2*)&normal);//((baseDurability-durability)/baseDurability);
       // totalMag /= ySize*xSize;
        //float velMag = gyVec3MagAndNormalize(&velDir);


        //gyVec3Scale(&velDir, &localPos, crossProd);

     //   gyVec3AddAccum(&velDir, &normal);
       // gyVec3AddAccum(&velDir, &localPosDir);
       // gyVec3ScaleAccum(&velDir, totalMag);

        //gyVec3CrossProd(&localPosDir, &localPosDir, &normal);

        gyVec3ScaleAccum(&localPosDir, totalMag);

        shard->getRigidBody()->setVelocity(localPosDir);

#else

        GYVector3 localPos;
        gyVec3Sub(&localPos, &shard->transform._pos, &entity->transform._pos);
        //gyVec3Sub(&localPos, &entity->transform._pos, &shard->transform._pos);
        float mag = gyVec3Magnitude(&localPos);
        //gyVec3Normalize(&localPos);
//        float crossProd = gyVec3DotProd(&localPos, &velDir);



        //gyVec3ScaleAccum(&velDir, -1.0f/(ySize*xSize));
        gyVec3AddScaled(&velDir, &localPos, 1.0f/mag);

        shard->getRigidBody()->setVelocity(velDir);

#endif

	};

	//float invMass = rigidBody->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass *= ySize*xSize;
	//rigidBody->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->invMomOfInertia = 1.0f / (((1.0f/invMass)*(yinc*yinc+xinc*xinc)) /12.0f);

	for(float y = 0.0f; y < height; y += yinc) {
		for(float x = 0.0f; x < width; x += xinc) {
			_createAndAddShard(rigidBody->getEntity(), x, y, xinc, yinc);
			++col;
		}
		col = 0;
		++row;

	}

	//rigidBody->getEntity()->transform._pos.x += 1000;
	getCurArea()->removeEntity(rigidBody->getEntity());


}


}
