
#ifndef AREA_H
#define AREA_H

#include <stdint.h>
#include <QUndoStack>
#include <gyro.h>
#include "elysian_asset_typedefs.hpp"
#include "elysian_area.hpp"
#include "level.h"
#include "treenode.h"
#include "selectionScene.h"
#include "assets.h"
#include "propertyStackModel.h"
#include "assetManager.h"
#include "mapEntry.h"

#define AREA_SAVE_MAP_TILE_0_REEST_CHECK  true

class QPainter;
class QPoint;

class TkEntity;
typedef TkEntity Entity;
class Component;
class TkLevel;
typedef TkLevel Level;
class SourceView;
class CheckeredAlphaRectGraphicsItem;
class MapViewDepthBufferGraphicsItem;

struct MapChange {
    QPoint              loc;
    TileEntry           oldEntry;
    TileEntry           newEntry;
    Assets::LAYER_TYPE  layer;
};


ELYSIAN_ASSET_TYPEDEF(Area)

class TkArea: public PropertyStackModel, public TreeNode, public elysian::AreaBase {
    Q_OBJECT
	friend class Assets;
private:
	//Scene Data
    static unsigned                         _currentSceneWidth;
    static unsigned                         _currentSceneHeight;
    static MapViewDepthBufferGraphicsItem*  _depthBuffGraphicsItem;
    static SelectionScene*                  _editorScene;
    static bool                             _inactiveTransparency;

    QVector<MapChange>  _mapChanges;
    QUndoStack          _undoStack;
    QString             _location;
    QGraphicsRectItem*  _bgRect;
    bool                _active         = false;
    bool                _open           = false;
    bool                _emitMapChange  = true;
    bool                _batchMapUpdate = false;

    void         connectSlots(Entity *const entity);


private slots:
	void onComponentAttached(Entity *const entity, Component *const comp);
	void onComponentRemoved(Entity *const entity, Component *const comp);

public slots:
	void onUndoCleanStateChanged(bool changed);
public:

    using AreaBase::getTileEntry;
    using AreaBase::setTileEntry;
    using AreaBase::load;

                        TkArea(Level *const level=nullptr);
    virtual             ~TkArea(void) override;

    virtual bool activate(void);
    virtual bool deactivate(void);

    bool                load(const char*const str);
    bool                save(void);

    QUndoStack&         getUndoStack(void);
    elysian::Area&      getElysianArea(void);
    void                setNodeName(const QString str);
    void                setLocation(const QString loc);
    void                setString(const QString str);
    QString             getString(void) const;
    void                setMapWidth(const unsigned short w);
    void                setMapHeight(const unsigned short h);
    int                 getUndoStackCount(void);
    bool                saveAs(const QString dir);
    bool                duplicate(void);

    QString             getLocation(void) const;
    unsigned short      getMapWidth(void) const;
    unsigned short      getMapHeight(void) const;

    uint32_t            getTileValue(const QPoint loc, const Assets::LAYER_TYPE layer) const;
    void                setTileValue(const QPoint loc, const Assets::LAYER_TYPE layer, const uint32_t value);
    uint16_t            getTileIndex(const QPoint loc, const Assets::LAYER_TYPE layer) const;
    GY_RECT_TEX_ORIENT  getTileOrient(const QPoint loc, const Assets::LAYER_TYPE layer) const;
    unsigned char       getTileElev(const QPoint loc, const Assets::LAYER_TYPE layer) const;
    void                setTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer, const TileEntry entry);
    TileEntry           getTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer) const;
    void                beginMapUpdate(void);
    void                endMapUpdate(void);
    void                setEmitMapChange(const bool value);


    Entity*             newEntity(void);
    Entity*             getFirstEntity(void) const;
    bool                deleteEntity(QString name);
    bool                deleteEntity(Entity *const entity);
    std::vector<Entity*>& getEntities(void);

    static bool         staticConstructor(const SourceView *const view);
    static void         resizeEditorScene(void);
    static void         updateScene(void);
    static bool         getInactiveTransparency(void);
    static void         setInactiveTransparency(const bool val);
    static SelectionScene* getEditorScene(void);

    virtual void        populateModel(void);

    bool                open(void);
    bool                close(void);
    bool                isOpen(void) const;
    bool                isActive(void) const;

    virtual Selectable::TYPE    getType(void) const { return Selectable::AREA; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_AREA; }

