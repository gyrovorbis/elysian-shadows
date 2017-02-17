#ifndef ELYSIAN_LAYOUT_PROXY_HPP
#define ELYSIAN_LAYOUT_PROXY_HPP

#include <gui/elysian_layout.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::Layout             Layout;
    typedef elysian::HorizontalLayout   HorizontalLayout;
    typedef elysian::VerticalLayout     VerticalLayout;
    typedef elysian::Widget             Widget;

    template<> class Proxy_class<elysian::Widget>;
}

OOLUA_PROXY(Layout)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MEM_FUNC(void, addWidget, cpp_in_p<Widget*>)
    OOLUA_MEM_FUNC(Widget*, removeWidget, Widget*)
    OOLUA_MEM_FUNC_RENAME(setWidgets, void, setWidgetsLua, LuaVariant)
    OOLUA_MEM_FUNC_CONST_RENAME(getWidgets, LuaVariant, getWidgetsLua)
    OOLUA_MEM_FUNC_CONST(unsigned, getWidgetCount)
    OOLUA_MEM_FUNC_CONST(maybe_null<Widget*>, getWidget, unsigned)
    OOLUA_MEM_FUNC_CONST(maybe_null<Widget*>, getParent)
    OOLUA_MEM_FUNC(void, setHorizontalSpacing, int)
    OOLUA_MEM_FUNC_CONST(int, getHorizontalSpacing)
    OOLUA_MEM_FUNC(void, setVerticalSpacing, int)
    OOLUA_MEM_FUNC_CONST(int, getVerticalSpacing)
    OOLUA_MEM_FUNC(void, setSpacing, int, int)
    OOLUA_MEM_FUNC_CONST(Layout::ALIGNMENT, getHorizontalAlignment)
    OOLUA_MEM_FUNC(void, setHorizontalAlignment, Layout::ALIGNMENT)
    OOLUA_MEM_FUNC_CONST(Layout::ALIGNMENT, getVerticalAlignment)
    OOLUA_MEM_FUNC(void, setVerticalAlignment, Layout::ALIGNMENT)
    OOLUA_MEM_FUNC(void, setAlignment, Layout::ALIGNMENT, Layout::ALIGNMENT)
OOLUA_PROXY_END

OOLUA_PROXY(HorizontalLayout, Layout)
OOLUA_PROXY_END

OOLUA_PROXY(VerticalLayout, Layout)
OOLUA_PROXY_END



#endif // ELYSIAN_LAYOUT_PROXY_HPP
