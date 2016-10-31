#ifndef SHEET_MANAGER_DEST_VIEW_H
#define SHEET_MANAGER_DEST_VIEW_H

#include <tk_dest_view.h>
#include "tk_sheet_manager_asset_manager.h"
#include "tk_sheet_manager_tile_entry.h"

class SheetManagerDestView : public DestView<SheetManagerTileEntry>, public SheetManagerAssetManager  {
public:
	explicit SheetManagerDestView(QWidget *parent=0);

private:
	// reimplemented from sourceView
	virtual void _externalSelectionCreate(void);
	virtual void _externalSelectionClear(void);

	// implemented from DestView
	virtual TileSelection<SheetManagerTileEntry> *_getTileSelection(void) const;
	virtual void _setTileSelection(TileSelection<SheetManagerTileEntry> *const sel);
	virtual QUndoStack *_getUndoStack(void) const;
	virtual PlaceTileGroupUndoItem<SheetManagerTileEntry> *_createPlaceTileGroup(void);
	virtual TileSelection<SheetManagerTileEntry> *_copyConstructTileSelection(const TileSelection<SheetManagerTileEntry> &rval) const;

};

#endif // SMSHEETVIEW_H
