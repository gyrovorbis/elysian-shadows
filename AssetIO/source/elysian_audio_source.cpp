#include <algorithm>
#include "elysian_audio_source.hpp"
#include "elysian_generic.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AudioSource)

	AudioSystem* AudioSource::_audioSys = nullptr;

	bool AudioSource::load(FILE* /*file*/) {
		//implement this bitch later
		return true;
	}

	bool AudioSource::save(FILE* /*file*/) {
		//implement this bitch later
		return true;
	}

	void AudioSource::update(void) {
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

	//O(1)
	bool AudioSource::addToLevel(void) {
		_curArea->audioSources.push_back(this);
		return true;
	}

	//O(n)
	bool AudioSource::removeFromLevel(void) {
        return containerEraseOnMatch(this, _curArea->audioSources);
	}
}
