#ifndef TILE_SELECTABLE_SELECTION_VIEW_DELEGATE_H
#define TILE_SELECTABLE_SELECTION_VIEW_DELEGATE_H

#include <QStyledItemDelegate>
#include <QIcon>
#include "elysian_tile.hpp"
#include "assetManager.h"

/*
  This class abuses the LIVING SHIT out of OO programming. We're using this class to add
  additional functionality to objects that were allocated as elysian::Tile. Since we aren't
  adding any nonstatic data members, the objects should be the same in memory, and this
  should still be legit. We have to do this to hook elysian::Tile to PropertyStandardItem.
*/
class ElysianTileWrapper: public elysian::Tile, public AssetManager {
private:
	static QIcon **_icons;
	static const char _colliderStringFromEnum[elysian::Tile::COLLIDER_MAX][100];
	static const char _colliderIconNameFromEnum[elysian::Tile::COLLIDER_MAX][100];
public:
	QString getColliderString(void) const;
	void setColliderString(const QString string);
	QIcon updateColliderIcon(const QString string) const;

	static COLLIDER_TYPE getColliderEnumFromString(const QString string);
	static QString getColliderStringFromEnum(const COLLIDER_TYPE type);
	static QIcon getColliderIconFromEnum(const COLLIDER_TYPE type);
	static QIcon getColliderIconFromString(const QString string);
	static void allocIcons(void);
};

//Used as a delegate for SelectionView when a TileSelectable is the active model
class TileSelectableSelectionViewDelegate: public QStyledItemDelegate {
public:
	TileSelectableSelectionViewDelegate(void);
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};


//===== INLINE METHODS =====


inline TileSelectableSelectionViewDelegate::TileSelectableSelectionViewDelegate(void) {
	ElysianTileWrapper::allocIcons();
}

inline QString ElysianTileWrapper::getColliderString(void) const {
	return getColliderStringFromEnum(colliderType);
}


inline QIcon ElysianTileWrapper::updateColliderIcon(const QString string) const {
	return getColliderIconFromString(string);
}


inline ElysianTileWrapper::COLLIDER_TYPE ElysianTileWrapper::getColliderEnumFromString(const QString string) {
	for(unsigned i = 0; i < elysian::Tile::COLLIDER_MAX; ++i) {
		if(strcmp(_colliderStringFromEnum[i], string.toStdString().c_str()) == 0) {
			return (elysian::Tile::COLLIDER_TYPE)i;
		}
	}
	Q_ASSERT(0); //Wasn't a valid collider name, dumbass.
	return elysian::Tile::COLLIDER_NONE;
}

inline QString ElysianTileWrapper::getColliderStringFromEnum(const COLLIDER_TYPE type) {
	return _colliderStringFromEnum[type];
}

inline QIcon ElysianTileWrapper::getColliderIconFromEnum(const COLLIDER_TYPE type) {
	return *_icons[type];
}

inline QIcon ElysianTileWrapper::getColliderIconFromString(const QString string) {
	return getColliderIconFromEnum(getColliderEnumFromString(string));
}

#endif // TILESELECTABLESELECTIONVIEWDELEGATE_H
