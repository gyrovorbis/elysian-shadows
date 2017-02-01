#ifndef ELYSIAN_GUI_SYSTEM_PROXY_HPP
#define ELYSIAN_GUI_SYSTEM_PROXY_HPP

#include <crtp/elysian_gui_system.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::MenuWidget     MenuWidget;
    typedef elysian::MenuText       MenuText;
    typedef elysian::MenuEntity     MenuEntity;
    typedef elysian::GuiSystem      GuiSystem;
    typedef elysian::AdventureLog   AdventureLog;
}


OOLUA_PROXY(MenuWidget)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

OOLUA_PROXY(MenuText, MenuWidget)
    OOLUA_MEM_FUNC(void, setRelPos, float, float)
    OOLUA_MEM_FUNC(void, setFontType, unsigned)
    OOLUA_MEM_FUNC(void, setColor, GYColor4)
    OOLUA_MEM_FUNC(void, setAlignment, elysian::Font::ALIGNMENT)
    OOLUA_MEM_FUNC(void, setText, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(MenuEntity, MenuWidget, Entity)
    OOLUA_MEM_FUNC(void, addChildWidget, cpp_in_p<MenuWidget*>)
    OOLUA_MEM_FUNC(void, setSizeType, MenuEntity::SIZE_TYPE)
    OOLUA_MEM_FUNC(void, removeChildWidget, in_out_p<MenuWidget*>)
OOLUA_PROXY_END

OOLUA_PROXY(AdventureLog)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    //OOLUA_MEM_FUNC(void, setEnabled, bool)
    OOLUA_MEM_FUNC(void, setLabel, const char*)
    OOLUA_MEM_FUNC(void, setStaticText, bool)
    OOLUA_MEM_FUNC(void, addLine, const char*)
    OOLUA_MEM_FUNC(void, clear)
OOLUA_PROXY_END


OOLUA_PROXY(GuiSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addCombatText, const char*, GYColor4, const GYVector2&, const GYVector2&)
    OOLUA_MEM_FUNC(void, setFgMenuWidget, MenuWidget*)
    OOLUA_MEM_FUNC(void, clearFgMenuWidget)
    OOLUA_MEM_FUNC(void, setBgMenuWidget, MenuWidget*)
    OOLUA_MEM_FUNC(void, clearBgMenuWidget)
    OOLUA_MEM_FUNC(void, setClearColor, GYColor4)
    OOLUA_MEM_FUNC(void, setAdvLogEnabled, bool)
    OOLUA_MEM_FUNC(AdventureLog&, getAdvLog)
    OOLUA_MEM_FUNC(void, addVictoryText, const GYVector2&, const char*)
    OOLUA_MEM_FUNC_CONST(void, getResolution, out_p<unsigned&>, out_p<unsigned&>)
    OOLUA_MEM_FUNC_CONST(GYColor4, getFontColorPaletteEntry, const unsigned)
    OOLUA_MEM_FUNC(void, setFontColorPaletteEntry, const unsigned, const GYColor4)
OOLUA_PROXY_END

#endif // ELYSIAN_GUI_SYSTEM_PROXY_HPP

