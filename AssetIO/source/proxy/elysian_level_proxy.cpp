#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/proxy/elysian_level_proxy.hpp>

/*!
    \qmltype Tile
    \since 1.6.0
    \inmodule Engine
    \brief Represents a group of attributes for a single tile type

    Tile is a table of attributes that is shared by every TileEntry on the map with the same index.
    A table of tile attributes for the current tilesheet is stored within Level and is shared for every
    Area in the same Level.

    \sa {Area}, {TileEntry}, {Level}
*/


/*!
\qmlproperty enumeration Tile::COLLIDER_TYPE

Represents the different types of colliders that a tile can have.

The different Tile collder types are:
\list
\li Tile.COLLIDER_NONE          - fuly walkable
\li Tile.COLLIDER_FULL          - fully solid
\li Tile.COLLIDER_HALF_E        - half solid
\li Tile.COLLIDER_HALF_N
\li Tile.COLLIDER_HALF_W
\li Tile.COLLIDER_HALF_S
\li Tile.COLLIDER_QUAD_SE       - quarter solid
\li Tile.COLLIDER_QUAD_NE
\li Tile.COLLIDER_QUAD_NW
\li Tile.COLLIDER_QUAD_SW
\li Tile.COLLIDER_L_SE          - L-shaped solid (3/4)
\li Tile.COLLIDER_L_NE
\li Tile.COLLIDER_L_NW
\li Tile.COLLIDER_L_SW
\li Tile.COLLIDER_QUADS_SE_NW   - 2 diagonal quarters solid
\li Tile.COLLIDER_QUADS_SW_NE
\li Tile.COLLIDER_45DEG_SE      - single corner, right-triangle solid
\li Tile.COLLIDER_45DEG_NE
\li Tile.COLLIDER_45DEG_NW
\li Tile.COLLIDER_45DEG_SW
\endlist
*/
/*!
 * \qmlmethod void Tile::setDamping(float factor)
 *
 * Sets the tile's damping factor to \a{factor}. The damping factor is how much of a rigid body's velocity
 * is conserved every frame the body is on the tile. A damping factor of 1.0 means no velocity is lost. A
 * damping factor of 0 means the object literally cannot move while on the tile. This can be used to simulate
 * sticky tiles or to slow the player ground in sand or tall grasses.
 */
/*!
 * \qmlmethod COLLIDER_TYPE Tile::getColliderType()
 *
 * Returns the type of collider associated with the Tile.
 * \sa setColliderType(), COLLIDER_TYPE
 */
/*!
 * \qmlmethod void Tile::setColliderType(COLLIDER_TYPE type)
 *
 * Sets the tile's collider type to \a{type}.
 * \sa getColliderType(), COLLIDER_TYPE
 */

OOLUA_EXPORT_FUNCTIONS(Tile,
                        setColliderType,
                        setDamping,
                        setWalkSfx,
                        setHitSfx)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Tile,
                               getColliderType)


/*!
    \qmltype Level
    \since 1.6.0
    \inmodule Engine
    \brief Contains a group of Areas, tile attributes, and tile sheets.

    A Level contains a group of Areas, all sharing the same tile sheet texture and tile attribute array.
    Levels contain a 512-entry Tile array containing attributes for every tile in the sheet.

    \section2 Retrieving the Current Level
    A reference to the current Level can be retrieved through the Engine.
    \code
    level = Engine.getCurLevel()
    \endcode
    NOTE: \e {Do not EVER cache a Level object between frames, since the player can warp to another level, and it can change.
    Always fetch it from the Engine at the beginning of the frame.}

    \sa {Levels}, {Engine}, {Area}, {Tile}
*/
/*!
 * \qmlmethod Tile* Level::getTile(int index)
 *
 * Returns the Tile object from the attribute array at the given \a{index}. Valid values for \a{index} range from 0-511.
 * \sa Tile
 */
/*!
 * \qmlmethod string Level::getDirName()
 *
 * Returns the name of the directory containing the Level.
 */
/*!
 * \qmlmethod string Level::getPath()
 *
 * Returns the asset-relative path to the directory containing the level.
 */

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(Level,
                                    getTile)
EXPORT_OOLUA_FUNCTIONS_2_CONST(Level,
                                //getTileAmount,
                                getDirName,
                                getPath)

