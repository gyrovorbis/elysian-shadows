#ifndef SHEET_MANAGER_ASSETS_H
#define SHEET_MANAGER_ASSETS_H

#include <QPixmap>
#include <QObject>
#include <QUndoStack>
#include "gyro.h"
#include "assets.h"
#include "sheetManagerTileSelection.h"
#include "sheetManagerTileEntry.h"

class SheetManagerTileSelection;
class SourceSheetEntry;
class Level;

class SheetManagerAssets: public QObject {
	Q_OBJECT
public:
	struct DestSheetEntry {
		SheetManagerTileEntry tileEntry[512];
	};
private:

	//Destination sheet current state
	SheetManagerTileSelection *_curSel;
	Level *_curLevel;

	DestSheetEntry _destSheetEntry;

	//Source sheet current state
	const SourceSheetEntry *_curSrcSheet;

	QUndoStack _undoStack;

public:

	inline SheetManagerAssets(void): _curSel(NULL), _curLevel(NULL) {
		for(unsigned i = 0; i < 512; ++i) {
			_destSheetEntry.tileEntry[i].setIndex(i);
		}
	}

	inline SheetManagerTileSelection *getCurrentSelection(void) const { return _curSel; }
	inline Level *getCurrentLevel(void) const { return _curLevel; }
	inline const SourceSheetEntry *getCurrentSourceSheetEntry(void) const { return _curSrcSheet; }
	inline QUndoStack *getUndoStack(void) { return &_undoStack ; }
	inline DestSheetEntry &getDestSheetEntry(void) { return _destSheetEntry; }

	inline void setCurrentSelection(SheetManagerTileSelection *const sel) {
		delete _curSel;
		_curSel = sel;
		emit onCurrentSelectionChanged(sel);
	}
	inline void setCurrentLevel(Level *const level) { _curLevel = level; }
	inline void setCurrentSourceSheetEntry(const SourceSheetEntry *const sheet) { _curSrcSheet = sheet; }

signals:
	void onCurrentSelectionChanged(SheetManagerTileSelection *const selection);

};


#endif // SHEETMANAGERASSETS_H
