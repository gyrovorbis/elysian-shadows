#ifndef SHEET_MANAGER_TILE_ENTRY_H
#define SHEET_MANAGER_TILE_ENTRY_H

#include <QPixmap>
#include <gyro.h>

class QPainter;

class SheetManagerTileEntry {
	friend bool operator== (SheetManagerTileEntry& e1, SheetManagerTileEntry& e2);
private:
	QPixmap _pixmap;
	GY_RECT_TEX_ORIENT _orient;
	uint16_t _index;
public:
	SheetManagerTileEntry(void) {}
	SheetManagerTileEntry(const uint16_t index,
						  const GY_RECT_TEX_ORIENT orient,
						  const QPixmap pixmap):
		_pixmap(pixmap), _orient(orient), _index(index) {}

	GY_RECT_TEX_ORIENT getOrient(void) const { return _orient; }
	uint16_t getIndex(void) const { return _index; }
	const QPixmap getPixmap(void) const { return _pixmap; }

	void setOrient(const GY_RECT_TEX_ORIENT orient) { _orient = orient; }
	void setIndex(const uint16_t index) { _index = index; }
	void setPixmap(const QPixmap pixmap) { _pixmap = pixmap; }

	void renderDecoration(QPainter *const /*painter*/, const unsigned /*x*/, const unsigned /*y*/) const { }

};

inline bool operator== (const SheetManagerTileEntry& e1, const SheetManagerTileEntry& e2)
{
	//return (e1._pixmap == e2._pixmap && e1._index == e2._index && e1._orient == e2._orient);
	return false;
}


#endif // SHEETMANAGERTILEENTRY_H
