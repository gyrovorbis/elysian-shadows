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

OOLUA_EXPORT_FUNCTIONS(RenderableObject,
                                    getTransform)
OOLUA_EXPORT_FUNCTIONS_CONST(RenderableObject)


OOLUA_EXPORT_FUNCTIONS(Entity,
                        move,
                        setName,
                        setElev,
                        addRenderable,
                        addAnimation,
                        addCollidable,
                        addCharacter,
                        addWarp,
                        addAudioSource,
                        addLightSource,
                        addExaminable,
                        addRigidBody,
                        addCombat,
                        addParticleEmitter,
                        addSkeleton,
                        addBehavior,
                        addLuaBehavior,
                        removeRenderable,
                        removeAnimation,
                        removeCollidable,
                        removeCharacter,
                        removeWarp,
                        removeAudioSource,
                        removeLightSource,
                        removeExaminable,
                        removeRigidBody,
                        removeCombat,
                        removeParticleEmitter,
                        removeSkeleton,
                        removeBehavior,
                        removeLuaBehavior,
                        setSelfTable,
                        getSelfTable,
                        setShadowDepth,
                        setShadowVolume,
                       setPlaneType,
                       setTransform,
                       setRenderable,
                       setAnimation,
                       setCollidable,
                       setRigidBody,
                       setCharacter,
                       setWarp,
                       setAudioSource,
                       setLightSource,
                       setParticleEmitter,
                       setCombat,
                       setExaminable,
                       setSkeleton,
                       setPersistent,
                       setDispName,
                       setLuaBehaviorByIndex)


OOLUA_EXPORT_FUNCTIONS_CONST(Entity,
                                getName,
                                getElev,
                                getRenderable,
                                getAnimation,
                                getCollidable,
                                getCharacter,
                                getWarp,
                                getAudioSource,
                                getLightSource,
                                getExaminable,
                                getRigidBody,
                                getCombat,
                                getParticleEmitter,
                                getSkeleton,
                                getBehavior,
                                getLuaBehavior,
                                isShadowVolume,
                                getPlaneType,
                                getShadowDepth,
                                isPersistent,
                                getDispName,
                                getLuaBehaviorByIndex,
                                getLuaBehaviorCount)
