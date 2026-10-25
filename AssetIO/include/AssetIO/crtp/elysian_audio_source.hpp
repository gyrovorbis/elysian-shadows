/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_AUDIO_SOURCE_HPP
#define ELYSIAN_AUDIO_SOURCE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_audio_source_base.hpp>
namespace elysian {
    typedef AudioSourceBase AudioSource;
}
#else
#   include <ESToolkit/AssetIO/tk_audio_source.h>
namespace elysian {
    typedef TkAudioSource AudioSource;
}
typedef TkAudioSource AudioSource;

#endif


#endif // ELYSIAN_AUDIO_SOURCE_HPP

