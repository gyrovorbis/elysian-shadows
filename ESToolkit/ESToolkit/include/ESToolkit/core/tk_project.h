#ifndef TK_PROJECT_H
#define TK_PROJECT_H
//QT
#include <QString>
#include <QVector>

#include <ESToolkit/ui/item/tk_tree_node.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <ESToolkit/core/tk_asset_manager.h>
#include <ESGamma/system/elysian_system.hpp>
#include <QDir>

class TkLevel;
class TkArea;
typedef TkArea Area;
class Assets;
class TkEntity;
typedef TkEntity Entity;
class TkComponent;
typedef TkComponent Component;
struct MapChange;


class Project: public PropertyStackModel, public TreeNode, public AssetManager, public elysian::System {
    Q_OBJECT
    friend class Assets;
    friend class TkLevel;
private:

    QHash<QString, TreeNode*>   _subDirs;
    QVector<TkLevel *>          _level;
    QString                     _location;
    TreeNode*                   _curNodeParent      = nullptr;
    bool                        _active             = false;
    bool                        _indexHiddenLevels  = false;


    virtual bool    deactivate(void) override;
    void            connectSlots(TkLevel *const level);

public:
                    Project(void);
                    ~Project(void); //delete all levels

    virtual void    populateModel(void) override;
    virtual auto    getModel(void) const -> QAbstractItemModel* override;

    void            setName(const QString name);
    void            setLocation(const QString loc);
    QString         getLocation(void) const;
    virtual bool    activate(void);
    bool            isActive(void);

    TkLevel*        newLevel(const QString n=QString("new_level"));
    TkLevel*        reloadLevel(TkLevel* level);
    TkLevel*        loadLevel(const QString path); //assumed to be within current project, so directory is not necessary
    bool            saveLevel(const QString path);
    bool            removeLevel(const QString path, const bool deleteFolder=true);
    unsigned int    getLevelAmount() const;
    TkLevel*        getLevel(const QString path) const;
    TkLevel*        getLevelByIndex(const unsigned int index) const;
    TkLevel*        getFirstLevel(void) const;
    TreeNode*       getTreeNode(const QString path) const;

    bool            load(const QString loc);
    bool            save(void);
    bool            scanDirectory(const QString path);
    bool            scanBackLevelPath(const QString path);
    bool            isLevelPath(const QString path);
    void            setIndexHiddenLevels(const bool val);
    bool            getIndexHiddenLevels(void) const;


    TkLevel*        importLevel(const QString dir);
    bool            saveAs(const QString dir);

    void            clearUndoStacks(void);

    virtual auto    getType(void) const -> TYPE;
    virtual auto    getTreeWidgetItemType(void) -> ITEM_TYPE;

private Q_SLOTS:
      bool          activateByLevel(Level *const level, Area *const area);
      void          onAreaAdded(Level *const level, Area *const area);
      void          onAreaRenamed(const QString oldName, const QString newName);
      void          onAreaRemoved(Level *const level, Area *const area);
      void          onLevelRenamed(const QString oldName, const QString newName);
      void          onAddEntity(Level *const level, Area *const area, Entity *const entity);
      void          onRemoveEntity(Level *const level, Area *const area, Entity *const entity);
      void          onComponentAttached(Level *const level, Area *const area, Entity *const entity, Component *const comp);
      void          onComponentRemoved(Level *const level, Area *const area, Entity *const entity, Component *const comp);
      void          onMapChanged(Area* const area, const QVector<MapChange>& changes);
      void          onEntityParented(Entity*const child, Entity*const parent);

Q_SIGNALS:
    void            activating(Project *const project, Level *const level, Area *const area);
    void            areaAdded(Project *const project, Level *const level, Area *const area);
    void            areaRenamed(const QString oldName, const QString newName);
    void            areaRemoved(Project *const project, Level *const level, Area *const area);
    void            levelAdded(Project *const project, Level *const level);
    void            levelRemoved(Project *const project, Level *const level);
    void            levelRenamed(const QString oldName, const QString newName);
    void            entityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void            entityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void            inactiveEntityAdded(Project*const project, Level*const level, Area*const area, Entity*const entity);
    void            inactiveEntityRemoved(Project*const project, Level*const level, Area*const area, Entity*const entity);
    void            componentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void            componentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void            mapChanged(Area * const area, const QVector<MapChange>& changes);
    void            entityParented(Entity*const child, Entity*const parent);
};








// ========== INLINEZ ==========
inline auto Project::getType(void) const -> TYPE { return Selectable::PROJECT; }
inline auto Project::getTreeWidgetItemType(void) ->ITEM_TYPE { return ITEM_PROJECT; }
inline bool Project::getIndexHiddenLevels(void) const { return _indexHiddenLevels; }


#endif // PROJECT_H
