#include <proxy/elysian_input_system_proxy.hpp>

/*!
    \qmltype Keyboard
    \since 1.6.0
    \inmodule ESToolkit
    \brief Class representing a keyboard device and key states.

    The engine supports a single keyboard device, which uses the standard default keyboard on Windows, MacOS, and Linux,
    optional removable USB keyboards on OUYA and ForgeTV, and the Sega Dreamcast keyboard. Because many platforms do not
    necessarily have a keyboard attached, Lua code should check to see if the keyboard is connected before using it.

    \section2 Fetching a Keyboard Object
    \code
    keybd = Engine.getInputSys():getKeyboard()
    \endcode
    NOTE: \e {Since keyboards can be dynamically added and removed on certain platforms, do NOT cache the keyboard
    object between frames, as it may no longer be connected or valid the next frame. Always fetch the keyboard again
    from the InputSystem.}

    \section2 Keyboard as Virtual Controller
    By default, the engine maps certain keys on the keyboard to buttons on the controller on port 0. This is provided
    as a convenience when trying to test on PCs without requiring an external USB controller to be connected to play.

    \sa {Input Devices}, {InputSystem}, {Controller}
*/


/*!
\qmlproperty enumeration Keyboard::KEY_TYPE

Represents the various different keys on a keyboard.

A key value can be one of the following:
\list
\li Keyboard.KEY_NONE - No key, used in internal code by the engine
\li Keyboard.KEY_A
\li Keyboard.KEY_B
\li Keyboard.KEY_C
\li Keyboard.KEY_D
\li Keyboard.KEY_E
\li Keyboard.KEY_F
\li Keyboard.KEY_G
\li Keyboard.KEY_H
\li Keyboard.KEY_I
\li Keyboard.KEY_J
\li Keyboard.KEY_K
\li Keyboard.KEY_L
\li Keyboard.KEY_M
\li Keyboard.KEY_N
\li Keyboard.KEY_O
\li Keyboard.KEY_P
\li Keyboard.KEY_Q
\li Keyboard.KEY_R
\li Keyboard.KEY_S
\li Keyboard.KEY_T
\li Keyboard.KEY_U
\li Keyboard.KEY_V
\li Keyboard.KEY_W
\li Keyboard.KEY_X
\li Keyboard.KEY_Y
\li Keyboard.KEY_Z
\li Keyboard.KEY_DPAD_U
\li Keyboard.KEY_DPAD_D
\li Keyboard.KEY_DPAD_L
\li Keyboard.KEY_DPAD_R
\li Keyboard.KEY_SPACE
\li Keyboard.KEY_RETURN
\li Keyboard.KEY_ESC
\li Keyboard.KEY_BACKSLASH
\li Keyboard.KEY_FORWARDSLASH
\li Keyboard.KEY_LSHIFT
\li Keyboard.KEY_RSHIFT
\li Keyboard.KEY_LCTRL
\li Keyboard.KEY_RCTRL
\li Keyboard.KEY_LALT
\li Keyboard.KEY_RALT
\li Keyboard.KEY_BACKQUOTE
\li Keyboard.KEY_COUNT    - # of keys supported, provided for looping
\endlist
*/

/*!
 * \qmlmethod BUT_STATE Keyboard::getButtonState(KEY_TYPE key)
 *
 *  Returns the state of the keyboard key given by \a key.
 * \sa InputSystem::BUT_STATE
 */
/*!
 * \qmlmethod bool Keyboard::isPressed(KEY_TYPE key)
 *
 *  Returns true if the keyboard key given by \a key is currently being held down.
 *
 */
/*!
 * \qmlmethod bool Keyboard::isTapped(KEY_TYPE key)
 *
 *  Returns true if the keyboard key given by \a key was pressed down this frame.
 *
 */

OOLUA_EXPORT_FUNCTIONS(Keyboard)
OOLUA_EXPORT_FUNCTIONS_CONST(Keyboard,
                             getButtonState,
                             isPressed,
                             isTapped)

