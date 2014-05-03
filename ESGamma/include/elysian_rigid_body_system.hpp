#ifndef ELYSIAN_RIGID_BODY_SYSTEM_HPP
#define ELYSIAN_RIGID_BODY_SYSTEM_HPP

#define ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE		100
#define ELYSIAN_RIGID_BODY_FORCE_GENERATOR_MAX_COUNT	20
#define ELYSIAN_RIGID_BODY_CLOSING_VELOCITY_EPSILON		1.0f

#include "elysian_system.hpp"
#include "elysian_mem_pool.hpp"

namespace elysian {

	class CollisionContact;
	class ForceGenerator;

	class RigidBodySystem: public System {
		CollisionContact*		_contactPool[ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE];
		Pool<ForceGenerator*>	_forces			= Pool<ForceGenerator*>(ELYSIAN_RIGID_BODY_FORCE_GENERATOR_MAX_COUNT);
		unsigned				_contactCount	= 0;
		unsigned				_velResIter		= 0;

		void _updateContacts(void);

	public:
		void update(void);
		void integrate3D(void);

		unsigned getVelResIterUsed(void) const;

		void resolveContactVelocities(void);
		void integrate(void);
		void applyForces(void);

		void addForce(ForceGenerator*const force);
		void applyForce(ForceGenerator*const force);
		void removeForce(ForceGenerator*const force);
		void clearForces(void);

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



}

#endif // ELYSIAN_RIGID_BODY_SYSTEM_HPP
