#ifndef ELYSIAN_AUDIO_SOURCE_BASE_HPP
#define ELYSIAN_AUDIO_SOURCE_BASE_HPP

#include <elysian_component_base.hpp>
#include <elysian_lua_table_proxy.hpp>
#include <gyro_wav_api.h>
#include <gyro_audio_api.h>

namespace elysian {

	class AudioSystem;

    class AudioSourceBase:
        public Component,
        public LuaTableProxy<AudioSourceBase, AudioSource>
    {
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

                AudioSourceBase(void) = default;
                AudioSourceBase(Entity& entity);
        virtual ~AudioSourceBase(void) override;

		//reimplemented from component
        static constexpr const Component::TYPE componentType = Component::AUDIO_SOURCE;

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









    // ======= INLINEZ =======



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
/*
	inline void AudioSource::setWav(const GYWav* const wav) { _wav = wav; }
	inline void AudioSource::playWav(const GYWav* const wav) const { gyAudSourcePlayWav(_src, wav); }
	inline void AudioSource::play(void) const { gyAudSourcePlayWav(_src, _wav); }
	inline void AudioSource::stop(void) const { }
	inline void AudioSource::setGain(const float val) const { gyAudSourceSetf(_src, GY_SOURCE_GAIN, val); }
	inline void AudioSource::setPitch(const float val) const { gyAudSourceSetf(_src, GY_SOURCE_PITCH, val); }
	inline void AudioSource::setLooping(const bool val) const { gyAudSourceSeti(_src, GY_SOURCE_LOOPING, val); }

	*/
	
    inline AudioSourceBase::AudioSourceBase(Entity& entity): Component(entity) {

	}

    inline AudioSourceBase::~AudioSourceBase(void) {
		stop();
	}

    inline void AudioSourceBase::setWav(const GYWav* const wav) { _wav = wav; }

    inline void AudioSourceBase::play(void) { playWav(_wav); }

    inline void AudioSourceBase::setGain(const float val) {
		_gain = val;
		if(_src) gyAudSourceSetf(_src, GY_SOURCE_GAIN, val);
	}

    inline void AudioSourceBase::setPitch(const float val) {
		_pitch = val;
		if(_src) gyAudSourceSetf(_src, GY_SOURCE_PITCH, val);
	}

    inline void AudioSourceBase::setLooping(const bool val)  {
		_looping = val;
		if(_src) gyAudSourceSeti(_src, GY_SOURCE_LOOPING, val);
	}

    inline void AudioSourceBase::setAudioSys(AudioSystem* const sys) {
		_audioSys = sys;
	}



}

#endif // ELYSIAN_AUDIO_SOURCE_HPP
