#ifndef ELYSIAN_LAYOUT_PROXY_HPP
#define ELYSIAN_LAYOUT_PROXY_HPP

#include <gui/elysian_layout.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::Layout             Layout;
    typedef elysian::HorizontalLayout   HorizontalLayout;
    typedef elysian::VerticalLayout     VerticalLayout;
}

#if 0

void        addWidget(Widget* widget);
void        removeWidget(Widget* widget);
void        setWidgets(std::vector<Widget*> widgets);
auto        getWidgets(void) const -> const std::vector<Widget*>&;
auto        getWidgets(void) -> std::vector<Widget*>&;
unsigned    getWidgetCount(void) const;
Widget*     getWidget(unsigned index) const;

AXIS        getGroupingAxis(void) const;
Widget*     getParent(void) const;
void        setParent(Widget* parent);

int         getSpacing(AXIS axis) const;
void        setSpacing(AXIS axis, int spacing);
int         getHorizontalSpacing(void) const;
int         getVerticalSpacing(void) const;
void        setHorizontalSpacing(int spacing);
void        setVerticalSpacing(int spacing);
void        setSpacing(int horizontal, int vertical);

ALIGNMENT   getAlignment(AXIS axis) const;
void        setAlignment(AXIS axis, ALIGNMENT align);
ALIGNMENT   getHorizontalAlignment(void) const;
ALIGNMENT   getVerticalAlignment(void) const;
void        setHorizontalAlignment(ALIGNMENT align);
void        setVerticalAlignnment(ALIGNMENT align);
void        setAlignment(ALIGNMENT horizontal, ALIGNMENT vertical);
#endif

OOLUA_PROXY(Layout)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MEM_FUNC(void, addWidget, cpp_in_p<Widget*>)
    OOLUA_MEM_FUNC(maybe_null<lua_out_p<Widget*>>, removeWidget, Widget*)
OOLUA_PROXY_END



#endif // ELYSIAN_LAYOUT_PROXY_HPP
