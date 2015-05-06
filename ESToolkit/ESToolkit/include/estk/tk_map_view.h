#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QVector>
#include <QRect>
#include <tk_dest_view.h>
#include "tk_asset_manager.h"
#include "elysian_area.hpp"
#include "elysian_collision_system.hpp"

#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK	(1<<31)

class Project;
class TkLevel;
typedef TkLevel Level;
class TkArea;
typedef TkArea Area;
class TkEntity;
class MapViewPlaceTileGroupUndoItem;

class MapView : public DestView<TileEntry>,
				public AssetManager,
				public elysian::CollisionContactEventMonitor  {
	Q_OBJECT
private:
	typedef Assets::LAYER_TYPE			LAYER_TYPE;
	typedef elysian::Collidable			Collidable;
	typedef elysian::CollisionContact	CollisionContact;

    bool _tileSelectionArray[ELYSIAN_AREA_MAP_HEIGHT][ELYSIAN_AREA_MAP_WIDTH] = {{false}};

    //the timer id of a 30ms draw refresh
	int								_refreshTimerHandle = 30;

	QVector<TkEntity*>				_selectionColliders;
	QVector<TkEntity*>				_selectedEntities;
    MapViewPlaceTileGroupUndoItem*	_curUndoItem		= nullptr;
    TkEntity*                       _cursorEntity       = nullptr;
    TkEntity*                       _underCursorEntity  = nullptr;
    bool                            _entityDrag         = false;

    void _updateScrollBars(void);

	//Internal selection management
	void _tileSelectionArrayUpdate(void);
	void _entitySelectionUpdate(void);
	void _entitySelectableCreate(void);
    bool _entityAddToSelection(TkEntity*const entity);
    bool _entityRemoveFromSelection(TkEntity*const entity);
    void _entityClearSelection(void);
    bool _entityIsSceneSelected(const TkEntity*const entity) const;

	//SourceView virtual implementation
    virtual void            _sceneSelectionAddRect(const QRectF &rect) override;
    virtual void            _sceneSelectionUpdateLastRect(const QRectF &rect) override;
    virtual void            _sceneSelectionClear(void) override;
    virtual void            _externalSelectionCreate(void) override;
    virtual void            _externalSelectionClear(void) override;
    virtual void            _forceSceneUpdate(void) override;
    virtual const QPointF&  _mapViewPosToScenePos(const QPoint& viewPos, QPointF& out) const override;
    virtual void            _startPan(void) override;
    virtual void            _endPan(void) override;

	//DestView virtual implementation
    virtual TileSelection<TileEntry>*           _getTileSelection(void) const override;
    virtual void                                _setTileSelection(TileSelection<TileEntry> *const sel) override;
    virtual QUndoStack*                         _getUndoStack(void) const override;
    virtual PlaceTileGroupUndoItem<TileEntry>*  _createPlaceTileGroup(void) override;
    virtual TileSelection<TileEntry>*           _copyConstructTileSelection(const TileSelection<TileEntry> &rval) const override;
	virtual bool								_populatePlaceTileGroup(const TileSelection<TileEntry> *const terSel, PlaceTileGroupUndoItem<TileEntry> *const groupCmd) const override;
	virtual bool								_populatePlaceTileGroupRegionFill(const TileSelection<TileEntry> *const terSel, PlaceTileGroupUndoItem<TileEntry> *const groupCmd) const override;

	//elysian::CollisionContactMonitor virtual implementation
	virtual void onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
	virtual void onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
	virtual void onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) override;

public:
    explicit    MapView(QWidget *parent = nullptr);

	inline bool isSelected(const unsigned x, const unsigned y) const { return _tileSelectionArray[y][x]; }
	inline bool isTerrainLayer(void) const { return getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY; }
	inline bool isEntityLayer(void) const { return getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY; }

    //QGraphicsView virtual implementation
    virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void timerEvent(QTimerEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void scrollContentsBy(int dx, int dy) override;

    //DestView virtual implementation
    virtual void createSelectionItemGroup(void) override;
    virtual void updateSelectionItemGroup(void) override;
    virtual void deleteSelectionItemGroup(void) override;

public Q_SLOTS:

    void onEngineInitialize(void);
    void onEngineUpdated(void);
	void onAreaChange(Area* const area);
	void onLayerChange(LAYER_TYPE type);
    void onMapChange(Area*const area, const QVector<MapChange>& changes);
};

#endif
