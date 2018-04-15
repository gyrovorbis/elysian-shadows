#include <proxy/elysian_input_system_proxy.hpp>


/*!
    \qmltype PlayerIODevice
    \since 1.6.0
    \inmodule Engine
    \brief Base class for all user interface devices (controllers, keyboards, mice, VMUs, etc)

    \sa {Input Devices}, {InputSystem}, {Controller}, {Keyboard}, {Mouse}, {RumbleDevice}, {VisualMemoryUnit}

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


    \sa {Input Devices}, {InputSystem}, {PlayerIODevice}, {Controller}, {Keyboard}, {RumbleDevice}, {VisualMemoryUnit}
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
    \qmltype RumbleEffect
    \since 1.6.2
    \inmodule Engine
    \brief Base class for all playable effects for RumbleDevices

    RumbleEffect is the base class for all effects that can be played on a \l{RumbleDevice}.
    This is a noninstantiable base-class with only one attribute common to all effects: duration.
    Refer to specific effects for more details.

    \table
        \header
            \li Effect Type
            \li Description
        \row
            \li \l{LeftRightRumbleEffect}
            \li Direct, low-level control over the intensities of the large and small motor.
        \row
            \li \l{ConstantRumbleEffect}
            \li Constant-intensity directional effect.
        \row
            \li \l{RampRumbleEffect}
            \li Linearly interpolate between two intensity levels (with direction).
        \row
            \li \l{PeriodicRumbleEffect}
            \li Wave-like oscillating effects (with direction)
    \endtable

    \section2 Demo Area
    Refer to \l{Rumble Test and Demo Area} for a live demo and source code.

    \sa {RumbleDevice}, {LeftRightRumbleEffect}, {ConstantRumbleEffect}, {RampRumbleEffect}, {PeriodicRumbleEffect}
*/
/*!
 * \qmlproperty float RumbleEffect::INFINITE
 *
 *  Constant used to specify the duration of an effect that plays indefinitely until the effect has been manually stopped.
 * \e{NOTE: Many \l{RumbleDevice}s do not honor infinite effects!}
 */
/*!
 * \qmlmethod void RumbleEffect::setDuration(float secs)
 *
 *  Sets the duration of the effect in seconds to \a{secs}.
 * \sa {RumbleEffect::INFINITE}
 */

OOLUA_EXPORT_FUNCTIONS(RumbleEffect,
                       setDuration)
OOLUA_EXPORT_FUNCTIONS_CONST(RumbleEffect)


/*!
    \qmltype LeftRightRumbleEffect
    \since 1.6.2
    \inherits RumbleEffect
    \inmodule Engine
    \brief Low-level, direct 2-motor intensity control

    LeftRightRumbleEffect is the lowest-level primitive RumbleEffect for directly controlling the intensity
    of each motor, without any additional abstraction or functionality. Most RumbleDevices contain two
    different motors, a small one (right) and large one (left) which can be operated independently and whose
    combined characteristics define the type of effect.

    \e{NOTE: This is normally not the preferred effect type, as the higher-level effects (\l{ConstantRumbleEffect}, \l{RampRumbleEffect},
    \l{PeriodicRumbleEffect}) offer a more powerful level of abstraction.}

    \section2 Demo Area
    Refer to \l{Rumble Test and Demo Area} for a live demo and source code.

    \sa {RumbleDevice}, {RumbleEffect}, {ConstantRumbleEffect}, {RampRumbleEffect}, {PeriodicRumbleEffect}
*/
/*!
 * \qmlmethod void LeftRightRumbleEffect::LeftRightRumbleEffect()
 *
 *  Default constructs a LeftRightRumble effect. The default large motor intensity, small motor intensity, and
 *  duration are all 0, meaning the effect will do nothing.
 */
/*!
 * \qmlmethod void LeftRightRumbleEffect::LeftRightRumbleEffect(float duration, float largeIntensity, float smallIntensity)
 *
 *  Constructs a LeftRightRumbleEffect with the given \a{duration}, setting the large motor intensity to \a{largeIntensity} and
 *  the small motor intensity to \a{smallIntensity}.
 */
/*!
 * \qmlmethod void LeftRightRumbleEffect::setLargeIntensity(float intensity)
 *
 *  Sets the intensity of the RumbleDevice's large motor to \a{intensity}.
 *
 * \e{NOTE: \a{intensity} is a normalized float, meaning it can range from 0 (off) to 1 (highest intensity).}
 *   \sa setSmallIntensity
 */
