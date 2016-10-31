#include "tk_sheet_view.h"
#include "tk_tile_selectable.h"
#include <tk_level.h>

SheetView::SheetView(QWidget *parent) : SourceView(parent) {}


//===== EXTERNAL SELECTION =====

void SheetView::_externalSelectionCreate(void) {
    if(scene()->selectedItems().size() <= 0 || !getCurrentLevel()->validSheets()) return;

	TileSelectable *sel = new TileSelectable(scene()->selectedItems(), getCurrentArea(), getCurrentLevel(), getAssets()->getCurrentLayer());
    if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) getAssets()->setCurrentLayer(Assets::LAYER_TILE1);

    sel->setElev(_elev);
    sel->setPlane(_plane);
    sel->setOrient(_orient);

	getAssets()->setCurrentSelection(sel);
}

void SheetView::_externalSelectionClear(void) {
    getAssets()->setCurrentSelection(nullptr);
}
