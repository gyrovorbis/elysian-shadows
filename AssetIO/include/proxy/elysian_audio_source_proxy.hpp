#ifndef ELYSIAN_AUDIO_SOURCE_PROXY_HPP
#define ELYSIAN_AUDIO_SOURCE_PROXY_HPP

#include <proxy/elysian_component_proxy.hpp>
#include <crtp/elysian_audio_source.hpp>

namespace OOLUA {
    typedef elysian::AudioSource        AudioSource;
}

OOLUA_PROXY(GYWav)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

OOLUA_PROXY(AudioSource, Component)
     OOLUA_TAGS (
         No_public_constructors,
         No_public_destructor
     )
    OOLUA_MFUNC(setWavFile)
    OOLUA_MFUNC_CONST(getWavFile)
    OOLUA_MFUNC(setBasePath)
    OOLUA_MFUNC_CONST(getBasePath)
    OOLUA_MFUNC(setRolloffFactor)
    OOLUA_MFUNC_CONST(getRolloffFactor)
    OOLUA_MFUNC_CONST(isPlaying)
    OOLUA_MFUNC(setWav)
    OOLUA_MFUNC(playWav)
    OOLUA_MFUNC(play)
    OOLUA_MFUNC(setStartLooping)
    OOLUA_MFUNC_CONST(getStartLooping)
    OOLUA_MFUNC(setStartWavFile)
    OOLUA_MFUNC_CONST(getStartWavFile)
    OOLUA_MEM_FUNC(void, stop)
    OOLUA_MEM_FUNC(void, setGain, float)
    OOLUA_MEM_FUNC(void, setPitch, float)
    OOLUA_MEM_FUNC(void, setLooping, bool)
    OOLUA_MEM_FUNC_CONST(float, getPitch)
    OOLUA_MEM_FUNC_CONST(float, getGain)
    OOLUA_MEM_FUNC_CONST(bool, getLooping)
OOLUA_PROXY_END

#endif // ELYSIAN_AUDIO_SOURCE_PROXY_HPP

