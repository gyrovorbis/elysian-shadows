#include <QPainter>
#include "sheetManagerDestTileGraphicsItem.h"
#include "level.h"
#include "tileImage.h"
#include "sheetManagerTileSelection.h"
#include "sheetManagerAssets.h"

SheetManagerDestTileGraphicsItem::SheetManagerDestTileGraphicsItem(const unsigned x,
                                                                   const unsigned y,
                                                                   const SourceView *const view):
	TileGraphicsItem<SheetManagerTileEntry>::TileGraphicsItem(TYPE_DEST),
	_value(y*ELYSIAN_TILESHEET_TILE_WIDTH+x),
	_x(x),
	_y(y)
{
	setFlags(QGraphicsItem::ItemIsSelectable);
	setZValue(1.0f);
	setTilePosition(x, y);
	setView(view);
	renderTile0(true);
}


bool SheetManagerDestTileGraphicsItem::_isSelectedWithinView(const unsigned /*x*/,
                                                             const unsigned /*y*/) const
{
	return isSelected();
}

const QPixmap SheetManagerDestTileGraphicsItem::_getTilePixmap(const SheetManagerTileEntry entry,
                                                               const bool /*selection*/) const
{
	/*return (selection)? getSMAssets()->getCurrentSelection()->getPixmap(index):
						//getSMAssets()->getCurrentLevel()->getTerrainImage(getSMAssets()->getCurrentTerrainType(), index)->getPixmap();
						getSMAssets()->getCurrentDestSheetEntry().tileEntry[index].getPixmap();*/

	return entry.getPixmap();
}


const TileSelection<SheetManagerTileEntry> *SheetManagerDestTileGraphicsItem::_getTileSelection(void) const {
	return getSMAssets()->getCurrentSelection();
}

SheetManagerTileEntry SheetManagerDestTileGraphicsItem::getTileEntry(const unsigned /*x*/,
                                                                     const unsigned /*y*/) const
{
	return getSMAssets()->getDestSheetEntry().tileEntry[_y*ELYSIAN_TILESHEET_TILE_WIDTH+_x];
}

/*
uint16_t SheetManagerDestTileGraphicsItem::getIndex(const unsigned x, const unsigned y) const {
	//VALUE MUST BE PACKED WITH ORIENTATION
	//return TerrainSelection::ReorientTerrainValue(_value, getSMAssets()->getCurrentDestSheetEntry().orients[_value]);
	return _value;
}

GY_RECT_TEX_ORIENT SheetManagerDestTileGraphicsItem::getOrient(const unsigned x, const unsigned y) const {
	return getSMAssets()->getCurrentDestSheetEntry().orients[_value];
}*/
