#ifndef ELYSIAN_INPUT_SYSTEM_PROXY_HPP
#define ELYSIAN_INPUT_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <AssetIO/proxy/elysian_instance_transform_proxy.hpp>
#include <ESGamma/device/elysian_rumble_device.hpp>
#include <ESGamma/device/elysian_controller_device.hpp>
#include <ESGamma/device/elysian_visual_memory_device.hpp>
#include <ESGamma/device/elysian_keyboard_device.hpp>
#include <ESGamma/device/elysian_mouse_device.hpp>
#include <ESGamma/device/elysian_touch_device.hpp>

namespace OOLUA {
    typedef elysian::PlayerIODevice             PlayerIODevice;
    typedef elysian::Keyboard                   Keyboard;
    typedef elysian::Mouse                      Mouse;
    typedef elysian::Controller                 Controller;
    typedef elysian::TouchDevice                TouchDevice;
    typedef elysian::RumbleDevice               RumbleDevice;
    typedef elysian::VisualMemoryUnit           VisualMemoryUnit;

    typedef elysian::RumbleEffect               RumbleEffect;
    typedef elysian::DirectionalRumbleEffect    DirectionalRumbleEffect;
    typedef elysian::LeftRightRumbleEffect      LeftRightRumbleEffect;
    typedef elysian::ConstantRumbleEffect       ConstantRumbleEffect;
    typedef elysian::RampRumbleEffect           RampRumbleEffect;
    typedef elysian::PeriodicRumbleEffect       PeriodicRumbleEffect;

    typedef elysian::VirtualDPad                VirtualDPad;

    typedef elysian::InputSystem                InputSystem;

    typedef elysian::InputEvent                 InputEvent;
    typedef elysian::ControllerEvent            ControllerEvent;
    typedef elysian::ControllerButtonEvent      ControllerButtonEvent;
    typedef elysian::ControllerAnalogEvent      ControllerAnalogEvent;
    typedef elysian::ControllerTriggerEvent     ControllerTriggerEvent;
    typedef elysian::KeyEvent                   KeyEvent;
    typedef elysian::MouseBaseEvent             MouseBaseEvent;
    typedef elysian::MouseEvent                 MouseEvent;
    typedef elysian::MouseWheelEvent            MouseWheelEvent;
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
    OOLUA_MFUNC_CONST(isDoubleClicked)
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
    OOLUA_MEM_FUNC_CONST(float, getTriggerPressure, const GY_CONTROLLER_TRIGGER)
OOLUA_PROXY_END

OOLUA_PROXY(TouchDevice, PlayerIODevice)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END


OOLUA_PROXY(InputEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MFUNC_CONST(getPlayerIndex)
OOLUA_PROXY_END

OOLUA_PROXY(ControllerEvent, InputEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MFUNC_CONST(getController)
OOLUA_PROXY_END

OOLUA_PROXY(ControllerButtonEvent, ControllerEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getButton)
    OOLUA_MFUNC_CONST(getButtonState)
    OOLUA_MFUNC_CONST(isPressed)
    OOLUA_MFUNC_CONST(isTapped)
OOLUA_PROXY_END

OOLUA_PROXY(ControllerAnalogEvent, ControllerEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getAnalogStickType)
    OOLUA_MFUNC_CONST(getAnalogStick)
    OOLUA_MFUNC_CONST(getAnalogDir)
    OOLUA_MFUNC_CONST(getAnalogMag)
    OOLUA_MFUNC_CONST(getVirtualDPad)
OOLUA_PROXY_END

OOLUA_PROXY(ControllerTriggerEvent, ControllerEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getTrigger)
    OOLUA_MFUNC_CONST(getTriggerPressure)
OOLUA_PROXY_END


OOLUA_PROXY(KeyEvent, InputEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getKeyboard)
    OOLUA_MFUNC_CONST(isPressed)
    OOLUA_MFUNC_CONST(isTapped)
    OOLUA_MFUNC_CONST(getState)
    OOLUA_MFUNC_CONST(getKey)
OOLUA_PROXY_END

OOLUA_PROXY(MouseBaseEvent, InputEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
    OOLUA_MFUNC_CONST(getMouse)
OOLUA_PROXY_END

OOLUA_PROXY(MouseEvent, MouseBaseEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getScreenPos)
    OOLUA_MFUNC_CONST(getButton)
    OOLUA_MFUNC_CONST(getButtonState)
    OOLUA_MFUNC_CONST(isPressed)
    OOLUA_MFUNC_CONST(isTapped)
    OOLUA_MFUNC_CONST(isDoubleClicked)
OOLUA_PROXY_END

OOLUA_PROXY(MouseWheelEvent, MouseBaseEvent)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getScrollWheel)
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
    OOLUA_MEM_FUNC_CONST(Controller*, getController, const unsigned)
    OOLUA_MEM_FUNC_CONST(Keyboard*, getKeyboard, const unsigned)
    OOLUA_MFUNC_CONST(getMouse)
    OOLUA_MFUNC_CONST(getVMU)
    OOLUA_MFUNC_CONST(getRumbleDevice)
    OOLUA_MFUNC_CONST(getTouchDevice)
    OOLUA_MFUNC(setDefaultVMUIcon)
    OOLUA_MFUNC(broadcastVMUIcon)
    OOLUA_MFUNC(broadcastVMUBeep)
OOLUA_PROXY_END


#endif // EYSIAN_INPUT_SYSTEM_PROXY_HPP

