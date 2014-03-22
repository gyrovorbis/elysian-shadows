#ifndef ELYSIAN_FORCE_GENERATOR_HPP
#define ELYSIAN_FORCE_GENERATOR_HPP

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <stdint.h>

namespace elysian {

	class RigidBody;

	class ForceGenerator {
	private:
		uint32_t		_forceMask = ~0x0;
	public:
		virtual void	applyForce(RigidBody*const body, const float deltaTime) = 0;

		uint32_t		getForceMask(void) const;
		void			setForceMask(const uint32_t mask);
	};

	class GravityForceGenerator: public ForceGenerator {
	private:
		GYVector2 _accel = GYVector2 { 0.0f, 0.7f };
	public:
		void			setAccel(const GYVector2& accel);
		virtual void	applyForce(RigidBody*const body, const float deltaTime) override;

	};



	// ======= INLINEZ ======

	inline uint32_t ForceGenerator::getForceMask(void) const { return _forceMask; }
	inline void ForceGenerator::setForceMask(const uint32_t mask) { _forceMask = mask; }


	inline void GravityForceGenerator::setAccel(const GYVector2 &accel) { _accel = accel; }

}


#endif // ELYSIAN_FORCE_GENERATOR_HPP
