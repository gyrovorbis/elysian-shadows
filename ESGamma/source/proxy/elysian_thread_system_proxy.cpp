/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/proxy/elysian_thread_system_proxy.hpp>

/*!
    \qmltype ThreadSystem
    \since 1.6.1
    \inmodule Engine
    \brief Engine subsystem responsible for managing CPU thread parallelization

    \section2 Thread Task Queues

    \section2 Retrieving the ThreadSystem
    The ThreadSystem object can be retrieved through the static \l{Engine} class:
    \code
    threadSys = Engine.getThreadSys()
    \endcode

    \sa Engine
*/

OOLUA_EXPORT_FUNCTIONS(ThreadSystem)
OOLUA_EXPORT_FUNCTIONS_CONST(ThreadSystem)
