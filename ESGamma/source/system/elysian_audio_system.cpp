#include "elysian_audio_system.hpp"
#include <crtp/elysian_audio_source.hpp>
#include "elysian_party_system.hpp"
#include <crtp/elysian_entity.hpp>
#include "elysian_timer_system.hpp"
#include <crtp/elysian_area.hpp>

namespace elysian {

    void AudioSystem::setEnabled(const bool val) {
        if(val) {
            for(unsigned i = 0; i <= AudioChannel::DYN_16; ++i) {
                gyAudSourceSetf(_channels[i].src, GY_SOURCE_GAIN, 1.0f);
            }
        } else {
            for(unsigned i = 0; i <= AudioChannel::DYN_16; ++i) {
                gyAudSourceSetf(_channels[i].src, GY_SOURCE_GAIN, 0.0f);
            }
        }
    }

	AudioSystem::AudioSystem(void) {
		AudioSource::setAudioSys(this);
		gyAudSetReferenceDistance(ELYSIAN_AUDIO_REFERENCE_DIST);
		//gyAudSetZScale(ELYSIAN_AUDIO_ZSCALE);

		//Allocate audio channels
		//and initialize channel data
		for(unsigned i = 0; i < ELYSIAN_AUDIO_CHANNELS; ++i) {
			_channels[i].src = (i >= AudioChannel::BGM_1 && i <= AudioChannel::AUX_2_TRAN)? gyAudOggStreamCreate() : gyAudSourceCreate();
			_channels[i].owner = AudioChannel::FREE;
		}

		//Give OggTracks their channel pointers
		_streamChannels[0].src = _channels[AudioChannel::BGM_1].src;
		_streamChannels[1].src = _channels[AudioChannel::BGM_1_TRAN].src;
		_streamChannels[2].src = _channels[AudioChannel::AUX_1].src;
		_streamChannels[3].src = _channels[AudioChannel::AUX_1_TRAN].src;
		_streamChannels[4].src = _channels[AudioChannel::AUX_2].src;
		_streamChannels[5].src = _channels[AudioChannel::AUX_2_TRAN].src;

		//Initialize all nondynamic sources to be nondirectional
		for(unsigned i = AudioChannel::BGM_1; i < AudioChannel::DYN_1; ++i) {
            gyAudSourceSetf(_channels[i].src, GY_SOURCE_ROLLOFF_FACTOR, 0.0f);
			gyAudOggStreamSetLooping(_channels[i].src, 1);
			//gyAudSourceSeti(_channels[i].src, GY_SOURCE_RELATIVE, 1);
		}
	}

	AudioSystem::~AudioSystem(void) {
		//Deallocate channels and their sources
		for(unsigned i = 0; i < ELYSIAN_AUDIO_CHANNELS; ++i) {
			gyAudSourceStop(_channels[i].src);
			if(i >= AudioChannel::BGM_1 && i <= AudioChannel::AUX_2) gyAudOggStreamDestroy(_channels[i].src);
			else gyAudSourceDestroy(_channels[i].src);
		}

		//Deallocate BGM tracks
	}

