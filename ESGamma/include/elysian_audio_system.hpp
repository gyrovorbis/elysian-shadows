#ifndef ELYSIAN_AUDIO_SYSTEM_HPP
#define ELYSIAN_AUDIO_SYSTEM_HPP

#include <gyro_wav.h>
#include "elysian_system.hpp"

#define ELYSIAN_AUDIO_REFERENCE_DIST 10
#define ELYSIAN_AUDIO_ZSCALE		 32.0f

namespace elysian {

	class Entity;

	class AudioSystem: public System {
	public:
		AudioSystem(void);

		void update(void);

		//WAV API
		void playWav(const GYWav& wav) const;
		void playWavFromEntity(const Entity& entity, const GYWav& wav) const;
		void playWavFromPos(const float x, const float y, const float z, const GYWav& wav) const;


		//OGG API

	};

}

#endif // ELYSIAN_AUDIO_SYSTEM_HPP
