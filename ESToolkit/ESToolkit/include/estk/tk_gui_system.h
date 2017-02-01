#ifndef TK_GUI_SYSTEM_HPP
#define TK_GUI_SYSTEM_HPP

#include <system/elysian_gui_system_base.hpp>

class TkMenuEntity: public elysian::MenuEntityBase {
public:
    using LuaTableProxy<elysian::MenuEntityBase, MenuEntity, Entity>::initializeDelegates;
    using LuaTableProxy<elysian::MenuEntityBase, MenuEntity, Entity>::initializeVirtualTableDelegates;
};

#endif // TK_GUI_SYSTEM_HPP
