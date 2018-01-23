#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_audio_system_proxy.hpp>

/*!
    \qmltype AudioSystem
    \since 1.6.0
    \inmodule Engine
    \brief Subsystem responsible for handling background music, sound effects, and \l{AudioSource}s.

    The AudioSystem is a subsystem of the engine which manages all aspects of audio playback,
    including sound effects, multitrack background music, and audio channels. It also manages any
    active \l{Entity} within the current \l{Area} using an \l{AudioSource} \l{Component} as well.

    All audio uses Dolby surround sound in 3D space by default. On platforms where this is not
    available or when the player is using headphones or shitty speakers, libGyro attemps to
    emulate the positional audio in 2D for stereo sound. The default implementation of the
    libGyro audio framework used by Windows, MacOS, Linux, iOS, and Droid uses \l{http://www.openal.org/}
    {OpenAL} as its back-end, and other platforms seek to emulate OpenAL's behavior. The
    \l {https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf} {OpenAL Documentation}
    may be referenced for the exact behaviors and formulae of gain, pitch, rolloff, and similar values.

    \section2 Obtaining a Reference to the AudioSystem
    The AudioSystem is contained within the \l{Engine} class and can be retrieved with the following Lua code:
    \code
    audioSys = Engine.getAudioSys()
    \endcode

    \section2 Audio Channels
    Every sound effect or song played through the AudioSystem is played on a particular "channel."
    The AudioSystem maintains 32 audio channels internally. Some of these channels are reserved, some
    are managed automatically, and some are managed manually by Lua or the engine. The channels are
    allocated as follows:

    \table
    \header
        \li Range
        \li Use
    \row
        \li 0-4
        \li Customizable SFX channels for use by Lua scripts
    \row
        \li 5-9
        \li Reserved for Engine SFX (\l{DialogSystem}, \l{PartySystem}, etc)
    \row
        \li 10
        \li Reserved for BGM OGG track
    \row
        \li 11
        \li Reserved for BGM OGG morph track
    \row
        \li 12
        \li Reserved for AUX 1 OGG track
    \row
        \li 13
        \li Reserved for AUX 1 OGG morph track
    \row
        \li 14
        \li Reserved for AUX 2 OGG track
    \row
        \li 15
        \li Reserved for AUX 2 OGG morph track
    \row
        \li 16-31
        \li Reserved to be dynamically allocated to \l{AudioSource} \l{Component} for
            playing SFX
    \endtable

    \section3 Managing Audio Channels
    Channel management is handled automatically for OGG tracks and for positional audio played through an
    \l{Entity} with an \l{AudioSource} \l{Component}. Since there is no limit on the number of \l{AudioSource}s
    that can be active within an \l{Area} at once or on the number of WAVs played simultaneously, if more than 15
    \l{AudioSource}s attempt to play SFX simultaneously, the AudioSystem will be forced to recycle a channel
    and reallocate it before one of the previous sounds is complete, which is probably not noticeable for minor
    combat sound effects, but could potentially be very bad for more important sounds.

    For this reason, channels 5-9 are managed manually by the engine and channels 0-4 are managed manually
    by you through Lua scripts. These channels are generally used for high-priority sound effects that are not emitted
    positionally from within an \l{Area}, such as menu sounds, death or level-up sound effects, or notifications,
    that should never be interrupted by another sound. Managing channels 0-4 manually for SFX is more work,
    but it's FAR better than having something like a long level-up or death sound effect be interrupted by a
    quarter-second slash attack sound. By managing these channels manually, you can ensure that your own
    priorities are respected. A good practice is to use channel 0 for the highest priority Lua sounds and channel
    4 for the lowest priority Lua sounds.

    \section3 Audio Channel Properties
    The following is a list of the different properties that can be manipulated for a given audio channel:
    \list
    \li \b{gain} - volume level
    \li \b{pitch} - pitch factor applied to audio samples
    \li \b{looping} - whether audio played on the channel loops indefinitely or stops after a single playback
    \li \b{rolloff factor} - factor used for attenuating volume based on distance from an audio source
    \li \b{position} - position of the audio source in 3D space
    \endlist

    \section2 Positional Vs Nonpositional Audio
    \section3 Positioal Audio
    Positional sounds are given a location in the game world, so that the audio sounds like it's
    coming from a particular direction relative to the player by playing the audio louder in certain
    speakers than in others. For example, an enemy using positional audio to emit a growl from the
    left of the player will result in the growl being heard mostly in the left speaker. The volume of the audio
    also attenuates as you move away from it, so that it can be heard more loudly the closer you are to the object.
    Positional audio is very useful for sound effects being emitted from an \l{Entity}.

    \section3 Nonpositional Audio
    Nonpositional audio does not have a position and does not attenuate volume based on distance. It is
    played uniformly through all speakers as though the sound does not have any one particular origin.
    Nonpositional audio is traditionally used for background music and is also very useful for sound
    effects playing from menus, GUIs, or overlays that are not part of the game world.

    \section2 Sound Effects
    All sound effects loaded and played by libGyro must be in the wave format with the a ".wav" extension,
    as it has no compression and requires no overhead to decode before being submitted to the sound hardware.

    There are two ways to play a sound effect in the engine:
    \list
    \li \b{Directly through AudioSystem} - Handles nonpositional SFX, although they can also optionally
    be made positional by manually setting a position. Channels are managed manually.
    \li \b{Indirectly through the \l{AudioSource} \l{Component}} - Handles ONLY positional SFX emitted from
    an Entity within an \l{Area}. Position and channels are handled automatically.
    \endlist

    To play or manage sound effects through the \l{AudioSource} \l{Component} (which should be prefered for
    positional SFX), refer to the \l{AudioSource} {AudioSource Documentation}. To play or manage sounds effects
    directly through the AudioSystem, refer to the following methods:
    \list
    \li playSfxChannel()
    \li stopSfxChannel()
    \li setSfxChannelGain()
    \li setSfxChannelPitch()
    \li setSfxChannelLooping()
    \li setSfxChannelRolloffFactor()
    \li setSfxChannelPos()
    \endlist

    \section3 Playing a Sound Effect through Lua
    To play a sound effect on one of the manually-managed channels in Lua refer to the following code:
    \code
    audioSys = Engine.getAudioSys()
    wav = gyro.createWav("/assets/sfx/slash.wav")
    audioSys:setSfxChannelGain(0, 2.0)
    audioSys:setSfxChannelLooping(0, true)
    audioSys:playSfxChannel(0, wav)
    \endcode
    The preceding code retrieves a reference to the AudioSystem from the \l{Engine} then creates a \l{GYWav}
    object by specifying the asset-relative path to a .wav file. Then the volume of channel 0 is doubled,
    and it is set to loop. Finally, the wav is played on channel 0, looping indefinitely, with twice the
    normal volume level.

    \section2 Music
    Music is divided into a number of different background music tracks and includes two auxiliary tracks.
    All music is streamed from disk/CD/flash using separate threads and must be saved using the OGG
    format with a ".ogg" extension, as it's free, open-source, and has a good tradeoff between compression
    size and processing required for decoding. There is one dedicated background music channel and two
    (optional) auxiliary music channels.

    Both the background track and the auxiliary tracks can optionally be set to positional and be
    given a position in 3D space. You can create effects such as making the background theme
    sound like it's being played from a jukebox, radio, or piano player in-game in this manner.

    \section3 Fading OGG Tracks
    Each OGG track (BGM track + 2 Auxiliary tracks) is capable of performing a fade effect, to
    fade out, fade in, and fade into another OGG.

    \section3 Morphing OGG Tracks
    Depending on hardware restraints, each OGG track may also have a mirror OGG track allowing it
    to perform a "morph" effect when transitioning into another OGG. The morph effect is accomplished
    by fading out one track while another track simultaneously fades in. When this is used to
    fade between two different versions of the same song, the transition sounds seamless. This can
    be used to achieve situational audio.

    \e {NOTE: Certain platforms (Dreamcast, OUYA, smartphones), do not have the resources required to
    simultaneously decode two OGGs for each track, so using a morph transition will automatically default
    to using a fade transition by the engine for these platforms.}

    \section3 Background Music
    The main background song that is played when the player enters an \l{Area} is called the background
    track. There are three different background tracks with only one of the three serving as the active
    track at any given time. By providing three tracks, you are able to load three different OGGs at once
    then swap between them as needed at any point during run-time. You can use different songs for different
    portions of the map, trigger a song change based on an event, or even use three variations of the same
    song and dynamically fade/morph between them to use a different variation depending on the
    circumstances.

    To manage background music and its three tracks, refer to the following member functions:
    \list
    \li loadBgmTrack()
    \li playBgmTrack()
    \li stopBgmTrack()
    \li pauseBgmTrack()
    \li fadeOutBgmTrack()
    \li fadeInBgmTrack()
    \li fadeIntoBgmTrack()
    \li fadeIntoNewBgmTrack()
    \li morphIntoBgmTrack()
    \li morphIntoNewBgmTrack()
    \li setBgmTrack()
    \li setBgmTrackLooping()
    \li setBgmTrackGain()
    \li isBgmTrackPlaying()
    \li getBgmTrack()
    \li getBgmTrackPath()
    \li getBgmTrackTime()
    \li setBgmTrackPitch()
    \li setBgmTrackPos()
    \li setBgmTrackRolloffFactor()
    \endlist

    \section3 Auxiliary Tracks
    Auxiliary tracks are 2 optional general-purpose OGG tracks. They can be used for adding persistent sound effects
    over the background music. For example, these tracks can be used for the howling wind on a mountain, or the
    sound of waves crashing around an ocean.

    \e{NOTE: Due to system limitations, not every platform is guaranteed to have these auxiliary tracks.
    For platforms that can only handle a single track, aux track 0 has higher priority, and aux track 1 has
    lower priority, although neither track is necessarily guaranteed to exist on any platform other than PC.
    Because of this, ensure that audio played on these tracks is merely supplementary and do not base gameplay
    off of it!}

    The auxiliary tracks are managed the same way as the background music tracks, except that both of them
    can be played simultaneously, whereas only one BGM track can ever be active at one time. To manage the
    auxililary tracks and channels, refer to the following member functions:
    \list
    \li loadAuxTrack()
    \li playAuxTrack()
    \li stopAuxTrack()
    \li pauseAuxTrack()
    \li fadeOutAuxTrack()
    \li fadeInAuxTrack()
    \li fadeIntoNewAuxTrack()
    \li morphIntoNewAuxTrack()
    \li isAuxTrackPlaying()
    \li getAuxTrackPath()
    \li getAuxTrackTime()
    \li setAuxTrackPitch()
    \li setAuxTrackPos()
    \li setAuxTrackRolloffFactor()
    \li setAuxTrackLooping()
    \li setAuxTrackGain()
    \endlist

    \section3 Playing and Managing OGGs
    \section4 Setting OGGs through ESTk GUI for Automatic Playback
    In general, the 3 BGM and 2 AUX OGG tracks should not have to be loaded or played manually through the AudioSystem
    API. \l{Area} contains properties for the paths corresponding to each OGG track (see Area::music), so that when
    the active \l{Area} changes, the AudioSystem automatically loads the OGG files provided by \l{Area} and
    begins playback using a fade-in effect.

    \section4 Manual OGG Playback
    If more control is required, OGGs can still be managed manually by accessing the AudioSystem directly from
    a Lua script. For example, maybe the tracks shouldn't fade in. Maybe the BGM track should fade in, but
    it should later transition to another BGM track programmatically. Maybe the auxiliary tracks should only
    start playing after an event has been triggered. Maybe you want to adjust volume levels or some other \l{Audio Channel Properties}
    {channel property}... These are all scenarios where manual track management is required through a Lua script.

    Some simple Lua code for manually loading and manipulating tracks is as follows:
    \code
    audioSys = Engine.getAudioSys()
    audioSys:loadBgmTrack(1, "/assets/music/forest2.ogg")
    audioSys:loadBgmTrack(2, "/assets/music/forest3.ogg")
    audioSys:setBgmTrackGain(0.5)
    audioSys:setBgmTrackRolloffFactor(1.0)
    audioSys:setBgmTrackPosition(0, 0, 0)
    audioSys:fadeIntoNewBgmTrack(0, 5.0, "/assets/music/forest1.ogg")

    \endcode
    The preceding code fetches a reference to the AudioSystem from the \l{Engine}. It then loads "forest2.ogg" on the
    second BGM track, "forest3.ogg" on the third BGM track, then fades in "forest1.ogg" as the first track over the next 5 seconds,
    before continuing playback normally. The volume is then reduced by half, the audio is made positional by setting a rolloff factor,
    and it is given a position of <0, 0, 0>, so it sounds as though the background music is coming from the top
    left of the map.

    Then later on the active BGM track can be swapped from track 0 to track 1 immediately with the following code:
    \code
    audioSys:setBgmTrack(1)
    \endcode
    or BGM track 0 can fade into track 1:
    \code
    audioSys:fadeIntoBgmTrack(1, 3.0)
    \endcode
    or
    or BGM track 0 can morph into track 1:
    \code
    audioSys:morphIntoBgmTrack(1, 3.0)
    \endcode


    \sa AudioSource, GYWav, Area
*/
/*!
 * \qmlmethod void AudioSystem::playSfxChannel(int channel, GYWav* wav, EventMonitor* monitor)
 *
 * Plays the WAV contained by \a{wav} on the channel given by \a{channel}.
 * If the given \a{channel} was already playing a WAV, it will be interrupted. If \a{monitor}
 * is not nil, an event notification will be triggered when the sound effect is done playing.
 *
 * \e{NOTE: A channel of -1 can by used to specify a channel within the dynamic range which
 * will automatically be allocated to play the WAV and deallocated once playback is complete.}
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 * \sa stopSfxChannel()
 */
