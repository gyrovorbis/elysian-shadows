#include "sheetManagerSourceView.h"
#include "sheetManagerTileSelection.h"
#include "sheetManagerAssets.h"
#include "sheetManager.h"

SheetManagerSourceView::SheetManagerSourceView(QWidget *parent): SourceView(parent) {}

void SheetManagerSourceView::_externalSelectionCreate(void) {
	if(scene()->selectedItems().size() <= 0) return;

	getSMAssets()->setCurrentSelection(new SheetManagerTileSelection(scene()->selectedItems()));
}

void SheetManagerSourceView::_externalSelectionClear(void) {
	getSMAssets()->setCurrentSelection(NULL);
}
