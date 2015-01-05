#ifndef MAP_VIEW_TILE_GRAPHICS_ITEM_H
#define MAP_VIEW_TILE_GRAPHICS_ITEM_H

#include "tileGraphicsItem.h"
#include "assetManager.h"
#include "assets.h"
#include "mapView.h"
#include "tileImage.h"
#include "tileSelectable.h"
#include "elysian_level.hpp"

class MapViewTileGraphicsItem: public TileGraphicsItem<TileEntry>, public AssetManager {
protected:
	Assets::LAYER_TYPE _currentLayer;

    virtual bool                            _isSelectedWithinView(const unsigned x=0, const unsigned y=0) const;
    virtual const QPixmap                   _getTilePixmap(const TileEntry entry, const bool selection=0) const;
    virtual const TileSelection<TileEntry>* _getTileSelection(void) const;
    virtual TileEntry                       _getTileEntryFromDrawStyle(const unsigned x, const unsigned y, const DRAW_STYLE style) const;


public:
	MapViewTileGraphicsItem(void);
	QRect getVisibleTileRegion(void) const;
	void prepareResize(void);

	virtual QRectF boundingRect(void) const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual TileEntry getTileEntry(const unsigned x=0, const unsigned y=0) const;
};




//==== INLINEZ ====

inline bool MapViewTileGraphicsItem::_isSelectedWithinView(const unsigned x, const unsigned y) const {
	return reinterpret_cast<const MapView*>(this->_view.dst)->isSelected(x, y);
}

inline const QPixmap MapViewTileGraphicsItem::_getTilePixmap(const TileEntry entry, const bool /*selection*/) const {
	return getCurrentLevel()->getTileImage(entry.getIndex())->getPixmap();
}

inline const TileSelection<TileEntry> *MapViewTileGraphicsItem::_getTileSelection(void) const {
	return (getCurrentSelectionType() == Selectable::TILE)?
                reinterpret_cast<const TileSelection<TileEntry>* >(static_cast<const TileSelectable *>(getCurrentSelection())) :
                nullptr;
}

inline TileEntry MapViewTileGraphicsItem::getTileEntry(const unsigned x, const unsigned y) const {
	return getCurrentArea()->getTileEntry(QPoint(x,y), _currentLayer);
}

inline void MapViewTileGraphicsItem::prepareResize(void) { this->prepareGeometryChange(); }

#endif // MAPVIEWTILEGRAPHICSITEM_H