/*!
 * \qmlmethod void AudioSystem::stopSfxChannel(int channel)
 * Stops the SFX that was playing on the given \a{channel}, if there was one.
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 * \sa playSfxChannel()
 */
/*!
 * \qmlmethod void AudioSystem::setSfxChannelGain(int channel, float factor)
 * Sets the gain value (volume) for the given \a{channel} to \a{factor}.
 * By default, the gain is set to 1.0, which is the normal volume level.
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 */
/*!
 * \qmlmethod void AudioSystem::setSfxChannelPitch(int channel, float pitch)
 * Sets the pitch value for the given \a{channel} to \a{pitch}. The default
 * value is 1.0.
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 */
/*!
 * \qmlmethod void AudioSystem::setSfxChannelLooping(int channel, bool value)
 * If \a{value} is true, sets the given \a{channel} to repeat any SFX played on it
 * in a loop. Otherwise (default), SFX only play once before stopping.
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 */
/*!
 * \qmlmethod void AudioSystem::setSfxChannelRolloffFactor(int channel, float factor)
 * After making the channel positional, sets the rolloff factor for \a{channel} to \a{factor}.
 * The rolloff factor is how much the volume attenuates as a function of distance from
 * the audio source. The default rolloff factor values for static channels are 0.0,
 * meaning they aren't positional, because distance does not attenuate the volume.
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 *
 * \e{NOTE: If a channel is given a nonzero rolloff factor, it also needs to be give a position
 * for positional audio to play correctly!}
 * \sa setSfxChannelPos()
 */
