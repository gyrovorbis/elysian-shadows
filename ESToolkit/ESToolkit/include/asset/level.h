#ifndef LEVEL_H
#define LEVEL_H
//QT
#include <QString>
#include <QVector>
#include "propertyStackModel.h"
#include "treenode.h"
#include "elysian_level.hpp"
#include "elysian_level_manager.hpp"
#include "assets.h"

class QGraphicsRectItem;

class Project;
class TileImage;
class SheetViewTileGraphicsItem;
class Area;
class AssetManager;
class Entity;
class Component;
class SelectionScene;
class SourceView;
class CheckeredAlphaRectGraphicsItem;

class Level: public PropertyStackModel, public TreeNode, public elysian::LevelManager {
    friend class Assets;
	friend class TileManagerDialog;

    Q_OBJECT
private:
	Project *_project;
	QVector<TileImage *> _tileSheet;
	QVector<Area *> _area;
    QString _location;
    bool _active, _open, _validSheets;
    elysian::Level _elysianLevel;

	virtual bool deactivate(void);
	void connectSlots(Area *area);
public:
	Project *getProject(void) const { return _project; }
	void setName(const QString str);
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

	Level(Project *const project);
    ~Level();
	unsigned int getAreaAmount(void) const;
	inline unsigned char getTileAmount(void) const {  return _tileSheet.size(); }

	inline TileImage *getTileImage(const uint16_t index) const {
		return (index < _tileSheet.size())? _tileSheet[index] : NULL;
	}

	Area *newArea(void);
	Area *newArea(QString name);
    Area *getArea(const QString name);
    Area *getAreaByIndex(const unsigned int index);
	Area *loadArea(const QString name);
	bool saveArea(const QString name) const;
	bool removeArea(const QString name);
    Area *getFirstArea() const;
	elysian::Level *getElysianLevel(void) { return &_elysianLevel; }
	elysian::Tile *getElysianTile(const uint16_t index);

	bool load(const QString loc);
	bool save(void);
	bool saveAll(void);
	bool saveAs(const QString loc);

	Area *importArea(const QString dir);

	bool reloadSheets(void);
	bool validSheets(void) const;
	bool isActive(void) const;
	bool isOpen(void) const;

	bool open(void);
	bool close(void);

	virtual void populateModel(void);
	bool generateTileSheet(const QString location);
	bool saveTileSheet(void);
	void clearAllStacks(void);

	virtual Selectable::TYPE getType(void) const { return Selectable::LEVEL; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_LEVEL; }

private slots:
	bool activateByArea(Area * const area);
	void onEntityAdded(Area *const area, Entity *const entity);
	void onEntityRemoved(Area *const area, Entity *const entity);
	void onComponentAttached(Area *const area, Entity *const entity, Component *const comp);
	void onComponentRemoved(Area *const area, Entity *const entity, Component *const comp);
	void onAreaRenamed(const QString oldName, const QString newName);

signals:
	void activating(Level *const level, Area *const area);
	void areaAdded(Level *const level, Area *const area);
	void areaRemoved(Level *const level, Area *const area);
	void entityAdded(Level *const level, Area *const area, Entity *const entity);
	void entityRemoved(Level *const level, Area *const area, Entity *const entity);
	void componentAttached(Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void componentRemoved(Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void areaRenamed(const QString oldName, const QString newName);

};


#endif // LEVEL_H