/*!
 * \qmlmethod void LeftRightRumbleEffect::setSmallIntensity(float intensity)
 *
 *  Sets the intensity of the RumbleDevice's small motor to \a{intensity}.
 *
 * \e{NOTE: \a{intensity} is a normalized float, meaning it can range from 0 (off) to 1 (highest intensity).}
 *   \sa setLargeIntensity
 */

OOLUA_EXPORT_FUNCTIONS(LeftRightRumbleEffect,
                       setLargeIntensity,
                       setSmallIntensity)
OOLUA_EXPORT_FUNCTIONS_CONST(LeftRightRumbleEffect)


/*!
    \qmltype DirectionalRumbleEffect
    \since 1.6.2
    \inherits RumbleEffect
    \inmodule Engine
    \brief Base-class for all RumbleEffects supporting direction and envelope

    DirectionalRumbleEffect is a noninstantiable base class inherited by all directional RumbleEffects providing several common
    properties and shared functionality. DirectionalRumbleEffects have a delay, a direction vector, and an "envelope."

    \section2 Direction
    All DirectionalRumbleEffects can have a 3D direction vector represting the direction of the force to be applied. MANY devices
    will completely ignore the directional parameter(s), and certain devices may only support 2D or 1D directions. Whether the device
    fully supports the dimensionality of the provided direction or not, libGyro should play the closest effect the hardware is capable
    of, so you should not have to worry about varying levels of support.

    \section2 Envelope
    Rumble "Envelope" is an SDL2 Haptic term used to describe the fade-in and fade-out characteristics of a particular effect. Again,
    many devices will totally ignore this too, in which case the main effect will be played for the entire duration without any fade-in
    or fade-out, so you should once again not have to worry about this. You may have a better chance at implementing fade-ins and fade-outs
    stringing together two \l{RampRumbleEffect}s and one \l{ConstantRumbleEffect}, if this functionality is absolutely necessary.
    \code
    intensity
    ^
    |
    |       intensity -->  _________________
    |                     /                 \
    |                    /                   \
    |                   /                     \
    |                  /                       \
    startIntensity --> |                        \
    |                  |                        |  <---  endIntensity
    |
    +--------------------------------------------------> Time

                       [--]                 [---]
                  fadeInDuration       fadeOutDuration

    [------------------][-----------------------]
           delay                 duration
    \endcode

    \table
        \header
            \li Property
            \li Description
            \li Value
        \row
            \li intensity
            \li Intensity of main effect (handled and stored separtely for each subtype)
            \li [0 (no effect) - 1 (max)]
        \row
            \li delay
            \li Amount of time to wait before playing (or fading into) the effect
            \li seconds (must be less than total duration)
        \row
            \li startIntensity
            \li The initial intensity of the effect, before the fade starts
            \li [0 (no effect) - 1 (max)]
        \row
            \li fadeInDuration
            \li Amount of time the effect takes to fade from the startIntensity to the effect intensity
            \li seconds (must be less than total duration)
        \row
            \li endIntensity
            \li The final intensity of the effect, once fading has ended
            \li [0 (no effect) - 1 (max)]
        \row
            \li fadeOutDuration
            \li Amount of time the effect takes to fade from intensity to endIntensity
            \li seconds (must be less than total duration)
   \endtable

    \section2 Types of Directional Effects
    For more detailed descriptions of the different types of DirectionalRumbleEffects, refer to the documentation of each child class.
    \list
        \li \l{ConstantRumbleEffect}
        \li \l{RampRumbleEffect}
        \li \l{PeriodicRumbleEffect}
    \endlist

    \section2 Demo Area
    Refer to \l{Rumble Test and Demo Area} for a live demo and source code.

    \sa {RumbleDevice}, {RumbleEffect}, {ConstantRumbleEffect}, {RampRumbleEffect}, {PeriodicRumbleEffect}, {LeftRightRumbleEffect}
*/
/*!
 * \qmlmethod void DirectionalRumbleEffect::setDelay(float secs)
 *
 *  Sets the delay of the applied effect to the value given by \a{secs} in seconds.
 */
/*!
 * \qmlmethod void DirectionalRumbleEffect::setDirection(Vector3 dir)
 *
 *  Sets the direction of the applied effect to the direction vector given by \a{dir}.
 */
/*!
 * \qmlmethod void DirectionalRumbleEffect::setEnvelope(float startIntensity, float fadeInDuration, float endIntensity, float fadeOutDuration)
 *
 *  Sets the properties of the effect's envelope. See \l{Envelope} for more details.
 */

