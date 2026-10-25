/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_EVENT_PROXY_HPP
#define ELYSIAN_EVENT_PROXY_HPP

#include <oolua.h>
#include <AssetIO/level/elysian_event.hpp>

namespace OOLUA {
    typedef elysian::Event          Event;
    typedef elysian::LuaEvent       LuaEvent;
    typedef elysian::EventTarget    EventTarget;
    typedef elysian::EventObserver  EventObserver;
}

OOLUA_PROXY(EventTarget)
    OOLUA_TAGS(
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getEventTargetType)
OOLUA_PROXY_END


OOLUA_PROXY(EventObserver)
    OOLUA_TAGS(
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END


OOLUA_PROXY(Event)
    OOLUA_TAGS(
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getEventType)
    OOLUA_MFUNC_CONST(getEventState)
    OOLUA_MFUNC(setEventState)
OOLUA_PROXY_END

OOLUA_PROXY(LuaEvent, Event)
    OOLUA_CTORS(
        OOLUA_CTOR(LuaVariant)
    )
    OOLUA_MFUNC_CONST(getVariant)
    OOLUA_MFUNC(setVariant)
OOLUA_PROXY_END




#endif // ELYSIAN_EVENT_PROXY_HPP
