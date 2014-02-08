#ifndef MAP_VIEW_DEPTH_BUFFER_GRAPHICS_ITEM_H
#define MAP_VIEW_DEPTH_BUFFER_GRAPHICS_ITEM_H

#include "mapViewTileGraphicsItem.h"
#include "elysian_map_depth_buffer.hpp"
#include "area.h"
#include "elysian_zcoords.hpp"
#include "entity.h"

struct ESTkTileDepthBuffEntry: public elysian::MapDepthBuffEntry {
	TileGraphicsItem<TileEntry>::DRAW_STYLE drawStyle;
};

struct ESTkEntityDepthBuffEntry: public elysian::EntityDepthBuffEntry {
	Entity *entity;
};

class MapViewDepthBufferGraphicsItem:
		public MapViewTileGraphicsItem,
		public elysian::MapDepthBuffer<MapViewDepthBufferGraphicsItem,
										ESTkTileDepthBuffEntry,
										ESTkEntityDepthBuffEntry> {
private:
	QPainter *_curPainter;
	QVector<Entity *> *_curEntities;

public:

	//=== Implemented from elysian::MapDepthBuffer ===

	void getOnscreenTileRegion(unsigned &startX, unsigned &startY, unsigned &endX, unsigned &endY) const;
	float getBaseZCoord(void) const;
	void getObjectLayerEntry(const unsigned x, const unsigned y, ESTkTileDepthBuffEntry &entry);
	void renderObjectEntry(const ESTkTileDepthBuffEntry &entry, const float z);
	void beginObjectList(void) const;
	void endObjectList(void) const;
	unsigned getEntityCount(void) const;
	bool getOnscreenEntity(const unsigned index, ESTkEntityDepthBuffEntry &entry, elysian::DEPTH_TYPE &type, float &lowestY, unsigned char &elev) const;
	void renderEntityEntry(ESTkEntityDepthBuffEntry &entry);
	void enablePolygonOffset(void) const;
	void disablePolygonOffset(void) const;

	//=== /Implemened from elysian::MapDepthBuffer ===

	MapViewDepthBufferGraphicsItem(void) {}

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {
		if(!getCurrentLevel() || !getCurrentArea()) return;
		_curEntities = &getCurrentArea()->getEntities();
		_curPainter = painter;

		render();
	}
};


inline void MapViewDepthBufferGraphicsItem::getOnscreenTileRegion(unsigned &startX, unsigned &startY, unsigned &endX, unsigned &endY) const {
	QRect renderRegion = getVisibleTileRegion();
	startX	= renderRegion.left();
	endX	= renderRegion.right();
	startY	= renderRegion.top();
	endY	= renderRegion.bottom();
}

inline float MapViewDepthBufferGraphicsItem::getBaseZCoord(void) const {
	return elysian::ZCoordConstants::mapBase;
}

inline void MapViewDepthBufferGraphicsItem::getObjectLayerEntry(const unsigned x, const unsigned y, ESTkTileDepthBuffEntry &entry) {
	_currentLayer = (Assets::LAYER_TYPE)entry.layer;
	entry.drawStyle = (getAssets()->getCurrentLayer() == _currentLayer)? _getTileDrawStyle(x, y) : DRAW_NORMAL;
	entry.mapEntry = _getTileEntryFromDrawStyle(x, y, entry.drawStyle);
}

inline void MapViewDepthBufferGraphicsItem::renderObjectEntry(const ESTkTileDepthBuffEntry &entry, const float /*z*/)  {
	if(!getAssets()->getLayerVisible((Assets::LAYER_TYPE)entry.layer)) return;
	Assets::LAYER_TYPE curLayer = getAssets()->getCurrentLayer();

	_curPainter->save();
	_curPainter->translate(entry.x*32, entry.y*32);
	if(Area::inactiveTransparency && entry.layer != (unsigned)curLayer) _curPainter->setOpacity(0.5f);
	_renderTile(_curPainter, TileEntry(entry.mapEntry.value), entry.drawStyle, entry.x, entry.y,
				(curLayer == (Assets::LAYER_TYPE)entry.layer));
	_curPainter->restore();
}

inline void MapViewDepthBufferGraphicsItem::beginObjectList(void) const {
	//Tk doesn't use batch texture submission
}

inline void MapViewDepthBufferGraphicsItem::endObjectList(void) const {
	//Tk doesn't use batch texture submission
}

inline unsigned MapViewDepthBufferGraphicsItem::getEntityCount(void) const {
	return _curEntities->count();
}

inline bool MapViewDepthBufferGraphicsItem::getOnscreenEntity(const unsigned index, ESTkEntityDepthBuffEntry &entry, elysian::DEPTH_TYPE &type, float &lowestY, unsigned char &elev) const {
	Entity &entity = *(*_curEntities)[index];
		if(true) { //CHECK IF NIGGER IS ONSCREEN
			entry.entity = &entity;
			type = entity.getElysianEntity()->getDepthType();
			lowestY = entity.getElysianEntity()->getDepthBaseY();
			elev = entity.getElysianEntity()->getElev();
			return true;
		}
		return false;
}

inline void MapViewDepthBufferGraphicsItem::renderEntityEntry(ESTkEntityDepthBuffEntry &/*entry*/) {
	//entry.entity->paint(_curPainter, NULL, NULL);
}

inline void MapViewDepthBufferGraphicsItem::enablePolygonOffset(void) const { }


void MapViewDepthBufferGraphicsItem::disablePolygonOffset(void) const {}

#endif // MAPVIEWDEPTHBUFFERTILEGRAPHICSITEM_H
