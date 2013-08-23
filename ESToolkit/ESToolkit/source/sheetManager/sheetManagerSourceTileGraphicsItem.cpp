#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "sheetManager.h"
#include "sheetManagerSourceTileGraphicsItem.h"
#include "tileImage.h"
#include "sheetManagerTileSelection.h"

SheetManagerSourceTileGraphicsItem::SheetManagerSourceTileGraphicsItem(const unsigned x, const unsigned y, const SourceSheetEntry *const entry, const SourceView *const view):
	TileGraphicsItem<SheetManagerTileEntry>::TileGraphicsItem(TYPE_SOURCE), _entry(entry), _value(y*entry->tileWidth+x) {
	setFlags(QGraphicsItem::ItemIsSelectable);
	setZValue(1.0f);
	setTilePosition(x, y);
	setView(view);
	useFlipRotBits(false);
	renderTile0(true);
	//_value = y*entry->tileWidth + x;
}

SheetManagerTileEntry SheetManagerSourceTileGraphicsItem::getTileEntry(const unsigned /*x*/, const unsigned /*y*/) const {
	return SheetManagerTileEntry(_value, GY_RECT_TEX_ORIENT_NORMAL, _entry->imgs[_value]);
}

bool SheetManagerSourceTileGraphicsItem::_isSelectedWithinView(const unsigned /*x*/, const unsigned /*y*/) const {
	return isSelected();
}

const QPixmap SheetManagerSourceTileGraphicsItem::_getTilePixmap(const SheetManagerTileEntry entry, const bool /*selection*/) const {
	return entry.getPixmap();
}

const TileSelection<SheetManagerTileEntry> *SheetManagerSourceTileGraphicsItem::_getTileSelection(void) const {
	return NULL;
}