OOLUA_EXPORT_FUNCTIONS(DirectionalRumbleEffect,
                       setDirection,
                       setDelay,
                       setEnvelope)
OOLUA_EXPORT_FUNCTIONS_CONST(DirectionalRumbleEffect)


/*!
    \qmltype ConstantRumbleEffect
    \since 1.6.2
    \inherits DirectionalRumbleEffect
    \inmodule Engine
    \brief Simple, constant-intensity directional rumble effect

    ConstantRumbleEffect is the most basic instantiable type of DirectionalRumbleEffect. It has only a single property,
    intensity, which is used to create a constant-level rumble effect.

    \section2 Demo Area
    Refer to \l{Rumble Test and Demo Area} for a live demo and source code.

    \sa {RumbleDevice}, {RumbleEffect}, {DirectionalRumbleEffect}, {RampRumbleEffect}, {PeriodicRumbleEffect}, {LeftRightRumbleEffect}
*/
/*!
 * \qmlmethod void ConstantRumbleEffect::ConstantRumbleEffect()
 *
 *  Default constructs a ConstantRumbleEffect. The default duration and intensity are both set to 0,
 * meaning the effect will do nothing.
 */
/*!
 * \qmlmethod void ConstantRumbleEffect::ConstantRumbleEffect(float duration, float intensity)
 *
 * Constructs a ConstantRumbleEffect with the given \a{duration} in seconds and the given normalized
 * \a{intensity} ranging from 0 (no effect) to 1 (maximum intensity).
 */
/*!
 * \qmlmethod void ConstantRumbleEffect::setIntensity(float intensity)
 *
 * Sets the normalized constant effect level to \a{intensity} which can range from 0 (no effect) to 1 (maximum intensity).
 */
OOLUA_EXPORT_FUNCTIONS(ConstantRumbleEffect,
                       setIntensity)
OOLUA_EXPORT_FUNCTIONS_CONST(ConstantRumbleEffect)


/*!
    \qmltype RampRumbleEffect
    \since 1.6.2
    \inherits DirectionalRumbleEffect
    \inmodule Engine
    \brief Increasing or decreasing intensity fading rumble effect

    RampRumbleEffect is a DirectionalRumbleEffect producing an effect that either increases or decreases from an initial to a
    final intensity over the given duration through linear interpolation. When startIntensity is smaller than endIntensity,
    the effect produces a rumble fade-in. When startIntensity is greater than endIntensity, the effect produces a rumble fade-out.

    \e{NOTE: When a RampRumbleEffect is used with a fadeIn/fadeOut \l{Envelope}, the two fades are multiplied together, resulting
    in a quadratic rate of change, but all devices support envelopes, and not all envelope-supporting devices support this functionality}

    \section2 Demo Area
    Refer to \l{Rumble Test and Demo Area} for a live demo and source code.

    \sa {RumbleDevice}, {RumbleEffect}, {DirectionalRumbleEffect}, {ConstantRumbleEffect}, {PeriodicRumbleEffect}, {LeftRightRumbleEffect}
*/
/*!
 * \qmlmethod void RampRumbleEffect::RampRumbleEffect()
 *
 *  Default constructs a RampRumbleEffect. The default duration, rampStartIntensity, and rampEndIntensity are all set to 0,
 *  meaning the effect will do nothing.
 */
/*!
 * \qmlmethod void RampRumbleEffect::RampRumbleEffect(float duration, float rampStartIntensity, float rampEndIntensity)
 *
 * Constructs a RampRumbleEffect with the given \a{duration} in seconds and the given normalized
 * \a{rampStartIntensity} and \a{rampEndIntensity} ranging from 0 (no effect) to 1 (maximum intensity).
 */
/*!
 * \qmlmethod void RampRumbleEffect::setRampStartIntensity(float intensity)
 *
 * Sets the normalized starting intensity of the ramp to \a{intensity} which can range from 0 (no effect) to 1 (maximum intensity).
 * \sa setRampEndIntensity
 */
/*!
 * \qmlmethod void RampRumbleEffect::setRampEndIntensity(float intensity)
 *
 * Sets the normalized ending intensity of the ramp to \a{intensity} which can range from 0 (no effect) to 1 (maximum intensity).
 * \sa setRampStartIntensity
 */
OOLUA_EXPORT_FUNCTIONS(RampRumbleEffect,
                       setRampStartIntensity,
                       setRampEndIntensity)
