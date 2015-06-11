#ifndef SHEET_VIEW_TILE_GRAPHICS_ITEM_H
#define SHEET_VIEW_TILE_GRAPHICS_ITEM_H

#include <tk_tile_graphics_item.h>
#include "tk_assets.h"
#include "tk_asset_manager.h"

class SheetViewTileGraphicsItem: public TileGraphicsItem<TileEntry>, public AssetManager {
private:
	static bool _renderColliders;
	//Assets::TILE_TYPE _tileType;
	uint16_t _value;

protected:
	virtual bool _isSelectedWithinView(const unsigned x=0, const unsigned y=0) const;
	virtual const QPixmap _getTilePixmap(const TileEntry entry, const bool selection=0) const;
	virtual const TileSelection<TileEntry> *_getTileSelection(void) const;

public:

	inline SheetViewTileGraphicsItem(void):
		TileGraphicsItem<TileEntry>::TileGraphicsItem(TileGraphicsItem<TileEntry>::TYPE_SOURCE)
	{}

	//inline void setTerrainType(const Assets::TILE_TYPE type) { _tileType = type; }
	inline void setValue(const uint16_t value) { _value = value; }

	virtual TileEntry getTileEntry(const unsigned x=0, const unsigned y=0) const;


	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


};

#endif // SHEETVIEWTILEGRAPHICSITEM_H
