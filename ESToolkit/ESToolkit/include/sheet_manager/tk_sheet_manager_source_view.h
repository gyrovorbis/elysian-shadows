#ifndef SHEET_MANAGER_SOURCE_VIEW_H
#define SHEET_MANAGER_SOURCE_VIEW_H

#include <tk_source_view.h>
#include "tk_sheet_manager_asset_manager.h"

class SheetManagerSourceView: public SourceView, public SheetManagerAssetManager {
friend class SheetManager;
public:
	explicit SheetManagerSourceView(QWidget *parent=0);

private:

	// reimplemented from sourceView
	virtual void _externalSelectionCreate(void);
	virtual void _externalSelectionClear(void);
};


#endif // SOURCE_VIEW_H