OOLUA_EXPORT_FUNCTIONS_CONST(RampRumbleEffect)


/*!
    \qmltype PeriodicRumbleEffect
    \since 1.6.2
    \inherits DirectionalRumbleEffect
    \inmodule Engine
    \brief Wave-like oscillating directional rumble effects

    PeriodicRumbleEffect is a DirectionalRumbleEffect used to create different types of oscillating, wave-like effects. Its properties
    offer full control over the shape of the waves and its characteristics.

    \section2 Waveform Properties
    \code
                      period
                     |     |
                __    __    __    __    __    _
               |  |  |  |  |  |  |  |  |  |   peakIntensity
               |  |__|  |__|  |__|  |__|  |   _
          -----
            |
          delay

        -------------------------------------
                      duration
        ===================================================
    \endcode
    \table
        \header
            \li Property
            \li Description
            \li Values
        \row
            \li period
            \li Time between two rising edges of the wave
            \li seconds
        \row
            \li intensityPeak
            \li Peak value; negative results in a 180 degree extra phase shift
            \li [0 - 1]
        \row
            \li intensityOffset
            \li Average value; extra intensity added at every point of the waveform
            \li [0 - 1]
        \row
            \li phaseShift
            \li Positive phase shift, moving the wave along the x (time) axis
            \li 1/100th of a degree
    \endtable

    \section2 Wave Shape Types
    The waveform generated by a PeriodicRumbleEffect may take one of the following shapes (by setting the appropriate
    \l{PERIODIC_TYPE}). Note not every device supports every wave type, but libGyro will choose the closest-supported waveform,
    so you should not have to worry about whether a device supports an exact shape or not.
    \table
        \header
            \li Type
            \li Waveform
         \row
            \li SINE
            \li \code
                  __      __      __      __
                 /  \    /  \    /  \    /
                /    \__/    \__/    \__/
                \endcode
        \row
            \li TRIANGLE
            \li \code
                  /\    /\    /\    /\    /\
                 /  \  /  \  /  \  /  \  /
                /    \/    \/    \/    \/
                \endcode
        \row
            \li SAWTOOTHUP
            \li \code
                  /|  /|  /|  /|  /|  /|  /|
                 / | / | / | / | / | / | / |
                /  |/  |/  |/  |/  |/  |/  |
                \endcode
        \row
            \li SAWTOOTHDOWN
            \li \code
                \  |\  |\  |\  |\  |\  |\  |
                 \ | \ | \ | \ | \ | \ | \ |
                  \|  \|  \|  \|  \|  \|  \|
                \endcode
    \endtable

    \section2 Demo Area
    Refer to \l{Rumble Test and Demo Area} for a live demo and source code.

    \sa {RumbleDevice}, {RumbleEffect}, {DirectionalRumbleEffect}, {ConstantRumbleEffect}, {RampRumbleEffect}, {LeftRightRumbleEffect}
*/

/*!
\qmlproperty enumeration PeriodicRumbleEffect::PERIODIC_TYPE

Selects the desired shape of the periodic waveform. See \l{Wave Shape Types} for a more detailed description of the waveform shapes.

PERIODIC_TYPE can have one of the following values
\list
\li PeriodicRumbleEffect.SINE
\li PeriodicRumbleEffect.TRIANGLE
\li PeriodicRumbleEffect.SAWTOOTHUP
\li PeriodicRumbleEffect.SAWTOOTHDOWN
\endlist

\sa {Wave Shape Types}
*/

/*!
 * \qmlmethod void PeriodicRumbleEffect::PeriodicRumbleEffect(PeriodicRumbleEffect::PERIODIC_TYPE type)
 *
 *  Default constructs a PeriodicRumbleEffect with the waveform shape given by \a{type}. The period and all other
 *  waveform properties default to 0, so the effect will do nothing if played.
 * \sa {PeriodicRumbleEffect::PERIODIC_TYPE}, {Wave Shape Types}
 */
/*!
 * \qmlmethod void PeriodicRumbleEffect::PeriodicRumbleEffect(float duration, float period, float intensityPeak)
 *
 * Constructs a RampRumbleEffect with the given \a{duration}, \a{period}, and \a{intensityPeak}.
 * \sa {Waveform Properties}
 */
/*!
 * \qmlmethod void PeriodicRumbleEffect::setPeriod(float secs)
 *
 * Sets the period of the intensity waveform to the duration given by \a{secs} in seconds.
 * \sa {Waveform Properties}, setIntensityPeak, setIntensityOffset, setPhaseShift
 */