	void AudioSystem::update(void) {
		auto* player = getPartySys()->getPartyLeader();

        updateAreaTracks();

		//the fuck do we do otherwise?
		if(player) {
            _listenerPos = { player->_transform._pos.x, player->_transform._pos.y, player->_transform._pos.z };
			gyAudListenerSetPos(&_listenerPos);
		}

		//update AudioSources' channels
		if(_curArea) {
			for(auto it : _curArea->audioSources) {
				it->update();
			}
		}

		//update OggTracks
		for(unsigned i = 0; i < 6; ++i) {

			//if(gyAudSourceIsPlaying(_oggTracks[i].src)) {
			//if(_streamChannels[i].ogg) gyAudOggUpdate(_streamChannels[i].src, _streamChannels[i].ogg);
			//}

			/*Reested to have to do this every frame, but looping is tied to the OGG track, not the source,
			so we can't set all tracks at once when this value changes */
			/*if(i < 2) { //Do the same for AUX TRACKS!!!!!
				if(_streamChannels[i].ogg) gyAudOggSetLooping(_streamChannels[i].ogg, _bgmLooping);
			} else if(i >= 2 && i < 4) {
				if(_streamChannels[i].ogg) gyAudOggSetLooping(_streamChannels[i].ogg, _aux1Looping);
			} else {
				if(_streamChannels[i].ogg) gyAudOggSetLooping(_streamChannels[i].ogg, _aux2Looping);
			}*/

			switch(_streamChannels[i].state) {
			case StreamChannel::State::FADE_OUT:
				_streamChannels[i].fadeEllapsed += getTimerSys()->getDeltaTime();
				if(_streamChannels[i].fadeEllapsed >= _streamChannels[i].fadeTime) {
					gyAudOggStreamPause(_streamChannels[i].src);
					_streamChannels[i].fadeEllapsed = 0.0f;
					_streamChannels[i].state = StreamChannel::State::NORMAL;
					gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_GAIN, _streamChannels[i].baseGain);

					switch((StreamChannel::Type)i) {
					case StreamChannel::Type::BGM:
						if(_globalStates[TYPE_BGM].state == GlobalState::FADE_OUT) {
							_globalStates[TYPE_BGM].state = GlobalState::FADE_IN;
		//					if(_streamChannels[i].ogg)
	//							gyAudOggStreamSync(_streamChannels[i].src, _bgmTracks[_globalStates[TYPE_BGM].newTrack]);
							_streamChannels[i].ogg = _bgmTracks[_globalStates[TYPE_BGM].newTrack];
							_streamChannels[i].track = _globalStates[TYPE_BGM].newTrack;
							fadeInBgmTrack(_globalStates[TYPE_BGM].fadeTime/2.0f);

						} else if(_globalStates[TYPE_BGM].state == GlobalState::FADE_OUT_NEW) {
							_globalStates[TYPE_BGM].state = GlobalState::FADE_IN;
	//						if(_streamChannels[i].ogg)
		//						gyAudOggSync(_streamChannels[i].ogg, _bgmTracks[5]);

							if(_bgmTracks[_globalStates[TYPE_BGM].newTrack])
								gyAudOggDestroy(_bgmTracks[_globalStates[TYPE_BGM].newTrack]);
							_bgmTracks[_globalStates[TYPE_BGM].newTrack] = _bgmTracks[5];
							_bgmTracks[5] = nullptr;


							_streamChannels[i].ogg = _bgmTracks[_globalStates[TYPE_BGM].newTrack];
							_streamChannels[i].track = _globalStates[TYPE_BGM].newTrack;
							fadeInBgmTrack(_globalStates[TYPE_BGM].fadeTime/2.0f);
						}
						break;
					case StreamChannel::Type::AUX_1:
						if(_globalStates[TYPE_AUX1].state == GlobalState::FADE_OUT_NEW) {
							_globalStates[TYPE_AUX1].state = GlobalState::FADE_IN;
	//						if(_streamChannels[i].ogg)
	//							gyAudOggSync(_streamChannels[i].ogg, _auxTracks[1]);

							if(_auxTracks[_globalStates[TYPE_AUX1].newTrack])
								gyAudOggDestroy(_auxTracks[_globalStates[TYPE_AUX1].newTrack]);
							_auxTracks[_globalStates[TYPE_AUX1].newTrack] = _auxTracks[1];
							_auxTracks[1] = nullptr;

							_streamChannels[i].ogg = _auxTracks[_globalStates[TYPE_AUX1].newTrack];
							_streamChannels[i].track = _globalStates[TYPE_AUX1].newTrack;
							fadeInAuxTrack(0, _globalStates[TYPE_AUX1].fadeTime/2.0f);
						}
						break;
					case StreamChannel::Type::AUX_2:
						if(_globalStates[TYPE_AUX2].state == GlobalState::FADE_OUT_NEW) {
							_globalStates[TYPE_AUX2].state = GlobalState::FADE_IN;
//							if(_streamChannels[i].ogg)
	//							gyAudOggSync(_streamChannels[i].ogg, _auxTracks[3]);

							if(_auxTracks[_globalStates[TYPE_AUX2].newTrack])
								gyAudOggDestroy(_auxTracks[_globalStates[TYPE_AUX2].newTrack]);
							_auxTracks[_globalStates[TYPE_AUX2].newTrack] = _auxTracks[3];
							_auxTracks[3] = nullptr;

							_streamChannels[i].ogg = _auxTracks[_globalStates[TYPE_AUX2].newTrack];
							_streamChannels[i].track = _globalStates[TYPE_AUX2].newTrack;
							fadeInAuxTrack(1, _globalStates[TYPE_AUX2].fadeTime/2.0f);
						}
						break;
					default: break;
						
					}
					
				} else {
					//multiply gain by decreasing scaling factor
					float gain = _streamChannels[i].baseGain;
					gain *= (_streamChannels[i].fadeTime - _streamChannels[i].fadeEllapsed)/_streamChannels[i].fadeTime;
					gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_GAIN, gain);
				}
				break;
			case StreamChannel::State::FADE_IN:
				_streamChannels[i].fadeEllapsed += getTimerSys()->getDeltaTime();
				if(_streamChannels[i].fadeEllapsed >= _streamChannels[i].fadeTime) {
					_streamChannels[i].fadeEllapsed = 0.0f;
					_streamChannels[i].state = StreamChannel::State::NORMAL;
					gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_GAIN, _streamChannels[i].baseGain);
					StreamChannel tempChannel;

					/*If TRANSITIONAL channels are done fading in, they are supposed to be replacing
					  their primary channels as the current main track. */
					switch((StreamChannel::Type)i) {
					case StreamChannel::Type::BGM_TRAN:
						tempChannel = _streamChannels[StreamChannel::Type::BGM_TRAN];
						_streamChannels[StreamChannel::Type::BGM_TRAN] = _streamChannels[StreamChannel::Type::BGM];
						_streamChannels[StreamChannel::Type::BGM] = tempChannel;
						_streamChannels[StreamChannel::Type::BGM_TRAN].ogg = nullptr;
						_streamChannels[StreamChannel::Type::BGM_TRAN].state = StreamChannel::State::NORMAL;
						_streamChannels[StreamChannel::Type::BGM_TRAN].fadeEllapsed = 0.0f;
						//It's a temp track, replace old track
						if(_streamChannels[StreamChannel::Type::BGM].track == 5) {
							if(_bgmTracks[_streamChannels[StreamChannel::Type::BGM].newTrack])
								gyAudOggDestroy(_bgmTracks[_streamChannels[StreamChannel::Type::BGM].newTrack]);
							_bgmTracks[_streamChannels[StreamChannel::Type::BGM].newTrack] = _bgmTracks[5];
							_bgmTracks[5] = nullptr;
							_streamChannels[StreamChannel::Type::BGM].track = _streamChannels[StreamChannel::Type::BGM].newTrack;
						}
						break;
					case StreamChannel::Type::AUX_1_TRAN:
						tempChannel = _streamChannels[StreamChannel::Type::AUX_1_TRAN];
						_streamChannels[StreamChannel::Type::AUX_1_TRAN] = _streamChannels[StreamChannel::Type::AUX_1];
						_streamChannels[StreamChannel::Type::AUX_1] = tempChannel;
						_streamChannels[StreamChannel::Type::AUX_1_TRAN].ogg = nullptr;
						_streamChannels[StreamChannel::Type::AUX_1_TRAN].state = StreamChannel::State::NORMAL;
						_streamChannels[StreamChannel::Type::AUX_1_TRAN].fadeEllapsed = 0.0f;
						//It's a temp track, replace old track
						if(_streamChannels[StreamChannel::Type::AUX_1].track == 1) {
							if(_auxTracks[_streamChannels[StreamChannel::Type::AUX_1].newTrack])
								gyAudOggDestroy(_auxTracks[_streamChannels[StreamChannel::Type::AUX_1].newTrack]);
							_auxTracks[_streamChannels[StreamChannel::Type::AUX_1].newTrack] = _auxTracks[1];
							_auxTracks[1] = nullptr;
							_streamChannels[StreamChannel::Type::AUX_1].track = _streamChannels[StreamChannel::Type::AUX_1].newTrack;
						}



						break;
					case StreamChannel::Type::AUX_2_TRAN:
						tempChannel = _streamChannels[StreamChannel::Type::AUX_2_TRAN];
						_streamChannels[StreamChannel::Type::AUX_2_TRAN] = _streamChannels[StreamChannel::Type::AUX_2];
						_streamChannels[StreamChannel::Type::AUX_2] = tempChannel;
						_streamChannels[StreamChannel::Type::AUX_2_TRAN].ogg = nullptr;
						_streamChannels[StreamChannel::Type::AUX_2_TRAN].state = StreamChannel::State::NORMAL;
						_streamChannels[StreamChannel::Type::AUX_2_TRAN].fadeEllapsed = 0.0f;
						//It's a temp track, replace old track
						if(_streamChannels[StreamChannel::Type::AUX_2].track == 3) {
							if(_auxTracks[_streamChannels[StreamChannel::Type::AUX_2].newTrack])
								gyAudOggDestroy(_auxTracks[_streamChannels[StreamChannel::Type::AUX_2].newTrack]);
							_auxTracks[_streamChannels[StreamChannel::Type::AUX_2].newTrack] = _auxTracks[3];
							_auxTracks[3] = nullptr;
							_streamChannels[StreamChannel::Type::AUX_2].track = _streamChannels[StreamChannel::Type::AUX_2].newTrack;
						}

						break;
					case StreamChannel::Type::BGM:
						_globalStates[TYPE_BGM].fadeTime = 0.0f;
						_globalStates[TYPE_BGM].state = GlobalState::NORMAL;
						break;
					default: break;
					}

				} else {
					//multiply gain by increasing scaling factor
					float gain = _streamChannels[i].baseGain;
					gain *= 1.0f-(_streamChannels[i].fadeTime - _streamChannels[i].fadeEllapsed)/_streamChannels[i].fadeTime;
					gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_GAIN, gain);
				}

				break;

			default:
				break;
			}


		}

	}

	//===== INTERNAL ======

	GYAudioSource* AudioSystem::reserveChannel(AudioSource* const comp) {
		signed channel = -1;

		for(unsigned i = AudioChannel::DYN_1; i < ELYSIAN_AUDIO_CHANNELS; ++i) {
			if(_channels[i].owner == AudioChannel::FREE) {
				channel = i;
				break;
			}
		}

		//There was no free channel. Steal the bitch.
		if(channel == -1) {
			//Find channel to steal, and KINDA be fair about it
			if(++_lastStolenChannel == ELYSIAN_AUDIO_CHANNELS) _lastStolenChannel = AudioChannel::DYN_1;
			if(_channels[_lastStolenChannel].comp) _channels[_lastStolenChannel].comp->stop();

			channel = _lastStolenChannel;
		}

		if(comp) {
			_channels[channel].owner = AudioChannel::COMPONENT;
			_channels[channel].comp = comp;
		} else {
			_channels[channel].owner = AudioChannel::SYSTEM;
			_channels[channel].comp = nullptr;
		}

		return _channels[channel].src;
	}

	void AudioSystem::releaseChannel(const GYAudioSource* const src) {
		for(unsigned i = AudioChannel::DYN_1; i < ELYSIAN_AUDIO_CHANNELS; ++i) {
			if(_channels[i].src == src) {
				_channels[i].comp = nullptr;
				_channels[i].owner = AudioChannel::FREE;
				return;
			}
		}

		//uhhhh.... You're dumb as hell, but okay.
	}

	//========= SFX WAV API =======

	bool AudioSystem::isSfxChannelPlaying(const unsigned channel) const {
		return gyAudSourceIsPlaying(_channels[channel].src);
	}

	void AudioSystem::playSfxChannel(const int channel, const GYWav& wav) {
		auto *src = (channel == -1)? reserveChannel() : _channels[channel].src;
		gyAudSourcePlayWav(src, &wav);
	}

	void AudioSystem::stopSfxChannel(const unsigned channel) const {
		gyAudSourceStop(_channels[channel].src);
	}

	void AudioSystem::setSfxChannelGain(const unsigned channel, const float val) const {
		gyAudSourceSetf(_channels[channel].src, GY_SOURCE_GAIN, val);
	}

	void AudioSystem::setSfxChannelPitch(const unsigned channel, const float val) const {
		gyAudSourceSetf(_channels[channel].src, GY_SOURCE_PITCH, val);
	}

	void AudioSystem::setSfxChannelLooping(const unsigned channel, const bool val) const {
		gyAudSourceSeti(_channels[channel].src, GY_SOURCE_LOOPING, static_cast<int>(val));
	}

	void AudioSystem::setSfxChannelPos(const unsigned channel, const GYVector2 &pos, const unsigned elev) const {
        //gyAudSourceSetf(_channels[channel].src, GY_SOURCE_ROLLLOG_OFF_FACTOR, 1.0f);
        //gyAudSourceSeti(_channels[channel].src, GY_SOURCE_RELATIVE, 1);
		GYVector3 pos3 = { pos.x, pos.y, static_cast<float>(elev) };
		gyAudSourceSetfv(_channels[channel].src, GY_SOURCE_POS, &pos3);
	}

    void AudioSystem::setSfxChannelRolloffFactor(const unsigned channel, const float val) const {
        gyAudSourceSetf(_channels[channel].src, GY_SOURCE_ROLLOFF_FACTOR, val);
    }

	//======= BGM OGG API ========

	void AudioSystem::loadBgmTrack(const unsigned track, const char*const path) {
        bool validPath = (path && strcmp(path, "") != 0);

		//Track is already allocated
		if(_bgmTracks[track]) {
            if(validPath && strcmp(_bgmTracks[track]->path, path) == 0) {
                //SAME TRACK, DON'T BOTHER!!
                return;
            }

            if(isBgmTrackPlaying() && getBgmTrack()) {
                stopBgmTrack();
            }

			//Deallocate old track
			gyAudOggDestroy(_bgmTracks[track]);
		}

		//Allocate new track
        _bgmTracks[track] = validPath? gyAudOggCreate(path) : nullptr;

		if(track == _streamChannels[StreamChannel::Type::BGM].track)
			_streamChannels[StreamChannel::Type::BGM].ogg = _bgmTracks[track];
    }

	void AudioSystem::playBgmTrack(void) const {
		gyAudOggStreamPlay(_streamChannels[StreamChannel::Type::BGM].src, _streamChannels[StreamChannel::Type::BGM].ogg);
	}

	void AudioSystem::stopBgmTrack(void) const {
		gyAudOggStreamStop(_streamChannels[StreamChannel::Type::BGM].src);
	}

	void AudioSystem::pauseBgmTrack(void) const {
		gyAudOggStreamPause(_streamChannels[StreamChannel::Type::BGM].src);
	}

	bool AudioSystem::isBgmTrackPlaying(void) const {
		return (gyAudSourceIsPlaying(_streamChannels[StreamChannel::Type::BGM].src));
	}

	unsigned AudioSystem::getBgmTrack(void) const {
		return _streamChannels[StreamChannel::Type::BGM].track;
	}

	void AudioSystem::setBgmTrack(const unsigned track) {
		bool play = false;

		if(isBgmTrackPlaying()) {
			pauseBgmTrack();
			play = true;
		}

		_streamChannels[StreamChannel::Type::BGM].track = track;
		_streamChannels[StreamChannel::Type::BGM].ogg = _bgmTracks[track];

		if(play) playBgmTrack();
	}

	void AudioSystem::fadeOutBgmTrack(const float secs) {
		_streamChannels[StreamChannel::Type::BGM].fadeTime = secs;
		_streamChannels[StreamChannel::Type::BGM].state = StreamChannel::State::FADE_OUT;
	}

	void AudioSystem::fadeInBgmTrack(const float secs) {
		_streamChannels[StreamChannel::Type::BGM].fadeTime = secs;
		_streamChannels[StreamChannel::Type::BGM].state = StreamChannel::State::FADE_IN;
		playBgmTrack();
	}

	void AudioSystem::setBgmTrackLooping(const bool val) {
		_bgmLooping = val;
	}

	const char* AudioSystem::getBgmTrackPath(const unsigned track) const {
		return _bgmTracks[track]->path;
	}

	void AudioSystem::setBgmTrackPos(const float x, const float y, const float z) const {
		GYVector3 pos3 = { x, y, z };
		for(unsigned i = 0; i < 2; ++i) {
			//gyAudSourceSetf(_channels[i].src, GY_SOURCE_ROLLLOG_OFF_FACTOR, 1.0f);
			gyAudSourceSetfv(_streamChannels[i].src, GY_SOURCE_POS, &pos3);
		}
	}

	void AudioSystem::setBgmTrackRolloffFactor(const float val) const {
		for(unsigned i = 0; i < 2; ++i) {
            gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_ROLLOFF_FACTOR, val);
		}
	}

	void AudioSystem::setBgmTrackGain(const float val) {
		for(unsigned i = 0; i < 2; ++i) {
			_streamChannels[i].baseGain = val;
			gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_GAIN, val);
		}
	}


	void AudioSystem::setBgmTrackPitch(const float val) const {
		for(unsigned i = 0; i < 2; ++i) {
			gyAudSourceSetf(_streamChannels[i].src, GY_SOURCE_PITCH, val);
		}
	}

	double AudioSystem::getBgmTrackTime(void) const {
		return gyAudOggStreamTime(_streamChannels[StreamChannel::Type::BGM].src);
	}

	void AudioSystem::morphIntoBgmTrack(const unsigned track, const float secs) {
		if(_streamChannels[StreamChannel::Type::BGM].track == track || !_bgmTracks[track]) return;

		//gyAudOggSync(_streamChannels[StreamChannel::Type::BGM].ogg, _bgmTracks[track]);
		fadeOutBgmTrack(secs);

		_streamChannels[StreamChannel::Type::BGM_TRAN].ogg = _bgmTracks[track];
		_streamChannels[StreamChannel::Type::BGM_TRAN].state = StreamChannel::State::FADE_IN;
		_streamChannels[StreamChannel::Type::BGM_TRAN].fadeTime = secs;
		_streamChannels[StreamChannel::Type::BGM_TRAN].track = track;
		gyAudOggStreamPlay(_streamChannels[StreamChannel::Type::BGM_TRAN].src, _streamChannels[StreamChannel::Type::BGM_TRAN].ogg);
	}


	void AudioSystem::fadeIntoBgmTrack(const unsigned track, const float secs) {
		if(_streamChannels[StreamChannel::Type::BGM].track == track || !_bgmTracks[track]) return;
		_globalStates[TYPE_BGM].state = GlobalState::FADE_OUT;
		_globalStates[TYPE_BGM].newTrack = track;
		_globalStates[TYPE_BGM].fadeTime = secs;

		fadeOutBgmTrack(secs/2.0f);
	}


	void AudioSystem::morphIntoNewBgmTrack(const unsigned track, const float secs, const char* path) {
		//Temp track is already allocated
		if(_bgmTracks[5]) {
			//Deallocate old track
			gyAudOggDestroy(_bgmTracks[5]);
		}

		//Allocate new track
		_bgmTracks[5] = gyAudOggCreate(path);

	//	if(_streamChannels[StreamChannel::Type::BGM].ogg)
		//	gyAudOggSync(_streamChannels[StreamChannel::Type::BGM].ogg, _bgmTracks[5]);
		fadeOutBgmTrack(secs);

		_streamChannels[StreamChannel::Type::BGM_TRAN].ogg = _bgmTracks[5];
		_streamChannels[StreamChannel::Type::BGM_TRAN].state = StreamChannel::State::FADE_IN;
		_streamChannels[StreamChannel::Type::BGM_TRAN].fadeTime = secs;
		_streamChannels[StreamChannel::Type::BGM_TRAN].track = 5;
		_streamChannels[StreamChannel::Type::BGM_TRAN].newTrack = track;
		gyAudOggStreamPlay(_streamChannels[StreamChannel::Type::BGM_TRAN].src, _streamChannels[StreamChannel::Type::BGM_TRAN].ogg);

	}

    void AudioSystem::fadeIntoNewBgmTrack(const unsigned track, const float secs, const char* path) {
        if(path) {
            if(_bgmTracks[track]) {
                if(strcmp(path, _bgmTracks[track]->path) == 0) {
                    return; //Already have this track loaded. Nothing to do here.
                }
            }
        } else if(!_bgmTracks[track]) return; //both tracks are empty, nothing to do

		//Temp track is already allocated
		if(_bgmTracks[5]) {
			//Deallocate old track
			gyAudOggDestroy(_bgmTracks[5]);
		}

		//Allocate new track
		_bgmTracks[5] = gyAudOggCreate(path);

		_globalStates[TYPE_BGM].state = GlobalState::FADE_OUT_NEW;
		_globalStates[TYPE_BGM].newTrack = track;
		_globalStates[TYPE_BGM].fadeTime = secs;

		fadeOutBgmTrack(secs/2.0f);


	}


	//======= AUX OGG API =========

	inline unsigned AudioSystem::_auxIndexToStreamChannel(const unsigned index) {
		return (index == 0)?
				StreamChannel::Type::AUX_1 :
				StreamChannel::Type::AUX_2;
	}

	inline unsigned AudioSystem::_auxIndexToTrack(const unsigned index) {
		return (index == 0)? 0 : 2;
	}

	void AudioSystem::loadAuxTrack(const unsigned track, const char*const path) {
        bool validPath = (path && strcmp(path, "") != 0);
		const unsigned oggTrack = _auxIndexToTrack(track);

		//Track is already allocated
		if(_auxTracks[oggTrack]) {
            if(validPath && strcmp(_auxTracks[track]->path, path) == 0) {
                //SAME TRACK, DON'T BOTHER!!
                return;
            }

            if(isAuxTrackPlaying(track)) {
                stopAuxTrack(track);
            }

			//Deallocate old track
			gyAudOggDestroy(_auxTracks[oggTrack]);
		}

		//Allocate new track
        _auxTracks[oggTrack] = validPath? gyAudOggCreate(path) : nullptr;

		const unsigned channel = _auxIndexToStreamChannel(track);
		_streamChannels[channel].ogg = _auxTracks[oggTrack];
		_streamChannels[channel].track = oggTrack;
	}

	void AudioSystem::playAuxTrack(const unsigned track) const {
		const unsigned channel = _auxIndexToStreamChannel(track);
		gyAudOggStreamPlay(_streamChannels[channel].src, _streamChannels[channel].ogg);
	}

	void AudioSystem::stopAuxTrack(const unsigned track) const {
		const unsigned channel = _auxIndexToStreamChannel(track);
		gyAudOggStreamStop(_streamChannels[channel].src);
	}

	void AudioSystem::pauseAuxTrack(const unsigned track) const {
		const unsigned channel = _auxIndexToStreamChannel(track);
		gyAudOggStreamPause(_streamChannels[channel].src);
	}

	bool AudioSystem::isAuxTrackPlaying(const unsigned track) const {
		return (gyAudSourceIsPlaying(_streamChannels[_auxIndexToStreamChannel(track)].src));
	}

	const char*	AudioSystem::getAuxTrackPath(const unsigned track) const {
		return _auxTracks[_auxIndexToTrack(track)]->path;
	}

	void AudioSystem::fadeOutAuxTrack(const unsigned track, const float secs) {
		const unsigned channel = _auxIndexToStreamChannel(track);
		_streamChannels[channel].fadeTime = secs;
		_streamChannels[channel].state = StreamChannel::State::FADE_OUT;
	}

	void AudioSystem::fadeInAuxTrack(const unsigned track, const float secs) {
		const unsigned channel = _auxIndexToStreamChannel(track);
		_streamChannels[channel].fadeTime = secs;
		_streamChannels[channel].state = StreamChannel::State::FADE_IN;
		playAuxTrack(track);
	}


	void AudioSystem::morphIntoNewAuxTrack(const unsigned track, const float secs, const char* path) {
		//Temp track is already allocated
		const unsigned oggTrack = _auxIndexToTrack(track)+1;
		const unsigned channel = _auxIndexToStreamChannel(track);

		if(_auxTracks[oggTrack]) {
			//Deallocate old track
			gyAudOggDestroy(_bgmTracks[oggTrack]);
		}

		//Allocate new track
		_bgmTracks[oggTrack] = gyAudOggCreate(path);

//		if(_streamChannels[channel].ogg)
//			gyAudOggSync(_streamChannels[channel].ogg, _bgmTracks[oggTrack]);
		fadeOutAuxTrack(track, secs);

		_streamChannels[channel+1].ogg = _bgmTracks[oggTrack];
		_streamChannels[channel+1].state = StreamChannel::State::FADE_IN;
		_streamChannels[channel+1].fadeTime = secs;
		_streamChannels[channel+1].track = oggTrack;
		_streamChannels[channel+1].newTrack = track;
		gyAudOggStreamPlay(_streamChannels[channel+1].src, _streamChannels[channel+1].ogg);

	}

	void AudioSystem::fadeIntoNewAuxTrack(const unsigned track, const float secs, const char *path) {
		//Temp track is already allocated
		const unsigned oggTrack = _auxIndexToTrack(track)+1;
//		const unsigned channel = _auxIndexToStreamChannel(track);

        if(path) {
            if(_auxTracks[oggTrack]) {
                if(strcmp(path, _auxTracks[oggTrack]->path) == 0) {
                    return; //Already have this track loaded. Nothing to do here.
                }
            }
        } else if(!_auxTracks[oggTrack]) return; //both tracks are empty, nothing to do

		//Temp track is already allocated
		if(_auxTracks[oggTrack]) {
			//Deallocate old track
			gyAudOggDestroy(_auxTracks[oggTrack]);
		}

		//Allocate new track
		_auxTracks[oggTrack] = gyAudOggCreate(path);

		_globalStates[TYPE_AUX1+track].state = GlobalState::FADE_OUT_NEW;
		_globalStates[TYPE_AUX1+track].newTrack = oggTrack-1;
		_globalStates[TYPE_AUX1+track].fadeTime = secs;

		fadeOutAuxTrack(track, secs/2.0f);

	}

	void AudioSystem::setAuxTrackLooping(const unsigned track, const bool val) {
		if(track == 0)
			_aux1Looping = val;
		else
			_aux2Looping = val;
	}

	void AudioSystem::setAuxTrackGain(const unsigned track, const float val) {
		if(track == 0) {
			_streamChannels[StreamChannel::Type::AUX_1].baseGain = val;
			_streamChannels[StreamChannel::Type::AUX_1_TRAN].baseGain = val;
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_1].src, GY_SOURCE_GAIN, val);
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_1_TRAN].src, GY_SOURCE_GAIN, val);
		} else {
			_streamChannels[StreamChannel::Type::AUX_2].baseGain = val;
			_streamChannels[StreamChannel::Type::AUX_2_TRAN].baseGain = val;
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_2].src, GY_SOURCE_GAIN, val);
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_2_TRAN].src, GY_SOURCE_GAIN, val);
		}
	}

	void AudioSystem::setAuxTrackPitch(const unsigned track, const float val) const {
		if(track == 0) {
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_1].src, GY_SOURCE_PITCH, val);
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_1_TRAN].src, GY_SOURCE_PITCH, val);
		} else {
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_2].src, GY_SOURCE_PITCH, val);
			gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_2_TRAN].src, GY_SOURCE_PITCH, val);
		}
	}

	void AudioSystem::setAuxTrackPos(const unsigned track, const float x, const float y, const float z) const {
		GYVector3 pos3 = { x, y, z  };
		if(track == 0) {
			gyAudSourceSetfv(_streamChannels[StreamChannel::Type::AUX_1].src, GY_SOURCE_POS, &pos3);
			gyAudSourceSetfv(_streamChannels[StreamChannel::Type::AUX_1_TRAN].src, GY_SOURCE_POS, &pos3);
		} else {
			gyAudSourceSetfv(_streamChannels[StreamChannel::Type::AUX_2].src, GY_SOURCE_POS, &pos3);
			gyAudSourceSetfv(_streamChannels[StreamChannel::Type::AUX_2_TRAN].src, GY_SOURCE_POS, &pos3);
		}
	}

	void AudioSystem::setAuxTrackRolloffFactor(const unsigned track, const float val) const {
		if(track == 0) {
            gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_1].src, GY_SOURCE_ROLLOFF_FACTOR, val);
            gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_1_TRAN].src, GY_SOURCE_ROLLOFF_FACTOR, val);
		} else {
            gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_2].src, GY_SOURCE_ROLLOFF_FACTOR, val);
            gyAudSourceSetf(_streamChannels[StreamChannel::Type::AUX_2_TRAN].src, GY_SOURCE_ROLLOFF_FACTOR, val);
		}
	}

	double AudioSystem::getAuxTrackTime(const unsigned track) const {
		if(track == 0) {
			return gyAudOggStreamTime(_streamChannels[StreamChannel::Type::AUX_1].src);
		} else {
			return gyAudOggStreamTime(_streamChannels[StreamChannel::Type::AUX_2].src);
		}
	}

    void AudioSystem::updateAreaTracks(void) {
        for(unsigned i = 0; i < 3; ++i) {
            fadeIntoNewBgmTrack(i, 1.0f, getCurrentArea()->getBgmOggByTrack(i));
        }

        for(unsigned i = 0; i < 2; ++i) {
            fadeIntoNewAuxTrack(i, 1.0f, getCurrentArea()->getAuxOggByTrack(i));
        }
    }
}
