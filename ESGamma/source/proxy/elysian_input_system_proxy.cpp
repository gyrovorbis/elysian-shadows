#include <proxy/elysian_input_system_proxy.hpp>


/*!
    \qmltype PlayerIODevice
    \since 1.6.0
    \inmodule Engine
    \brief Base class for all user interface devices (controllers, keyboards, mice, VMUs, etc)

    \sa {Input Devices}, {InputSystem}, {Controller}, {Keyboard}, {Mouse}, {VisualMemoryUnit}

    PlayerIODevice is the base-level abstraction for all interactive input and output devices used
    by a player to interface with the engine (or vice-versa). These devices include input devices
    such as mice, keyboards, controllers, joysticks, and touch screens, as well as output devices
    such as memory cards, VMUs, and rumble units. These devices are usually assigned to a particular
    player, which can be retrieved by index.
*/
OOLUA_EXPORT_FUNCTIONS(PlayerIODevice)
OOLUA_EXPORT_FUNCTIONS_CONST(PlayerIODevice,
                             getPlayerIndex,
                             isAttached)

/*!
 * \qmlmethod unsigned PlayerIODevice::getPlayerIndex()
 *
 *  Returns the player index (0-3) of the player currently assigned the given device.
 */
/*!
 * \qmlmethod bool PlayerIODevice::isAttached()
 *
 *  Returns true if the given device is currently attached and is active or false if the
 *  device does not exist or has been removed.
 */

/*!
    \qmltype Mouse
    \since 1.6.0
    \inherits PlayerIODevice
    \inmodule Engine
    \brief Generic Mouse class representing PC, Dreamcast, USB, and other mice used as inputs.

    The engine supports a single mouse instance, which can represent any type of mouse on a PC,
    OUYA, ForgeTV, usually being removable USB. The Dreamcast mouse is also supported which the engine
    will automatically treat like any other mouse for you. Note that some platforms like smartphones aren't
    going to have mice, so do not count on a mouse always being connected and active.


    \sa {Input Devices}, {InputSystem}, {PlayerIODevice}, {Controller}, {Keyboard}, {VisualMemoryUnit}
*/
/*!
  \qmlproperty enumeration Mouse::BUT_TYPE
    The following button values for Mouse.BUT_TYPE are supported, although they cannot always be guaranteed to be available,
    on any platform configuration or at any given point during the runtime, as mice do vary:
    \list
    \li Mouse.BUT_NONE - No button, used in internal code by the engine
    \li Mouse.BUT_LEFT
    \li Mouse.BUT_MIDDLE
    \li Mouse.BUT_RIGHT
    \li Mouse.BUT_BACK
    \endlist
*/

/*!
 * \qmlmethod InputSystem::BUT_STATE Mouse::getButtonState(Mouse::BUT_TYPE button)
 *
 *  Returns the enumerated value representing the current state of the given \a{button}
 * \sa InputSystem::BUT_STATE, isPressed, isTapped
 */
/*!
 * \qmlmethod bool Mouse::isPressed(Mouse::BUT_TYPE button)
 *
 *  Returns true if the given \a{button} is currently being held down.
 * \sa getButtonState, isTapped
 */
/*!
 * \qmlmethod bool Mouse::isTapped(Mouse::BUT_TYPE button)
 *
 *  Returns true if the given \a{button} has just been pressed down this frame, and will return false every subsequent frame
 *  until it has been released and pressed again.
 * \sa isPressed, getButtonState
 */
/*!
 * \qmlmethod Vector2 Mouse::getScrollWheel()
 *
 *  Returns a 2D vector containing the current state of the mouse's scroll wheel. For normal scroll wheels, only the y
 *  component is used (for vertical scrolling). For trackballs and trackpads, both the x and y components are used for
 *  two-dimensional scrolling.
 */
/*!
 * \qmlmethod Vector2 Mouse::getScreenPos()
 *
 *  Returns a 2D vector containing the current screen-space position of the Mouse cursor.
 */

