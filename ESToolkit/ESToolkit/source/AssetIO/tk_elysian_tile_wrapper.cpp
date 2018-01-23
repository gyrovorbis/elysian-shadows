#include <AssetIO/tk_elysian_tile_wrapper.h>
#include <AssetIO/tk_level.h>
#include <AssetIO/tk_area.h>

QIcon **ElysianTileWrapper::_icons = nullptr;

void ElysianTileWrapper::allocIcons(void) {
    if(_icons) return;

    //ALLOCATE THE BASTARDS
    _icons = new QIcon*[elysian::Tile::COLLIDER_MAX];
    for(unsigned i = 0; i < elysian::Tile::COLLIDER_MAX; ++i)
        _icons[i] = new QIcon(_colliderIconNameFromEnum[i]);
}

//Don't forget this table has to be manually updated every time elysian::Tile::COLLIDER_TYPE changes, fuckmite!!!
const char ElysianTileWrapper::_colliderStringFromEnum[elysian::Tile::COLLIDER_MAX][100] =
{
    "COLLIDER_NONE", "COLLIDER_FULL",
    "COLLIDER_HALF_E", "COLLIDER_HALF_N", "COLLIDER_HALF_W", "COLLIDER_HALF_S",
    "COLLIDER_QUAD_SE", "COLLIDER_QUAD_NE", "COLLIDER_QUAD_NW", "COLLIDER_QUAD_SW",
    "COLLIDER_L_SE", "COLLIDER_L_NE", "COLLIDER_L_NW", "COLLIDER_L_SW",
    "COLLIDER_QUADS_SE_NW", "COLLIDER_QUADS_SW_NE",
    "COLLIDER_45DEG_SE", "COLLIDER_45DEG_NE", "COLLIDER_45DEG_NW", "COLLIDER_45DEG_SW"
};

const char ElysianTileWrapper::_colliderIconNameFromEnum[elysian::Tile::COLLIDER_MAX][100] =
{
    ":/colliders/images/colliderNone.png", ":/colliders/images/colliderFull.png",
    ":/colliders/images/colliderHalfEast.png", ":/colliders/images/colliderHalfNorth.png", ":/colliders/images/colliderHalfWest.png", ":/colliders/images/colliderHalfSouth.png",
    ":/colliders/images/colliderQuadSE.png", ":/colliders/images/colliderQuadNE.png", ":/colliders/images/colliderQuadNW.png", ":/colliders/images/colliderQuadSW.png",
    ":/colliders/images/colliderLSouthEast.png", ":/colliders/images/colliderLNorthEast.png", ":/colliders/images/colliderLNorthWest.png", ":/colliders/images/colliderLSouthWest.png",
    ":/colliders/images/colliderDiagSENW.png", ":/colliders/images/colliderDiagSWNE.png",
    ":/colliders/images/collider45SE.png", ":/colliders/images/collider45NE.png", ":/colliders/images/collider45NW.png", ":/colliders/images/collider45SW.png"
};


//===== INSTANCE MEMBERS =====

void ElysianTileWrapper::setColliderString(const QString string) {
    _elysianTile->colliderType = getColliderEnumFromString(string);

    //Update scenes in case they are rendering solidity, and that solid changed!
    if(TileEntry::renderColliders) {
        Level::updateScenes();
        Area::updateScene();
    }
}

bool ElysianTileWrapper::getNormalMapEnabled(void) const { return _level->getNormalMapEnabled(_index); }
bool ElysianTileWrapper::getSpecularMapEnabled(void) const { return _level->getSpecularMapEnabled(_index); }
void ElysianTileWrapper::setNormalMapEnabled(const bool val) { _level->setNormalMapEnabled(_index, val); }
void ElysianTileWrapper::setSpecularMapEnabled(const bool val) { _level->setSpecularMapEnabled(_index, val); }
