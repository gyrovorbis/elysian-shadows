/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_EXAMINABLE_SYSTEM_PROXY_HPP
#define ELYSIAN_EXAMINABLE_SYSTEM_PROXY_HPP

#include <ESGamma/system/elysian_examinable_system.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::ExaminableSystem     ExaminableSystem;
}

OOLUA_PROXY(ExaminableSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

#endif // ELYSIAN_EXAMINABLE_SYSTEM_PROXY_HPP
