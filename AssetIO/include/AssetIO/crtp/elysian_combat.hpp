/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_COMBAT_HPP
#define ELYSIAN_COMBAT_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_combat_base.hpp>
namespace elysian {
    typedef CombatBase Combat;
}
#else
#   include <ESToolkit/AssetIO/tk_combat.h>
namespace elysian {
    typedef TkCombat Combat;
}
typedef TkCombat Combat;

#endif

#endif // ELYSIAN_COMBAT_HPP

