#ifndef ELYSIAN_AUDIO_SOURCE_HPP
#define ELYSIAN_AUDIO_SOURCE_HPP

#include "elysian_component.hpp"
#include <gyro_wav.h>
#include <gyro_audio.h>

namespace elysian {

	class AudioSource: public Component {
	public:
		AudioSource(Entity& entity);
		virtual ~AudioSource(void) override;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(std::ifstream& stream) override;
		virtual bool save(std::ofstream& stream) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

		//public API
		void setWav(const GYWav*const wav);
		void playWav(const GYWav*const wav) const;
		void play(void) const;
		void stop(void) const ;
		void setGain(const float val) const;
		void setPitch(const float val) const;
		void setLooping(const bool val) const;

		//subsystem
		void update(void) const;

	private:
		GYWavSource*		_src = nullptr;
		const GYWav*		_wav = nullptr;

	};

	inline AudioSource::AudioSource(Entity& entity): Component(entity) {
#ifdef ELYSIAN_ENGINE
		_src = gyAudSourceCreate();
#endif
	}
	inline AudioSource::~AudioSource(void) {
#ifdef ELYSIAN_ENGINE
		gyAudSourceDestroy(_src);
#endif
}
	inline bool AudioSource::canAddTo(Entity &/*entity*/) { return true; }
	inline Component::TYPE AudioSource::getType(void) const { return AUDIO_SOURCE; }

	inline void AudioSource::setWav(const GYWav* const wav) { _wav = wav; }
	inline void AudioSource::playWav(const GYWav* const wav) const { gyAudSourcePlayWav(_src, wav); }
	inline void AudioSource::play(void) const { gyAudSourcePlayWav(_src, _wav); }
	inline void AudioSource::stop(void) const { }
	inline void AudioSource::setGain(const float val) const { gyAudSourceSetf(_src, GY_SOURCE_GAIN, val); }
	inline void AudioSource::setPitch(const float val) const { gyAudSourceSetf(_src, GY_SOURCE_PITCH, val); }
	inline void AudioSource::setLooping(const bool val) const { gyAudSourceSeti(_src, GY_SOURCE_LOOPING, val); }

}

#endif // ELYSIAN_AUDIO_SOURCE_HPP
