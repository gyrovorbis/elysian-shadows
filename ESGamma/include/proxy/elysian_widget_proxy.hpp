#ifndef ELYSIAN_WIDGET_PROXY_HPP
#define ELYSIAN_WIDGET_PROXY_HPP

#include <gui/elysian_widget.hpp>
#include <gui/elysian_entity_widget.hpp>
#include <gui/elysian_label_widget.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::Widget         Widget;
    typedef elysian::GroupWidget    GroupWidget;
    typedef elysian::EntityWidget   EntityWidget;
    typedef elysian::LabelWidget    LabelWidget;
    typedef elysian::Layout         Layout;
    typedef elysian::Color4         Color4;

    template<> class Proxy_class<elysian::Layout>;
}

OOLUA_PROXY(Widget)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MEM_FUNC_CONST(Vector2, getPosition)
    OOLUA_MEM_FUNC(void, setPosition, Vector2)
    OOLUA_MEM_FUNC_CONST(Vector2, getSize)
    OOLUA_MEM_FUNC(void, setSize, Vector2)
    OOLUA_MEM_FUNC(Vector2, getMaxSize)
    OOLUA_MEM_FUNC(void, setMaxSize, Vector2)
    OOLUA_MEM_FUNC(Vector2, getMinSize)
    OOLUA_MEM_FUNC(void, setMinSize, Vector2)
    OOLUA_MEM_FUNC(Vector2, getMarginSize)
    OOLUA_MEM_FUNC(void, setMarginSize, Vector2)
    OOLUA_MEM_FUNC(void, setHorizontalSizePolicy, Widget::SIZE_POLICY)
    OOLUA_MEM_FUNC(void, setVerticalSizePolicy, Widget::SIZE_POLICY)
    OOLUA_MEM_FUNC(void, setSizePolicy, Widget::SIZE_POLICY, Widget::SIZE_POLICY)
    OOLUA_MEM_FUNC(void, setLayout, Layout*)
    OOLUA_MEM_FUNC(void, setActive, bool)
    OOLUA_MEM_FUNC_CONST(Widget::SIZE_POLICY, getHorizontalSizePolicy)
    OOLUA_MEM_FUNC_CONST(Widget::SIZE_POLICY, getVerticalSizePolicy)
    OOLUA_MEM_FUNC_CONST(Layout*, getLayout)
    OOLUA_MEM_FUNC_CONST(bool, isActive)
OOLUA_PROXY_END

OOLUA_PROXY(GroupWidget, Widget)
OOLUA_PROXY_END

OOLUA_PROXY(EntityWidget, Widget)
    OOLUA_CTORS(
        OOLUA_CTOR(Entity*)
    )
    OOLUA_MEM_FUNC(void, setEntity, Entity*)
    OOLUA_MEM_FUNC_CONST(Entity*, getEntity)
OOLUA_PROXY_END

OOLUA_PROXY(LabelWidget, Widget)
    OOLUA_CTORS(
        OOLUA_CTOR(const char*)
    )
    OOLUA_MEM_FUNC(void, setText, const char*)
    OOLUA_MEM_FUNC(void, setColor, Color4)
    OOLUA_MEM_FUNC_CONST(const char*, getText)
    OOLUA_MEM_FUNC_CONST(Color4, getColor)
OOLUA_PROXY_END

#endif // ELYSIAN_WIDGET_PROXY_HPP
