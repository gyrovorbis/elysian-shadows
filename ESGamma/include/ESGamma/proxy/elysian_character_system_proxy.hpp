#ifndef ELYSIAN_CHARACTER_SYSTEM_PROXY_HPP
#define ELYSIAN_CHARACTER_SYSTEM_PROXY_HPP

#include <ESGamma/system/elysian_character_system.hpp>

namespace OOLUA {
    typedef elysian::CharacterSystem    CharacterSystem;
    typedef elysian::EventMonitor       EventMonitor;
    typedef elysian::Entity             Entity;

}

OOLUA_PROXY(CharacterSystem)
OOLUA_TAGS (
    No_public_constructors,
    No_public_destructor
)
OOLUA_MEM_FUNC(void, walkToWayPoint, Entity*, float, float, cpp_in_p<EventMonitor*>)
OOLUA_MEM_FUNC(void, moveToWayPoint, Entity*, float, float, float, cpp_in_p<EventMonitor*>)
OOLUA_PROXY_END

#endif // ELYSIAN_CHARACTER_SYSTEM_PROXY_HPP

