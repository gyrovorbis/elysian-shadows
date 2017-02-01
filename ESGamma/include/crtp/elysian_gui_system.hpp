#ifndef ELYSIAN_GUI_SYSTEM_HPP
#define ELYSIAN_GUI_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <system/elysian_gui_system_base.hpp>
namespace elysian {
    typedef MenuEntityBase MenuEntity;
}
#else
#   include <estk/tk_gui_system.h>
namespace elysian {
    typedef TkMenuEntity MenuEntity;
}
typedef TkMenuEntity MenuEntity;

#endif


#endif // ELYSIAN_GUI_SYSTEM_HPP