/*!
    \qmltype VirtualDPad
    \since 1.6.0
    \inmodule ESToolkit
    \brief Class that maps a controller's analog stick to a directional pad.

    The VirtualDPad class is useful for supporting the analog stick in menus or dialog boxes, where traditionally
    a directional pad is used. The dpad buttons are considered down if the analog stick crosses a certain threshold,
    and they contain traditional button states such as down, up, released, and tapped.
*/

/*!
 * \qmlmethod BUT_STATE VirtualDPad::getButtonState(BUT_TYPE button)
 *
 *  Returns the state of the directional pad button given by \a button.
 * \sa InputSystem::BUT_STATE, Controller::BUT_TYPE
 */

/*!
 * \qmlmethod bool VirtualDPad::isPressed(BUT_TYPE button)
 *
 *  Returns whether or not the directional pad button given by \a button is currently being pressed.
 * \sa Controller::BUT_TYPE
 */

/*!
 * \qmlmethod bool VirtualDPad::isTapped(BUT_TYPE button)
 *
 *  Returns whether or not the directional pad button given by \a button became pressed this frame.
 * \sa Controller::BUT_TYPE
 */

OOLUA_EXPORT_FUNCTIONS(VirtualDPad)
OOLUA_EXPORT_FUNCTIONS_CONST(VirtualDPad,
                             getButtonState,
                             isPressed,
                             isTapped)

/*!
    \qmltype Controller
    \since 1.6.0
    \inmodule ESToolkit
    \brief Class representing a joystick input device for a single player on a single port.

    Controller is a virtual input device that may represent a Sega Dreamcast controller,
    USB controller, Xbox or PlayStation controller, or any other form of controller supported by the engine.
    The engine supports 4 different ports, meaning each port has its own controller object.

    \section2 Virtual Button Mapping
    Due to the fact that there is a metric shitload of different controller types that may be supported by the engine, they
    are all mapped to this Controller structure, which maps a specific joystick to the PlayStation-style
    controller layout. This allows game code to be written independent of specific controller type.
    The PlayStation configuration is favored, since using shapes instead of letters for buttons
    provides the least ambiguity for button placement.

    \section2 Fetching a Controller Connected to a Port
    A reference to a Controller may be fetched through the InputSystem:
    \code
    controller = Engine.getInputSys():getController(0) --fetches controller connected to port 0
    \endcode

    \e{NOTE: Since controllers can be disconnected and reconnected in the middle of the game, NEVER cache a controller object
    between frames within a Lua script. Always fetch the controller by port again each frame, to ensure that you are
    using the current controller connected to the port.}

    \section2 Keyboard as Virtual Controller
    By default, the engine maps certain Keyboard keys to Controller buttons on port 0. This provides easier debugging and
    testing on PCs and other devices without requiring an external USB controller.
    \sa {Input Devices}, {Keyboard}, {InputSystem}
*/

/*!
\qmlproperty enumeration Controller::BUT_TYPE

Represents the different virtual buttons of a controller. Note that these buttons use the PlayStation controller layout.

The button can be one of:
\list
\li Controller.BUT_DPAD_UP
\li Controller.BUT_DPAD_DOWN
\li Controller.BUT_DPAD_LEFT
\li Controller.BUT_DPAD_RIGHT
\li Controller.BUT_START
\li Controller.BUT_SELECT
\li Controller.BUT_HOME         - Special-function middle button (like PlayStation or Xbox buttons)
\li Controller.BUT_LSHOULDER    - First, non pressure-sensitive L button (L1 on PlayStation)
\li Controller.BUT_RSHOULDER    - First, non pressure-sensitive R button (R1 on PlayStation)
\li Controller.BUT_LANALOG      - Button pressed by pushing in the left analog stick
\li Controller.BUT_RANALOG      - Button pressed by pushing in the right analog stick
\li Controller.BUT_X
\li Controller.BUT_SQUARE
\li Controller.BUT_CIRCLE
\li Controller.BUT_TRIANGLE
\li Controller.BUT_COUNT
\endlist
*/

