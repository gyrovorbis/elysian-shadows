#ifndef ELYSIAN_RIGID_BODY_HPP
#define ELYSIAN_RIGID_BODY_HPP

#include <kinetic/gyro_kinetic_kinetic.h>
#include "elysian_component.hpp"

namespace elysian {

	class RigidBody: public Component, private GYKinetic {
		friend class RigidBodySystem;

	public:

		RigidBody(Entity& entity);
		virtual ~RigidBody(void) override = default;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(std::ifstream& stream) override;
		virtual bool save(std::ofstream& stream) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

		//GYKinetic rigid-bodies
		void integrate(const float deltaTime);
		void applyForce(const GYVector2& force);
		void applyPointForce(const GYVector2& loc, const GYVector2& force);

	};


}

#endif // ELYSIAN_RIGID_BODY_H
