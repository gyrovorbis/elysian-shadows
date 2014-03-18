#ifndef ELYSIAN_PARTICLE_SYSTEM_HPP
#define ELYSIAN_PARTICLE_SYSTEM_HPP

#include <gyro_video_api.h>
#include <gyro_math_api.h>
#include "elysian_system.hpp"
#include "elysian_particle_emitter.hpp"

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT		4

namespace elysian {

	class ParticleSystem: public System {
	private:
		ParticleEmitterInternals	_emitters[ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT];

	public:
		ParticleSystem(void);

		void update(void);
		void render(void);

		//Internal particle emitters
		void emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity);
		ParticleEmitterAttrib *getAttrib(const unsigned index) const;
		void resizePool(const unsigned index, const unsigned size);
	};


	//====== INLINEZ =====


	inline void ParticleEmitterAttrib::setAcceleration(const GYVector3& accel) {
		_acceleration = accel;
	}

	inline void ParticleEmitterAttrib::setMaxLife(const unsigned life) {
		_maxLife = life;
	}

	inline void ParticleEmitterAttrib::setSize(const unsigned newSize) {
		_size = newSize;
	}

	inline void ParticleEmitterAttrib::setAngularDamping(const float factor) {
		_angularDamping = factor;
	}

	inline void ParticleEmitterAttrib::setFadeOut(const bool val) {
		_fadeOut = val;
	}

	inline void ParticleEmitterAttrib::setFrameRange(const unsigned min, const unsigned max) {
		_minFrame = min; _maxFrame = max;
	}


}



#endif // ELYSIAN_PARTICLE_SYSTEM_HPP