signals:
	void activating(Area *const area);
	void entityAdded(Area *const area, Entity *const entity);
	void entityRemoved(Area *const area, Entity *const entity);
	void componentAttached(Area *const area, Entity *const entity, Component *const comp);
	void componentRemoved(Area *const area, Entity *const entity, Component *const comp);
	void renamed(const QString oldName, const QString newName);
    void mapChanged(Area*const area, const QVector<MapChange>& changes);
};

typedef TkArea Area;




//======== INLINEZ ========

inline uint32_t TkArea::getTileValue(const QPoint loc, const Assets::LAYER_TYPE layer) const {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
    return getTileEntry((unsigned)layer, loc.x(), loc.y()).getValue();
}

inline void TkArea::setTileValue(const QPoint loc, const Assets::LAYER_TYPE layer, const uint32_t value) {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
    MapChange change;

    if(_emitMapChange) {
        change.oldEntry = getTileEntry(loc.x(), loc.y(), layer);
        change.newEntry = TileEntry(value);
        change.loc      = loc;
        change.layer    = layer;
        _mapChanges.push_back(change);

        if(!_batchMapUpdate) {
            emit mapChanged(this, _mapChanges);
            _mapChanges.clear();
        }
    }

    setTileEntry((unsigned)layer, loc.x(), loc.y(), TileEntry(value));
}

inline void TkArea::setTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer, const TileEntry entry) {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
    MapChange change;

    if(_emitMapChange) {
        change.oldEntry = getTileEntry(loc.x(), loc.y(), layer);
        change.newEntry = entry;
        change.loc      = loc;
        change.layer    = layer;
        _mapChanges.push_back(change);

        if(!_batchMapUpdate) {
            emit mapChanged(this, _mapChanges);
            _mapChanges.clear();
        }
    }

    setTileEntry((unsigned)layer, loc.x(), loc.y(), entry);


}

inline TileEntry TkArea::getTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer) const {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
    return getTileEntry((unsigned)layer, loc.x(), loc.y());
}

inline uint16_t TkArea::getTileIndex(const QPoint loc, const Assets::LAYER_TYPE layer) const { return getTileEntry(loc, layer).getIndex(); }
inline GY_RECT_TEX_ORIENT TkArea::getTileOrient(const QPoint loc, const Assets::LAYER_TYPE layer) const { return getTileEntry(loc, layer).getOrient(); }
inline unsigned char TkArea::getTileElev(const QPoint loc, const Assets::LAYER_TYPE layer) const { return getTileEntry(loc, layer).getElev(); }
inline unsigned short TkArea::getMapHeight(void) const { return height; }
inline unsigned short TkArea::getMapWidth(void) const { return width; }
inline std::vector<Entity*>& TkArea::getEntities(void) { return entities; }
inline bool TkArea::isOpen(void) const { return _open; }
inline bool TkArea::isActive(void) const { return _active; }
inline void TkArea::setEmitMapChange(const bool value) { _emitMapChange = value; }
inline void TkArea::beginMapUpdate(void) { _batchMapUpdate = true; }
inline void TkArea::endMapUpdate(void) {
    _batchMapUpdate = false;
    if(_emitMapChange) {
        emit mapChanged(this, _mapChanges);
        _mapChanges.clear();
    }
}
inline bool TkArea::getInactiveTransparency(void) { return _inactiveTransparency; }
inline void TkArea::setInactiveTransparency(const bool val) { _inactiveTransparency = val; }
inline SelectionScene* TkArea::getEditorScene(void) { return _editorScene; }


#endif // AREA_H
