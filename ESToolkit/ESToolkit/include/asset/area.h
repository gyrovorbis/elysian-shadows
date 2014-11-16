
#ifndef AREA_H
#define AREA_H

#include <stdint.h>
#include <QUndoStack>
#include <gyro.h>
#include "elysian_asset_typedefs.hpp"
#include "elysian_area.hpp"
#include "elysian_level_manager.hpp"
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
class TkArea;
typedef TkArea Area;

class TkArea: public PropertyStackModel, public TreeNode, public elysian::AreaBase {
    Q_OBJECT
	friend class Assets;
private:
	//Scene Data
    static unsigned int _currentSceneWidth, _currentSceneHeight;
	static MapViewDepthBufferGraphicsItem	*_depthBuffGraphicsItem;

	QUndoStack _undoStack;
    //QVector<Entity *> _entities;
	QString _location;
	QGraphicsRectItem *_bgRect;
	Level *_level;
	bool _active, _open;

	virtual bool activate(void);
	virtual bool deactivate(void);
	void connectSlots(Entity *const entity);


private slots:
	void onComponentAttached(Entity *const entity, Component *const comp);
	void onComponentRemoved(Entity *const entity, Component *const comp);

public slots:
	void onUndoCleanStateChanged(bool changed);
public:
    using AreaBase::getTileEntry;
    using AreaBase::setTileEntry;
    using AreaBase::load;
    using AreaBase::save;

	static bool inactiveTransparency;

    TkArea(Level *const level=nullptr);
    virtual ~TkArea(void) override;

	bool load(const QString dir);
	bool save(void);

    Level* getLevel(void) const { return _level; }
    QUndoStack& getUndoStack(void);
	elysian::Area &getElysianArea(void);
    void setNodeName(const QString str);
    void setLocation(const QString loc);
	void setString(const QString str);
	QString getString(void) const;
    void setMapWidth(const unsigned short w);
    void setMapHeight(const unsigned short h);
	int getUndoStackCount(void);
	bool saveAs(const QString dir);
    bool duplicate(void);

	QString getLocation(void) const;
    unsigned short getMapWidth(void) const;
    unsigned short getMapHeight(void) const;
	uint32_t getTileValue(const QPoint loc, const Assets::LAYER_TYPE layer) const;
	void setTileValue(const QPoint loc, const Assets::LAYER_TYPE layer, const uint32_t value);
	uint16_t getTileIndex(const QPoint loc, const Assets::LAYER_TYPE layer) const;
	GY_RECT_TEX_ORIENT getTileOrient(const QPoint loc, const Assets::LAYER_TYPE layer) const;
	unsigned char getTileElev(const QPoint loc, const Assets::LAYER_TYPE layer) const;
    void setTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer, const TileEntry entry);
    TileEntry getTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer) const;

	Entity *newEntity(void);
	Entity *getFirstEntity(void) const;
	bool deleteEntity(QString name);
	bool deleteEntity(Entity *const entity);
    std::vector<Entity *>& getEntities(void);

	static SelectionScene *editorScene;

	static bool staticConstructor(const SourceView *const view);
	static void resizeEditorScene(void);
	static void updateScene(void);

	virtual void populateModel(void);

	bool open(void);
	bool close(void);
	bool isOpen(void) const;
	bool isActive(void) const;

	virtual Selectable::TYPE getType(void) const { return Selectable::AREA; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_AREA; }

signals:
	void activating(Area *const area);
	void entityAdded(Area *const area, Entity *const entity);
	void entityRemoved(Area *const area, Entity *const entity);
	void componentAttached(Area *const area, Entity *const entity, Component *const comp);
	void componentRemoved(Area *const area, Entity *const entity, Component *const comp);
	void renamed(const QString oldName, const QString newName);
};



//======== INLINEZ ========

inline uint32_t Area::getTileValue(const QPoint loc, const Assets::LAYER_TYPE layer) const {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
	switch(layer) {
	case Assets::LAYER_TILE1:
        return getL1TileEntry(loc.x(), loc.y()).getValue();	case Assets::LAYER_TILE2:
        return getL2TileEntry(loc.x(), loc.y()).getValue();
	case Assets::LAYER_TILE3:
        return getL3TileEntry(loc.x(), loc.y()).getValue();
	case Assets::LAYER_LIGHT:
//		return _elysianArea.getLightEntry(loc.x(), loc.y()).getValue();
	default:
		break;
	}
	return 0;
}

inline void Area::setTileValue(const QPoint loc, const Assets::LAYER_TYPE layer, const uint32_t value) {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
	switch(layer) {
	case Assets::LAYER_TILE1:
        setL1TileEntry(loc.x(), loc.y(), elysian::TileEntry(value));
		break;
	case Assets::LAYER_TILE2:
        setL2TileEntry(loc.x(), loc.y(), elysian::TileEntry(value));
		break;
	case Assets::LAYER_TILE3:
        setL3TileEntry(loc.x(), loc.y(), elysian::TileEntry(value));
		break;
	case Assets::LAYER_LIGHT:
        //_elysianArea.setLightEntry(loc.x(), loc.y(), elysian::LightEntry(value));
		break;
	default:
		break;
	}
}

inline void Area::setTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer, const TileEntry entry) {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
	switch(layer) {
	case Assets::LAYER_TILE1:
        setL1TileEntry(loc.x(), loc.y(), entry);
		break;
	case Assets::LAYER_TILE2:
        setL2TileEntry(loc.x(), loc.y(), entry);
		break;
	case Assets::LAYER_TILE3:
        setL3TileEntry(loc.x(), loc.y(), entry);
		break;
	default:
		break;
	}
}

inline TileEntry Area::getTileEntry(const QPoint loc, const Assets::LAYER_TYPE layer) const {
	Q_ASSERT(loc.x() < ELYSIAN_AREA_MAP_WIDTH && loc.y() < ELYSIAN_AREA_MAP_HEIGHT);
	switch(layer) {
	case Assets::LAYER_TILE1:
        return TileEntry(getL1TileEntry(loc.x(), loc.y()).getValue());
		break;
	case Assets::LAYER_TILE2:
        return TileEntry(getL2TileEntry(loc.x(), loc.y()).getValue());
		break;
	case Assets::LAYER_TILE3:
        return TileEntry(getL3TileEntry(loc.x(), loc.y()).getValue());
		break;
	default:
		break;
	}
	return 0;
}

inline uint16_t Area::getTileIndex(const QPoint loc, const Assets::LAYER_TYPE layer) const { return getTileEntry(loc, layer).getIndex(); }
inline GY_RECT_TEX_ORIENT Area::getTileOrient(const QPoint loc, const Assets::LAYER_TYPE layer) const { return getTileEntry(loc, layer).getOrient(); }
inline unsigned char Area::getTileElev(const QPoint loc, const Assets::LAYER_TYPE layer) const { return getTileEntry(loc, layer).getElev(); }
inline unsigned short Area::getMapHeight(void) const { return height; }
inline unsigned short Area::getMapWidth(void) const { return width; }
inline std::vector<Entity*>& Area::getEntities(void) { return entities; }
inline bool Area::isOpen(void) const { return _open; }
inline bool Area::isActive(void) const { return _active; }


#endif // AREA_H
