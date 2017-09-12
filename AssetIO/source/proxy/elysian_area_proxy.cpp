#include <proxy/elysian_area_proxy.hpp>
#include <proxy/elysian_entity_proxy.hpp>


/*!
    \qmltype TileEntry
    \since 1.6.0
    \inmodule Engine
    \brief Represents the tile attributes at a single map location.

    A TileEntry represents a tile index and various other attributes stored at ever map location.
    The map is a 3D array of TileEntry objects stored within an Area. For performance reasons,
    a TileEntry is actually a packed 32-bit integer whose attributes are represented by bitfields.
    For the sake of simplicity and your sanity, a TileEntry is treated like a traditional object in Lua.

    \sa {Area}, {Tile Layers}, {Tile}, {Areas}
*/

/*!
 * \qmlmethod int TileEntry::getIndex()
 *
 * Returns the tile index of the TileEntry. This index represents its tile ID and is used to determine the tile's
 * position within the tilesheet and its attributes within a Level's tileset array.
 * \sa Tile, setIndex()
 */
/*!
 * \qmlmethod void TileEntry::setIndex(int index)
 *
 * Sets the tile index of the TileEntry to \a{index}.
 * \sa Tile, getIndex()
 */
/*!
 * \qmlmethod int TileEntry::getElev()
 *
 * Returns the elevation of the TileEntry.
 * \sa setElev()
 */
/*!
 * \qmlmethod void TileEntry::setElev(int elev)
 *
 * Sets the elevation of the TileEntry to \a{elev}. Elevation is essentially the z coordinate of the tile and is used
 * to give the tile height in 3D space. Every unit of elevation is equal to 32 pixels, or one tile size. Valid values
 * for \a{elev} range from 0-31.
 * \sa getElev()
 */
/*!
 * \qmlmethod RECT_PLANE_TYPE TileEntry::getPlane()
 *
 * Returns the plane type of the TileEntry. Plane type can be thought of as the face of a cube the tile is applied to
 * in 3D space.
 * \sa getPlane(), gyro::RECT_PLANE_TYPE
 */
/*!
 * \qmlmethod void TileEntry::setPlane(RECT_PLANE_TYPE plane)
 *
 * Sets the face of the 3D cube the TileEntry will be applied to to \a{plane}.
 * \sa setPlane(), gyro::RECT_PLANE_TYPE
 */
/*!
 * \qmlmethod TEX_ORIENT_TYPE TileEntry::getOrient()
 *
 * Retuns the orientation of the TileEntry (flipped horizontal/vertical, rotated 90/180/270 degrees, etc).
 * \sa setOrient(), gyro::TEX_ORIENT_TYPE
 */
/*!
 * \qmlmethod void TileEntry::setOrient(TEX_ORIENT_TYPE type)
 *
 * Sets the orientation of the TileEntry to the flip/rotation given by \a{type}.
 * \sa getOrient(), gyro::TEX_ORIENT_TYPE
 */
/*!
 * \qmlmethod bool TileEntry::getNormal()
 *
 * Convenience function around getPlane() which returns true for any plane type that is not flat.
 * \sa setNormal(), getPlane(), gyro::RECT_PLANE_TYPE
 */
/*!
 * \qmlmethod bool TileEntry::setNormal(bool val)
 *
 * Convenience function around setPlane() which sets the plane type to gyro.RECT_PLANE_FRONT when \a{val} is true
 * and gyro.RECT_PLANE_BOTTOM when \a{val} is false.
 * \sa getNormal(), setPlane(), gyro::RECT_PLANE_TYPE
 */
/*!
 * \qmlmethod int TileEntry::getValue()
 *
 * Returns the raw 32-bit integer representation of the TileEntry.
 * \sa setValue()
 */
/*!
 * \qmlmethod void TileEntry::setValue(int val)
 *
 * Sets the 32-bit integer value of TilEntry to \a{val}, which means it must be the correct bitfield format.
 * Do not use this function unless you really know what the fuck you're doing.
 * \sa getValue()
 */

OOLUA_EXPORT_FUNCTIONS(TileEntry,
                                    setIndex,
                                    setNormal,
                                    setPlane,
                                   setValue,
                                   setOrient,
                                   setElev)
OOLUA_EXPORT_FUNCTIONS_CONST(TileEntry,
                                getIndex,
                                getNormal,
                                getPlane,
                                 getValue,
                                 getOrient,
                                 getElev)

/*!
    \qmltype Area
    \since 1.6.0
    \inmodule Engine
    \brief Class representing an Area and encapsulating a map and its entities.

    An Area represents a sigle map within ESTk and is located within a Level. The Area gets its tile attributes
    and tile sheets from its parent level, but contains its own map and all entities inhabiting the map.

    \section2 Retrieving the Current Area
    The current Area can be retrieved through the Engine static class:
    \code
    curArea = Engine.getCurArea()
    \endcode
    NOTE: \e {Do not EVER cache an Area object between frames, since the player can warp to another area, and it can change.
    Always fetch it from the Engine at the beginning of the frame.}

    \section2 Representation
    \list
    \li \b {Map}: The map is represented as a 3D array of TileEntry objects. The first dimension is the layer. The engine supports 3
    separate tile layers. The other two represent the <x, y> location of the tile.
    \li \b {Entities}: Entities are stored within two separate arrays. The first is activeEntities, which stores entities
    that are active within the current scene. The second array is inactiveEntities, which can store entities that will
    later be spawned or added to the scene dynamically through a Lua script.
    \endlist

    \sa {Areas}, {Tile Layers}, {TileEntry}, {Engine}
*/

