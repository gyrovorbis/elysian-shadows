#ifndef ELYSIAN_RIGID_BODY_SYSTEM_HPP
#define ELYSIAN_RIGID_BODY_SYSTEM_HPP

#define ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE	100

#include "elysian_system.hpp"

namespace elysian {

	class CollisionContact;

	class RigidBodySystem: public System {

		CollisionContact*	_contactPool[ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE];
		unsigned			_contactCount = 0;

		void _updateContacts(void);

	public:
		void update(void);

		void resolveContactVelocities(void);
		void integrate(void);

	};

}

#endif // ELYSIAN_RIGID_BODY_SYSTEM_HPP
