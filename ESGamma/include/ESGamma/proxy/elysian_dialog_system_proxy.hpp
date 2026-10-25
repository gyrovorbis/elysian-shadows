/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_DIALOG_SYSTEM_PROXY_HPP
#define ELYSIAN_DIALOG_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/system/elysian_dialog_system.hpp>


namespace OOLUA {
    typedef elysian::DialogSystem   DialogSystem;
    typedef elysian::Dialog         Dialog;
    typedef elysian::EventMonitor   EventMonitor;

}

OOLUA_PROXY(DialogSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, startDialog, std::shared_ptr<Dialog>, cpp_in_p<EventMonitor*>)
OOLUA_PROXY_END


#endif // ELYSIAN_DIALOG_SYSTEM_PROXY_HPP

