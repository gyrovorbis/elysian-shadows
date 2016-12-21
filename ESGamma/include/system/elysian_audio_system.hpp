#ifndef ELYSIAN_AUDIO_SYSTEM_HPP
#define ELYSIAN_AUDIO_SYSTEM_HPP

#include <gyro_audio_api.h>
#include <gyro_wav_api.h>
#include <gyro_ogg_api.h>
#include <gyro_matrix_api.h>
#include "elysian_system.hpp"

#define ELYSIAN_AUDIO_CHANNELS			32
#define ELYSIAN_AUDIO_REFERENCE_DIST	10
#define ELYSIAN_AUDIO_ZSCALE			32.0f
#define ELYSIAN_AUDIO_BGM_TRACKS        6
#define ELYSIAN_AUDIO_AUX_TRACKS        4

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(AudioSource)

namespace elysian {

  class AudioSystem: public System {
  public:
                    AudioSystem(void);
                    ~AudioSystem(void);

    void            update(void);
    void            setEnabled(const bool val);
    bool            isEnabled(void) const;
    void            updateAreaTracks(void);
    void            setListenerGain(const float gain);

    //WAV SFX API
    //accept -1 for dynamic channel
    void            playSfxChannel(int channel, const GYWav& wav);
    void            stopSfxChannel(const unsigned channel) const;
    bool            isSfxChannelPlaying(const unsigned channel) const;
    void            setSfxChannelGain(const unsigned channel, const float val) const;
    void            setSfxChannelPitch(const unsigned channel, const float val) const;
    void            setSfxChannelLooping(const unsigned channel, const bool val) const;
    void            setSfxChannelRolloffFactor(const unsigned channel, const float val) const;
    void            setSfxChannelPos(const unsigned channel, const float x, const float y, const float z) const;

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
    GYOgg*				_bgmTracks[ELYSIAN_AUDIO_BGM_TRACKS]	= { nullptr };
    GYOgg*				_auxTracks[ELYSIAN_AUDIO_AUX_TRACKS]    = { nullptr };
    GYVector3			_listenerPos                            = { 0.0f, 0.0f, 0.0f };

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

}

#endif // ELYSIAN_AUDIO_SYSTEM_HPP
