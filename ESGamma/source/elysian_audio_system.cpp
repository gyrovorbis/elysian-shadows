#include "elysian_audio_system.hpp"
#include "elysian_audio_source.hpp"
#include "elysian_area.hpp"
#include "elysian_party_system.hpp"
#include "elysian_entity.hpp"

namespace elysian {

	AudioSystem::AudioSystem(void) {
		gyAudSetReferenceDistance(ELYSIAN_AUDIO_REFERENCE_DIST);
		gyAudSetZScale(ELYSIAN_AUDIO_ZSCALE);
	}

	void AudioSystem::update(void) {
		auto* player = getPartySys()->getPartyLeader();

		if(player) {
			GYVector3 pos3 = { player->transform._pos.x, player->transform._pos.y, static_cast<float>(player->getElev()) };
			gyAudListenerSetPos(&pos3);
		}
		if(_curArea)
			for(auto it : _curArea->audioSources) {
				it->update();
			}
	}

	void AudioSystem::playWav(const GYWav& wav) const {

	}

	void AudioSystem::playWavFromEntity(const Entity& entity, const GYWav& wav) const {

	}

	void AudioSystem::playWavFromPos(const float x, const float y, const float z, const GYWav& wav) const {

	}

}
