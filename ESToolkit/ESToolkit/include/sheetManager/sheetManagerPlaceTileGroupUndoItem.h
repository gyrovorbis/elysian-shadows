#ifndef SHEET_MANAGER_PLACE_TILE_GROUP_UNDO_ITEM_H
#define SHEET_MANAGER_PLACE_TILE_GROUP_UNDO_ITEM_H

#include ELYSIAN_ASSET_HEADER("elysian_level.hpp", "level.h")
#include "tileGroupUndoItem.h"
#include "sheetManager.h"
#include "sheetManagerAssets.h"
#include "sheetManagerAssetManager.h"
#include "sheetManagerDestView.h"
#include "sheetManagerTileEntry.h"

class SheetManagerPlaceTileGroupUndoItem:
	public PlaceTileGroupUndoItem<SheetManagerTileEntry> {
private:
	SheetManagerAssets::DestSheetEntry &_destEntry;
	SheetManagerDestView *const _destView;

public:
	inline SheetManagerPlaceTileGroupUndoItem(SheetManagerAssets::DestSheetEntry &destEntry, SheetManagerDestView *const destView):
		PlaceTileGroupUndoItem<SheetManagerTileEntry>::PlaceTileGroupUndoItem(UndoItem::PLACE_TILE_SM), _destEntry(destEntry), _destView(destView) {}

	virtual bool _withinBoundaries(const QPoint loc) const {
		return (loc.x() < 16 && loc.y() < ELYSIAN_TILESHEET_TILE_HEIGHT && loc.x() >= 0 && loc.y() >= 0 && (loc.x() != 0 || loc.y() != 0));
	}

	virtual SheetManagerTileEntry _getOldTileData(const QPoint loc) const {
		/*SheetManagerPlaceTileGroupUndoItemTileData data;
		data.index = loc.y()*16+loc.x();
		data.orient = _destEntry.orients[data.index];
		data.pixmap = _destEntry.pixmaps[data.index];
		return data;*/
		return _destEntry.tileEntry[loc.y()*ELYSIAN_TILESHEET_TILE_WIDTH+loc.x()];
	}

	virtual void undo(void) {
		for(QMap<QPoint, UndoEntry>::const_iterator i = _undoGroup.begin(); i != _undoGroup.end(); ++i) {
			//_area->setTileValue(i.key(), _layer, ELYSIAN_MAP_PACK_VALUE(i.value().oldData.index, i.value().oldData.orient));

			//_destEntry.pixmaps[i.value().oldData.index] = i.value().oldData.pixmap;
			//_destEntry.orients[i.value().oldData.index] = i.value().oldData.orient;
			//Do NOT copy indices over!!!
			_destEntry.tileEntry[i.value().oldData.getIndex()].setPixmap(i.value().oldData.getPixmap());
			_destEntry.tileEntry[i.value().oldData.getIndex()].setOrient(i.value().oldData.getOrient());
			//memcpy(&_destEntry.tileEntry[i.value().oldData.getIndex()], &i.value().oldData, sizeof(SheetManagerTileEntry));
		}
		_destView->scene()->update();
	}

	virtual void redo(void) {
		for(QMap<QPoint, UndoEntry>::const_iterator i = _undoGroup.begin(); i != _undoGroup.end(); ++i) {
			//_area->setTileValue(i.key(), _layer, ELYSIAN_MAP_PACK_VALUE(i.value().oldData.index, i.value().oldData.orient));
/*
			_destEntry.pixmaps[i.value().oldData.index] = i.value().newData.pixmap;
			_destEntry.orients[i.value().oldData.index] = i.value().newData.orient;*/

			//_destEntry.tileEntry[i.value().oldData.getIndex()] = i.value().newData;
			//memcpy(&_destEntry.tileEntry[i.value().oldData.getIndex()], &i.value().newData, sizeof(SheetManagerTileEntry));
			//Do NOT copy indices over!!!
			_destEntry.tileEntry[i.value().oldData.getIndex()].setPixmap(i.value().newData.getPixmap());
			_destEntry.tileEntry[i.value().oldData.getIndex()].setOrient(i.value().newData.getOrient());
		}
		_destView->scene()->update();
	}

};
#endif // SHEETMANAGERPLACETILEGROUPUNDOITEM_H
