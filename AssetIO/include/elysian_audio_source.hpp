#ifndef ELYSIAN_AUDIO_SOURCE_HPP
#define ELYSIAN_AUDIO_SOURCE_HPP

#include "elysian_component.hpp"
#ifdef ELYSIAN_ENGINE
#	include "elysian_audio_system.hpp"
#endif
#include <gyro_wav.h>
#include <gyro_audio.h>

namespace elysian {

	class AudioSystem;

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
		void playWav(const GYWav*const wav);
		void play(void);
		void stop(void);
		void setGain(const float val);
		void setPitch(const float val);
		void setLooping(const bool val);

		//subsystem
		void			update(void);
		void			setChannel(GYWavSource* const src);
		GYWavSource*	getChannel(void) const;
		static void		setAudioSys(AudioSystem* const sys);


	private:
		static AudioSystem* _audioSys;
		GYWavSource*		_src = nullptr;
		const GYWav*		_wav = nullptr;

		//source attributes
		float				_gain = 1.0f;
		float				_pitch = 1.0f;
		bool				_looping = false;

	};
/*
	inline AudioSource::AudioSource(Entity& entity): Component(entity) {
#ifdef ELYSIAN_ENGINE
		_src = gyAudSourceCreate();
#endif
	}
	inline AudioSource::~AudioSource(void) {
#ifdef ELYSIAN_ENGINE
		gyAudSourceDestroy(_src);
#endif
}*/
	inline bool AudioSource::canAddTo(Entity &/*entity*/) { return true; }
	inline Component::TYPE AudioSource::getType(void) const { return AUDIO_SOURCE; }
/*
	inline void AudioSource::setWav(const GYWav* const wav) { _wav = wav; }
	inline void AudioSource::playWav(const GYWav* const wav) const { gyAudSourcePlayWav(_src, wav); }
	inline void AudioSource::play(void) const { gyAudSourcePlayWav(_src, _wav); }
	inline void AudioSource::stop(void) const { }
	inline void AudioSource::setGain(const float val) const { gyAudSourceSetf(_src, GY_SOURCE_GAIN, val); }
	inline void AudioSource::setPitch(const float val) const { gyAudSourceSetf(_src, GY_SOURCE_PITCH, val); }
	inline void AudioSource::setLooping(const bool val) const { gyAudSourceSeti(_src, GY_SOURCE_LOOPING, val); }

	*/
	
	inline AudioSource::AudioSource(Entity& entity): Component(entity) {

	}

	inline AudioSource::~AudioSource(void) {
		stop();
	}

	inline void AudioSource::setWav(const GYWav* const wav) { _wav = wav; }

	inline void AudioSource::playWav(const GYWav* const wav) {
		(void)wav;
#ifdef ELYSIAN_ENGINE
		//Reserve a channel if we haven't already
		if(!_src) {
			_src = _audioSys->reserveChannel(this);
		}

		//Make sure to set channel attributes before playing
		gyAudSourceSetf(_src, GY_SOURCE_GAIN, _gain);
		gyAudSourceSetf(_src, GY_SOURCE_PITCH, _pitch);
		gyAudSourceSeti(_src, GY_SOURCE_LOOPING, _looping);
		gyAudSourcePlayWav(_src, wav);
#endif
	}

	inline void AudioSource::stop(void) {
#ifdef ELYSIAN_ENGINE
		if(_src) {
			gyAudSourceStop(_src);
			//give the bitch back if we're done with it
			_audioSys->releaseChannel(_src);
			_src = nullptr;
		}
#endif
	}

	inline void AudioSource::play(void) { playWav(_wav); }

	inline void AudioSource::setGain(const float val) {
		_gain = val;
#ifdef ELYSIAN_ENGINE
		if(_src) gyAudSourceSetf(_src, GY_SOURCE_GAIN, val);
#endif
	}

	inline void AudioSource::setPitch(const float val) {
		_pitch = val;
#ifdef ELYSIAN_ENGINE
		if(_src) gyAudSourceSetf(_src, GY_SOURCE_PITCH, val);
#endif
	}

	inline void AudioSource::setLooping(const bool val)  {
		_looping = val;
#ifdef ELSYSIAN_ENGINE
		if(_src) gyAudSourceSeti(_src, GY_SOURCE_LOOPING, val);
#endif
	}


	inline void AudioSource::setAudioSys(AudioSystem* const sys) {
		_audioSys = sys;
	}



}

#endif // ELYSIAN_AUDIO_SOURCE_HPP
