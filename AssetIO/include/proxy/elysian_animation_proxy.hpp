#ifndef ELYSIAN_ANIMATION_PROXY_HPP
#define ELYSIAN_ANIMATION_PROXY_HPP

#include <proxy/elysian_component_proxy.hpp>
#include <crtp/elysian_animation.hpp>

namespace OOLUA {
    typedef elysian::AnimationSequence          AnimationSequence;
    typedef elysian::AnimationAttrib            AnimationAttrib;
    typedef elysian::ChildAnimation             ChildAnimation;
    typedef elysian::ChildAnimationSequence     ChildAnimationSequence;
    typedef elysian::ChildAnimationAttrib       ChildAnimationAttrib;
    typedef elysian::Animation                  Animation;
    typedef elysian::EventMonitor               EventMonitor;
    typedef elysian::Component                  Component;


    template<> class Proxy_class<Component>;
    template<> class Proxy_class<EventMonitor>;
}


OOLUA_PROXY(AnimationSequence)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addFrame, float, float, float, unsigned, GY_RECT_TEX_ORIENT)
    OOLUA_MEM_FUNC(AnimationSequence*, addSequence, const char*)
    OOLUA_MEM_FUNC(void, setLinearInterpolation, bool)
OOLUA_PROXY_END

OOLUA_PROXY(AnimationAttrib)
    OOLUA_MEM_FUNC(AnimationSequence*, addSequence, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(ChildAnimationSequence)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addFrame, float, float, float, float)
    OOLUA_MEM_FUNC(ChildAnimationSequence*, addSequence, const char*)
    OOLUA_MEM_FUNC(void, setLinearInterpolation, bool)
OOLUA_PROXY_END

OOLUA_PROXY(ChildAnimation)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(ChildAnimationSequence*, addSequence, const char*)
    OOLUA_MEM_FUNC(void, setGlobalOffset, float, float, float)
    OOLUA_MEM_FUNC(void, setInheritSpeedMultiplier, bool)
    OOLUA_MEM_FUNC(void, setInheritScale, bool)
OOLUA_PROXY_END

OOLUA_PROXY(ChildAnimationAttrib)
    OOLUA_MEM_FUNC(ChildAnimation*, addAnimation, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(Animation, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(AnimationAttrib*, getAttrib)
    OOLUA_MEM_FUNC_CONST(ChildAnimationAttrib*, getChildAttrib)
    OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<AnimationAttrib *>)
    OOLUA_MEM_FUNC(void, setChildAttrib, cpp_in_p<ChildAnimationAttrib*>)
    OOLUA_MEM_FUNC(void, playAnimation, const char*, cpp_in_p<EventMonitor*>)
    OOLUA_MEM_FUNC(void, stopAnimation)
    OOLUA_MEM_FUNC(void, addChildEntity, Entity*, const char*)
    OOLUA_MEM_FUNC(void, removeChildEntity, const char*)
    OOLUA_MEM_FUNC_CONST(bool, isLooping)
    OOLUA_MEM_FUNC(void, setLooping, bool)
    OOLUA_MEM_FUNC_CONST(unsigned, getCurrentFrame)
    OOLUA_MEM_FUNC(void, setCurrentFrame, const unsigned)
    OOLUA_MEM_FUNC(void, setChildPivot, float, float)
    OOLUA_MEM_FUNC(void, setSpeedMultiplier, float)
    OOLUA_MEM_FUNC(void, addState, const char*)
    OOLUA_MEM_FUNC(void, removeState, const char*)
    OOLUA_MEM_FUNC(void, replaceState, const char*, const char*)
OOLUA_PROXY_END

#endif // ELYSIAN_ANIMATION_PROXY_HPP

