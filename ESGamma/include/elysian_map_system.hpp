#ifndef ELYSIAN_MAP_SYSTEM_HPP
#define ELYSIAN_MAP_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include "elysian_map_system_base.hpp"
namespace elysian {
    typedef MapSystemBase MapSystem;
}
#else
#   include "tkMapSystem.h"
namespace elysian {
    typedef TkMapSystem MapSystem;
}
typedef TkMapSystem MapSystem;

#endif



#endif // ELYSIAN_MAP_SYSTEM_HPP
