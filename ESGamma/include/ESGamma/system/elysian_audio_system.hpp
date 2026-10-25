/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_AUDIO_SYSTEM_HPP
#define ELYSIAN_AUDIO_SYSTEM_HPP

#include <libGyro/gyro_audio_api.h>
#include <libGyro/gyro_wav_api.h>
#include <libGyro/gyro_ogg_api.h>
#include <libGyro/gyro_matrix_api.h>
#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/utility/elysian_subprocess_manager.hpp>
#include <AssetIO/utility/elysian_lua_variant_base.hpp>

#define ELYSIAN_AUDIO_CHANNELS              32
#define ELYSIAN_AUDIO_REFERENCE_DIST        32
#define ELYSIAN_AUDIO_ZSCALE                1.0f
#define ELYSIAN_AUDIO_BGM_OGGS              6
#define ELYSIAN_AUDIO_AUX_OGGS              4
#define ELYSIAN_AUDIO_BGM_TRACKS            3
#define ELYSIAN_AUDIO_AUX_TRACKS            2
#define ELYSIAN_AUDIO_SFX_CHANNELS          26
#define ELYSIAN_AUDIO_SFX_CHANNEL_INVALID   -1

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(AudioSource)

namespace elysian {


    class WavPlaybackSubProcess:
        public MonitorableSubProcess<WavPlaybackSubProcess, AudioSystem>
    {
      protected:
        const GYWav*      _wav      = nullptr;
        int               _channel  = ELYSIAN_AUDIO_SFX_CHANNEL_INVALID;
        GYAudioSource*    _src      = nullptr;

      public:

          void            _start(AudioSystem* sys);
          void            _stop(AudioSystem* sys);
          void            _update(AudioSystem* sys, const float deltaTime);
          void            setChannel(int channel);
          void            setWav(const GYWav* wav);
    };



    using WavPlaybackSubProcessPool = MonitorableSubProcessPool<WavPlaybackSubProcess, AudioSystem, ELYSIAN_AUDIO_SFX_CHANNELS>;


