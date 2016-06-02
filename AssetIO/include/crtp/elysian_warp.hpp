#ifndef ELYSIAN_WARP_HPP
#define ELYSIAN_WARP_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_warp_base.hpp>
namespace elysian {
    typedef WarpBase Warp;
}
typedef WarpBase Warp;
#else
#   include <tk_warp.h>
namespace elysian {
    typedef TkWarp Warp;
}
typedef TkWarp Warp;

#endif

#endif // ELYSIAN_WARP_HPP