/*!
 * \qmlproperty string Area::dirName
 * The name of the directory containing the Area.
 * \sa getDirName(), setDirName()
 */
/*!
 * \qmlproperty string Area::nameString
 * Display name of the Area that will be used in-game.
 * \sa getNameString(), setNameString()
 */
/*!
 * \qmlproperty string Area::path
 * Read-only asset-relative file path of Area directory.
 * \sa getPath()
 */
/*!
 * \qmlproperty table Area::size
 * Virtual table containing the width and height dimensions of the map in tiles.
 * Members:
 * \list
 * \li width: int
 * \li height: int
 * \endlist
 * \sa getMapWidth(), setMapWidth(), getMapHeight(), setMapHeight()
 */
/*!
 * \qmlproperty int Area::size::width
 * Width of the Area's map in tiles.
 * \sa getMapWidth(), getMapHeight()
 */
/*!
 * \qmlproperty array Area::activeEntities
 * An array containing all active entities within the Area.
 * \sa getActiveEntityByIndex(), setActiveEntityByIndex(), getActiveEntityCount()
 */
/*!
 * \qmlproperty array Area::inactiveEntities
 * An array containing all inactive entities within the Area.
 * \sa getInactiveEntityByIndex(), setInactiveEntityByIndex(), getInactiveEntityCount()
 */
/*!
 * \qmlproperty table Area::music
 * Virtual table containing arrays for both background and auxiliary music OGG tracks.
 * Members:
 * \list
 * \li bgm: array
 *  \list
 *  \li 1: string
 *  \li 2: string
 *  \li 3: string \endlist
 * \li aux: array
 *  \list
 *  \li 1: string
 *  \li 2: string
 *  \li 3: string
 *  \endlist
 * \endlist
 * \sa getBgmOggByTrack(), setBgmOggByTrack(), getBgmOggTrackCount(), getAuxOggByTrack(), setAuxOggByTrack(), getAuxOggTrackCount()
 */
/*!
 * \qmlproperty bool Area::useSun
 * When set to true, tells the \l{DaySystem} to automatically add the sun \l{Entity} to the Area to simulate the
 * time of day with the sun rising and setting for outdoor Areas. If set to false, no sun is added. The
 * default value is true.
 * \sa getUseSun(), setUseSun(), DaySystem
 */
/*!
 * \qmlproperty Vector4 Area::ambientColor
 * 4D RGBA vector representing the intensity of ambient light in the Area. Ambient light is light that is always present everywhere
 * in equal amounts. If this is set to <0, 0, 0, 0>, the entire Area will be pitch black. For outdoor Areas, this should be
 * set to a low value to simulate night, then the \l{useSun} property should be set to true, so the sun entity emits sunlight
 * to light the rest of the Area during the day. Without the sun, this can be used to simulate ambient color within caves or
 * indoor locations.
 * \sa getAmbientColor(), setAmbientColor()
 */

/*!
 * \qmlmethod string Area::getDirName()
 *
 * Retrieves the name of the directory containing the Area.
 */
/*!
 * \qmlmethod string Area::getPath()
 *
 * Retrieves the asset-relative path of the Area directory.
 */
/*!
 * \qmlmethod void Area::setDirName(string name)
 *
 * Changes the name of the Area directory to \a name.
 */
/*!
 * \qmlmethod void Area::setNameString(string name)
 *
 * Changes the display name of the Area to \a name.
 */
/*!
 * \qmlmethod string Area::getNameString()
 *
 * Returns the display name of the Area.
 */
/*!
 * \qmlmethod TileEntry Area::getTileEntry(int layer, int x, int y)
 *
 * Returns the TileEntry object for the map location given by <\a{x}, \a{y}> on the tile layer given by \a layer.
 * \a{x} and \a{y} must be within the range of the map, and \a{layer} must be between 0 and 2.
 */
/*!
 * \qmlmethod void Area::setTileEntry(int layer, int x, int y, TileEntry entry)
 *
 * Sets the TileEntry at position <\a{x}, \a{y}> on \a{layer} to \a{entry}. The position and layer must be within
 * the map boundaries.
 */
/*!
 * \qmlmethod void Area::addEntity(Entity* entity)
 *
 * Adds \a{entity} to the Area. Ownership of \a{entity} is transferred from Lua to the engine.
 */
/*!
 * \qmlmethod void Area::removeEntity(Entity* entity)
 *
 * Removes \a{entity} from the Area. Ownership is transferred from the engine to Lua.
 */
