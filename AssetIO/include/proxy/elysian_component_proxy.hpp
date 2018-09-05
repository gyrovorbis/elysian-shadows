#ifndef ELYSIAN_COMPONENT_PROXY_HPP
#define ELYSIAN_COMPONENT_PROXY_HPP

#include <oolua.h>
#include <AssetIO/crtp/elysian_component.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>

namespace OOLUA {

    typedef elysian::Component  Component;
    typedef elysian::Entity     Entity;

    template<> class Proxy_class<Entity>;
}

OOLUA_PROXY(Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(Entity*, getEntity)
OOLUA_PROXY_END


#endif // ELYSIAN_COMPONENT_PROXY_HPP

