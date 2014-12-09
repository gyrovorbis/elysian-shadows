#ifndef MAPVIEW_PLACE_TILE_GROUP_UNDO_ITEM_H
#define MAPVIEW_PLACE_TILE_GROUP_UNDO_ITEM_H

#include "tileGroupUndoItem.h"
#include "tileSelectable.h"

class MapViewPlaceTileGroupUndoItem: public PlaceTileGroupUndoItem<TileEntry> {
private:
	Area*					_area		= nullptr;
	Assets::LAYER_TYPE		_layer		= Assets::LAYER_MAX;
	bool					_skipFirst	= false;
public:

	inline MapViewPlaceTileGroupUndoItem(Area *const area=nullptr, const Assets::LAYER_TYPE layer=Assets::LAYER_MAX):
	PlaceTileGroupUndoItem<TileEntry>::PlaceTileGroupUndoItem(UndoItem::PLACE_TILE), _area(area), _layer(layer) {

	}

	virtual bool _withinBoundaries(const QPoint loc) const {
		//This can't be right?
		//x = ELYSIAN_AREA_MAP_WIDTH; y = ELYSIAN_AREA_MAP_HEIGHT;
		return (loc.x() < _area->getMapWidth() && loc.y() < _area->getMapHeight() && loc.x() >= 0 && loc.y() >= 0);
	}

	virtual TileEntry _getOldTileData(const QPoint loc) const {
		return _area->getTileEntry(loc, _layer);
		//return _prevEntry[loc.y()-_sceneY][loc.x()-_sceneX];
	}

	virtual void undo(void) {
        _area->beginMapUpdate();
		for(QMap<QPoint, UndoEntry>::const_iterator i = _undoGroup.begin(); i != _undoGroup.end(); ++i) {
			//_area->setTileValue(i.key(), _layer, ELYSIAN_MAP_TERRAIN_PACK_VALUE(i.value().oldData.index, i.value().oldData.orient));
			_area->setTileEntry(i.key(), _layer, i.value().oldData);
		}
        _area->endMapUpdate();
		//_area->editorScene->update();
	}

	virtual void redo(void) {
        //if(_skipFirst) { //assume changes have already been applied to map from updating
        //	_skipFirst = false;
        //} else {
            _area->beginMapUpdate();
			for(QMap<QPoint, UndoEntry>::const_iterator i = _undoGroup.begin(); i != _undoGroup.end(); ++i) {
				//_area->setTileValue(i.key(), _layer, ELYSIAN_MAP_TERRAIN_PACK_VALUE(i.value().newData.index, i.value().newData.orient));
				_area->setTileEntry(i.key(), _layer, i.value().newData);
			}
            _area->endMapUpdate();
        //}
		//_area->editorScene->update();
	}

	void beginUpdateEntries(void) {
		for(auto& it : _undoGroup) {
			it.dirty = true;
		}
	}

	void endUpdateEntries(void) {
        _area->beginMapUpdate();
		for(auto it = _undoGroup.begin(); it != _undoGroup.end(); ++it) {
			if(it.value().dirty) {
				_area->setTileEntry(it.key(), _layer, it.value().oldData);
			} else if(it.value().changed) {
				_area->setTileEntry(it.key(), _layer, it.value().newData);
			}
		}

		for(auto it = _undoGroup.begin(); it != _undoGroup.end(); ++it)
			if(it.value().dirty)
				_undoGroup.erase(it);

        _area->endMapUpdate();
    }

	void clearEntries(void) {
		undo();
		_undoGroup.clear();
	}

	void refreshOldData(void) {
		for(auto it = _undoGroup.begin(); it != _undoGroup.end(); ++it) {
			it.value().dirty = false;
			it.value().oldData = _getOldTileData(it.key());
		}
	}

	void setLayer(const Assets::LAYER_TYPE layer) {
		if(_undoGroup.size() && _layer != layer) {
			undo();
			_layer = layer;
			refreshOldData();
			endUpdateEntries();
		} else {
			_layer = layer;
		}
	}

	void setArea(Area*const area) {
		if(_undoGroup.size() && _area != area) {
			undo();
			_area = area;
			refreshOldData();
			endUpdateEntries();
		} else {
			_area = area;
		}
	}

    void onMapChanged(const QVector<MapChange>& changes) {
        if(!_undoGroup.size()) return;

        int count = changes.size();

        for(int i = 0; i < count; ++i) {

            auto entry = _undoGroup.find(changes[i].loc);

            if(entry != _undoGroup.end() && changes[i].layer == _layer) {
                entry.value().oldData = changes[i].newEntry;
                _area->setTileEntry(entry.key(), _layer, entry.value().newData);
            }

        }
    }

#if 0
        for(auto it = _undoGroup.begin(); it != _undoGroup.end(); ++it) {

            //updated outside scene
            auto entry = _area->getTileEntry(it.key(), _layer);
            if(entry.getValue() != it.value().newData.getValue()) {
                it.value().oldData = entry;                                 //maintain actual tile value
                _area->setTileEntry(it.key(), _layer, it.value().newData);    //update selected tile
            }

            //if(it.value().newData.getValue() == 0) {
            //    qDebug() << "UPDATING A ZERO!!!!";
            //}

        }
    }
#endif

};

#endif // MAPVIEWPLACETILEGROUPUNDOITEM_H
