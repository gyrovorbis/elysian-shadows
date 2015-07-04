#ifndef ELYSIAN_ANIMATION_SYSTEM_PROXY_HPP
#define ELYSIAN_ANIMATION_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <system/elysian_animation_system.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>

namespace OOLUA {
    typedef elysian::AnimationSystem    AnimationSystem;

}

OOLUA_PROXY(AnimationSystem)
OOLUA_TAGS (
    No_public_constructors,
    No_public_destructor
)
OOLUA_MEM_FUNC(void, linearInterpTransform, InstanceTransform*, const InstanceTransform&, float, cpp_in_p<EventMonitor*>)
OOLUA_PROXY_END


#endif // ELYSIAN_ANIMATION_SYSTEM_PROXY_HPP

