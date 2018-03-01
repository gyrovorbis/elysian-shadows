#ifndef ELYSIAN_AUDIO_SOURCE_BASE_HPP
#define ELYSIAN_AUDIO_SOURCE_BASE_HPP

#include <crtp/elysian_component.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <gyro_wav_api.h>
#include <gyro_audio_api.h>

#define ELYSIAN_AUDIO_SOURCE_DEBUG

namespace elysian {

    class AudioSystem;
    class EventMonitor;

    class AudioSourceBase:
        public Component,
        public LuaTableProxy<AudioSourceBase, AudioSource>
    {
    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(wavFile,            getWavFile,         setWavFile)
            ELYSIAN_LUA_PROXY_FIELD(basePath,           getBasePath,        setBasePath)
            ELYSIAN_LUA_PROXY_FIELD(gain,               getGain,            setGain)
            ELYSIAN_LUA_PROXY_FIELD(pitch,              getPitch,           setPitch)
            ELYSIAN_LUA_PROXY_FIELD(looping,            getLooping,         setLooping)
            ELYSIAN_LUA_PROXY_FIELD(rolloffFactor,      getRolloffFactor,   setRolloffFactor)
            ELYSIAN_LUA_PROXY_FIELD(startLooping,       getStartLooping,    setStartLooping)
            ELYSIAN_LUA_PROXY_FIELD(startWavFile,       getStartWavFile,    setStartWavFile)
            //ELYSIAN_LUA_PROXY_FIELD(allowConcurrent,    getAllowConcurrent, setAllowConcurrent)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                            AudioSourceBase(void) = default;
                            AudioSourceBase(Entity& entity, const AudioSourceBase*const other=nullptr);
                            ~AudioSourceBase(void);

        AudioSourceBase&    operator=(const AudioSourceBase& rhs);

		//reimplemented from component
        static constexpr const Component::TYPE componentType = Component::AUDIO_SOURCE;

		//public API
        void                setWav(const GYWav*const wav);
        void                playWav(const GYWav*const wav);
        void                setWavFile(const char* filepath);
        const char*         getWavFile(void) const;
        void                setBasePath(const char* dir);
        const char*         getStartWavFile(void) const;
        void                setStartWavFile(const char* filepath);
        const char*         getBasePath(void) const;
        void                play(EventMonitor* monitor=nullptr);
        void                playLua(LuaVariant v=LuaVariant());
        void                stop(void);
        void                setGain(const float val);
        void                setPitch(const float val);
        void                setLooping(const bool val);
        void                setRolloffFactor(float factor);
        void                setStartLooping(bool val);
        float               getRolloffFactor(void) const;
        float               getGain(void) const;
        bool                getLooping(void) const;
        float               getPitch(void) const;
        bool                getStartLooping(void) const;
        bool                getAllowConcurrent(void) const;
        void                setAllowConcurrent(bool allowed=true);
        bool                isPlaying(void) const;

        void                makeFullPath(char* buffer, const char* wavfile) const;

		//subsystem
        void                update(void);
        static void         setAudioSys(AudioSystem* const sys);

    protected:

        void                _reserveChannel(void);
        void                _releaseChannel(void);

		static AudioSystem* _audioSys;
        char                _wavFile[GYRO_PATH_MAX_SIZE]        = { '\0' };
        char                _startWavFile[GYRO_PATH_MAX_SIZE]   = { '\0' };
        char                _basePath[GYRO_PATH_MAX_SIZE]       = { '\0' };
        GYAudioSource*		_src                                = nullptr;
        GYWav*              _wav                                = nullptr;

		//source attributes
        float				_gain                               = 1.0f;
        float				_pitch                              = 1.0f;
        float               _rolloffFactor                      = 1.0f;
        int                 _channelIndex                       = -1;
        bool				_looping                            = false;
        bool                _startLooping                       = false;
        bool                _allowConcurrent                    = false;

	};









    // ======= INLINEZ =======

	
    inline AudioSourceBase::AudioSourceBase(Entity& entity, const AudioSourceBase*const other): Component(entity) {
        if(other) *this = *other;
	}
    inline AudioSourceBase& AudioSourceBase::operator=(const AudioSourceBase& rhs) {
        setBasePath(rhs.getBasePath());
        setWavFile(rhs.getWavFile());
        setGain(rhs._gain);
        setPitch(rhs._pitch);
        setRolloffFactor(rhs._rolloffFactor);
        setLooping(rhs._looping);
        return *this;
    }

    inline AudioSourceBase::~AudioSourceBase(void) {
        setWavFile(nullptr);
	}

    inline float AudioSourceBase::getPitch(void) const { return _pitch; }

    inline float AudioSourceBase::getGain(void) const { return _gain; }

    inline bool AudioSourceBase::getLooping(void) const { return _looping; }

    inline const char* AudioSourceBase::getWavFile(void) const {
        return _wavFile;
    }

    inline float AudioSourceBase::getRolloffFactor(void) const { return _rolloffFactor; }

    inline const char* AudioSourceBase::getBasePath(void) const { return _basePath; }
    inline void AudioSourceBase::setBasePath(const char *dir) { strcpy(_basePath, dir); }
    inline bool AudioSourceBase::getAllowConcurrent(void) const { return _allowConcurrent; }
    inline void AudioSourceBase::setAllowConcurrent(bool allowed) { _allowConcurrent = allowed; }

    inline void AudioSourceBase::setWav(const GYWav*const wav) {
        setWavFile(wav? wav->filename : nullptr);
    }
    inline void AudioSourceBase::playWav(const GYWav*const wav) {
        setWav(wav);
        play();
    }

    inline void AudioSourceBase::setStartWavFile(const char *filepath) {
        strcpy(_startWavFile, filepath);
        setLooping(_startLooping);
        setWavFile(filepath);
        play();
    }

    inline const char* AudioSourceBase::getStartWavFile(void) const {
        return _startWavFile;
    }

    inline bool AudioSourceBase::getStartLooping(void) const {
        return _startLooping;
    }

    inline void AudioSourceBase::setStartLooping(bool val) {
        _startLooping = val;
    }



}

#endif // ELYSIAN_AUDIO_SOURCE_HPP
