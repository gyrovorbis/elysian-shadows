#ifndef TK_AREA_H
#define TK_AREA_H

#include <stdint.h>
#include <QUndoStack>
#include <level/elysian_area_base.hpp>
#include <ESToolkit/ui/item/tk_tree_node.h>
#include <ESToolkit/core/tk_asset_manager.h>
#include <AssetIO/tk_map_entry.h>

#define AREA_SAVE_MAP_TILE_0_REEST_CHECK    true
#define TK_AREA_BACKUP_COUNT                5

class QPainter;
class QPoint;

namespace elysian {
    class CameraKeyFrame;
}

class TkEntity;
typedef TkEntity Entity;
class TkComponent;
typedef TkComponent Component;
class TkLevel;
typedef TkLevel Level;
class SourceView;
class CheckeredAlphaRectGraphicsItem;
class MapViewDepthBufferGraphicsItem;
class SelectionScene;
class LuaTableStackModel;

struct MapChange {
    QPoint                  loc;
    elysian::TileEntry      oldEntry;
    elysian::TileEntry      newEntry;
    Assets::LAYER_TYPE      layer;
};


//ELYSIAN_ASSET_TYPEDEF(Area)

class TkArea: /*public PropertyStackModel,*/ public elysian::AreaBase, public TreeNode {
    Q_OBJECT
    friend class Assets;

ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_FIELD(dirName, getDirName)
    ELYSIAN_LUA_PROXY_FIELD(startPosition, getStartPosition, setStartPosition)
   // ELYSIAN_LUA_PROXY_FIELD(name, )
    //ELYSIAN_LUA_PROXY_FIELD(path, )
ELYSIAN_LUA_PROXY_FIELDS_END()


private:
    //Scene Data
    static unsigned                         _currentSceneWidth;
    static unsigned                         _currentSceneHeight;
    static MapViewDepthBufferGraphicsItem*  _depthBuffGraphicsItem;
    static SelectionScene*                  _editorScene;
    static bool                             _inactiveTransparency;

    QVector<MapChange>  _mapChanges;
    elysian::Vector3    _defaultStartPosition;
    QUndoStack          _undoStack;
    QString             _location;
    mutable
    LuaTableStackModel* _luaModel       = nullptr;
    bool                _active         = false;
    bool                _open           = false;
    bool                _emitMapChange  = true;
    bool                _batchMapUpdate = false;
    elysian::CameraKeyFrame*
                        _cameraKeyFrame = nullptr;
    bool                _bgmTracksValid[ELYSIAN_AREA_BGM_TRACKS] = { false };
    bool                _auxTracksValid[ELYSIAN_AREA_AUX_TRACKS] = { false };

    void                _connectSlots(Entity *const entity);
    void                _disconnectSlots(Entity*const entity);

private Q_SLOTS:
    void                onComponentAttached(Entity *const entity, Component *const comp);
    void                onComponentRemoved(Entity *const entity, Component *const comp);
    void                onEntityParented(Entity*const child, Entity*const parent);

public Q_SLOTS:
    void                onUndoCleanStateChanged(bool changed);
public:

    using               AreaBase::getActiveEntityByIndex;
    using               AreaBase::getActiveEntityCount;
    using               AreaBase::setActiveEntityByIndex;
    using               AreaBase::getTileEntry;
    using               AreaBase::setTileEntry;
    using               AreaBase::load;

                        TkArea(Level *const level=nullptr);
    virtual             ~TkArea(void) override;

    //elysian::EntityBase overrides
    void                addEntity(Entity* entity);
    void                addInactiveEntity(Entity* entity);
    void                removeEntity(Entity* entity);

    virtual bool        activate(void);
    virtual bool        deactivate(void);

    bool                load(const char*const str);
    bool                save(void);
    bool                saveBackup(void);
    bool                duplicate(void);

