#ifndef ELYSIAN_CAMERA_SYSTEM_HPP
#define ELYSIAN_CAMERA_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <system/elysian_camera_system_base.hpp>
namespace elysian {
    typedef CameraSystemBase CameraSystem;
}
#else
#   include <tk_camera_system.h>
namespace elysian {
    typedef TkCameraSystem CameraSystem;
}
typedef TkCameraSystem CameraSystem;

#endif

#endif // ELYSIAN_CAMERA_SYSTEM_HPP