  class AudioSystem:
          public SubProcessManager<AudioSystem, WavPlaybackSubProcessPool>,
          public System
  {
  private:
    enum SUBPROCS   { SUBPROC_WAV_PLAYBACK_POOL };
  public:
                    AudioSystem(void);
                    ~AudioSystem(void);

    void            update(void);
    void            setEnabled(const bool val);
    bool            isEnabled(void) const;
    void            updateAreaTracks(void);
    void            setListenerGain(const float gain);
    void            dumpDebugInfo(void) const;

    //WAV SFX API
    //accept -1 for dynamic channel
    void            playSfxChannel(const int channel, const GYWav& wav, EventMonitor* monitor=nullptr);
    void            playSfxChannelLua(const int channel, const GYWav& wav, LuaVariant v=LuaVariant());
    void            stopSfxChannel(const int channel) const;
    bool            isSfxChannelPlaying(const int channel) const;
    void            setSfxChannelGain(const int channel, const float val) const;
    void            setSfxChannelPitch(const int channel, const float val) const;
    void            setSfxChannelLooping(const int channel, const bool val) const;
    void            setSfxChannelRolloffFactor(const int channel, const float val) const;
    void            setSfxChannelPos(const int channel, const float x, const float y, const float z) const;

    //OGG BGM API
    void            loadBgmTrack(const unsigned track, const char*const path);
    void            playBgmTrack(void) const;
    void            stopBgmTrack(void) const;
    void            pauseBgmTrack(void) const;
    bool            isBgmTrackPlaying(void) const;
    unsigned        getBgmTrack(void) const;
    void            setBgmTrack(const unsigned track);
    const char*     getBgmTrackPath(const unsigned track) const;
    double          getBgmTrackTime(void) const;
    void            fadeOutBgmTrack(const float secs);
    void            fadeInBgmTrack(const float secs);
    void            morphIntoBgmTrack(const unsigned track, const float secs);
    void            setBgmTrackLooping(const bool val);
    void            setBgmTrackGain(const float val);
    void            setBgmTrackPitch(const float val) const;
    void            setBgmTrackPos(const float x, const float y, const float z) const;
    void            setBgmTrackRolloffFactor(const float val) const;
    void            fadeIntoBgmTrack(const unsigned track, const float secs);
    void            morphIntoNewBgmTrack(const unsigned track, const float secs, const char* path);
    void            fadeIntoNewBgmTrack(const unsigned track, const float secs, const char *path);


    //OGG AUX API
    void            loadAuxTrack(const unsigned track, const char*const path);
    void            playAuxTrack(const unsigned track) const;
    void            stopAuxTrack(const unsigned track) const;
    void            pauseAuxTrack(const unsigned track) const;
    bool            isAuxTrackPlaying(const unsigned track) const;
    const char*     getAuxTrackPath(const unsigned track) const;
    double          getAuxTrackTime(const unsigned track) const;
    void            fadeOutAuxTrack(const unsigned track, const float secs);
    void            fadeInAuxTrack(const unsigned track, const float secs);
    void            morphIntoNewAuxTrack(const unsigned track, const float secs, const char* path);
    void            fadeIntoNewAuxTrack(const unsigned track, const float secs, const char *path);
    void            setAuxTrackLooping(const unsigned track, const bool val);
    void            setAuxTrackGain(const unsigned track, const float val);
    void            setAuxTrackPitch(const unsigned track, const float val) const;
    void            setAuxTrackPos(const unsigned track, const float x, const float y, const float z) const;
    void            setAuxTrackRolloffFactor(const unsigned track, const float val) const;

    //Internal
    GYAudioSource*	reserveChannel(AudioSource*const comp = nullptr);
    void			releaseChannel(const GYAudioSource* const src);
    int             sourceToChannelIndex(const GYAudioSource* src) const;
    void            _playSfxChannel(const int channel, const GYWav& wav);

    //called from Component
    struct AudioChannel {
    enum TYPE {
        //Lua static SFX range
        LUA_SFX_1, LUA_SFX_2, LUA_SFX_3, LUA_SFX_4, LUA_SFX_5,
        //Engine static SFX range
        SFX_1, SFX_2, SFX_3, SFX_4, SFX_5,
        //BGM range
        BGM_1, BGM_1_TRAN, AUX_1, AUX_1_TRAN, AUX_2, AUX_2_TRAN,
        //Dynamic SFX (AudioSource) range
        DYN_1, DYN_2, DYN_3, DYN_4, DYN_5, DYN_6, DYN_7, DYN_8, DYN_9,
        DYN_10, DYN_11, DYN_12, DYN_13, DYN_14, DYN_15, DYN_16
    };

      enum OWNER {
          SYSTEM, COMPONENT, FREE
      }                 owner;

        GYAudioSource*  src;
        AudioSource*    comp;
    };


  private:

    enum TRACK_TYPE {
        TYPE_BGM, TYPE_AUX1, TYPE_AUX2, TYPE_MAX
    };

    struct StreamChannel {
        enum Type {
            BGM, BGM_TRAN,
            AUX_1, AUX_1_TRAN,
            AUX_2, AUX_2_TRAN,
            COUNT
        };

        enum class State {
            NORMAL, FADE_OUT, FADE_IN
        }				state			= State::NORMAL;
        GYAudioSource*	src				= nullptr;
        GYOgg*			ogg				= nullptr;
        unsigned		track			= 0;
        unsigned		newTrack		= 0;
        float			fadeTime		= 0.0f;
        float			fadeEllapsed	= 0.0f;
        float			baseGain		= 1.0f;

    }					_streamChannels[6];

    AudioChannel        _channels[ELYSIAN_AUDIO_CHANNELS];
    GYOgg*				_bgmTracks[ELYSIAN_AUDIO_BGM_OGGS]	= { nullptr };
    GYOgg*				_auxTracks[ELYSIAN_AUDIO_AUX_OGGS]    = { nullptr };
    GYVector3			_listenerPos                            = { 0.0f, 0.0f, 0.0f };

    //TODO: Get the fuck rid of this shit and make it Tk-specific
    char                _prevBgmPaths[ELYSIAN_AUDIO_BGM_TRACKS][GYRO_PATH_MAX_SIZE];
    char                _prevAuxPaths[ELYSIAN_AUDIO_AUX_TRACKS][GYRO_PATH_MAX_SIZE];
    float               _prevGain = 1.0f;

    struct GlobalState {
        enum STATE {
            NORMAL, FADE_OUT, FADE_OUT_NEW, FADE_IN
        }               state               = NORMAL;
        unsigned        newTrack            = 0;
        float           fadeTime            = 0.0f;
    }                   _globalStates[TYPE_MAX];

    unsigned			_lastStolenChannel	= AudioChannel::DYN_1;
    bool				_bgmLooping			= true;
    bool				_aux1Looping		= true;
    bool				_aux2Looping		= true;
    bool                _enabled            = true;

    static unsigned     _auxIndexToStreamChannel(const unsigned index);
    static unsigned     _auxIndexToTrack(const unsigned index);
  };






    // ====== INLINEZ =======

    inline bool AudioSystem::isEnabled(void) const { return _enabled; }

    inline void WavPlaybackSubProcess::_start(AudioSystem* sys) {
        if(_channel == ELYSIAN_AUDIO_SFX_CHANNEL_INVALID) {
            _src = sys->reserveChannel();
            _channel = sys->sourceToChannelIndex(_src);
        }
        sys->setSfxChannelRolloffFactor(_channel, 0.0f);
        sys->setSfxChannelGain(_channel, 1.0f);
        sys->_playSfxChannel(_channel, *_wav);

    }
    inline void WavPlaybackSubProcess::_stop(AudioSystem* sys) {
        sys->stopSfxChannel(_channel);
        if(_src) {
            sys->releaseChannel(_src);
            _src = nullptr;
        }
        _channel = ELYSIAN_AUDIO_SFX_CHANNEL_INVALID;
        _wav = nullptr;
    }
    inline void WavPlaybackSubProcess::_update(AudioSystem* sys, const float) {
        if(!sys->isSfxChannelPlaying(_channel)) {
            stop(sys);
        }
    }
    inline void WavPlaybackSubProcess::setChannel(int channel) {
        _channel = channel;
    }
    inline void WavPlaybackSubProcess::setWav(const GYWav* wav) {
        _wav = wav;
    }

}

#endif // ELYSIAN_AUDIO_SYSTEM_HPP
