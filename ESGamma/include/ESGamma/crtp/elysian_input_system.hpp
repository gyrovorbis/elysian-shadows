/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_INPUT_SYSTEM_HPP
#define ELYSIAN_INPUT_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <system/elysian_input_system_base.hpp>
namespace elysian {
    using InputSystem = InputSystemBase;
}
#else

#   include <ESToolkit/ESGamma/tk_input_system.h>

namespace elysian {
    using InputSystem = TkInputSystem;
}
using InputSystem = TkInputSystem;

#endif




#endif // ELYSIAN_INPUT_SYSTEM_HPP

