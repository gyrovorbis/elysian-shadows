#include <proxy/elysian_entity_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(Animation,
                       setName,
                       setTrack)
OOLUA_EXPORT_FUNCTIONS_CONST(Animation,
                             getName,
                             getTrack,
                             getTrackCount)

OOLUA_EXPORT_FUNCTIONS(AnimationTrack,
                       setName,
                       setLooping,
                       setTimeScale)
OOLUA_EXPORT_FUNCTIONS_CONST(AnimationTrack,
                             getName,
                             getLooping,
                             getTimeScale)


OOLUA_EXPORT_FUNCTIONS(SkeletonAttrib,
                        loadAssets,
                        setJsonPath,
                        setAtlasPath,
                        setAnimation)
OOLUA_EXPORT_FUNCTIONS_CONST(SkeletonAttrib,
                             getJsonPath,
                             getAtlasPath,
                             getAnimation,
                             getAnimationCount)

OOLUA_EXPORT_FUNCTIONS_CONST(Skeleton,
                             getAttrib,
                             getAnimTimeScale,
                             getEntityAttachedToSlot,
                             getSkin,
                             getAnimName)
OOLUA_EXPORT_FUNCTIONS(Skeleton,
                       setAttrib,
                       setAnimName,
                       playAnimNameBlocking,
                       setAnimTimeScale,
                       attachEntityToSlot,
                       removeEntityFromSlot,
                       setAnimByName,
                       setSkin)
