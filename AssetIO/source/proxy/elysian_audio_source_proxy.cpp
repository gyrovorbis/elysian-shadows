#include <proxy/elysian_entity_proxy.hpp>

EXPORT_OOLUA_FUNCTIONS_0_CONST(GYWav)
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(GYWav)


/*!
    \qmltype AudioSource
    \inherits Component
    \since 1.6.0
    \inmodule ESToolkit
    \brief Component for adding sound effects to an Entity.

    An AudioSource is a Component that can be attached to an Entity, providing it with an audio emitter
    enabling it to play WAV sound effects in 3D positional space. The sound is emitted from the entity's
    position and will be relative to the audio listener at the party lead's location.
    Sounds started through an AudioSource will automatically claim a free channel from the AudioSystem
    and release it once the sound is complete, so audio channels do not have to be managed manually.

    \sa {AudioSystem}
*/
/*!
 * \qmlmethod void AudioSource::stop()
 *
 * Stops the WAV currently being played by the AudioSource (if there is one playing).
 */
/*!
 * \qmlmethod void AudioSource::start()
 *
 * Claims a free channel from the AudioSystem or the oldest channel if none are available
 * and plays the current WAV if one has been set.
 */
/*!
 * \qmlmethod void AudioSource::setGain(float factor)
 *
 * Sets the gain of the AudioSource to the value specified by \a{factor}. Gain can be thought of
 * as a volume multiplier. Possible gain values:
 * \list
 * \li 0.0   - muted
 * \li < 1.0 - decreased volume
 * \li 1.0   - normal volume
 * \li > 1.0 - increased volume
 * \endlist
 * \sa getGain()
 */
/*!
 * \qmlmethod float AudioSource::getGain()
 *
 * Returns the floating point gain of the emitter.
 * \sa setGain()
 */
/*!
 * \qmlmethod void AudioSource::setPitch(float pitch)
 *
 * Sets the pitch of the emitter to \a{pitch}.
 * \sa getPitch()
 */
/*!
 * \qmlmethod float AudioSource::getPitch()
 *
 * Returns the floating point pitch of the emitter.
 * \sa setPitch()
 */
/*!
 * \qmlmethod bool AudioSource::getLooping()
 *
 * Returns whether or not sound effects started on the AudioSource are played continuously in a loop.
 * \sa setLooping()
 */
/*!
 * \qmlmethod void AudioSource::setLooping(bool value)
 *
 * Sets the looping value of the AudioSource to \a{value}. If \a{value} is true, all sound effects played on the
 * AudioSource are played continuously in a loop. If \a{value} is false, they are only played once before stopping.
 * \sa getLooping()
 */

OOLUA_EXPORT_FUNCTIONS(AudioSource,
                                    setWav,
                                    playWav,
                                    play,
                                    stop,
                                    setGain,
                                    setPitch,
                                    setLooping)
OOLUA_EXPORT_FUNCTIONS_CONST(AudioSource,
                            getLooping,
                            getPitch,
                            getGain)
