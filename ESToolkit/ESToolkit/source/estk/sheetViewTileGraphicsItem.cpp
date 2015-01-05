#include "sheetViewTileGraphicsItem.h"
#include "elysian_level.hpp"
#include "tileSelectable.h"
#include "tileImage.h"

bool SheetViewTileGraphicsItem::_isSelectedWithinView(const unsigned /*x*/, const unsigned /*y*/) const {
	return isSelected();
}

const QPixmap SheetViewTileGraphicsItem::_getTilePixmap(const TileEntry /*entry*/, const bool /*selection*/) const {
	return getCurrentLevel()->getTileImage(_value)->getPixmap();
}

const TileSelection<TileEntry> *SheetViewTileGraphicsItem::_getTileSelection(void) const {
	return NULL;
}

/*
uint16_t SheetViewTileGraphicsItem::getIndex(const unsigned x, const unsigned y) const {
	return _value;
}

GY_RECT_TEX_ORIENT SheetViewTileGraphicsItem::getOrient(const unsigned x, const unsigned y) const {
	return GY_RECT_TEX_ORIENT_NORMAL;
}*/

TileEntry SheetViewTileGraphicsItem::getTileEntry(const unsigned /*x*/, const unsigned /*y*/) const {
	return _value;
}

void SheetViewTileGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	if(!getCurrentLevel()) return;
	if(!getCurrentLevel()->getTileImage(511)) return; //band-aid for broken levels without sheets.
	TileGraphicsItem<TileEntry>::paint(painter, option, widget);
}
