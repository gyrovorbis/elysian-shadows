/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <vector>
#include <algorithm>
#include <AssetIO/crtp/elysian_audio_source.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <AssetIO/utility/elysian_generic.hpp>
#include <AssetIO/utility/elysian_asset_typedefs.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <ESGamma/proxy/elysian_event_monitor_proxy.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AudioSourceBase)

    AudioSystem* AudioSourceBase::_audioSys = nullptr;

    void AudioSourceBase::update(void) {
		if(_src) {
            //Give the channel back to be dynamically recycled if it's not being used!
            if(!_audioSys->isSfxChannelPlaying(_channelIndex)) {
                stop();
            } else {
                auto& pos = _entity->_transform._pos;
                _audioSys->setSfxChannelPos(_channelIndex, pos.x, pos.y, pos.z);
            }
		}
	}

    void AudioSourceBase::stop(void) {
        if(_src) {
            _audioSys->stopSfxChannel(_channelIndex);
            //give the bitch back if we're done with it
            _audioSys->releaseChannel(_src);
            _src = nullptr;
        }
    }

    void AudioSourceBase::_reserveChannel(void) {
        if(!_src) {
            _src = _audioSys->reserveChannel(static_cast<AudioSource*>(this));
            _channelIndex = _audioSys->sourceToChannelIndex(_src);
        }
    }

    void AudioSourceBase::_releaseChannel(void) {
        if(_src) {
            stop();
            _audioSys->releaseChannel(_src);
            _src = nullptr;
            _channelIndex = -1;
        }
    }

    void AudioSourceBase::playLua(LuaVariant v) {
        play(v.getValue<EventMonitor*>());
    }

    void AudioSourceBase::play(EventMonitor* monitor) {
        if(_wav) {
            //Reserve a channel if we haven't already
            if(!_src) {
                _reserveChannel();
            }

            //Make sure to set channel attributes before playing
            _audioSys->setSfxChannelGain(_channelIndex, _gain);
            _audioSys->setSfxChannelPitch(_channelIndex, _pitch);

            _audioSys->setSfxChannelLooping(_channelIndex, _looping);

            _audioSys->playSfxChannel(_channelIndex, *_wav, monitor);

                        _audioSys->setSfxChannelRolloffFactor(_channelIndex, _rolloffFactor);
        }

#ifdef ELYSIAN_AUDIO_SOURCE_DEBUG
        else {
            errprintf("Failed to play WAV on Entity[%s].audioSource: WAV not loaded!", getEntity()->getName());
        }
#endif
    }

    bool AudioSourceBase::isPlaying(void) const {
        return _src? _audioSys->isSfxChannelPlaying(_channelIndex) : false;
    }

    void AudioSourceBase::setWavFile(const char* filepath) {
        char buff[GYRO_PATH_MAX_SIZE];
        stop();
        gyAudWavDestroy(_wav);
        if(filepath) {
            strcpy(_wavFile, filepath);
            makeFullPath(buff, filepath);
            _wav = gyAudWavCreate(buff);
        } else {
            _wav = nullptr;
            _wavFile[0] = '\0';
        }
    }

    void AudioSourceBase::makeFullPath(char* buffer, const char* wavfile) const {
        if(strstr(wavfile, "/assets/")) {
            strcpy(buffer, wavfile);
        } else {
            strcpy(buffer, _basePath);
            strcat(buffer, "/");
            strcat(buffer, wavfile);
            strcat(buffer, "_00.wav");
        }
    }

    void AudioSourceBase::setGain(const float val) {
        _gain = val;
        if(_src) _audioSys->setSfxChannelGain(_channelIndex, val);
    }

    void AudioSourceBase::setPitch(const float val) {
        _pitch = val;
        if(_src) _audioSys->setSfxChannelPitch(_channelIndex, val);
    }

    void AudioSourceBase::setLooping(const bool val)  {
        _looping = val;
        if(_src) _audioSys->setSfxChannelLooping(_channelIndex, val);
    }

    void AudioSourceBase::setRolloffFactor(float factor) {
        _rolloffFactor = factor;
        if(_src) _audioSys->setSfxChannelRolloffFactor(_channelIndex, factor);
    }

    void AudioSourceBase::setAudioSys(AudioSystem* const sys) {
        _audioSys = sys;
    }


}


