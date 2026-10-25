/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TKAUDIOSOURCE_H
#define TKAUDIOSOURCE_H

#include <AssetIO/component/elysian_audio_source_base.hpp>

class TkAudioSource: public elysian::AudioSourceBase {

public:

    TkAudioSource(TkEntity& parent, const TkAudioSource*const prefab=nullptr);
    TkAudioSource(void) = default;
    //virtual ~TkAnimation(void) = default;
};


inline TkAudioSource::TkAudioSource(TkEntity& entity, const TkAudioSource*const prefab): elysian::AudioSourceBase(entity, prefab) {}


#endif // TKAUDIOSOURCE_H

