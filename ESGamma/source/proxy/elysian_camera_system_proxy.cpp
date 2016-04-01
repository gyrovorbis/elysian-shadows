#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_camera_system_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS_CONST(CameraKeyFrame)
OOLUA_EXPORT_FUNCTIONS(CameraKeyFrame)

OOLUA_EXPORT_FUNCTIONS(CameraSystem,
                       scale,
                       moveToKeyFrame,
                       setBoundaries,
                       setDynamic,
                       pushPerspectiveState,
                       popState,
                       shake,
                       setFollowEntity)
OOLUA_EXPORT_FUNCTIONS_CONST(CameraSystem,
                       addEntity,
                       removeEntity,
                       getKeyFrame)
