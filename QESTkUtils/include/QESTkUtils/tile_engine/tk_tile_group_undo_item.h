#ifndef PLACE_TILE_GROUP_H
#define PLACE_TILE_GROUP_H

#include <QMap>
#include <QDebug>

#include <ESToolkit/core/tk_undo_item.h>
#include <ESToolkit/AssetIO/tk_area.h>

//Implicitly used as comparator for map of QPoints
inline bool operator<(const QPoint &p1, const QPoint &p2) {
	return (p1.y() < p2.y() || (p1.y() == p2.y() && p1.x() < p2.x()));
}

/*
class TileGroupUndoItem: public UndoItem {
public:
	struct BaseTileData {
		unsigned index;
		GY_RECT_TEX_ORIENT orient;
	};

protected:
	bool _combine;

	TileGroupUndoItem(const UndoItem::TYPE type): UndoItem(type), _combine(true) {}

public:
	inline void setCombine(const bool combine) { _combine = combine; }
	virtual bool addTile(const QPoint loc, const BaseTileData &newBaseData) = 0;
};*/

template <typename T>
class PlaceTileGroupUndoItem: public UndoItem {
protected:

	//Structure representing past and future data for each tile location
	struct UndoEntry {
        T       oldData;
        T       newData;
        bool    dirty		= false;
        bool    changed     = true;
	};
	//Map of every tile location set by this action
	QMap<QPoint, UndoEntry> _undoGroup;
	//Do we combine the next undo item with this one?
	bool _combine;

	//SUBCLASS ME, CUNTWASPS! OWAIT, I'M ABSTRACT ANYWAY. --FG
	inline PlaceTileGroupUndoItem(const UndoItem::TYPE type): UndoItem(type), _combine(true) {}

	//subclass determines whether the given tile loction is within the selectable region
	virtual bool _withinBoundaries(const QPoint loc) const = 0;
	//subclass retrieves the old data that was previously at this tile location
	virtual T _getOldTileData(const QPoint loc) const = 0;

public:
	inline void setCombine(const bool combine) { _combine = combine; }

	bool addTile(const QPoint loc, const T newData) {
		if(!_withinBoundaries(loc)) return false;

		const typename QMap<QPoint, UndoEntry>::iterator i = _undoGroup.find(loc);
		//Entry already exists. Keep old data and override new data
		if(i != _undoGroup.end()) {
			if(i.value().newData == newData) {
				i.value().changed = false;
			} else {
				i.value().changed = true;
				i.value().newData	= newData;
			}

			i.value().dirty		= false;
		}
		else { //Create new entry
			UndoEntry newEntry;
			newEntry.newData	= newData;
			newEntry.oldData	= _getOldTileData(loc);
			_undoGroup[loc]		= newEntry;
		}
		return true;
	}

	virtual bool mergeWith(const QUndoCommand *other) {
		//Make goddamn sure these two commands are mergeable
		if (other->id() != id() || !_combine) return false;

		const QMap<QPoint, UndoEntry> &newUndoGroup = static_cast<const PlaceTileGroupUndoItem *>(other)->_undoGroup;

		for(typename QMap<QPoint, UndoEntry>::const_iterator j = newUndoGroup.begin(); j != newUndoGroup.end(); ++j) {
			const typename QMap<QPoint, UndoEntry>::iterator i = _undoGroup.find(j.key());
			//New tile entry already exists in old group
			if(i != _undoGroup.end()) { //Update newData, keeping same oldData of entry
				i.value().newData = j.value().newData;
			} else { //Insert new tile entry into old group
				_undoGroup[j.key()] = j.value();
			}
		}
		return true;
	}
};

#endif // PLACETILEGROUP_H
