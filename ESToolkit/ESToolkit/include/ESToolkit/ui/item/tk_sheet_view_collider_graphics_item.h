#ifndef SHEET_VIEW_COLLIDER_GRAPHICS_ITEM_H
#define SHEET_VIEW_COLLIDER_GRAPHICS_ITEM_H

#include "colliderGraphicsItem.h"
#include <ESToolkit/core/tk_assets.h>
#include <ESToolkit/core/tk_asset_manager.h>


class SheetViewColliderGraphicsItem: public ColliderGraphicsItem, public AssetManager {
private:
	//Assets::TILE_TYPE _layerType;
public:

	inline SheetViewColliderGraphicsItem(void)/*: _layerType(Assets::TILE_TERRAIN)*/ {}

	//inline void setTileLayerType(Assets::TILE_TYPE type) { _layerType = type; }

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // SHEETVIEWCOLLIDERGRAPHICSITEM_H