/*!
 * \qmlmethod void AudioSystem::setSfxChannelPos(int channel, float x, float y, float z)
 * Makes the given \a{channel} positional, by moving its audio source to the 3D position
 * given by <\a{x}, \a{y}, \a{z}>.
 *
 * Refer to \l{Audio Channels} for valid \a{channel} values.
 *
 * \e {NOTE: If a position is set for an SFX channel with a rolloff factor of 0.0, that position
 * is ignored. A nonzero rolloff factor must be set to make the channel positional.}
 *
 * \sa setSfxChannelRolloffFactor()
 */
/*!
 * \qmlmethod void AudioSystem::loadBgmTrack(int track, string filePath)
 *
 * Loads the OGG file at the asset-relative location given by \a{filePath}
 * to be played on the given background \a{track}. If there was another OGG loaded for
 * the given \a{track} it will be freed before the new one is loaded. Valid
 * values for \a{track} are 0-2.
 * \sa fadeIntoNewBgmTrack(), morphIntoNewBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::setBgmTrack(int track)
 *
 * Sets the given \a{track} to be the active track. If the previous track was
 * in the middle of playing, the new track will also begin playing. Valid
 * values for \a{track} are 0-2.
 * \sa getBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::fadeOutBgmTrack(float secs)
 *
 * Linearly interpolates the gain (volume) from the current level to 0.0
 * over a span of \a{secs} seconds for the currently active BGM track.
 * \sa fadeInBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::fadeInBgmTrack(float secs)
 *
 * Linearly interpolates the gain (volume) from 0.0 to 1.0 (normal)
 * over a span of \a{secs} seconds and continues playback,
 * for the currently active BGM track.
 * \sa fadeOutBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::morphIntoBgmTrack(int newTrack, float secs)
 *
 * Morphs the currently active background track into the track given by \a{newTrack}
 * over the next \a{secs} seconds before continuing normal playback with \a{newTrack}
 * as the active track. The morph effect is done by fading out the first track while
 * simultaneously fading in the second track. Valid values for \a{newTrack} are 0-2.
 * \e{NOTE: If the platform does not have the resources to support multiple simultaneous
 * OGG streams, this automatically calls fadeIntoBgmTrack() instead.}
 * \sa morphIntoNewBgmTrack(), fadeIntoBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::setBgmTrackLooping(bool val)
 *
 * If \a{val} is true, tells the AudioSystem to loop any BGM track that becomes the
 * active track. If false, the active track will only be played once, then stop.
 * The default value is true.
 */