/*!
 * \qmlmethod Entity* Area::findEntity(string name)
 *
 * Searches the Area for an entity with the given \a{name}. Returns a reference to that entity if found or nil if not.
 * This function does a linear search, so it's relatively expensive. Only use this initially to find an entity, then
 * cache its reference rathe than looking it up every frame.
 */
/*!
 * \qmlmethod void Area::setMapHeight(int height)
 *
 * Sets the height of the map to the tile height given by \a{height}, either enlarging to shrinking the map. If enlarged, the TileEntry values
 * at each map location are nondeterministic and need to be filled in. If shrank, all TileEntry values that are no longer
 * within the map boundaries will be lost.
 */
/*!
 * \qmlmethod void Area::setMapWidth(int width)
 *
 * Sets the width of the map to the tile width given by \a{width}, either enlarging to shrinking the map. If enlarged, the TileEntry values
 * at each map location are nondeterministic and need to be filled in. If shrank, all TileEntry values that are no longer
 * within the map boundaries will be lost.
 */
/*!
 * \qmlmethod int Area::getMapHeight()
 *
 * Returns the height of the map in tiles.
 */
/*!
 * \qmlmethod int Area::getMapWidth()
 *
 * Returns the width of the map in tiles.
 */
/*!
 * \qmlmethod void Area::setActiveEntityByIndex(int index, Entity* entity)
 * Sets the active Entity at \a{index} to \a{entity}. If \a{index} is 1 greater than the number of active entities,
 * \a{entity} is added as an active entity. Ownership of \a{entity} is transferred from Lua to the engine.
 */
/*!
 * \qmlmethod void Area::setInactiveEntityByIndex(int index, Entity* entity)
 * Sets the inactive Entity at \a{index} to \a{entity}. If \a{index} is 1 greater than the number of inactive entities,
 * \a{entity} is added as an inactive entity. Ownership of \a{entity} is transferred from Lua to the engine.
 */
/*!
 * \qmlmethod Entity* Area::getActiveEntityByIndex(int index)
 * Returns the active entity at \a{index} or nil if there isn't one.
 */
/*!
 * \qmlmethod Entity* Area::getInactiveEntityByIndex(int index)
 * Returns the inactive entity at \a{index} or nil if there isn't one.
 */
/*!
 * \qmlmethod int Area::getActiveEntityCount()
 * Returns the number of active entities within the Area.
 */
/*!
 * \qmlmethod int Area::getInactiveEntityCount()
 * Returns the number of inactive entities within the Area.
 */
/*!
 * \qmlmethod bool Area::getUseSun()
 * Returns true if the Area uses the default sun directional light and false if it doesn't.
 * \sa {DaySystem}
 */
/*!
 * \qmlmethod void Area::setUseSun(bool val)
 * If \a{val} is true, tells the engine to use the default sun directional light within the Area.
 * If \a{val} is false, tells the engine not to use the sun.
 * This should be set to true for all outdoor scenes.
 * \sa {DaySystem}
 */
/*!
 * \qmlmethod Vector4 Area::getAmbientColor()
 * Retuns a Vector4 representing an ambient color in the format of <R, G, B, A>. The ambient color is the
 * base lighting color effecting the entire area.
 */
/*!
 * \qmlmethod void Area::setAmbientColor(Vector4 color)
 * Sets the ambient color of the Area to the <R, G, B, A> vector provided by \a{color}.
 */
/*!
 * \qmlmethod string Area::getBgmOggByTrack(int track)
 * Returns the path of the background OGG music file for the area to be played on \a{track}.
 * Valid tracks are 1-3.
 * \sa AudioSystem
 */
/*!
 * \qmlmethod void Area::setBgmOggByTrack(int track, string oggPath)
 * Loads the OGG music file from the path given by \a{oggPath} and sets it as the background music
 * for the Area on \a{track.} Valid tracks are 1-3.
 * \sa AudioSystem
 */
/*!
 * \qmlmethod int Area::getBgmOggTrackCount()
 * Returns the number of background OGG tracks per Area supported by the engine. This should always return 3.
 * \sa AudioSystem
 */
/*!
 * \qmlmethod string Area::getAuxOggByTrack(int track)
 * Returns the path of the auxiliary OGG sound file for the area to be played on \a{track}.
 * Valid tracks are 1-3.
 * \sa AudioSystem
 */
/*!
 * \qmlmethod void Area::setAuxOggByTrack(int track, string oggPath)
 * Loads the OGG music file from the path given by \a{oggPath} and sets it as the auxiliary sound
 * for the Area on \a{track.} Valid tracks are 1-3.
 * \sa AudioSystem
 */
/*!
 * \qmlmethod int Area::getAuxOggTrackCount()
 * Returns the number of auxiliary sound OGG tracks per Area supported by the engine. This should always return 3.
 * \sa AudioSystem
 */

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
                                    getNameString,
                                    getUseSun,
                                    getBgmOggByTrack,
                                    getAuxOggByTrack,
                                    getBgmOggTrackCount,
                                    getAuxOggTrackCount)
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
                                setMapHeight,
                                setUseSun,
                                getAmbientColor,
                                setAmbientColor,
                                setBgmOggByTrack,
                                setAuxOggByTrack)
