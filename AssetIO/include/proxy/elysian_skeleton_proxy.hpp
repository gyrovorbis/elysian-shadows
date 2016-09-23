#ifndef ELYSIAN_SKELETON_PROXY_HPP
#define ELYSIAN_SKELETON_PROXY_HPP

#include <crtp/elysian_skeleton.hpp>

namespace OOLUA
{
    typedef elysian::AnimationTrack     AnimationTrack;
    typedef elysian::Animation          Animation;
    typedef elysian::SkeletonAttrib     SkeletonAttrib;
    typedef elysian::Skeleton           Skeleton;

    //template<> class Proxy_class<SkeletonAttrib>;
    //template<> class Proxy_class<Skeleton>;
}

OOLUA_PROXY(AnimationTrack)
    OOLUA_MEM_FUNC_CONST(const char*, getName)
    OOLUA_MEM_FUNC(void, setName, const char*)
    OOLUA_MEM_FUNC_CONST(float, getTimeScale)
    OOLUA_MEM_FUNC(void, setTimeScale, const float)
    OOLUA_MEM_FUNC_CONST(bool, getLooping)
    OOLUA_MEM_FUNC(void, setLooping, const bool)
OOLUA_PROXY_END

OOLUA_PROXY(Animation)
    OOLUA_MEM_FUNC_CONST(const char*, getName)
    OOLUA_MEM_FUNC(void, setName, const char*)
    OOLUA_MEM_FUNC_CONST(maybe_null<AnimationTrack*>, getTrack, const unsigned)
    OOLUA_MEM_FUNC(void, setTrack, const unsigned, cpp_in_p<AnimationTrack*>)
    OOLUA_MEM_FUNC_CONST(unsigned, getTrackCount)
OOLUA_PROXY_END

OOLUA_PROXY(SkeletonAttrib)
    OOLUA_MEM_FUNC(void, loadAssets, const char*, const char*)
    OOLUA_MEM_FUNC_CONST(const char*, getAtlasPath)
    OOLUA_MEM_FUNC_CONST(const char*, getJsonPath)
    OOLUA_MEM_FUNC(void, setAtlasPath, const char*)
    OOLUA_MEM_FUNC(void, setJsonPath, const char*)
    OOLUA_MEM_FUNC(void, setAnimation, const unsigned, cpp_in_p<Animation*>)
    OOLUA_MEM_FUNC_CONST(maybe_null<Animation*>, getAnimation, const unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getAnimationCount)
OOLUA_PROXY_END

OOLUA_PROXY(Skeleton, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<SkeletonAttrib*const>)
    OOLUA_MEM_FUNC_CONST(maybe_null<SkeletonAttrib*>, getAttrib)
    OOLUA_MEM_FUNC(void, setAnimTimeScale, const float)
    OOLUA_MEM_FUNC_CONST(float, getAnimTimeScale)
    OOLUA_MEM_FUNC(void, setAnimByName, const int, const char*const, const bool)
    OOLUA_MEM_FUNC(void, setAnim, const char*)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getEntityAttachedToSlot, const char*)
    OOLUA_MEM_FUNC(void, attachEntityToSlot, const char*, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC(void, removeEntityFromSlot, const char*)
    OOLUA_MEM_FUNC(void, setSkin, const char*)
    OOLUA_MEM_FUNC_CONST(const char*, getSkin)
OOLUA_PROXY_END


#endif // ELYSIAN_SKELETON_PROXY_HPP