/*!
 * \qmlmethod void AudioSystem::setBgmTrackGain(float factor)
 *
 * Sets the gain value (volume) of any BGM track which becomes the active track to
 * \a{factor}. A gain of 0.0 mutes the track, while a gain of 2.0 doubles the volume.
 * The default gain is 1.0, which is the normal volume level.
 */
/*!
 * \qmlmethod void AudioSystem::fadeIntoBgmTrack(int newTrack, float secs)
 *
 * Fades the currently playing BGM track out after \a{secs}/2 seconds, then fades
 * the track given by \a{newTrack} in after another \a{secs}/2 seconds before setting
 * it as the active track. The whole transition takes \a{secs} seconds. If there is no
 * active track before the transition, the first half is skipped.
 * \sa morphIntoBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::morphIntoNewBgmTrack(int trackNumber, float secs, string filePath)
 *
 * This function is essentially the same as morphIntoBgmTrack(), only it doesn't require
 * the OGG already be loaded for the new track.
 *
 * Morphs the currently active BGM track into the track given by \a{trackNumber}, which
 * becomes a new OGG given by \a{filePath} over \a{secs} seconds. The old OGG loaded for
 * \a{trackNumber} is unloaded, and the new OGG given by \a{filePath} becomes loaded for
 * the track.
 *
 * The morph transition is created by fading out the old track while simultaneously
 * fading in the new track. This makes the transition sound seamless for similar
 * tracks. If no BGM track was playing, the new track is essentially just faded in.
 *
 * \e{NOTE: If the platform does not have the resources to support multiple simultaneous
 * OGG streams, this automatically calls fadeIntoNewBgmTrack() instead.}
 * \sa morphIntoBgmTrack(), fadeIntoNewBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::fadeIntoNewBgmTrack(int trackNumber, float secs, string filePath)
 *
 * This function is essentially the same as fadeIntoBgmTrack(), only it doesn't require
 * the OGG already be loaded for the new track.
 *
 * Fades the currently active BGM track into the track given by \a{trackNumber}, which
 * becomes a new OGG given by \a{filePath} over \a{secs} seconds. The old OGG loaded for
 * \a{trackNumber} is unloaded, and the new OGG given by \a{filePath} becomes loaded for
 * the track.
 *
 * The fade transition is created by first fading out the active BGM track over
 * \a{secs}/2 seconds, then fading in the new BGM track over the next \a{secs}/2
 * seconds. The whole effect completes in \a{secs} seconds.
 *
 * If no BGM track was playing, the new track is essentially just faded in.
 * \sa fadeIntoBgmTrack(), morphIntoNewBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::playBgmTrack()
 *
 * Begins playing the track set as the currently active BGM track.
 * \sa setBgmTrack(), getBgmTrack(), stopBgmTrack(), pauseBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::stopBgmTrack()
 *
 * Stops the BGM track set as the currently active track and resets its playback location
 * to the beginning of the OGG file. This function does nothing if the
 * active BGM track was not currently playing.
 * \sa setBgmTrack(), getBgmTrack(), playBgmTrack(), pauseBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::pauseBgmTrack()
 *
 * Pauses the BGM track set as the currently active track while still maintaining
 * its playback location within the OGG file. This function does nothing if the
 * active BGM track was not currently playing.
 * \sa setBgmTrack(), getBgmTrack(), playBgmTrack(), stopBgmTrack()
 */
