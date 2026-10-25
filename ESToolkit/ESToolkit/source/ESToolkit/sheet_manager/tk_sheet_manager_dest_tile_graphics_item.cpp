/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QPainter>
#include <ESToolkit/sheet_manager/tk_sheet_manager_dest_tile_graphics_item.h>
#include <ESToolkit/AssetIO/tk_level.h>
#include <ESToolkit/core/tk_tile_image.h>
#include <ESToolkit/sheet_manager/tk_sheet_manager_tile_selection.h>
#include <ESToolkit/sheet_manager/tk_sheet_manager_assets.h>

SheetManagerDestTileGraphicsItem::SheetManagerDestTileGraphicsItem(const unsigned x,
                                                                   const unsigned y,
                                                                   const SourceView *const view):
	TileGraphicsItem<SheetManagerTileEntry>::TileGraphicsItem(TYPE_DEST),
	//_value(y*ELYSIAN_TILESHEET_TILE_WIDTH+x),
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
