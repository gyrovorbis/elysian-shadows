#ifndef ELYSIAN_GAME_INPUT_PROXY_HPP
#define ELYSIAN_GAME_INPUT_PROXY_HPP

#include <oolua.h>
#include <utility/elysian_game_input.hpp>

namespace OOLUA {
    typedef elysian::GameInput      GameInput;
    typedef elysian::MouseInput     MouseInput;

}

OOLUA_PROXY(GameInput)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(bool, getTapped, GameInput::BUTTON)
    OOLUA_MEM_FUNC_CONST(bool, getDown, GameInput::BUTTON)
    OOLUA_MEM_FUNC(void, setUp, GameInput::BUTTON)
    OOLUA_MEM_FUNC(void, setDown, GameInput::BUTTON)
OOLUA_PROXY_END

OOLUA_PROXY(MouseInput)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(bool, getTapped, GY_MOUSE_BUTTON)
    OOLUA_MEM_FUNC_CONST(bool, getDown, GY_MOUSE_BUTTON)
    OOLUA_MEM_FUNC_CONST(void, getScreenPos, GYVector2&)
    OOLUA_MEM_FUNC_CONST(void, getScrollWheel, GYVector2&)
OOLUA_PROXY_END

#endif // ELYSIAN_GAME_INPUT_PROXY_HPP

