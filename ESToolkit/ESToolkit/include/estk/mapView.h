#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QVector>
#include <QRect>
#include "destView.h"
#include "assetManager.h"
#include ELYSIAN_ASSET_HEADER("elysian_area.hpp", "area.h")

class MapViewPlaceTileGroupUndoItem;

class MapView : public DestView<TileEntry>, public AssetManager  {
	Q_OBJECT
private:
	typedef Assets::LAYER_TYPE LAYER_TYPE;
    bool _tileSelectionArray[ELYSIAN_AREA_MAP_HEIGHT][ELYSIAN_AREA_MAP_WIDTH] = {{false}};

    //the timer id of a 30ms draw refresh
	int								_refreshTimerHandle = 30;

	MapViewPlaceTileGroupUndoItem*	_curUndoItem		= nullptr;

	void _tileSelectionArrayUpdate(void);

	//SourceView virtual implementation
    virtual void    _sceneSelectionAddRect(const QRectF &rect) override;
    virtual void    _sceneSelectionUpdateLastRect(const QRectF &rect) override;
    virtual void    _sceneSelectionClear(void) override;
    virtual void    _externalSelectionCreate(void) override;
    virtual void    _externalSelectionClear(void) override;
    virtual void    _forceSceneUpdate(void) override;
    virtual QPointF _mapViewPosToScenePos(const QPoint viewPos) const override;
    virtual void    _startPan(void) override;
    virtual void    _endPan(void) override;

	//DestView virtual implementation
    virtual TileSelection<TileEntry>*           _getTileSelection(void) const override;
    virtual void                                _setTileSelection(TileSelection<TileEntry> *const sel) override;
    virtual QUndoStack*                         _getUndoStack(void) const override;
    virtual PlaceTileGroupUndoItem<TileEntry>*  _createPlaceTileGroup(void) override;
    virtual TileSelection<TileEntry>*           _copyConstructTileSelection(const TileSelection<TileEntry> &rval) const override;
	virtual bool								_populatePlaceTileGroup(const TileSelection<TileEntry> *const terSel, PlaceTileGroupUndoItem<TileEntry> *const groupCmd) const override;
	virtual bool								_populatePlaceTileGroupRegionFill(const TileSelection<TileEntry> *const terSel, PlaceTileGroupUndoItem<TileEntry> *const groupCmd) const override;


public:
    explicit    MapView(QWidget *parent = nullptr);

	inline bool isSelected(const unsigned x, const unsigned y) const { return _tileSelectionArray[y][x]; }
	inline bool isTerrainLayer(void) const { return getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY; }
	inline bool isEntityLayer(void) const { return getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY; }

    //QGraphicsView virtual implementation
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void timerEvent(QTimerEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void scrollContentsBy(int dx, int dy) override {}

public slots:

	void onAreaChange(Area* const area);
	void onLayerChange(LAYER_TYPE type);
};

#endif