    QUndoStack&         getUndoStack(void);
    elysian::Area&      getElysianArea(void);
    void                setNodeName(const QString str);
    void                setLocation(const QString loc);
    void                setNodePath(const QString path);
    void                setString(const QString str);
    QString             getString(void) const;
    void                setMapWidth(const unsigned short w);
    void                setMapHeight(const unsigned short h);
    int                 getUndoStackCount(void);
    bool                saveAs(const QString dir);
    auto                getCameraKeyFrame(void) const -> const elysian::CameraKeyFrame&;
    void                setCameraKeyFrame(const elysian::CameraKeyFrame& keyframe);
    elysian::Vector3&   getStartPosition(void);
    void                setStartPosition(const elysian::Vector3& pos);

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

    void                setBgmOggByTrack(const unsigned index, const char* path);
    void                setAuxOggByTrack(const unsigned index, const char* path);
    bool                isBgmOggTrackValid(const unsigned index) const;
    bool                isAuxOggTrackValid(const unsigned index) const;


    Entity*             newEntity(void);
    Entity*             getFirstEntity(void) const;
    bool                deleteEntity(QString name);
    bool                deleteEntity(Entity *const entity);
    std::vector<Entity*>& getEntities(void);
    std::vector<Entity*>& getInactiveEntities(void);

    static bool         staticConstructor(const SourceView *const view);
    static void         resizeEditorScene(void);
    static void         updateScene(void);
    static bool         getInactiveTransparency(void);
    static void         setInactiveTransparency(const bool val);
    static SelectionScene* getEditorScene(void);

    //virtual void                populateModel(void) override;
    virtual QAbstractItemModel* getModel(void) const override;

    static void         initializeDelegates(void);
    static void         initializeVirtualTableDelegates(QString key);

    bool                open(void);
    bool                close(void);
    bool                isOpen(void) const;
    bool                isActive(void) const;

    virtual auto        getType(void) const -> TYPE override;
    virtual auto        getTreeWidgetItemType(void) -> ITEM_TYPE;

Q_SIGNALS:
    void                activating(Area *const area);
    void                entityAdded(Area *const area, Entity *const entity);
    void                entityRemoved(Area *const area, Entity *const entity);

    void                componentAttached(Area *const area, Entity *const entity, Component *const comp);
    void                componentRemoved(Area *const area, Entity *const entity, Component *const comp);
    void                renamed(const QString oldName, const QString newName);
    void                mapChanged(Area*const area, const QVector<MapChange>& changes);
    void                entityParented(Entity*const child, Entity*const parent);
    void                validChanged(const bool val);
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
            Q_EMIT mapChanged(this, _mapChanges);
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
            Q_EMIT mapChanged(this, _mapChanges);
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
inline std::vector<Entity*>& TkArea::getInactiveEntities(void) { return inactiveEntities; }
inline bool TkArea::isOpen(void) const { return _open; }
inline bool TkArea::isActive(void) const { return _active; }
inline void TkArea::setEmitMapChange(const bool value) { _emitMapChange = value; }
inline void TkArea::beginMapUpdate(void) { _batchMapUpdate = true; }
inline void TkArea::endMapUpdate(void) {
    _batchMapUpdate = false;
    if(_emitMapChange) {
        Q_EMIT mapChanged(this, _mapChanges);
        _mapChanges.clear();
    }
}
inline bool TkArea::getInactiveTransparency(void) { return _inactiveTransparency; }
inline void TkArea::setInactiveTransparency(const bool val) { _inactiveTransparency = val; }
inline SelectionScene* TkArea::getEditorScene(void) { return _editorScene; }

inline auto TkArea::getType(void) const -> TYPE { return Selectable::AREA; }
inline auto TkArea::getTreeWidgetItemType(void) -> ITEM_TYPE { return ITEM_AREA; }

inline elysian::Vector3& TkArea::getStartPosition(void) { return _defaultStartPosition; }
inline void TkArea::setStartPosition(const elysian::Vector3& pos) { _defaultStartPosition = pos; }

inline bool TkArea::isBgmOggTrackValid(const unsigned index) const { return (index < ELYSIAN_AREA_BGM_TRACKS)? _bgmTracksValid[index] : false; }
inline bool TkArea::isAuxOggTrackValid(const unsigned index) const { return (index < ELYSIAN_AREA_AUX_TRACKS)? _auxTracksValid[index] : false; }




#endif // AREA_H
