#ifndef ELYSIAN_PARTICLE_SYSTEM_HPP
#define ELYSIAN_PARTICLE_SYSTEM_HPP

#include <gyro_video_api.h>
#include <gyro_math_api.h>
#include "elysian_system.hpp"

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT		4
#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE	5000

namespace elysian {

	struct CameraState;

	class ParticleEmitterAttrib {
		friend class ParticleEmitterInternals;
	private:
		GYVector3	gravity		= GYVector3 { 0.0f, 0.7f, 0.0f };
		//GYVecture*	texture = nullptr;
		unsigned	maxLife		= 100;
		unsigned	size		= 3;
		unsigned	intensity	= 100; //for periodically generated particles, dunno if it should be here or not
		GYColor4	color		= GYColor4 { 255, 255, 255, 255 };
	public:

		void setGravity(const GYVector3& grav);
		void setMaxLife(const unsigned life);
		void setSize(const unsigned newSize);
		void setColor(const GYColor4& newColor);
	};

	struct Particle {
		GYVector3 position;
		GYVector3 velocity;
		int life;
		unsigned size;
		GYColor4 color;
		bool active = false;
	};

	class ParticleEmitterInternals {
	private:
		Particle*				_pool			= nullptr;
		ParticleEmitterAttrib*	_attrib			= nullptr;
		unsigned				_poolSize		= 100;
		unsigned				_activeCount	= 0;
		bool					_overflow		= false;
	public:
		~ParticleEmitterInternals(void);

		void allocatePool(const unsigned size);
		void setAttrib(ParticleEmitterAttrib*const attrib);
		ParticleEmitterAttrib* getAttrib(void) const;

		void update(const CameraState*const state);
		void render(void);

		//create particles
		void emitFrom(const GYVector3& pos, const float angle, const unsigned intensity);
	};

	class ParticleSystem: public System {
	private:
		ParticleEmitterInternals	_emitters[ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT];

	public:
		ParticleSystem(void);

		void update(void);
		void render(void);

		//Internal particle emitters
		void emitFrom(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity);
		ParticleEmitterAttrib *getAttrib(const unsigned index) const;
		void resizePool(const unsigned index, const unsigned size);
	};


	//====== INLINEZ =====



	inline void ParticleEmitterAttrib::setGravity(const GYVector3& grav) {
		gravity = grav;
	}

	inline void ParticleEmitterAttrib::setMaxLife(const unsigned life) {
		maxLife = life;
	}

	inline void ParticleEmitterAttrib::setSize(const unsigned newSize) {
		size = newSize;
	}

	inline void ParticleEmitterAttrib::setColor(const GYColor4& newColor) {
		color = newColor;
	}


}



#endif // ELYSIAN_PARTICLE_SYSTEM_HPP