/*!
 * \qmlmethod bool AudioSystem::isBgmTrackPlaying()
 *
 * Returns true if a track is currently being played as the active BGM track and
 * false otherwise.
 * \sa playBgmTrack(), pauseBgmTrack(), stopBgmTrack()
 */
/*!
 * \qmlmethod int AudioSystem::getBgmTrack()
 *
 * Returns the track number of the active BGM track. This defaults to track 0.
 * \sa setBgmTrack()
 */
/*!
 * \qmlmethod string AudioSystem::getBgmTrackPath(int trackNumber)
 *
 * Returns the asset-relative file path for the OGG loaded on
 * the track given by by \a{trackNumber} or an empty string if no OGG
 * has been loaded yet. Valid values for \a{trackNumber} are 0-2.
 * \sa loadBgmTrack()
 */
/*!
 * \qmlmethod void AudioSystem::setBgmTrackPitch(float val)
 *
 * Sets the pitch of any BGM track that becomems the active track to
 * the pitch given by \a{val}. The default value is 1.0.
 */
/*!
 * \qmlmethod void AudioSystem::setBgmTrackPos(float x, float y, float z)
 * Sets the world location of the background track to the 3D position
 * vector given by <\a{x}, \a{y}, \a{z}>.
 *
 * \e{NOTE: If the track has a rolloff factor of 0, then this position vector is ignored. To
 * make the track positional, a nonzero rolloff factor must also be set!}
 * \sa setBgmTrackRolloffFactor()
 */