/*!
 * \qmlmethod void PeriodicRumbleEffect::setIntensityPeak(float intensity)
 *
 * Sets the peak intensity of the intensity waveform to \a{intensity}, ranging from 0 (min) to 1 (max).
 * \sa {Waveform Properties}, setPeriod, setIntensityOffset, setPhaseShift
 */
/*!
 * \qmlmethod void PeriodicRumbleEffect::setIntensityOffset(float intensity)
 *
 * Sets the intensity offset to the value given by \a{intensity} ranging from 0 (min) to 1 (max).
 * \sa {Waveform Properties}, setPeriod, setIntensityPeak, setPhaseShift
 */
/*!
 * \qmlmethod void PeriodicRumbleEffect::setPhaseShift(float val)
 *
 * Sets the phaseShift of the intensity waveform to \a{val} given in 1/100ths of a degree.
 * \sa {Waveform Properties}, setPeriod, setIntensityPeak, setIntensityOffset
 */

OOLUA_EXPORT_FUNCTIONS(PeriodicRumbleEffect,
                       setPeriod,
                       setPhaseShift,
                       setIntensityPeak,
                       setIntensityOffset)
OOLUA_EXPORT_FUNCTIONS_CONST(PeriodicRumbleEffect)

/*!
    \qmltype RumbleDevice
    \since 1.6.1
    \inherits PlayerIODevice
    \inmodule Engine
    \brief Generic class representing force-feedback capable devices

    A RumbleDevice object represents any device capable of outputting any sort of rumble, vibration, or force-feedback
    to the player. Most controllers have this type of functionality built-in, but some controllers (such as the Sega Dreamcast),
    use additional peripherals that can be plugged into a slot located on the controller. RumbleDevice will handle all of these
    uniformly.

    \section2 Effects Support
    There is a wide array of different rumble effects that can be played by the Engine, but due to variation in
    hardware, no particular effect (other than the simple \l{play} functionality) can be guaranteed to be supported on
    any particular device. The Engine will do its best to play the closest suitable effect on devices incapable of playing
    the exact effect provided.

    For example, some devices support 3D directional rumble, while others support simple 2D directional rumble. Then devices
    like the Sega Dreamcast's Rumble (PuruPuru) with only a single motor can only play simple, nondirectional effects. If a
    3D effect is played on a 2D device, the z component is dropped, and it is played in 2D. If a 3D effect is played on a
    single-motor device like the Dreamcast, it will be converted from a positional effect to a simple intensity-based effect.


    \section2 Playing A Simple Effect
    \code
        --grab reference to the RumbleDevice attached to slot 0
        local rumbleDev = Engine.getInputSys():getRumbleDevice(0)

        --play a simple rumble effect at half intensity for 3 seconds
        rumbleDev:play(0.5, 3.0)

        --stop the effect
        rumbleDev:stop()
    \endcode

    \section2 Events
    \table
        \header
            \li Event
            \li Description
         \row
            \li Event.RUMBLE_DEVICE_ATTACHED
            \li Emitted whenever a new rumble-capable device is detected
         \row
            \li Event.RUMBLE_DEVICE_REMOVED
            \li Emitted whenever an existing rumble-capable device is removed
    \endtable

    \section2 LibGyro Driver Implementation
    Rumble support is implemented via the SDL2Haptic API on non-Dreamcast platforms and uses the KallistiOS Maple API on the
    Dreamcast.

    \section2 Rumble Test and Demo Area
    An Example \l{Area} has been provided that serves as an example of working with RumbleDevices and RumbleEffects and also
    as a means of testing the various different types of effects on different devices. The area provides and interactive
    means of playing and modifying the attributes of each RumbleEffect type. Interactive commands and outputs are provided
    via the \l{Real-Time Lua Terminal}.

    The behavior script is located at: \l{file:/assets/level/Examples/area/RumbleTest/rumble_behavior.lua}
    The Demo Area is located at: \l{file:/assets/level/Examples/area/RumbleTest}

    \section3 Keyboard Controls
    \table
        \header
            \li Key
            \li Action
        \row
            \li T
            \li Sets the Toolkit's selection to the various effect attributes tables, allowing you to configure the properties of each effect
        \row
            \li S
            \li Stops any active effect
        \row
            \li L
            \li Plays a LeftRightRumbleEffect with the given parameters
        \row
            \li C
            \li Plays a ConstantRumbleEffect with the given parameters
        \row
            \li R
            \li Plays a RampRumbleEffect with the given parameters
        \row
            \li P
            \li Plays a PeriodicRumbleEffect with the given parameters
   \endtable

    \sa {Input Devices}, {InputSystem}, {PlayerIODevice}, {Controller}, {Keyboard}, {RumbleDevice}, {VisualMemoryUnit}
*/

