#ifndef TKAUDIOSOURCE_H
#define TKAUDIOSOURCE_H

#include <elysian_audio_source_base.hpp>

class TkAudioSource: public elysian::AudioSourceBase {

public:

    TkAudioSource(TkEntity& parent);
    TkAudioSource(void) = default;
    //virtual ~TkAnimation(void) = default;
};


inline TkAudioSource::TkAudioSource(TkEntity& entity): elysian::AudioSourceBase(entity) {}


#endif // TKAUDIOSOURCE_H

