#ifndef ELYSIAN_EXAMINABLE_PROXY_HPP
#define ELYSIAN_EXAMINABLE_PROXY_HPP

#include <proxy/elysian_component_proxy.hpp>
#include <level/elysian_dialog.hpp>
#include <crtp/elysian_examinable.hpp>

namespace OOLUA {
    typedef elysian::DialogScreen       DialogScreen;
    typedef elysian::DialogText         DialogText;
    typedef elysian::DialogPrompt       DialogPrompt;
    typedef elysian::Dialog             Dialog;
    typedef elysian::Examinable         Examinable;
}


OOLUA_PROXY(DialogScreen)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(const char*, getText)
    OOLUA_MEM_FUNC(void, addSourceEntity, Entity*)
    OOLUA_MEM_FUNC_CONST(unsigned, getIndex)
    OOLUA_MEM_FUNC(void, setTrigger, bool)
OOLUA_PROXY_END

OOLUA_PROXY(DialogText, DialogScreen)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(int, getNextIndex)
    OOLUA_MEM_FUNC(void, setNextIndex, int)
OOLUA_PROXY_END

OOLUA_PROXY(DialogPrompt, DialogScreen)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addOption, const char*, int)
OOLUA_PROXY_END

OOLUA_PROXY(Dialog)
    OOLUA_MEM_FUNC(DialogText*, addTextScreen, const char*)
    OOLUA_MEM_FUNC(DialogPrompt*, addPromptScreen, const char*)
    OOLUA_MEM_FUNC(void, setTriggerEntity, Entity*)
    OOLUA_MEM_FUNC(void, setStartScreen, const int)
OOLUA_PROXY_END

OOLUA_PROXY(Examinable, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setDialog, cpp_in_p<Dialog*>)
    OOLUA_MEM_FUNC(void, setMaxDist, float)
OOLUA_PROXY_END


#endif // ELYSIAN_EXAMINABLE_PROXY_HPP

