#ifndef ELYSIAN_MAP_SYSTEM_HPP
#define ELYSIAN_MAP_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include "elysian_map_system_base.hpp"
#else
#   include "tk_map_system.h"
namespace elysian {
    typedef TkMapSystem MapSystem;
}
typedef TkMapSystem MapSystem;

#endif



#endif // ELYSIAN_MAP_SYSTEM_HPP
