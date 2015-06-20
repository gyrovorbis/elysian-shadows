#include <proxy/elysian_area_proxy.hpp>
#include <elysian_lua_wrappers.hpp>

OOLUA_EXPORT_FUNCTIONS(TileEntry,
                                    setIndex,
                                    setZTran,
                                    setNormal,
                                    setPlane,
                                   setValue,
                                   setOrient,
                                   setElev)
OOLUA_EXPORT_FUNCTIONS_CONST(TileEntry,
                                getIndex,
                                getZTran,
                                getNormal,
                                getPlane,
                                 getValue,
                                 getOrient,
                                 getElev)



OOLUA_EXPORT_FUNCTIONS_CONST(Area,
                                    getDirName,
                                    getPath,
                                    getTileEntry,
                                    getMapWidth,
                                    getMapHeight,
                                    getActiveEntityByIndex,
                                    getActiveEntityCount,
                                    getInactiveEntityByIndex,
                                    getInactiveEntityCount,
                                    getNameString)
OOLUA_EXPORT_FUNCTIONS(Area,
                                setTileEntry,
                                addEntity,
                                removeEntity,
                                findEntity,
                                setActiveEntityByIndex,
                                setInactiveEntityByIndex,
                                setDirName,
                                setNameString,
                                setMapWidth,
                                setMapHeight)
