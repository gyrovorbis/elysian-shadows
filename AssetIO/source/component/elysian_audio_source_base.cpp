#include <vector>
#include <algorithm>
#include "elysian_audio_source.hpp"
#include <elysian_audio_system.hpp>
#include "elysian_generic.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AudioSourceBase)

    AudioSystem* AudioSourceBase::_audioSys = nullptr;

    AudioSourceBase::AudioSourceBase(const AudioSourceBase &other) {
        *this = other;
    }

    void AudioSourceBase::update(void) {
		if(_src) {
			if(gyAudSourceIsPlaying(_src)) {
                gyAudSourceSetfv(_src, GY_SOURCE_POS, &_entity->_transform._pos);
			} else {
				//give the bitch back if we're done with it
                _audioSys->releaseChannel(_src);
				_src = nullptr;
			}
		}
	}

    void AudioSourceBase::playWav(const GYWav* const wav) {
        (void)wav;
        //Reserve a channel if we haven't already
        if(!_src) {
            _src = _audioSys->reserveChannel(static_cast<AudioSource*>(this));
        }

        //Make sure to set channel attributes before playing
        gyAudSourceSetf(_src, GY_SOURCE_GAIN, _gain);
        gyAudSourceSetf(_src, GY_SOURCE_PITCH, _pitch);
        gyAudSourceSeti(_src, GY_SOURCE_LOOPING, _looping);
        gyAudSourcePlayWav(_src, wav);
    }

    void AudioSourceBase::stop(void) {
        if(_src) {
            gyAudSourceStop(_src);
            //give the bitch back if we're done with it
            _audioSys->releaseChannel(_src);
            _src = nullptr;
        }
    }

}