OOLUA_EXPORT_FUNCTIONS_CONST(Mouse,
                             getScreenPos,
                             getScrollWheel,
                             isTapped,
                             isPressed,
                             getButtonState)
OOLUA_EXPORT_FUNCTIONS(Mouse)

/*!
    \qmltype VisualMemoryUnit
    \since 1.6.0
    \inherits PlayerIODevice
    \inmodule Engine
    \brief Generic VMU class for either physical or emulated EVMU devices.

    The Dreamcast Visual Memory Unit is a multi-function device, acting as a memory card, a
    real-time secondary display unit, an audio output device, and a standalone 8-bit handheld
    gaming device. The engine provides a Sega Dreamcast-style Visual Memory Unit/System (VMU/VMS)
    abstraction allowing for uniform platform-independent management of physical or virtual VMU
    VMU devices, without requiring a Dreamcast.

    The engine allows each player to use these devices as an additional 48x32 pixel
    display for additional in-game content, as additional storage for save file data, as an
    8-bit sound output device, and allows you to upload VMU minigames to the device for when
    they function in standalone mode independent of the Engine.


    \section2 Physical Devices
    Physical Devices can either be actual Dreamcast VMUs connected to a Sega Dreamcast in the
    standard manner, or actual Dreamcast VMUs connected to a PC through some sort of USB
    converter.

    \section2 Virtual Devices (ElysianVMU)
    Virtual VMU devices are supported by interfacing with the "ElysianVMU" emulator, which is a
    platform-indepenent VMU emulator, built upon the same libGyro framework used by the Engine,
    allowing any Engine-supported platform (such as smart phones and tablets) to become a fully-
    functional Visual Memory Device. Since this functionality is implemented as an emulator,
    there is no need to write special software for virtual VMUs or to treat them any differently.
    The only difference is that communication between the engine and an ElysianVMU device is done
    through TCP rather than through the Dreamcast's Maple BUS or through USB. TCP communication
    is all handled internally by the engine.

    \sa {Input Devices}, {InputSystem}, {PlayerIODevice}, {Controller}, {Keyboard}, {Mouse}
*/
/*!
 * \qmlmethod bool VisualMemoryUnit::beep(unsigned pulseTotalLength, unsigned pulseActiveLength)
 *
 *  Creates a raw beep sound effect (square wave) using the given information and submits it to
 *  the VMU device for playback on its 8-bit buzzer. Returns true if the VMU returns a success message
 *  or false otherwise.
 *
 * \a{pulseTotalLength} is the total length of the square wave to be created. \a{pulseActiveLength} is
 * the sublength of \a{pulseTotalLength} which is to be the positive portion of the wave. Both values
 * are of type byte, which gives them a range from 0 to 255. Note that a \a{pulseActiveLength} that is
 * greater than the \a{pulseTotalLength} results in a malformed square wave.
 *
 * \e{NOTE: Not wave forms generate the same volume output response from the buzzer hardware, so certain
 *   ranges may be louder or softer (or even inaudible) on the actual device. Refer to the VMU datasheet
 *   for more info. (ElysianVMU handles all waveforms properly and uniformly (if enabled, as an optional
 *   feature on the client-side). }
 */

/*!
 * \qmlmethod bool VisualMemoryUnit::displayIcon(string filepath)
 *
 *  Sets the LCD display of the VMU device to the icon given located at \a{filepath}.
 *  The image must be in PNG format and must have the dimensions of 48x32 pixels, with
 *  only black and white (1-bit) color depth.
 *
 *  Returns true if the remote device returns a successful status code or false otherwise
 * (message timed out, VMU is disconnected or unavailable, invalid file, etc).
 */

OOLUA_EXPORT_FUNCTIONS(VisualMemoryUnit,
                       beep,
                       loadIcon)
OOLUA_EXPORT_FUNCTIONS_CONST(VisualMemoryUnit)

