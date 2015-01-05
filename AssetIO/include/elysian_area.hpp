#ifndef ELYSIAN_AREA_HPP
#define ELYSIAN_AREA_HPP

#ifdef ELYSIAN_ENGINE
#   include "area_base.hpp"
namespace elysian {
    typedef AreaBase Area;
}
#else
#   include "tkArea.h"
namespace elysian {
    typedef TkArea Area;
}
typedef TkArea Area;

#endif


#endif // ELYSIAN_AREA_HPP
