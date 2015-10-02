#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>
#include <proxy/elysian_audio_system_proxy.hpp>


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
