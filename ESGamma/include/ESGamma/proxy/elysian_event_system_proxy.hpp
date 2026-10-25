/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_EVENT_SYSTEM_PROXY
#define ELYSIAN_EVENT_SYSTEM_PROXY

#include <AssetIO/level/elysian_event.hpp>
#include <ESGamma/system/elysian_event_system.hpp>
#include <AssetIO/proxy/elysian_event_proxy.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::EventType      EventType;
    typedef elysian::EventSystem    EventSystem;
}

OOLUA_PROXY(EventSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, sendEvent, Event*, EventTarget*)
    OOLUA_MEM_FUNC(void, emitEvent, Event*, EventTarget*)
    OOLUA_MEM_FUNC(void, broadcastEvent, Event*)
    OOLUA_MEM_FUNC(void, addEventObserverLua, EventObserver*, EventType, LuaVariant)
    OOLUA_MEM_FUNC(void, removeEventObserverLua, EventObserver*, EventType, LuaVariant)
    OOLUA_MEM_FUNC_RENAME(addEventObserver, void, addEventObserverLua, EventObserver*, EventType, LuaVariant)
    OOLUA_MEM_FUNC_RENAME(removeEventObserver, void, removeEventObserverLua, EventObserver*, EventType, LuaVariant)
OOLUA_PROXY_END

#endif
