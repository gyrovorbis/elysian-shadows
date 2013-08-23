#ifndef MAPVIEW_PLACE_TILE_GROUP_UNDO_ITEM_H
#define MAPVIEW_PLACE_TILE_GROUP_UNDO_ITEM_H

#include "tileGroupUndoItem.h"

class MapViewPlaceTileGroupUndoItem: public PlaceTileGroupUndoItem<TileEntry> {
private:
	Area *const _area;
	const Assets::LAYER_TYPE _layer;
public:

	inline MapViewPlaceTileGroupUndoItem(Area *const area, const Assets::LAYER_TYPE layer):
	PlaceTileGroupUndoItem<TileEntry>::PlaceTileGroupUndoItem(UndoItem::PLACE_TILE), _area(area), _layer(layer) { }

	virtual bool _withinBoundaries(const QPoint loc) const {
		//This can't be right?
		//x = ELYSIAN_AREA_MAP_WIDTH; y = ELYSIAN_AREA_MAP_HEIGHT;
		return (loc.x() < _area->getMapWidth() && loc.y() < _area->getMapHeight() && loc.x() >= 0 && loc.y() >= 0);
	}

	virtual TileEntry _getOldTileData(const QPoint loc) const {
		return _area->getTileEntry(loc, _layer);
	}

	virtual void undo(void) {
		for(QMap<QPoint, UndoEntry>::const_iterator i = _undoGroup.begin(); i != _undoGroup.end(); ++i) {
			//_area->setTileValue(i.key(), _layer, ELYSIAN_MAP_TERRAIN_PACK_VALUE(i.value().oldData.index, i.value().oldData.orient));
			_area->setTileEntry(i.key(), _layer, i.value().oldData);
		}
		_area->editorScene->update();
	}

	virtual void redo(void) {
		for(QMap<QPoint, UndoEntry>::const_iterator i = _undoGroup.begin(); i != _undoGroup.end(); ++i) {
			//_area->setTileValue(i.key(), _layer, ELYSIAN_MAP_TERRAIN_PACK_VALUE(i.value().newData.index, i.value().newData.orient));
			_area->setTileEntry(i.key(), _layer, i.value().newData);
		}
		_area->editorScene->update();
	}
};

#endif // MAPVIEWPLACETILEGROUPUNDOITEM_H
