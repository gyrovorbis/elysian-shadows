#ifndef ELYSIAN_EXAMINABLE_HPP
#define ELYSIAN_EXAMINABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_examinable_base.hpp>
namespace elysian {
    typedef ExaminableBase Examinable;
}
#else
#   include <ESToolkit/AssetIO/tk_examinable.h>
namespace elysian {
    typedef TkExaminable Examinable;
}
typedef TkExaminable Examinable;

#endif

#endif // ELYSIAN_EXAMINABLE_HPP

