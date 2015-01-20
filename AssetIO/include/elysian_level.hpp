#ifndef ELYSIAN_LEVEL_HPP
#define ELYSIAN_LEVEL_HPP

#ifdef ELYSIAN_ENGINE
#   include "elysian_level_base.hpp"
namespace elysian {
    typedef LevelBase Level;
}
#else
#   include "tkLevel.h"
namespace elysian {
    typedef TkLevel Level;
}
typedef TkLevel Level;

#endif


#endif // ELYSIAN_LEVEL_HPP
