#ifndef ASSETS_H
#define ASSETS_H

#include <QObject>
#include <QString>
#include <level/elysian_level_manager.hpp>
#include <ESToolkit/core/tk_selectable.h>

namespace elysian {
    class Engine;
}

class TkLevel;
typedef TkLevel Level;
class Project;
class TkArea;
typedef TkArea Area;
class TkComponent;
typedef TkComponent Component;
class TkEntity;
typedef TkEntity Entity;
struct MapChange;

class Assets: public QObject, public elysian::LevelManager {
    Q_OBJECT
public:
    enum LAYER_TYPE { LAYER_TILE1, LAYER_TILE2, LAYER_TILE3, LAYER_LIGHT, LAYER_COLLISION, LAYER_ENTITY, LAYER_MAX };

                        Assets(void);
    virtual             ~Assets(void);

    Selectable*         getCurrentSelection(void) const;
    Project*            getCurrentProject(void) const;
    LAYER_TYPE          getCurrentLayer(void) const;

    Selectable::TYPE    getCurrentSelectionType(void) const;

    void                setCurrentSelection(Selectable *const selection);
    bool                setCurrentArea(const QString path);
    void                setCurrentArea(Area *const area);
    bool                setCurrentLevel(const QString path);
    void                setCurrentLevel(Level *const level);
    bool                setCurrentProject(const QString path);
    void                setCurrentProject(Project *const project);
    void                setCurrentLayer(const LAYER_TYPE type);
    void                setLayerVisible(const LAYER_TYPE type, bool flag=true);
    bool                getLayerVisible(const LAYER_TYPE type) const;

    Level*              reloadLevel(Level* level=nullptr);
    Area*               reloadArea(Area* area=nullptr);
    Project*            reloadProject(void);

    Level*              findLevel(const QString path) const;
    Area*               findArea(const QString path) const;
    Level*              loadLevel(const QString path);
    Area*               loadArea(const QString path);

    Project*            getFirstProject(void) const;
    Project*            getProject(const QString name) const;
    Project*            loadProject(const QString directory);
    bool                saveProject(const QString name) const;
    bool                removeProject(const QString name);

    elysian::Engine*    getEngine(void) const;

    QString             getProjectDir(void);
    int                 getUndoStackCount(void);

    static QString      formatFileName(const QString name);
    static bool         validateProjectPath(const QString path);

Q_SIGNALS:

    void                selectionChanged(const Selectable::TYPE type, Selectable *const selection);
    void                layerChanged(LAYER_TYPE type);
    void                areaChanged(Area *const area);
    void                areaChanging(Area* const area);
    void                levelChanged(Level *const level);
    void                levelChanging(Level* const level);
    void                projectChanged(Project *const project);
    void                engineInitialize(void);
    void                engineUpdated(void);

    void                areaAdded(Project *const project, Level *const level, Area *const area);
    void                areaRenamed(const QString oldName, const QString newName);
    void                levelAdded(Project *const project, Level *const level);
    void                levelRenamed(const QString oldName, const QString newName);
    void                projectAdded(Project *const project);
    void                entityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void                inactiveEntityAdded(Project*const project, Level*const level, Area*const area, Entity*const entity);
    void                inactiveEntityRemoved(Project*const project, Level*const level, Area*const area, Entity*const entity);

    void                areaRemoved(Project *const project, Level *const level, Area *const area);
    void                levelRemoved(Project *const project, Level *const level);
    void                projectRemoved(Project *const project);
    void                entityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void                componentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void                componentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void                mapChanged(Area*const area, const QVector<MapChange>& changes);
    void                entityParented(Entity*const child, Entity*const parent);

    void                projectRemoveUI(void);

private Q_SLOTS:
    void                onChildProjectActivating(Project *const project, Level *const level, Area *const area);
    void                onAreaAdded(Project *const project, Level *const level, Area *const area);
    void                onAreaRenamed(const QString oldName, const QString newName);
    void                onAreaRemoved(Project *const project, Level *const level, Area *const area);
    void                onLevelAdded(Project *const project, Level *const level);
    void                onLevelRemoved(Project *const project, Level *const level);
    void                onLevelRenamed(const QString oldName, const QString newName);
    void                onEntityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void                onEntityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void                onComponentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void                onComponentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void                onMapChanged(Area*const area, const QVector<MapChange>& changes);
    void                onEntityParented(Entity*const child, Entity*const parent);

    //for when they've been changed outside of this API
    void                onAreaChanged(Area* area);
    void                onLevelChanged(Level* level);

private:
    void                connectSlots(Project *const project);

    static Project*             _project;
    static elysian::Engine*     _engine;

    //Current state
    static Selectable*          _currentSelection;
    static Assets::LAYER_TYPE   _currentLayer;
    static bool                 _layerVisible[Assets::LAYER_MAX];
    static Selectable::TYPE     _currentSelectionType;
    static int                  _areaChangeDepth;
    static int                  _levelChangeDepth;


};





// ==== INLINEZ=======

inline Selectable* Assets::getCurrentSelection(void) const { return _currentSelection; }
inline Project* Assets::getCurrentProject(void) const { return _project; }
inline auto Assets::getCurrentLayer(void) const -> LAYER_TYPE { return _currentLayer; }
inline Selectable::TYPE Assets::getCurrentSelectionType(void) const { return _currentSelectionType; }
inline elysian::Engine* Assets::getEngine(void) const { return _engine; }
inline Project* Assets::getFirstProject(void) const { return _project; }



#endif // ASSETMANAGER_H
