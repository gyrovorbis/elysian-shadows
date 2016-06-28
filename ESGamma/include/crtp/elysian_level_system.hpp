#ifndef ELYSIAN_LEVEL_SYSTEM_HPP
#define ELYSIAN_LEVEL_SYSTEM_HPP

#ifdef ELYSIAN_ENGINE
#   include <system/elysian_level_system_base.hpp>
namespace elysian {
    using LevelSystem = LevelSystemBase;
}
#else

#   include <estk/tk_level_system.h>

namespace elysian {
    using LevelSystem = TkLevelSystem;
}
using LevelSystem = TkLevelSystem;

#endif


#endif // ELYSIAN_LEVEL_SYSTEM_HPP
