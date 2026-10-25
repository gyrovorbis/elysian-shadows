/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/proxy/elysian_examinable_system_proxy.hpp>

/*!
    \qmltype ExaminableSystem
    \since 1.6.1
    \inmodule Engine
    \brief Engine subsystem handling examining/interacting with other Entities

    The ExaminableSystem is the \l{Engine} subsystem responsible for managing
    all active \l{Examinable} \l{Component}s within the current \l{Area}. It handles
    initiating \l{Dialog} or events triggered by examining or interacting with
    another Entity.

    \section2 Retrieving the ExaminableSystem
    The CharacterSystem object can be retrieved through the static Engine class:
    \code
    charSys = Engine.getExamSys()
    \endcode
    \sa {Examinable}, {Dialog}, {DialogSystem}, {Engine}
*/

OOLUA_EXPORT_NO_FUNCTIONS(ExaminableSystem)
