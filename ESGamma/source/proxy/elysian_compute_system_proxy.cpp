/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/proxy/elysian_compute_system_proxy.hpp>

/*!
    \qmltype ComputeSystem
    \since 1.6.1
    \inmodule Engine
    \brief Engine subsystem for managing parallel tasks utilizing GPGPU/hardware acceleraton

    \section2 Retrieving the ComputeSystem
    The ComputeSystem object can be retrieved through the static \l{Engine} class:
    \code
    computeSys = Engine.getComputeSys()
    \endcode

    \sa Engine
*/

OOLUA_EXPORT_FUNCTIONS(ComputeSystem)
OOLUA_EXPORT_FUNCTIONS_CONST(ComputeSystem)
