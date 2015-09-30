#ifndef ELYSIAN_SKELETON_PROXY_HPP
#define ELYSIAN_SKELETON_PROXY_HPP

#include <crtp/elysian_skeleton.hpp>

namespace OOLUA
{
    typedef elysian::SkeletonAttrib     SkeletonAttrib;
    typedef elysian::Skeleton           Skeleton;

    //template<> class Proxy_class<SkeletonAttrib>;
    //template<> class Proxy_class<Skeleton>;
}


OOLUA_PROXY(SkeletonAttrib)
    OOLUA_MEM_FUNC(void, loadAssets, const char*, const char*)
    OOLUA_MEM_FUNC_CONST(const char*, getAtlasPath)
    OOLUA_MEM_FUNC_CONST(const char*, getJsonPath)
    OOLUA_MEM_FUNC(void, setAtlasPath, const char*)
    OOLUA_MEM_FUNC(void, setJsonPath, const char*)
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
    OOLUA_MEM_FUNC_CONST(void, setAnimByName, const int, const char*, const bool)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getEntityAttachedToSlot, const char*)
    OOLUA_MEM_FUNC(void, attachEntityToSlot, const char*, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC(void, removeEntityFromSlot, const char*)
OOLUA_PROXY_END


#endif // ELYSIAN_SKELETON_PROXY_HPP

