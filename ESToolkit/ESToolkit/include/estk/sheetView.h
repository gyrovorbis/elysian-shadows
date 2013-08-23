#ifndef SHEET_VIEW_H
#define SHEET_VIEW_H

#include "sourceView.h"
#include "assetManager.h"

class SheetView : public SourceView, public AssetManager  {
public:
	explicit SheetView(QWidget *parent=0);

	//Implement external selection logic of SourceView
	virtual void _externalSelectionCreate(void);
	virtual void _externalSelectionClear(void);
};

#endif
