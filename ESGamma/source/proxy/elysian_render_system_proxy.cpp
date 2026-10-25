/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/proxy/elysian_render_system_proxy.hpp>

/*!
    \qmltype RenderSystem
    \since 1.6.1
    \inmodule Engine
    \brief Engine subsystem responsible for all graphics and rendering

    \section2 Post-Processing Effects Pipeline

    \section2 Retrieving the RenderSystem
    The RenderSystem object can be retrieved through the static \l{Engine} class:
    \code
    renderSys = Engine.getRenderSys()
    \endcode

    \sa Engine
*/

OOLUA_EXPORT_FUNCTIONS(RenderSystem)
OOLUA_EXPORT_FUNCTIONS_CONST(RenderSystem)

