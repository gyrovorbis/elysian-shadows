#ifndef ELYSIAN_WIDGET_PROXY_HPP
#define ELYSIAN_WIDGET_PROXY_HPP

#include <gui/elysian_widget.hpp>
#include <gui/elysian_entity_widget.hpp>
#include <gui/elysian_label_widget.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::Widget         Widget;
    typedef elysian::EntityWidget   EntityWidget;
    typedef elysian::LabelWidget    LabelWidget;
}

OOLUA_PROXY(Widget)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MEM_FUNC(void, setMinWidth, int)
    OOLUA_MEM_FUNC(void, setMinHeight, int)
    OOLUA_MEM_FUNC(void, setMinSize, int, int)
    OOLUA_MEM_FUNC(void, setMaxWidth, int)
    OOLUA_MEM_FUNC(void, setMaxHeight, int)
    OOLUA_MEM_FUNC(void, setMaxSize, int, int)
    OOLUA_MEM_FUNC(void, setHorizontalSizePolicy, SIZE_POLICY)
    OOLUA_MEM_FUNC(void, setVerticalSizePolicy, SIZE_POLICY)
    OOLUA_MEM_FUNC(void, setSizePolicy, SIZE_POLICY, SIZE_POLICY)
    OOLUA_MEM_FUNC(void, setMarginWidth, int)
    OOLUA_MEM_FUNC(void, setMarginHeight, int)
    OOLUA_MEM_FUNC(void, setMarginSize, int, int)
    OOLUA_MEM_FUNC(void, setLayout, cpp_in_p<Layout*>)
    OOLUA_MEM_FUNC(void, setActive, bool)
    OOLUA_MEM_FUNC_CONST(int, getMinWidth)
    OOLUA_MEM_FUNC_CONST(int, getMinHeight)
    OOLUA_MEM_FUNC_CONST(int, getMaxWidth)
    OOLUA_MEM_FUNC_CONST(int, getMaxHeight)
    OOLUA_MEM_FUNC_CONST(SIZE_POLICY, getHorizontalSizePolicy)
    OOLUA_MEM_FUNC_CONST(SIZE_POLICY, getVerticalSizePolicy)
    OOLUA_MEM_FUNC_CONST(int, getMarginWidth)
    OOLUA_MEM_FUNC_CONST(int, getMarginHeight)
    OOLUA_MEM_FUNC_CONST(Layout*, getLayout)
    OOLUA_MEM_FUNC_CONST(bool, isActive)
OOLUA_PROXY_END

OOLUA_PROXY(EntityWidget, Widget)
    OOLUA_MEM_FUNC(void, setEntity, Entity*)
    OOLUA_MEM_FUNC_CONST(Entity*, getEntity)
OOLUA_PROXY_END

OOLUA_PROXY(LabelWidget, Widget)
    OOLUA_MEM_FUNC(void, setText, const char*)
    OOLUA_MEM_FUNC(void, setColor, Color4)
    OOLUA_MEM_FUNC_CONST(const char*, getText)
    OOLUA_MEM_FUNC_CONST(Color4, getColor)
OOLUA_PROXY_END


LabelWidget(const char* str=nullptr, Color4 color=Color4::white, ALIGNMENT align=LEFT);

const char*     getText(void) const;
void            setText(const char* str);

Color4          getColor(void) const;
void            setColor(Color4 color);
unsigned char   getAlpha(void) const;
void            setAlpha(unsigned char alpha);


ALIGNMENT       getAlignment(void) const;
void            setAlignment(ALIGNMENT align);

Font*           getFont(void) const;
void            setFont(Font* font);

int             getStrlen(void) const;


#endif // ELYSIAN_WIDGET_PROXY_HPP