/*!
 * \qmlmethod bool RumbleDevice::play(float intensity, float duration)
 *
 *  Plays a simple rumble effect with the given \a{intensity} for the specified \a{duration}, in seconds.
 *  Returns true if successful or false otherwise.
 * \e{NOTE: \a{intensity} is a normalized float ranging from 0.0 (min) to 1.0 (max).}
 * \sa stop
 */
/*!
 * \qmlmethod bool RumbleDevice::stop()
 *  Stops any effect currently being played on the RumbleDevice. Returns true if the command was successful,
 *  or false otherwise.
 * \sa play
 */


OOLUA_EXPORT_FUNCTIONS_CONST(RumbleDevice,
                             stop,
                             play,
                             playEffect)
OOLUA_EXPORT_FUNCTIONS(RumbleDevice)

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

    \section2 Reading and Writing to the LCD Display
    The engine maintains an internal local copy of the VMU's framebuffer which is only submitted
    to the VMU for updating once per-frame, and only if the framebuffer has actually changed, so
    doing lots of reads and writes to the framebuffer will not negatively impact performance, as
    it's only referencing the local cache, and the Engine will handle updating the actual device
    screen for you.

    \sa {Input Devices}, {InputSystem}, {PlayerIODevice}, {Controller}, {Keyboard}, {Mouse}, {RumbleDevice}
*/
/*!
 * \qmlmethod bool VisualMemoryUnit::beep(unsigned pulseTotalLength, unsigned pulseActiveLength)
 *
 *  Creates a raw beep sound effect (square wave) using the given information and submits it to
 *  the VMU device for playback on its 8-bit buzzer. Returns true if the VMU returns a success message
 *  or false otherwise. The sound generated will essentially play on repeat until its cleared
 *  (send any pulse with a pulseActiveLength of 0).
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
 *  The image must be in .vmuico format and must have the dimensions of 48x32 pixels, with
 *  only black and white (1-bit) color depth. ESTk can convert PNGs of these resolutions to
 *  the compressed .vmuico format for you.
 *
 *  Returns true if the remote device returns a successful status code or false otherwise
 * (message timed out, VMU is disconnected or unavailable, invalid file, etc).
 */
/*!
 * \qmlmethod bool VisualMemoryUnit::frameBufferReadPixel(int x, int y)
 *
 *  Returns true if the given framebuffer pixel at coordinate <\a{x}, \a{y}> is
 *  black, and false if it is white.
 *
 *  \e{NOTE: The valid range for \a{x} is [0, 48] while the valid range for \a{y}
 *  is [0, 32] (the VMU LCD screen resolution).}
 * \sa frameBufferWritePixel
 *
 */
/*!
 * \qmlmethod bool VisualMemoryUnit::frameBufferWritePixel(int x, int y, bool value)
 *
 *  Sets the given pixel at coordinate <\a{x}, \a{y}> on the VMU's LCD screen to
 *  black if \a{value} is true or white if \a{value} is false.
 *
 *  \e{NOTE: The valid range for \a{x} is [0, 48] while the valid range for \a{y}
 *  is [0, 32] (the VMU LCD screen resolution).}
 * \sa displayIcon, frameBufferReadPixel
 */

OOLUA_EXPORT_FUNCTIONS(VisualMemoryUnit,
                       beep,
                       loadIcon,
                       frameBufferWritePixel)
OOLUA_EXPORT_FUNCTIONS_CONST(VisualMemoryUnit,
                             frameBufferReadPixel)

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
    \li 4 \l{RumbleDevice}s
    \endlist

    \sa {Controller}, {Keyboard}, {Mouse}, {RumbleDevice}, {VisualMemoryUnit}, {PlayerIODevice}, {Input Devices}
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
 * \qmlmethod RumbleDevice* InputSystem::getRumbleDevice(int port)
 *
 * Returns a RumbleDevice object associated with the specified \a{port} or nil if there
 * isn't one.
 * \sa RumbleDevice
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
                       getRumbleDevice,
                       getVMU,
                       setDefaultVMUIcon,
                       broadcastVMUIcon,
                       broadcastVMUBeep)
OOLUA_EXPORT_FUNCTIONS_CONST(InputSystem)

