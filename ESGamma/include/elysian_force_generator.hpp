#ifndef ELYSIAN_FORCE_GENERATOR_HPP
#define ELYSIAN_FORCE_GENERATOR_HPP

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <stdint.h>

namespace elysian {

	class ForceTarget;
	class Entity;

	class ForceGenerator {
	private:
		uint32_t		_forceMask = ~0x0;
	public:
		virtual void	applyForce(ForceTarget*const target, const float deltaTime) = 0;
		virtual			~ForceGenerator(void) = default;

		uint32_t		getForceMask(void) const;
		void			setForceMask(const uint32_t mask);
	};

	class GravityForceGenerator: public ForceGenerator {
	private:
		GYVector2 _accel = GYVector2 { 0.0f, 0.7f };
	public:
		virtual			~GravityForceGenerator(void) = default;
		void			setAccel(const GYVector2& accel);
		virtual void	applyForce(ForceTarget*const target, const float deltaTime) override;

	};

	class ImplosionForceGenerator: public ForceGenerator {
	private:
		Entity*		_focalEntity	= nullptr;
		float		_magnitude		= 0.0f;
		float		_radius			= 0.0f;

	public:
		virtual			~ImplosionForceGenerator(void) = default;
		void			setRadius(const float radius);
		void			setForceMagnitude(const float mag);
		void			setFocalEntity(Entity*const entity);
		virtual void	applyForce(ForceTarget*const target, const float deltaTime) override;

	};

	class ExplosionForceGenerator: public ForceGenerator {
	private:
		Entity*		_focalEntity	= nullptr;
		float		_magnitude		= 0.0f;
		float		_radius			= 0.0f;

	public:

		virtual			~ExplosionForceGenerator(void) = default;
		void			setRadius(const float radius);
		void			setForceMagnitude(const float mag);
		void			setFocalEntity(Entity*const entity);
		virtual void	applyForce(ForceTarget*const target, const float deltaTime) override;

	};




	// ======= INLINEZ ======

	inline uint32_t ForceGenerator::getForceMask(void) const { return _forceMask; }
	inline void ForceGenerator::setForceMask(const uint32_t mask) { _forceMask = mask; }


	inline void GravityForceGenerator::setAccel(const GYVector2 &accel) { _accel = accel; }

	inline void ImplosionForceGenerator::setForceMagnitude(const float mag) { _magnitude = mag;}
	inline void ImplosionForceGenerator::setFocalEntity(Entity * const entity) { _focalEntity = entity; }
	inline void ImplosionForceGenerator::setRadius(const float radius) { _radius = radius; }

	inline void ExplosionForceGenerator::setForceMagnitude(const float mag) { _magnitude = mag; }
	inline void ExplosionForceGenerator::setFocalEntity(Entity * const entity) { _focalEntity = entity; }
	inline void ExplosionForceGenerator::setRadius(const float radius) { _radius = radius; }



}


#endif // ELYSIAN_FORCE_GENERATOR_HPP