/*!
 * \qmlmethod void AudioSystem::setBgmTrackRolloffFactor(float factor)
 *
 * Sets the rolloff factor of any track played as the currently active
 * BGM track to \a{factor}. The rolloff factor determines how much
 * the volume fades away based on distance from an object. The default
 * rolloff factor for OGG tracks is 0.0, meaning no volume is lost as
 * a function of distance, meaning they aren't directional. The default
 * value for a directional \l{AudioSource} is 1.0.
 *
 * \e{NOTE: If a nonzero rolloff factor is used, a position must also be
 * given to the track for correct positional audio playback!}
 *
 * \sa setBgmTrackPos()
 */
/*!
 * \qmlmethod float AudioSystem::getBgmTrackTime()
 *
 * Returns the decoding offset of the current BGM track in seconds.
 */
/*!
 * \qmlmethod void AudioSystem::loadAuxTrack(int trackNum, string filePath)
 *
 * Loads the OGG given by the asset-relative path, \a{filePath}, as the auxiliary track
 * with the index given by \a{trackNum}. If there was already an OGG loaded on the given
 * track, it will automatically be freed. Valid values for \a{trackNum} are 0 and 1.
 * \sa fadeIntoNewAuxTrack(), morphIntoNewAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::fadeOutAuxTrack(int trackNum, float secs)
 *
 * Linearly interpolates the volume level of the auxiliary track given by \a{trackNum} from
 * its current gain to 0.0 over \a{secs} seconds before stopping the track.
 * Valid values for \a{trackNum} are 0 and 1.
 * \sa fadeInAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::fadeInAuxTrack(int trackNum, float secs)
 *
 * Begins playing the auxiliary track given by \a{trackNum} with muted volume (gain 0.0).
 * Linearly interpolates the gain to 1.0 over the next \a{secs} seconds then
 * continues playback normally. Valid values for \a{trackNum} are 0 and 1.
 * \sa fadeOutAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::fadeIntoNewAuxTrack(int trackNum, float secs, string filePath)
 *
 * Fades out the OGG playing on the auxiliary track index given by \a{trackNum} over \a{secs}/2 seconds
 * before freeing the OGG and replacing it with the OGG given by the asset-relative path, \a{filePath}.
 * The new OGG begins playing with a gain value of 0.0 (muted) and fades into a normal gain value of 1.0
 * before continuing playback normally. If no track was previously playing on \a{trackNum}, the fadeout
 * phase is skipped. The entire effect takes \a{secs} seconds. Valid values for \a{trackNum} are
 * 0 and 1.
 * \sa morphIntoNewAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::morphIntoNewAuxTrack(int trackNum, float secs, string filePath)
 *
 * Fades out the OGG currently playing on the auxiliary track given by \a{trackNum} while simultaneously
 * fading in the the OGG given by \a{filePath} over an interval of \a{secs} seconds. Afterwards the existing OGG is
 * replaced with the new OGG and playback is continued normally.
 *
 * If no track was previously playing on the auxiliary track index given by \a{trackNum}, the effect is
 * essentially a fade-in. Valid values for \a{trackNum} are 0 and 1.
 *
 * \e{NOTE: If the platform does not have the resources to support multiple simultaneous
 * OGG streams for the given auxiliary track, this automatically calls fadeIntoNewAuxTrack() instead.}
 * \sa fadeIntoNewAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::setAuxTrackLooping(int trackNum, bool value)
 *
 * Sets the auxiliary track with the given \a{trackNum} to play OGGs continuously in a loop once started
 * if \a{value} is true, otherwise only plays OGGs once before stopping. Auxiliary tracks
 * are set to loop by default. Valid values for \a{trackNum} are 0 and 1.
 */
