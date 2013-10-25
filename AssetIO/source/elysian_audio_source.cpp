#include <algorithm>
#include "elysian_audio_source.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"

namespace elysian {

	AudioSystem* AudioSource::_audioSys = nullptr;

	bool AudioSource::load(std::ifstream& /*stream*/) {
		//implement this bitch later
		return true;
	}

	bool AudioSource::save(std::ofstream& /*stream*/) {
		//implement this bitch later
		return true;
	}

	void AudioSource::update(void) {
#ifdef ELYSIAN_ENGINE
		if(_src) {
			if(gyAudSourceIsPlaying(_src)) {
				auto pos2 = _entity->transform._pos;
				GYVector3 pos3 = { pos2.x, pos2.y, static_cast<float>(_entity->getElev()) };
				gyAudSourceSetfv(_src, GY_SOURCE_POS, &pos3);
			} else {
				//give the bitch back if we're done with it
				_audioSys->releaseChannel(_src);
				_src = nullptr;
			}
		}
#endif
	}

	//O(1)
	bool AudioSource::addToLevel(void) {
		_curArea->audioSources.push_back(this);
		return true;
	}

	//O(n)
	bool AudioSource::removeFromLevel(void) {
		auto &comp = _curArea->audioSources;
		auto it = std::find(comp.begin(), comp.end(), this);

		if(it == comp.end()) {
			return false;
		} else {
			comp.erase(it);
			return true;
		}
	}
}
