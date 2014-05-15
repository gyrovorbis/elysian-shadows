#ifndef ELYSIAN_RIGID_BODY_SYSTEM_HPP
#define ELYSIAN_RIGID_BODY_SYSTEM_HPP

#define ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE		100
#define ELYSIAN_RIGID_BODY_FORCE_GENERATOR_MAX_COUNT	20
#define ELYSIAN_RIGID_BODY_CLOSING_VELOCITY_EPSILON		1.0f

#define ELYSIAN_RIGID_BODY_BREAKABLE_IMPULSE_VELOCITY_MIN 20.0f
#define ELYSIAN_RIGID_BODY_BREAKABLE_IMPULSE_ROTATION_MIN 0.4f

#include "elysian_system.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_force_generator.hpp"

#define ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE	50

namespace elysian {

	class RigidBody;
	class CollisionContact;
	class ForceGenerator;

	class RigidBodySystem: public System {
		CollisionContact*		_contactPool[ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE];
		Pool<Entity>			_shardPool		= Pool<Entity>(ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE);
		Pool<ForceGenerator*>	_forces			= Pool<ForceGenerator*>(ELYSIAN_RIGID_BODY_FORCE_GENERATOR_MAX_COUNT);
		GravityForceGenerator	_gravityGenerator;
		unsigned				_contactCount	= 0;
		unsigned				_velResIter		= 0;
		bool					_gravityEnabled = true;

		void _updateContacts(void);

	public:
				RigidBodySystem(void);
		void update(void);
		void integrate3D(void);

		unsigned getVelResIterUsed(void) const;

		void resolveContactVelocities(void);
		void breakRigidBodies(void);
		void integrate(void);
		void applyForces(void);

		void breakRigidBody(RigidBody*const rigidBody);
		void addForce(ForceGenerator*const force);
		void applyForce(ForceGenerator*const force);
		void removeForce(ForceGenerator*const force);
		void clearForces(void);

		void setGravityEnabled(const bool val);
		void setGravityAccel(const GYVector3& accel);

	};


	//====== INLINEZ =======
	inline void RigidBodySystem::addForce(ForceGenerator * const force) {
		_forces.insertEntry(force);
	}
	inline void RigidBodySystem::removeForce(ForceGenerator * const force) {
		_forces.removeEntry(force);
	}
	inline void RigidBodySystem::clearForces(void) {
		_forces.clear();
	}
	inline unsigned RigidBodySystem::getVelResIterUsed(void) const {
		return _velResIter;
	}

	inline void RigidBodySystem::setGravityEnabled(const bool val) {
		if(_gravityEnabled == val) return;

		if(val) {
			addForce(&_gravityGenerator);
		} else {
			removeForce(&_gravityGenerator);
		}
		_gravityEnabled = val;
	}

	inline void RigidBodySystem::setGravityAccel(const GYVector3 &accel) {
		_gravityGenerator.setAccel(accel);
	}

}

#endif // ELYSIAN_RIGID_BODY_SYSTEM_HPP
