/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_EXAMINABLE_SYSTEM_HPP
#define ELYSIAN_EXAMINABLE_SYSTEM_HPP

#include <ESGamma/system/elysian_system.hpp>

#define ELYSIAN_EXAMINABLE_SYSTEM_EXAMINE_SFX "/assets/audio/sfx/UI/examine.wav"

struct GYWav;

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class ExaminableSystem: public System {
	private:
        GYWav* _examineSfx;
        Entity* _curEntity = nullptr;

	public:
        bool initialize(void);
        void uninitialize(void);

        void processInput(void);


	};

}

#endif // ELYSIAN_EXAMINABLE_SYSTEM_HPP
