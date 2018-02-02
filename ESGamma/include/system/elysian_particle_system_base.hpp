#ifndef ELYSIAN_PARTICLE_SYSTEM_BASE_HPP
#define ELYSIAN_PARTICLE_SYSTEM_BASE_HPP

#include <gyro_video_api.h>
#include <gyro_math_api.h>
#include "elysian_system.hpp"
#include <crtp/elysian_particle_emitter.hpp>

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_DEFAULT_COUNT		4

namespace elysian {

	class ForceGenerator;

    class ParticleSystemBase: public System {
    protected:
        ParticleEmitterInternals*               _emitters;
        std::shared_ptr<ParticleEmitterAttrib>*	_defaultAttribs;
        const unsigned                          _emitterCount;

	public:
        ParticleSystemBase(unsigned emitterCount=ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_DEFAULT_COUNT);
        ~ParticleSystemBase(void);

		void update(void);
		void render(void);
		void renderAtElev(const unsigned elev);
		void applyForce(ForceGenerator*const forceGen);

        //Internal particle emitters
        void emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity);
        std::shared_ptr<ParticleEmitterAttrib> getAttrib(const unsigned index) const;
        void setAttrib(const unsigned index, std::shared_ptr<ParticleEmitterAttrib> attrib);
		void resizePool(const unsigned index, const unsigned size);
	};



	// ==== INLINEZ =====

    inline std::shared_ptr<ParticleEmitterAttrib> ParticleSystemBase::getAttrib(const unsigned index) const {
        GY_ASSERT(index < _emitterCount);
        return _emitters[index].getAttrib();
	}

    inline void ParticleSystemBase::setAttrib(const unsigned index, std::shared_ptr<ParticleEmitterAttrib> attrib) {
        GY_ASSERT(index < _emitterCount);
        _emitters[index].setAttrib((attrib)? attrib : _defaultAttribs[index]);
	}


}



#endif // ELYSIAN_PARTICLE_SYSTEM_HPP
