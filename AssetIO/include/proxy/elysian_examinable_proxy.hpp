#ifndef ELYSIAN_EXAMINABLE_PROXY_HPP
#define ELYSIAN_EXAMINABLE_PROXY_HPP

#include <AssetIO/proxy/elysian_component_proxy.hpp>
#include <AssetIO/proxy/elysian_dialog_proxy.hpp>
#include <AssetIO/crtp/elysian_examinable.hpp>

namespace OOLUA {
    typedef elysian::Dialog             Dialog;
    typedef elysian::Examinable         Examinable;
}

OOLUA_PROXY(Examinable, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setDialog, std::shared_ptr<Dialog>)
    OOLUA_MEM_FUNC(void, setMaxDist, float)
    OOLUA_MEM_FUNC_CONST(float, getMaxDist)
    OOLUA_MEM_FUNC_CONST(bool, isEnabled)
    OOLUA_MEM_FUNC(void, setEnabled, bool)
    OOLUA_MEM_FUNC_CONST(Direction::DIR4, getRelativeDir)
    OOLUA_MEM_FUNC(void, setRelativeDir, Direction::DIR4)
    OOLUA_MEM_FUNC_CONST(bool, hasDescription)
    OOLUA_MEM_FUNC(void, setHasDescription, bool)
    OOLUA_MEM_FUNC_CONST(maybe_null<std::shared_ptr<Dialog>>, getDialog)
OOLUA_PROXY_END


#endif // ELYSIAN_EXAMINABLE_PROXY_HPP

