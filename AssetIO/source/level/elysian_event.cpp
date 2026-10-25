/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <AssetIO/level/elysian_event.hpp>

namespace elysian {

EVENT_TARGET_TYPE EventTarget::getEventTargetType(void) const {
    return EVENT_TARGET_INVALID;
}

void EventTarget::onReceiveEvent(Event *event) {}

void EventObserver::onObserveEvent(Event *event, EventTarget *receiver) {}

}
