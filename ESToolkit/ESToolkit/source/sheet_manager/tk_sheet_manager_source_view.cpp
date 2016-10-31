#include "tk_sheet_manager_source_view.h"
#include "tk_sheet_manager_tile_selection.h"
#include "tk_sheet_manager_assets.h"
#include "tk_sheet_manager.h"

SheetManagerSourceView::SheetManagerSourceView(QWidget *parent): SourceView(parent) {}

void SheetManagerSourceView::_externalSelectionCreate(void) {
	if(scene()->selectedItems().size() <= 0) return;

	getSMAssets()->setCurrentSelection(new SheetManagerTileSelection(scene()->selectedItems()));
}

void SheetManagerSourceView::_externalSelectionClear(void) {
	getSMAssets()->setCurrentSelection(NULL);
}
