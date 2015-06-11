#ifndef ELYSIAN_DEBUG_HPP
#define ELYSIAN_DEBUG_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_debug_base.hpp>
namespace elysian {
    typedef DebugBase Debug;
}
#else
#   include <tk_debug.h>
namespace elysian {
    typedef TkDebug Debug;
}
typedef TkDebug Debug;

#endif



#endif // ELYSIAN_DEBUG_H
