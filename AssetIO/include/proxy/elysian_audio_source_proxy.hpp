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
    OOLUA_MEM_FUNC(void, setWav, const GYWav*)
    OOLUA_MEM_FUNC(void, playWav, const GYWav*)
    OOLUA_MEM_FUNC(void, play)
    OOLUA_MEM_FUNC(void, stop)
    OOLUA_MEM_FUNC(void, setGain, float)
    OOLUA_MEM_FUNC(void, setPitch, float)
    OOLUA_MEM_FUNC(void, setLooping, bool)
    OOLUA_MEM_FUNC_CONST(float, getPitch)
    OOLUA_MEM_FUNC_CONST(float, getGain)
    OOLUA_MEM_FUNC_CONST(bool, getLooping)
OOLUA_PROXY_END

#endif // ELYSIAN_AUDIO_SOURCE_PROXY_HPP

