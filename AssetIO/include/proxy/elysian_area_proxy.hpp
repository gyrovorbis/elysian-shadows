#ifndef ELYSIAN_AREA_PROXY_HPP
#define ELYSIAN_AREA_PROXY_HPP

#include <crtp/elysian_area.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>
#include <utility/elysian_oolua_string_traits.hpp>

namespace OOLUA
{
    typedef elysian::Area       Area;
    typedef elysian::TileEntry  TileEntry;
    typedef elysian::Entity     Entity;

    template<> class Proxy_class<Area>;
    template<> class Proxy_class<TileEntry>;
    template<> class Proxy_class<Entity>;
}

OOLUA_PROXY(TileEntry)
    OOLUA_MEM_FUNC_CONST(uint16_t, getIndex)
    OOLUA_MEM_FUNC(void, setIndex, uint16_t)
    OOLUA_MEM_FUNC(void, setNormal, bool)
    OOLUA_MEM_FUNC_CONST(bool, getNormal)
    OOLUA_MEM_FUNC_CONST(GY_RECT_PLANE_TYPE, getPlane)
    OOLUA_MEM_FUNC(void, setPlane, GY_RECT_PLANE_TYPE)
    OOLUA_MEM_FUNC_CONST(uint32_t, getValue)
    OOLUA_MEM_FUNC(void, setValue, uint32_t)
    OOLUA_MEM_FUNC_CONST(GY_RECT_TEX_ORIENT, getOrient)
    OOLUA_MEM_FUNC(void, setOrient, GY_RECT_TEX_ORIENT)
    OOLUA_MEM_FUNC_CONST(unsigned char, getElev)
    OOLUA_MEM_FUNC(void, setElev, unsigned char)
OOLUA_PROXY_END


OOLUA_PROXY(Area)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(const char*, getDirName)
    OOLUA_MEM_FUNC_CONST(const char*, getPath)
    OOLUA_MEM_FUNC(void, setDirName, const char*)
    OOLUA_MEM_FUNC(void, setNameString, const char*)
#ifndef ELYSIAN_ENGINE
    OOLUA_MEM_FUNC(Vector3&, getStartPosition)
    OOLUA_MEM_FUNC(void, setStartPosition, const Vector3&)
#endif
    OOLUA_MEM_FUNC_CONST(const char*, getNameString)
    OOLUA_MEM_FUNC_CONST(TileEntry, getTileEntry, const unsigned, const unsigned, const unsigned)
    OOLUA_MEM_FUNC(void, setTileEntry, unsigned, unsigned, unsigned, TileEntry)
    OOLUA_MEM_FUNC(void, addEntity, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC(void, removeEntity, Entity*)
    OOLUA_MEM_FUNC(maybe_null<Entity*>, findEntity, const char*)
    OOLUA_MEM_FUNC_CONST(unsigned short, getMapWidth);
    OOLUA_MEM_FUNC_CONST(unsigned short, getMapHeight);
    OOLUA_MEM_FUNC(void, setMapHeight, const unsigned short)
    OOLUA_MEM_FUNC(void, setMapWidth, const unsigned short)
    OOLUA_MEM_FUNC(void, setActiveEntityByIndex, const unsigned, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC_CONST(Entity*, getActiveEntityByIndex, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getActiveEntityCount)
    OOLUA_MEM_FUNC(void, setInactiveEntityByIndex, const unsigned, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC_CONST(Entity*, getInactiveEntityByIndex, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getInactiveEntityCount)
    OOLUA_MEM_FUNC_CONST(bool, getUseSun)
    OOLUA_MEM_FUNC(void, setUseSun, bool)
    OOLUA_MEM_FUNC(Vector4, getAmbientColor)
    OOLUA_MEM_FUNC(void, setAmbientColor, const Vector4&)
    OOLUA_MEM_FUNC_CONST(const char*, getBgmOggByTrack, unsigned)
    OOLUA_MEM_FUNC_CONST(const char*, getAuxOggByTrack, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getBgmOggTrackCount)
    OOLUA_MEM_FUNC_CONST(unsigned, getAuxOggTrackCount)
    OOLUA_MEM_FUNC(void, setBgmOggByTrack, unsigned, const char*)
    OOLUA_MEM_FUNC(void, setAuxOggByTrack, unsigned, const char*)
OOLUA_PROXY_END

#endif // ELYSIAN_AREA_PROXY_HPP