/*!
    \qmltype Keyboard
    \since 1.6.0
    \inmodule Engine
    \inherits PlayerIODevice
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
 * \qmlmethod InputSystem::BUT_STATE Keyboard::getButtonState(Keyboard::KEY_TYPE key)
 *
 *  Returns the state of the keyboard key given by \a key.
 * \sa InputSystem::BUT_STATE
 */
/*!
 * \qmlmethod bool Keyboard::isPressed(Keyboard::KEY_TYPE key)
 *
 *  Returns true if the keyboard key given by \a key is currently being held down.
 *
 */
/*!
 * \qmlmethod bool Keyboard::isTapped(Keyboard::KEY_TYPE key)
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
    \inmodule Engine
    \brief Class that maps a controller's analog stick to a directional pad.

    The VirtualDPad class is useful for supporting the analog stick in menus or dialog boxes, where traditionally
    a directional pad is used. The dpad buttons are considered down if the analog stick crosses a certain threshold,
    and they contain traditional button states such as down, up, released, and tapped.
*/

/*!
 * \qmlmethod InputSystem::BUT_STATE VirtualDPad::getButtonState(Controller::BUT_TYPE button)
 *
 *  Returns the state of the directional pad button given by \a button.
 * \sa InputSystem::BUT_STATE, Controller::BUT_TYPE
 */

/*!
 * \qmlmethod bool VirtualDPad::isPressed(Controller::BUT_TYPE button)
 *
 *  Returns whether or not the directional pad button given by \a button is currently being pressed.
 * \sa Controller::BUT_TYPE
 */

/*!
 * \qmlmethod bool VirtualDPad::isTapped(Controller::BUT_TYPE button)
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
    \inmodule Engine
    \inherits PlayerIODevice
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
 * \qmlmethod InputSystem::BUT_STATE Controller::getButtonState(Controller::BUT_TYPE button)
 *
 * Returns the state of the given \a button.
 * \sa InputSystem::BUT_STATE
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
 * \qmlmethod bool Controller::isPressed(Controller::BUT_TYPE button)
 *
 * Returns whether or not the specified \a button is currently being pressed and held down.
 */
/*!
 * \qmlmethod bool Controller::isTapped(Controller::BUT_TYPE button)
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

OOLUA_EXPORT_FUNCTIONS(Controller)
OOLUA_EXPORT_FUNCTIONS_CONST(Controller,
                             getAnalogStick,
                             getAnalogDir,
                             getAnalogMag,
                             getButtonState,
                             isPressed,
                             isTapped,
                             getVirtualDPad)


/*!
    \qmltype InputSystem
    \since 1.6.0
    \inmodule Engine
    \brief Subsystem for managing input devices such as controllers, keyboards, mice, and touch screens.

    The InputSystem updates and maintains objects for input devices, which are all stored and represented in a
    platform-independent manner. The InputSystem also handles hot swapping and dynamically adding/removing
    devices. Because of this, references to input devices should NEVER be cached between frames and should
    always be fetched from the InputSystem to ensure that they are valid and are still connected.

    \section2 Supported Devices
    \list
    \li 4 \l{Controller}s/Joysticks
    \li 1 \l{Keyboard}
    \li 1 \l{Mouse}
    \li 1 \e {Touch Screen}
    \li 4 \l{VisualMemoryUnit}s
    \li 4 \e{RumbleDevice}s
    \endlist

    \sa {Controller}, {Keyboard}, {Mouse}, {VisualMemoryUnit}, {PlayerIODevice}, {Input Devices}
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
 * \qmlmethod Mouse* InputSystem::getMouse()
 *
 * Returns a Mouse object if one is currently connected or nil if one isn't detected.
 * \sa Mouse
 */

/*!
 * \qmlmethod VisualMemoryUnit* InputSystem::getVMU(int port)
 *
 * Returns a VisualMemoryUnit object associated with the specified \a{port} or nil if there
 * isn't one.
 * \sa VisualMemoryUnit
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



OOLUA_EXPORT_FUNCTIONS(InputSystem,
                       getController,
                       getKeyboard,
                       getMouse,
                       getVMU)
OOLUA_EXPORT_FUNCTIONS_CONST(InputSystem)

