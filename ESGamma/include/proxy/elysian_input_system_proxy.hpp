#ifndef ELYSIAN_INPUT_SYSTEM_PROXY_HPP
#define ELYSIAN_INPUT_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <crtp/elysian_input_system.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>

namespace OOLUA {
    typedef elysian::PlayerIODevice             PlayerIODevice;
    typedef elysian::Keyboard                   Keyboard;
    typedef elysian::Mouse                      Mouse;
    typedef elysian::VirtualDPad                VirtualDPad;
    typedef elysian::Controller                 Controller;
    typedef elysian::RumbleEffect               RumbleEffect;
    typedef elysian::DirectionalRumbleEffect    DirectionalRumbleEffect;
    typedef elysian::LeftRightRumbleEffect      LeftRightRumbleEffect;
    typedef elysian::ConstantRumbleEffect       ConstantRumbleEffect;
    typedef elysian::RampRumbleEffect           RampRumbleEffect;
    typedef elysian::PeriodicRumbleEffect       PeriodicRumbleEffect;
    typedef elysian::RumbleDevice               RumbleDevice;
    typedef elysian::VisualMemoryUnit           VisualMemoryUnit;
    typedef elysian::InputSystem                InputSystem;
}

OOLUA_PROXY(PlayerIODevice)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getPlayerIndex)
    OOLUA_MFUNC_CONST(isAttached)
OOLUA_PROXY_END

OOLUA_PROXY(Keyboard, PlayerIODevice)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(GY_BUTTON_STATE, getButtonState, GY_KEY)
    OOLUA_MEM_FUNC_CONST(bool, isPressed, GY_KEY)
    OOLUA_MEM_FUNC_CONST(bool, isTapped, GY_KEY)
OOLUA_PROXY_END

OOLUA_PROXY(Mouse, PlayerIODevice)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getButtonState)
    OOLUA_MFUNC_CONST(getScrollWheel)
    OOLUA_MFUNC_CONST(getScreenPos)
    OOLUA_MFUNC_CONST(isPressed)
    OOLUA_MFUNC_CONST(isTapped)
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

OOLUA_PROXY(Controller, PlayerIODevice)
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

OOLUA_PROXY(RumbleEffect)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC(setDuration)
OOLUA_PROXY_END

OOLUA_PROXY(LeftRightRumbleEffect, RumbleEffect)
    OOLUA_CTORS(
        OOLUA_CTOR(float, float, float)
    )
    OOLUA_MFUNC(setLargeIntensity)
    OOLUA_MFUNC(setSmallIntensity)
OOLUA_PROXY_END

OOLUA_PROXY(DirectionalRumbleEffect, RumbleEffect)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC(setDirection)
    OOLUA_MFUNC(setDelay)
    OOLUA_MFUNC(setEnvelope)
OOLUA_PROXY_END

OOLUA_PROXY(ConstantRumbleEffect, DirectionalRumbleEffect)
    OOLUA_CTORS(
        OOLUA_CTOR(float, float)
    )
    OOLUA_MFUNC(setIntensity)
OOLUA_PROXY_END

OOLUA_PROXY(RampRumbleEffect, DirectionalRumbleEffect)
    OOLUA_CTORS(
        OOLUA_CTOR(float, float, float)
    )
    OOLUA_MFUNC(setRampStartIntensity)
    OOLUA_MFUNC(setRampEndIntensity)
OOLUA_PROXY_END

OOLUA_PROXY(PeriodicRumbleEffect, DirectionalRumbleEffect)
    OOLUA_TAGS(
        No_default_constructor
    )
    OOLUA_CTORS(
        OOLUA_CTOR(GY_RUMBLE_EFFECT_PERIODIC_TYPE)
        OOLUA_CTOR(GY_RUMBLE_EFFECT_PERIODIC_TYPE, float, float, float)
    )
    OOLUA_MFUNC(setPeriod)
    OOLUA_MFUNC(setPhaseShift)
    OOLUA_MFUNC(setIntensityPeak)
    OOLUA_MFUNC(setIntensityOffset)
OOLUA_PROXY_END

OOLUA_PROXY(RumbleDevice, PlayerIODevice)
    OOLUA_TAGS(
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(stop)
    OOLUA_MFUNC_CONST(play)
    OOLUA_MFUNC_CONST(playEffect)
OOLUA_PROXY_END

OOLUA_PROXY(VisualMemoryUnit, PlayerIODevice)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC(beep)
    OOLUA_MFUNC(loadIcon)
    OOLUA_MFUNC_CONST(frameBufferReadPixel)
    OOLUA_MFUNC(frameBufferWritePixel)
OOLUA_PROXY_END

OOLUA_PROXY(InputSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(Controller*, getController, const unsigned)
    OOLUA_MEM_FUNC(Keyboard*, getKeyboard)
    OOLUA_MFUNC(getMouse)
    OOLUA_MFUNC(getVMU)
    OOLUA_MFUNC(getRumbleDevice)
    OOLUA_MFUNC(setDefaultVMUIcon)
    OOLUA_MFUNC(broadcastVMUIcon)
    OOLUA_MFUNC(broadcastVMUBeep)
OOLUA_PROXY_END


#endif // EYSIAN_INPUT_SYSTEM_PROXY_HPP

