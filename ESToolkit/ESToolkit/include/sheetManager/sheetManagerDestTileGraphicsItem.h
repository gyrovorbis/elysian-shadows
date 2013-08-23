#ifndef SHEET_MANAGER_DEST_TILE_GRAPHICS_ITEM_H
#define SHEET_MANAGER_DEST_TILE_GRAPHICS_ITEM_H

#include "tileGraphicsItem.h"
#include "sheetManagerAssetManager.h"
#include "sheetManagerTileEntry.h"

class SheetManagerTileSelection;
class SourceView;

class SheetManagerDestTileGraphicsItem: public TileGraphicsItem<SheetManagerTileEntry>, public SheetManagerAssetManager {
private:

	virtual bool _isSelectedWithinView(const unsigned x=0, const unsigned y=0) const;
	virtual const QPixmap _getTilePixmap(const SheetManagerTileEntry entry, const bool selection=false) const;
	virtual const TileSelection<SheetManagerTileEntry> *_getTileSelection(void) const;

	uint16_t _value;
	uint16_t _x, _y;
public:

	SheetManagerDestTileGraphicsItem(const unsigned x, const unsigned y, const SourceView *const view);

	virtual SheetManagerTileEntry getTileEntry(const unsigned x=0, const unsigned y=0) const;
};

#endif // SHEETMANAGERDESTTILEITEM_H
