#ifndef TK_SEVERITY_H
#define TK_SEVERITY_H
//QT
#include <QString>
#include <QVector>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <ESToolkit/ui/item/tk_tree_node.h>
#include <ESGamma/system/elysian_system.hpp>
#include <AssetIO/level/elysian_level_base.hpp>
#include <ESToolkit/AssetIO/tk_elysian_tile_wrapper.h>

#define ESTK_SEVERITY_METADATA_SUBDIR          "estk"
#define ESTK_SEVERITY_AREA_SUBDIR              "area"
#define ESTK_SEVERITY_TILE_SUBDIR              "tile"
#define ESTK_SEVERITY_METADATA_TILESET         "estkTileset.bin"

#define ESTK_SEVERITY_NORMAL_MAPS_DIRECTORY    "normalMaps"
#define ESTK_SEVERITY_SPECULAR_MAPS_DIRECTORY  "specularMaps"

class QGraphicsRectItem;

class Project;
class TileImage;
class SheetViewTileGraphicsItem;
class AssetManager;
class TkEntity;
typedef TkEntity Entity;
class TkComponent;
typedef TkComponent Component;
class SelectionScene;
class SourceView;
class CheckeredAlphaRectGraphicsItem;
class TkArea;
typedef TkArea Area;
struct MapChange;

class TkLevel: public PropertyStackModel, public elysian::LevelBase, public TreeNode, public AssetManager, public elysian::System {
    friend class Assets;
    friend class TileManagerDialog;
    friend class Project;
    friend class TkArea;

    Q_OBJECT
private:
    ElysianTileWrapper _elysianTileWrappers[ELYSIAN_TILESHEET_TILES];
    Project *_project;
    QVector<TileImage *> _tileSheet;
    QVector<Area *> _area;
    QString _location;
    static Project* _curProj;
    bool _active, _open, _validSheets;

    bool _normalMapEnabled[ELYSIAN_TILESHEET_TILES] = { false };
    bool _specularMapEnabled[ELYSIAN_TILESHEET_TILES] = { false };

    bool _saveMetaData(const QString dir);
    bool _loadMetaData(const QString dir);

    void connectSlots(Area *area);

    bool _gatherMaps(const bool normal=true);
    bool _scatterMaps(const bool normal=true);

public:
    virtual bool deactivate(void);
    Project *getProject(void) const { return _project; }
    void setNodeName(const QString str);
    void setLocation(const QString loc);
    QString getLocation(void) const;
    QString getString(void) const;
    void setString(const QString str);
    virtual bool activate(void);
    bool duplicate(void);

    static bool initializeScene(const SourceView *const view, bool horiz=true);
    static void addBgRectItem(void);
    static void updateScenes(void);

    //SheetView static data
    static QGraphicsRectItem *_sheetViewTileParent;
    static CheckeredAlphaRectGraphicsItem *_bgRectItem;
    static SelectionScene *_sheetViewScene;

    TkLevel(Project *const project=nullptr);
    virtual ~TkLevel(void);
    unsigned int getAreaAmount(void) const;
    inline unsigned char getTileAmount(void) const {  return _tileSheet.size(); }

    inline TileImage *getTileImage(const uint16_t index) const {
        return (index < _tileSheet.size())? _tileSheet[index] : NULL;
    }

    Area* newArea(QString name=QString("new_area"));
    Area* getArea(const QString path);
    Area* getAreaByName(const QString name);
    Area* getAreaByIndex(const unsigned int index);
    Area* loadArea(const QString path);
    Area* reloadArea(Area* const area);
    bool saveArea(const QString path) const;
    bool removeArea(const QString path, const bool deleteDir=true);
    Area *getFirstArea() const;
    elysian::LevelBase* getElysianLevel(void) { return (elysian::LevelBase*)this; }
    elysian::Tile *getElysianTile(const uint16_t index);
    ElysianTileWrapper* getElysianTileWrapper(const uint16_t index);
    void setNodePath(const QString path);

    void setSpecularMapEnabled(const unsigned index, const bool val);
    void setNormalMapEnabled(const unsigned index, const bool val);
    bool getSpecularMapEnabled(const unsigned index) const;
    bool getNormalMapEnabled(const unsigned index) const;

    bool load(const QString path);
    bool save(void);
    bool saveAll(void);
    bool saveAs(const QString path);

    Area *importArea(const QString path);

    bool reloadSheets(void);
    bool reload(void);
    bool validSheets(void) const;
    bool isActive(void) const;
    bool isOpen(void) const;

    bool open(void);
    bool close(void);

    virtual void populateModel(void);
    virtual QAbstractItemModel* getModel(void) const override;
    bool generateTileSheet(const QString location);

    bool scatterTileNormalMaps(void);
    bool gatherTileNormalMaps(void);
    bool scatterTileSpecularMaps(void);
    bool gatherTileSpecularMaps(void);

    bool saveTileSheet(void);
    void clearAllStacks(void);

    virtual Selectable::TYPE getType(void) const { return Selectable::LEVEL; }
    virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_LEVEL; }

private Q_SLOTS:
    bool activateByArea(Area * const area);
    void onEntityAdded(Area *const area, Entity *const entity);
    void onEntityRemoved(Area *const area, Entity *const entity);
    void onComponentAttached(Area *const area, Entity *const entity, Component *const comp);
    void onComponentRemoved(Area *const area, Entity *const entity, Component *const comp);
    void onAreaRenamed(const QString oldName, const QString newName);
    void onMapChanged(Area*const area, const QVector<MapChange>& changes);
    void onEntityParented(Entity*const child, Entity*const parent);

Q_SIGNALS:
	void activating(Level *const level, Area *const area);
	void areaAdded(Level *const level, Area *const area);
	void areaRemoved(Level *const level, Area *const area);
	void entityAdded(Level *const level, Area *const area, Entity *const entity);
	void entityRemoved(Level *const level, Area *const area, Entity *const entity);
    void inactiveEntityAdded(Level*const level, Area*const area, Entity*const entity);
    void inactiveEntityRemoved(Level*const level, Area*const area, Entity*const entity);
	void componentAttached(Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void componentRemoved(Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void areaRenamed(const QString oldName, const QString newName);
	void levelRenamed(const QString oldName, const QString newName);
    void mapChanged(Area*const area, const QVector<MapChange>& changes);
    void entityParented(Entity*const child, Entity*const parent);
    void validChanged(const bool val);

};

typedef TkLevel Level;



//======= INLINEZ ======

inline void TkLevel::setSpecularMapEnabled(const unsigned index, const bool val) { _specularMapEnabled[index] = val; }
inline void TkLevel::setNormalMapEnabled(const unsigned index, const bool val) { _normalMapEnabled[index] = val; }
inline bool TkLevel::getSpecularMapEnabled(const unsigned index) const { return _specularMapEnabled[index]; }
inline bool TkLevel::getNormalMapEnabled(const unsigned index) const { return _normalMapEnabled[index]; }
inline ElysianTileWrapper* TkLevel::getElysianTileWrapper(const uint16_t index) { return &_elysianTileWrappers[index]; }


#endif // SEVERITY_H