/*!
 * \qmlmethod void AudioSystem::setAuxTrackGain(int trackNum, float factor)
 *
 * Sets the gain (volume) level of the auxiliary track given by \a{trackNum} to \a{factor}. A gain of 0.5
 * reduces the volume by half, while a gain of 2.0 doubles it. The gain is set to 1.0 by default,
 * which is the normal volume level. Valid values for \a{trackNum} are 0 and 1.
 */
/*!
 * \qmlmethod void AudioSystem::playAuxTrack(int trackNum)
 *
 * Begins playback of the OGG loaded on the auxiliary track given by \a{trackNum}.
 * Valid values for \a{trackNum} are 0 and 1.
 * \sa loadAuxTrack(), stopAuxTrack(), pauseAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::stopAuxTrack(int trackNum)
 *
 * Stops playback of the OGG playing on the auxiliary track given by \a{trackNum} and resets
 * the OGG stream to the beginning of the file. This function does nothing if there is no
 * OGG currently playing on \a{trackNum}. Valid values for \a{trackNum} are 0 and 1.
 * \sa playAuxTrack(), pauseAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::pauseAuxTrack(int trackNum)
 *
 * Pauses playback of the OGG playing on the auxiliary track given by \a{trackNum} while
 * maintaining the position of the OGG stream. This function does nothing if there is no
 * OGG currently playing on \a{trackNum}. Valid values for \a{trackNum} are 0 and 1.
 * \sa playAuxTrack(), stopAuxTrack()
 */
