#ifndef MAP_ENTRY_H
#define MAP_ENTRY_H

#include "elysian_asset_typedefs.hpp"
#include "elysian_area.hpp"
#include "elysian_tile.hpp"
#include "assetManager.h"

class QPainter;

/*
CAN'T ADD DATA, OR EVERYTHING WILL BE FUCKED.
JUST ADD METHODS.
Everything is instantiated as an elysian::TileEntry,
then we're unsafely static_cast-ing to a derived class
type that it technically isn't, but it's fine as long as
the memory layout is the same and you ONLY ADD
NONVIRTUAL METHODS.

This way we can add rendering functionality to existing data
structures... --FG
*/
struct TileEntry: public elysian::TileEntry, public AssetManager {
	static bool renderIndeces;
	static bool renderFlipRot;
	static bool renderElev;
	static bool renderColliders;
	static bool renderZTran;
    static bool renderPlane;
    inline TileEntry(void) = default;
	inline TileEntry(const elysian::TileEntry e): elysian::TileEntry(e.value) {}
	inline TileEntry(const uint32_t val): elysian::TileEntry(val) {}
	void renderDecoration(QPainter *const painter, const unsigned x, const unsigned y) const;
	static void renderCollider(QPainter *const painter, const elysian::Tile::COLLIDER_TYPE collider);
};

#endif // MAPENTRY_H
