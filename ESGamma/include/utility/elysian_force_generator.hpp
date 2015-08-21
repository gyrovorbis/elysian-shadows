#ifndef ELYSIAN_FORCE_GENERATOR_HPP
#define ELYSIAN_FORCE_GENERATOR_HPP

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <stdint.h>
#include <utility/elysian_asset_typedefs.hpp>
#include <utility/elysian_bit_mask.hpp>

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class ForceTarget;
	class ForceGenerator {
	private:
        BitMask32		_forceMask = ~0x0;
	public:
		virtual void	applyForce(ForceTarget*const target, const float deltaTime) = 0;
		virtual			~ForceGenerator(void) = default;

        BitMask32&		getForceMask(void);
        void			setForceMask(const BitMask32 mask);
	};

	class GravityForceGenerator: public ForceGenerator {
	private:
        GYVector3 _accel = GYVector3 { 0.0f, /*2000.0f*/0.0f, -2000.0f };
	public:
                        GravityForceGenerator(void);
		virtual			~GravityForceGenerator(void) = default;
		void			setAccel(const GYVector3& accel);
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

    inline BitMask32& ForceGenerator::getForceMask(void) { return _forceMask; }
    inline void ForceGenerator::setForceMask(const BitMask32 mask) { _forceMask = mask; }


	inline void ImplosionForceGenerator::setForceMagnitude(const float mag) { _magnitude = mag;}
	inline void ImplosionForceGenerator::setFocalEntity(Entity * const entity) { _focalEntity = entity; }
	inline void ImplosionForceGenerator::setRadius(const float radius) { _radius = radius; }

	inline void ExplosionForceGenerator::setForceMagnitude(const float mag) { _magnitude = mag; }
	inline void ExplosionForceGenerator::setFocalEntity(Entity * const entity) { _focalEntity = entity; }
	inline void ExplosionForceGenerator::setRadius(const float radius) { _radius = radius; }



}


#endif // ELYSIAN_FORCE_GENERATOR_HPP