/*!
 * \qmlmethod bool AudioSystem::isAuxTrackPlaying(int trackNum)
 * Returns true if there is an OGG currently playing on the auxiliary track index given
 * by \a{trackNum} or false otherwise. Valid values for \a{trackNum} are 0 and 1.
 *
 * \sa playAuxTrack(), stopAuxTrack(), pauseAuxTrack()
 */
/*!
 * \qmlmethod string AudioSystem::getAuxTrackPath(int trackNum)
 * Returns the asset-relative path to the OGG loaded on the auxiliary track index given by
 * \a{trackNum} or an empty string if there isn't one. Valid values for \a{trackNum} are 0 and 1.
 *
 * \sa loadAuxTrack()
 */
/*!
 * \qmlmethod void AudioSystem::setAuxTrackPitch(int trackNum, float pitch)
 * Sets the pitch for the auxiliary track index given by \a{trackNum} to \a{pitch}.
 * The default pitch value is 1.0. Valid values for \a{trackNum} are 0 and 1.
 */
/*!
 * \qmlmethod void AudioSystem::setAuxTrackPos(int trackNum, float x, float y, float z)
 * Sets the world location of the auxiliary track index given by \a{trackNum} to the
 * 3D position vector given by <\a{x}, \a{y}, \a{z}>. Valid values for \a{trackNum} are 0 and 1.
 *
 * \e{NOTE: If the track has a rolloff factor of 0, then this position vector is ignored. To
 * make the track positional, a nonzero rolloff factor must also be set!}
 * \sa setAuxTrackRolloffFactor()
 */
/*!
 * \qmlmethod void AudioSystem::setAuxTrackRolloffFactor(int trackNum, float factor)
 * Sets the rolloff factor of the auxiliary track index given by \a{trackNum} to \a{factor}.
 * The Rolloff factor effects how much of the volume is attenuated as you move away from
 * an object. The default value for OGG tracks is 0.0, meaning no volume is lost as a function
 * of distance, meaning the audio is not directional. The normal value for a directional
 * audio source is 1.0. Valid values for \a{trackNum} are 0 and 1.
 *
 * \e{NOTE: If the rolloff factor is set to a nonzero value, the position of the track
 * must also be set in order to achieve correct positional audio playback!}
 * \sa setAuxTrackPos()
 */
/*!
 * \qmlmethod float AudioSystem::getAuxTrackTime(int trackNum)
 * Returns the current playback position of the OGG stream for the auxiliary track index
 * given by \a{trackNum} in seconds. Valid values for \a{trackNum} are 0 and 1.
 */

OOLUA_EXPORT_FUNCTIONS(AudioSystem,
                       playSfxChannel,
                       loadBgmTrack,
                       setBgmTrack,
                       fadeOutBgmTrack,
                       fadeInBgmTrack,
                       morphIntoBgmTrack,
                       setBgmTrackLooping,
                       setBgmTrackGain,
                       fadeIntoBgmTrack,
                       morphIntoNewBgmTrack,
                       fadeIntoNewBgmTrack,
                       loadAuxTrack,
                       fadeOutAuxTrack,
                       fadeInAuxTrack,
                       morphIntoNewAuxTrack,
                       fadeIntoNewAuxTrack,
                       setAuxTrackLooping,
                       setAuxTrackGain)


OOLUA_EXPORT_FUNCTIONS_CONST(AudioSystem,
                             stopSfxChannel,
                             setSfxChannelGain,
                             setSfxChannelPitch,
                             setSfxChannelLooping,
                             setSfxChannelRolloffFactor,
                             setSfxChannelPos,
                             playBgmTrack,
                             stopBgmTrack,
                             pauseBgmTrack,
                             isBgmTrackPlaying,
                             getBgmTrack,
                             getBgmTrackPath,
                             setBgmTrackPitch,
                             setBgmTrackPos,
                             setBgmTrackRolloffFactor,
                             playAuxTrack,
                             stopAuxTrack,
                             pauseAuxTrack,
                             isAuxTrackPlaying,
                             getAuxTrackPath,
                             setAuxTrackPitch,
                             setAuxTrackPos,
                             setAuxTrackRolloffFactor,
                             getBgmTrackTime,
                             getAuxTrackTime)
