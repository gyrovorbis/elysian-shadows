#ifndef ELYSIAN_AUDIO_SOURCE_HPP
#define ELYSIAN_AUDIO_SOURCE_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_audio_source_base.hpp>
namespace elysian {
    typedef AudioSourceBase AudioSource;
}
#else
#   include <tk_audio_source.h>
namespace elysian {
    typedef TkAudioSource AudioSource;
}
typedef TkAudioSource AudioSource;

#endif


#endif // ELYSIAN_AUDIO_SOURCE_HPP

