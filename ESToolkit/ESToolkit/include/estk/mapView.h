#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QVector>
#include <QRect>
#include "destView.h"
#include "assetManager.h"
#include "elysian_area.hpp"

class MapView : public DestView<TileEntry>, public AssetManager  {
private:
	bool _tileSelectionArray[ELYSIAN_AREA_MAP_HEIGHT][ELYSIAN_AREA_MAP_WIDTH];

    //the timer id of a 30ms draw refresh
	int _refreshTimerHandle;

	void _populatePlaceTileGroup(const TileSelection<TileEntry> *const terSel, PlaceTileGroupUndoItem<TileEntry> *const groupCmd) const;
	void _populatePlaceTileGroupRegionFill(const TileSelection<TileEntry> *const terSel, PlaceTileGroupUndoItem<TileEntry> *const groupCmd) const;
	void _tileSelectionArrayUpdate(void);

	//SourceView virtual implementation
	virtual void _sceneSelectionAddRect(const QRectF &rect);
	virtual void _sceneSelectionUpdateLastRect(const QRectF &rect);
	virtual void _sceneSelectionClear(void);
	virtual void _externalSelectionCreate(void);
	virtual void _externalSelectionClear(void);

	//DestView virtual implementation
	virtual TileSelection<TileEntry> *_getTileSelection(void) const;
	virtual void _setTileSelection(TileSelection<TileEntry> *const sel);
	virtual QUndoStack *_getUndoStack(void) const;
	virtual PlaceTileGroupUndoItem<TileEntry> *_createPlaceTileGroup(void);
	virtual TileSelection<TileEntry> *_copyConstructTileSelection(const TileSelection<TileEntry> &rval) const;

public:
	explicit MapView(QWidget *parent = 0);

	inline bool isSelected(const unsigned x, const unsigned y) const { return _tileSelectionArray[y][x]; }
	inline bool isTerrainLayer(void) const { return getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY; }
	inline bool isEntityLayer(void) const { return getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY; }

	virtual void timerEvent(QTimerEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
};

#endif
