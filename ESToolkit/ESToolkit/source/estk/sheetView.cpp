#include "sheetView.h"
#include "tileSelectable.h"


SheetView::SheetView(QWidget *parent) : SourceView(parent) {}


//===== EXTERNAL SELECTION =====

void SheetView::_externalSelectionCreate(void) {
	if(scene()->selectedItems().size() <= 0) return;

	TileSelectable *sel = new TileSelectable(scene()->selectedItems(), getCurrentArea(), getCurrentLevel(), getAssets()->getCurrentLayer());
    if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) getAssets()->setCurrentLayer(Assets::LAYER_TILE1);

	getAssets()->setCurrentSelection(sel);
}

void SheetView::_externalSelectionClear(void) {
    getAssets()->setCurrentSelection(nullptr);
}
