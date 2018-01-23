#ifndef SHEET_MANAGER_TILE_SELECTION_H
#define SHEET_MANAGER_TILE_SELECTION_H

#include <QVector>
#include <QESTkUtils/tile_engine/tk_tile_selection.h>
#include <ESToolkit/sheet_manager/tk_sheet_manager_tile_entry.h>

class SheetManagerTileSelection: public TileSelection<SheetManagerTileEntry> {
private:
public:
	SheetManagerTileSelection(const QList<QGraphicsItem *> &items);

	SheetManagerTileSelection(const SheetManagerTileSelection &rvalue);
/*
	inline const QPixmap getPixmap(const uint16_t index) const {
		return (index < _pixmaps.size())? _pixmaps[index] : NULL;
	}

	inline const QVector<QPixmap> &getPixmaps(void) { return _pixmaps; }*/

};

inline SheetManagerTileSelection::SheetManagerTileSelection(const QList<QGraphicsItem *> &items): TileSelection<SheetManagerTileEntry>::TileSelection(items)
{}


inline SheetManagerTileSelection::SheetManagerTileSelection(const SheetManagerTileSelection &rvalue): TileSelection<SheetManagerTileEntry>::TileSelection(rvalue)
{

}


#endif // SHEETMANAGERTILESELECTION_H
