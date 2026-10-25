/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SHEET_MANAGER_SOURCE_TILE_GRAPHICS_ITEM_H
#define SHEET_MANAGER_SOURCE_TILE_GRAPHICS_ITEM_H

#include <QESTkUtils/tile_engine/tk_tile_graphics_item.h>
#include <ESToolkit/sheet_manager/tk_sheet_manager_tile_entry.h>

struct SourceSheetEntry;
class SheetManagerTileSelection;
class SourceView;

class SheetManagerSourceTileGraphicsItem: public TileGraphicsItem<SheetManagerTileEntry> {
private:

	const SourceSheetEntry *_entry;
	uint16_t _value;

	virtual bool _isSelectedWithinView(const unsigned x=0, const unsigned y=0) const;
	virtual const QPixmap _getTilePixmap(const SheetManagerTileEntry entry, const bool selection=0) const;
	virtual const TileSelection<SheetManagerTileEntry> *_getTileSelection(void) const;

public:
	SheetManagerSourceTileGraphicsItem(const unsigned x, const unsigned y, const SourceSheetEntry *const _entry, const SourceView *const view);

	virtual SheetManagerTileEntry getTileEntry(const unsigned x=0, const unsigned y=0) const;


};



#endif // SHEET_VIEW_TILE_ITEM_H
