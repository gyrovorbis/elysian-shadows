/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LIGHT_SYSTEM_PROXY_HPP
#define ELYSIAN_LIGHT_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/system/elysian_light_system.hpp>

namespace OOLUA {
    typedef elysian::LightSystem    LightSystem;
    typedef elysian::LightAttrib    LightAttrib;
    typedef elysian::EventMonitor   EventMonitor;

}

OOLUA_PROXY(LightSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, interpLightAttrib, float, const LightAttrib&, LightAttrib*, cpp_in_p<EventMonitor*>)
OOLUA_PROXY_END

#endif // ELYSIAN_LIGHT_SYSTEM_PROXY_HPP

