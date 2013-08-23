#ifndef SHEET_VIEW_COLLIDER_GRAPHICS_ITEM_H
#define SHEET_VIEW_COLLIDER_GRAPHICS_ITEM_H

#include "colliderGraphicsItem.h"
#include "assets.h"
#include "assetManager.h"


class SheetViewColliderGraphicsItem: public ColliderGraphicsItem, public AssetManager {
private:
	//Assets::TILE_TYPE _layerType;
public:

	inline SheetViewColliderGraphicsItem(void)/*: _layerType(Assets::TILE_TERRAIN)*/ {}

	//inline void setTileLayerType(Assets::TILE_TYPE type) { _layerType = type; }

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // SHEETVIEWCOLLIDERGRAPHICSITEM_H
