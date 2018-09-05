#ifndef ELYSIAN_SEVERITY_HPP
#define ELYSIAN_SEVERITY_HPP

#ifdef ELYSIAN_ENGINE
#   include <level/elysian_level_base.hpp>
namespace elysian {
    typedef LevelBase Level;
}
#else
#   include <ESToolkit/AssetIO/tk_level.h>
namespace elysian {
    typedef TkLevel Level;
}
typedef TkLevel Level;

#endif


#endif // ELYSIAN_SEVERITY_HPP
