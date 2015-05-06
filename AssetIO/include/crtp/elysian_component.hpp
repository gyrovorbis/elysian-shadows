#ifndef ELYSIAN_COMPONENT_HPP
#define ELYSIAN_COMPONENT_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_component_base.hpp>
namespace elysian {
    typedef ComponentBase Component;
}
#else
#   include <tk_component.h>
namespace elysian {
    typedef TkComponent Component;
}
typedef TkComponent Component;

#endif



#endif // ELYSIAN_COMPONENT_HPP

