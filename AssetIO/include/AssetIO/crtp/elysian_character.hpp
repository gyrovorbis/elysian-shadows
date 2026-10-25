/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_CHARACTER_HPP
#define ELYSIAN_CHARACTER_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_character_base.hpp>
namespace elysian {
    typedef CharacterBase Character;
}
#else
#   include <ESToolkit/AssetIO/tk_character.h>
namespace elysian {
    typedef TkCharacter Character;
}
typedef TkCharacter Character;

#endif


#endif // ELYSIAN_CHARACTER_HPP

