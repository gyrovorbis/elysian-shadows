/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_CAMERA_SYSTEM_HPP
#define ELYSIAN_CAMERA_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <system/elysian_camera_system_base.hpp>
namespace elysian {
    typedef CameraSystemBase CameraSystem;
}
#else
#   include <ESToolkit/ESGamma/tk_camera_system.h>
namespace elysian {
    typedef TkCameraSystem CameraSystem;
}
typedef TkCameraSystem CameraSystem;

#endif

#endif // ELYSIAN_CAMERA_SYSTEM_HPP

