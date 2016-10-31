#ifndef ELYSIAN_CAMERA_SYSTEM_PROXY_HPP
#define ELYSIAN_CAMERA_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <crtp/elysian_camera_system.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>

namespace OOLUA {
    typedef elysian::CameraKeyFrame     CameraKeyFrame;
    typedef elysian::CameraSystem       CameraSystem;
    typedef elysian::EventMonitor       EventMonitor;
}

OOLUA_PROXY(CameraKeyFrame, InstanceTransform)

OOLUA_PROXY_END

OOLUA_PROXY(CameraSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(void, addEntity, Entity*)
    OOLUA_MEM_FUNC_CONST(void, removeEntity, Entity*)
    OOLUA_MEM_FUNC(void, setFollowEntity, Entity*)
    OOLUA_MEM_FUNC(void, scale, float, float, cpp_in_p<EventMonitor*>)
    OOLUA_MEM_FUNC_CONST(CameraKeyFrame, getKeyFrame)
    OOLUA_MEM_FUNC(void, moveToKeyFrame, const CameraKeyFrame&, float, cpp_in_p<EventMonitor*>)
    OOLUA_MEM_FUNC(void, setBoundaries, float, float, float, float)
    OOLUA_MEM_FUNC(void, setDynamic, bool)
    OOLUA_MEM_FUNC(void, pushPerspectiveState)
    OOLUA_MEM_FUNC(void, popState)
    OOLUA_MEM_FUNC(void, shake, const GYVector3&, unsigned, float, bool, cpp_in_p<EventMonitor*>)
OOLUA_PROXY_END




#endif // ELYSIAN_CAMERA_SYSTEM_PROXY_HPP

