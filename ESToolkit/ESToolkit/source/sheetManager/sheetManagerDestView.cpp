#include "sheetManagerDestView.h"
#include "sheetManagerAssets.h"
#include "sheetManagerPlaceTileGroupUndoItem.h"

SheetManagerDestView::SheetManagerDestView(QWidget *parent):
	DestView<SheetManagerTileEntry>::DestView(parent)
{
	_autoScrollEnabled = true;
}

void SheetManagerDestView::_externalSelectionCreate(void) {
	if(scene()->selectedItems().size() <= 0) return;
	getSMAssets()->setCurrentSelection(new SheetManagerTileSelection(scene()->selectedItems()));
}

void SheetManagerDestView::_externalSelectionClear(void) {
	getSMAssets()->setCurrentSelection(NULL);
    scene()->setSceneRect(scene()->itemsBoundingRect());
}

TileSelection<SheetManagerTileEntry> *SheetManagerDestView::_getTileSelection(void) const {
	return getSMAssets()->getCurrentSelection();
}

void SheetManagerDestView::_setTileSelection(TileSelection<SheetManagerTileEntry> *const sel) {
	getSMAssets()->setCurrentSelection(static_cast<SheetManagerTileSelection *>(sel));
}

QUndoStack *SheetManagerDestView::_getUndoStack(void) const {
	return getSMAssets()->getUndoStack();
}

PlaceTileGroupUndoItem<SheetManagerTileEntry> *SheetManagerDestView::_createPlaceTileGroup(void) {
	return new SheetManagerPlaceTileGroupUndoItem(getSMAssets()->getDestSheetEntry(),
													this);
}

TileSelection<SheetManagerTileEntry> *SheetManagerDestView::_copyConstructTileSelection(const TileSelection<SheetManagerTileEntry> &rval) const {
	return new SheetManagerTileSelection(*static_cast<const SheetManagerTileSelection *>(&rval));
}
