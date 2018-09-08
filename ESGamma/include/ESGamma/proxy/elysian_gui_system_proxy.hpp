#ifndef ELYSIAN_GUI_SYSTEM_PROXY_HPP
#define ELYSIAN_GUI_SYSTEM_PROXY_HPP

#include <ESGamma/crtp/elysian_gui_system.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::GuiSystem      GuiSystem;
    typedef elysian::AdventureLog   AdventureLog;
    typedef elysian::Widget         Widget;
}

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
    OOLUA_MEM_FUNC(void, setFgWidget, Widget*)
    OOLUA_MEM_FUNC(void, clearFgWidget)
    OOLUA_MEM_FUNC(void, setBgWidget, Widget*)
    OOLUA_MEM_FUNC(void, clearBgWidget)
    OOLUA_MEM_FUNC(void, setClearColor, GYColor4)
    OOLUA_MEM_FUNC(void, setAdvLogEnabled, bool)
    OOLUA_MEM_FUNC(AdventureLog&, getAdvLog)
    OOLUA_MEM_FUNC(void, addVictoryText, const GYVector2&, const char*)
    OOLUA_MEM_FUNC_CONST(void, getResolution, out_p<unsigned&>, out_p<unsigned&>)
    OOLUA_MEM_FUNC_CONST(GYColor4, getFontColorPaletteEntry, const unsigned)
    OOLUA_MEM_FUNC(void, setFontColorPaletteEntry, const unsigned, const GYColor4)
OOLUA_PROXY_END

#endif // ELYSIAN_GUI_SYSTEM_PROXY_HPP

