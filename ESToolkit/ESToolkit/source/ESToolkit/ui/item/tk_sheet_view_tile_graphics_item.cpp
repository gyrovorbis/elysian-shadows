/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ui/item/tk_sheet_view_tile_graphics_item.h>
#include <ESToolkit/AssetIO/tk_level.h>
#include <ESToolkit/core/tk_tile_selectable.h>
#include <ESToolkit/core/tk_tile_image.h>

bool SheetViewTileGraphicsItem::_isSelectedWithinView(const unsigned /*x*/, const unsigned /*y*/) const {
	return isSelected();
}

const QPixmap SheetViewTileGraphicsItem::_getTilePixmap(const TileEntry /*entry*/, const bool /*selection*/) const {
	return getCurrentLevel()->getTileImage(_value)->getPixmap();
}

const TileSelection<TileEntry> *SheetViewTileGraphicsItem::_getTileSelection(void) const {
    return nullptr;
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
    if(!isVisible()) return;
	TileGraphicsItem<TileEntry>::paint(painter, option, widget);
}
