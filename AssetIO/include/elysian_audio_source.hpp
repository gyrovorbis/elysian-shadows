#ifndef ELYSIAN_AUDIO_SOURCE_HPP
#define ELYSIAN_AUDIO_SOURCE_HPP

#include "elysian_component.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_lua_table_proxy.hpp"
#include <gyro_wav_api.h>
#include <gyro_audio_api.h>

namespace elysian {

	class AudioSystem;

    class AudioSource:
        public Component,
        public LuaTableProxy<AudioSource>
    {
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

                AudioSource(Entity& entity);
		virtual ~AudioSource(void) override;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
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
		void			setChannel(GYAudioSource* const src);
		GYAudioSource*	getChannel(void) const;
		static void		setAudioSys(AudioSystem* const sys);


	private:
		static AudioSystem* _audioSys;
		GYAudioSource*		_src = nullptr;
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
		//Reserve a channel if we haven't already
		if(!_src) {
			_src = _audioSys->reserveChannel(this);
		}

		//Make sure to set channel attributes before playing
		gyAudSourceSetf(_src, GY_SOURCE_GAIN, _gain);
		gyAudSourceSetf(_src, GY_SOURCE_PITCH, _pitch);
		gyAudSourceSeti(_src, GY_SOURCE_LOOPING, _looping);
		gyAudSourcePlayWav(_src, wav);
	}

	inline void AudioSource::stop(void) {
		if(_src) {
			gyAudSourceStop(_src);
			//give the bitch back if we're done with it
			_audioSys->releaseChannel(_src);
			_src = nullptr;
		}
	}

	inline void AudioSource::play(void) { playWav(_wav); }

	inline void AudioSource::setGain(const float val) {
		_gain = val;
		if(_src) gyAudSourceSetf(_src, GY_SOURCE_GAIN, val);
	}

	inline void AudioSource::setPitch(const float val) {
		_pitch = val;
		if(_src) gyAudSourceSetf(_src, GY_SOURCE_PITCH, val);
	}

	inline void AudioSource::setLooping(const bool val)  {
		_looping = val;
		if(_src) gyAudSourceSeti(_src, GY_SOURCE_LOOPING, val);
	}

	inline void AudioSource::setAudioSys(AudioSystem* const sys) {
		_audioSys = sys;
	}



}

#endif // ELYSIAN_AUDIO_SOURCE_HPP
