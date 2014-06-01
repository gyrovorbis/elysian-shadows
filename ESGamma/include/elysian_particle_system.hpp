#ifndef ELYSIAN_PARTICLE_SYSTEM_HPP
#define ELYSIAN_PARTICLE_SYSTEM_HPP

#include <gyro_video_api.h>
#include <gyro_math_api.h>
#include "elysian_system.hpp"
#include "elysian_particle_emitter.hpp"

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT		4

namespace elysian {

	class ForceGenerator;

	class ParticleSystem: public System {
	private:
		ParticleEmitterInternals	_emitters[ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT];
		ParticleEmitterAttrib*		_defaultAttribs[ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT];

	public:
		ParticleSystem(void);
		~ParticleSystem(void);

		void update(void);
		void render(void);
		void renderAtElev(const unsigned elev);
		void applyForce(ForceGenerator*const forceGen);

		//Internal particle emitters
		void emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity);
		ParticleEmitterAttrib *getAttrib(const unsigned index) const;
		void setAttrib(const unsigned index, ParticleEmitterAttrib*const attrib);
		void resizePool(const unsigned index, const unsigned size);
	};



	// ==== INLINEZ =====

	inline ParticleEmitterAttrib* ParticleSystem::getAttrib(const unsigned index) const {
		return _emitters[index].getAttrib();
	}

	inline void ParticleSystem::setAttrib(const unsigned index, ParticleEmitterAttrib * const attrib) {
		_emitters[index].setAttrib((attrib)? attrib : _defaultAttribs[index]);
	}


}



#endif // ELYSIAN_PARTICLE_SYSTEM_HPP
