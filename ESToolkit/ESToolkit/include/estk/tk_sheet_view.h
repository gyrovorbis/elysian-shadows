#ifndef SHEET_VIEW_H
#define SHEET_VIEW_H

#include <tk_source_view.h>
#include "tk_asset_manager.h"

class SheetView : public SourceView, public AssetManager  {
public:
	explicit SheetView(QWidget *parent=0);

	//Implement external selection logic of SourceView
	virtual void _externalSelectionCreate(void);
	virtual void _externalSelectionClear(void);
};

#endif
