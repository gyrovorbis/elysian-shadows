#ifndef ELYSIAN_INPUT_SYSTEM_PROXY_HPP
#define ELYSIAN_INPUT_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <crtp/elysian_input_system.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>

namespace OOLUA {
    typedef elysian::Keyboard       Keyboard;
    typedef elysian::VirtualDPad    VirtualDPad;
    typedef elysian::Controller     Controller;
    typedef elysian::InputSystem    InputSystem;
}

OOLUA_PROXY(Keyboard)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(GY_BUTTON_STATE, getButtonState, GY_KEY)
    OOLUA_MEM_FUNC_CONST(bool, isPressed, GY_KEY)
    OOLUA_MEM_FUNC_CONST(bool, isTapped, GY_KEY)
OOLUA_PROXY_END

OOLUA_PROXY(VirtualDPad)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(GY_BUTTON_STATE, getButtonState, GY_CONTROLLER_BUTTON)
    OOLUA_MEM_FUNC_CONST(bool, isPressed, GY_CONTROLLER_BUTTON)
    OOLUA_MEM_FUNC_CONST(bool, isTapped, GY_CONTROLLER_BUTTON)
OOLUA_PROXY_END

OOLUA_PROXY(Controller)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(const GYVector2&, getAnalogStick, const GY_CONTROLLER_ANALOG)
    OOLUA_MEM_FUNC_CONST(const GYVector2&, getAnalogDir, const GY_CONTROLLER_ANALOG)
    OOLUA_MEM_FUNC_CONST(float, getAnalogMag, const GY_CONTROLLER_ANALOG)
    OOLUA_MEM_FUNC_CONST(GY_BUTTON_STATE, getButtonState, const GY_CONTROLLER_BUTTON)
    OOLUA_MEM_FUNC_CONST(bool, isPressed, const GY_CONTROLLER_BUTTON)
    OOLUA_MEM_FUNC_CONST(bool, isTapped, const GY_CONTROLLER_BUTTON)
    OOLUA_MEM_FUNC_CONST(const VirtualDPad&, getVirtualDPad, GY_CONTROLLER_ANALOG)
OOLUA_PROXY_END


OOLUA_PROXY(InputSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(Controller*, getController, const unsigned)
    OOLUA_MEM_FUNC(Keyboard*, getKeyboard)
OOLUA_PROXY_END


#endif // EYSIAN_INPUT_SYSTEM_PROXY_HPP