/*!
 * \qmlmethod Vector2 Controller::getAnalogStick(int stick)
 *
 * Returns a vector in the range of <0.0, 0.0> to <1.0, 1.0> for the analog stick specified by \a stick where 0 is the left analog stick and 1 is the right analog stick.
 */
/*!
 * \qmlmethod Vector2 Controller::getAnalogDir(int stick)
 *
 * Returns a normalized unit vector representing the direction of the analog stick specified by \a stick where 0 is the left analog stick and 1 is the right analog stick.
 */
/*!
 * \qmlmethod float Controller::getAnalogMag(int stick)
 *
 * Returns the floating point magnitude of the analog stick specified by \a stick where 0 is the left analog stick and 1 is the right analog stick.
 */
/*!
 * \qmlmethod bool Controller::isPressed(BUT_TYPE button)
 *
 * Returns whether or not the specified \a button is currently being pressed and held down.
 */
/*!
 * \qmlmethod bool Controller::isTapped(BUT_TYPE button)
 *
 * Similar to isPressed(), but only returns true the first frame \a button is held down.
 * \sa isPressed
 */
/*!
 * \qmlmethod VirtualDPad* Controller::getVirtualDPad(int analogStick)
 *
 * Returns a VirtualDPad object mapping the analog stick given by \a analogStick to a set of directional pad buttons. 0 specifies the left analog stick, and 1 specifies the right analog stick.
 * \sa isPressed
 */

/*!
    \qmltype InputSystem
    \since 1.6.0
    \inmodule ESToolkit
    \brief Subsystem for managing input devices such as controllers, keyboards, mice, and touch screens.

    The InputSystem updates and maintains objects for input devices, which are all stored and represented in a
    platform-independent manner. The InputSystem also handles hot swapping and dynamically adding/removing
    devices. Because of this, references to input devices should NEVER be cached between frames and should
    always be fetched from the InputSystem to ensure that they are valid and are still connected.

    \section2 Supported Devices
    \list
    \li 4 Controller Ports
    \li Keyboard
    \li \e Mouse
    \li \e {Touch Screen}
    \endlist

    \sa {Controller}, {Keyboard}, {Input Devices}
*/

/*!
 * \qmlmethod Controller* InputSystem::getController(int port)
 *
 * Returns Controller object associated with the specified \a port or nil if no controller is connected.
 * \sa Controller
 */

/*!
 * \qmlmethod Keyboard* InputSystem::getKeyboard()
 *
 * Returns a Keyboard object if one is currently connected or nil if one isn't detected.
 * \sa Keyboard
 */

/*!
\qmlproperty enumeration InputSystem::BUT_STATE

Represents the different states a button can be in.

The button can be one of:
\list
\li InputSystem.BUT_STATE_UP        - button has been up for at least two frames
\li InputSystem.BUT_STATE_TAPPED    - button was just pressed down this frame
\li InputSystem.BUT_STATE_RELEASED  - button was just released this frame
\li InputSystem.BUT_STATE_DOWN      - button has been held for at least two frames
\endlist
*/

/*!
 * \qmlmethod BUT_STATE Controller::getButtonState(BUT_TYPE button)
 *
 * Returns the state of the given \a button.
 * \sa InputSystem::BUT_STATE
 */


OOLUA_EXPORT_FUNCTIONS(Controller)
OOLUA_EXPORT_FUNCTIONS_CONST(Controller,
                             getAnalogStick,
                             getAnalogDir,
                             getAnalogMag,
                             getButtonState,
                             isPressed,
                             isTapped,
                             getVirtualDPad)

OOLUA_EXPORT_FUNCTIONS(InputSystem,
                       getController,
                       getKeyboard)
OOLUA_EXPORT_FUNCTIONS_CONST(InputSystem)

