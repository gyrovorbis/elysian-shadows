#ifndef ELYSIAN_AUDIO_SYSTEM_PROXY_HPP
#define ELYSIAN_AUDIO_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <system/elysian_audio_system.hpp>

namespace OOLUA {
    typedef elysian::AudioSystem    AudioSystem;
}


OOLUA_PROXY(AudioSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, playSfxChannelLua, int, const GYWav&, LuaVariant)
    OOLUA_MFUNC_CONST(stopSfxChannel)
    OOLUA_MFUNC_CONST(setSfxChannelGain)
    OOLUA_MFUNC_CONST(setSfxChannelPitch)
    OOLUA_MFUNC_CONST(setSfxChannelLooping)
    OOLUA_MFUNC_CONST(setSfxChannelRolloffFactor)
    OOLUA_MFUNC_CONST(setSfxChannelPos)
    OOLUA_MFUNC(loadBgmTrack)
    OOLUA_MFUNC_CONST(playBgmTrack)
    OOLUA_MFUNC_CONST(stopBgmTrack)
    OOLUA_MFUNC_CONST(pauseBgmTrack)
    OOLUA_MFUNC_CONST(isBgmTrackPlaying)
    OOLUA_MFUNC_CONST(getBgmTrack)
    OOLUA_MFUNC(setBgmTrack)
    OOLUA_MFUNC_CONST(getBgmTrackPath)
    OOLUA_MFUNC(fadeOutBgmTrack)
    OOLUA_MFUNC(fadeInBgmTrack)
    OOLUA_MFUNC(morphIntoBgmTrack)
    OOLUA_MFUNC(setBgmTrackLooping)
    OOLUA_MFUNC(setBgmTrackGain)
    OOLUA_MFUNC_CONST(setBgmTrackPitch)
    OOLUA_MFUNC_CONST(setBgmTrackPos)
    OOLUA_MFUNC_CONST(setBgmTrackRolloffFactor)
    OOLUA_MFUNC(fadeIntoBgmTrack)
    OOLUA_MFUNC(morphIntoNewBgmTrack)
    OOLUA_MFUNC(fadeIntoNewBgmTrack)
    OOLUA_MFUNC_CONST(getBgmTrackTime)
    OOLUA_MFUNC(loadAuxTrack)
    OOLUA_MFUNC_CONST(playAuxTrack)
    OOLUA_MFUNC_CONST(stopAuxTrack)
    OOLUA_MFUNC_CONST(pauseAuxTrack)
    OOLUA_MFUNC_CONST(isAuxTrackPlaying)
    OOLUA_MFUNC_CONST(getAuxTrackPath)
    OOLUA_MFUNC(fadeOutAuxTrack)
    OOLUA_MFUNC(fadeInAuxTrack)
    OOLUA_MFUNC(morphIntoNewAuxTrack)
    OOLUA_MFUNC(fadeIntoNewAuxTrack)
    OOLUA_MFUNC(setAuxTrackLooping)
    OOLUA_MFUNC(setAuxTrackGain)
    OOLUA_MFUNC_CONST(setAuxTrackPitch)
    OOLUA_MFUNC_CONST(setAuxTrackPos)
    OOLUA_MFUNC_CONST(setAuxTrackRolloffFactor)
    OOLUA_MFUNC_CONST(getAuxTrackTime)
    OOLUA_MEM_FUNC_RENAME(playSfxChannel, void, playSfxChannelLua, int, const GYWav&, LuaVariant)
OOLUA_PROXY_END



#endif // ELYSIAN_AUDIO_SYSTEM_PROXY_HPP

