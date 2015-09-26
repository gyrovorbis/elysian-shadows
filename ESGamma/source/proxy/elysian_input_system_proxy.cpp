#include <proxy/elysian_input_system_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS(Controller)
OOLUA_EXPORT_FUNCTIONS_CONST(Controller,
                             getAnalogStick,
                             getAnalogDir,
                             getAnalogMag,
                             getButtonState,
                             isPressed,
                             isTapped)

OOLUA_EXPORT_FUNCTIONS(InputSystem,
                       getController)
OOLUA_EXPORT_FUNCTIONS_CONST(InputSystem)

