#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_animation_proxy.hpp>
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

EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(AnimationSequence,
                                   addFrame,
                                   addSequence,
                                   setLinearInterpolation)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AnimationSequence)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(AnimationAttrib,
                                   addSequence)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AnimationAttrib)

OOLUA_EXPORT_FUNCTIONS(ChildAnimationSequence,
                                   addFrame,
                                    addSequence,
                                    setLinearInterpolation)
OOLUA_EXPORT_FUNCTIONS_CONST(ChildAnimationSequence)

OOLUA_EXPORT_FUNCTIONS(ChildAnimation,
                        addSequence,
                       setGlobalOffset,
                       setInheritSpeedMultiplier,
                       setInheritScale)
OOLUA_EXPORT_FUNCTIONS_CONST(ChildAnimation)


OOLUA_EXPORT_FUNCTIONS(ChildAnimationAttrib,
                                   addAnimation)
OOLUA_EXPORT_FUNCTIONS_CONST(ChildAnimationAttrib)

OOLUA_EXPORT_FUNCTIONS(Animation,
                                   setAttrib,
                                   playAnimation,
                                   stopAnimation,
                                   setLooping,
                                   setCurrentFrame,
                                   addChildEntity,
                                   removeChildEntity,
                                   setChildAttrib,
                                   setChildPivot,
                                    setSpeedMultiplier,
                                    addState,
                                    removeState,
                                    replaceState)
OOLUA_EXPORT_FUNCTIONS_CONST(Animation,
                               getAttrib,
                               isLooping,
                               getCurrentFrame,
                               getChildAttrib)
