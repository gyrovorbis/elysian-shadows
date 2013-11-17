#ifndef ASSETS_H
#define ASSETS_H

#include <QObject>
#include <QString>
#include "elysian_level_manager.hpp"
#include "selectable.h"

class Project;
class Level;
class Area;
class Component;
class Entity;


class Assets: public QObject, public elysian::LevelManager {
    Q_OBJECT
public:
	enum LAYER_TYPE { LAYER_TILE1, LAYER_TILE2, LAYER_TILE3, LAYER_LIGHT, LAYER_COLLISION, LAYER_ENTITY, LAYER_MAX };

    Assets();
    ~Assets();

    Selectable *getCurrentSelection(void) const { return _currentSelection; }
    Area *getCurrentArea(void) const { return _currentArea; }
    Level *getCurrentLevel(void) const { return _currentLevel; }
    Project *getCurrentProject(void) const { return m_project; }
    LAYER_TYPE getCurrentLayer(void) const { return _currentLayer; }

    Selectable::TYPE getCurrentSelectionType(void) const { return _currentSelectionType; }

    void setCurrentSelection(Selectable *const selection);

    bool setCurrentArea(const QString name);
    void setCurrentArea(Area *const area);
    bool setCurrentLevel(const QString name);
    void setCurrentLevel(Level *const level);
    bool setCurrentProject(const QString name);
    void setCurrentProject(Project *const project);
    //void setCurrentSelection(Selectable *const selection);
	void setCurrentLayer(const LAYER_TYPE type);
	void setLayerVisible(const LAYER_TYPE type, bool flag=true);
	bool getLayerVisible(const LAYER_TYPE type) const;

    //Project *NewProject();
    //Project *NewProject(QString n);
	Project *getFirstProject(void) const;
    Project *getProject(const QString name) const;
	Project *loadProject(const QString directory);
	bool saveProject(const QString name) const;
	bool removeProject(const QString name);

    //static bool VerifyDirectory(const QString dir);
	static QString formatFileName(const QString name);
	QString getProjectDir();
	int getUndoStackCount();

signals:
    //void EntityChanged(Entity *const entity);
	void selectionChanged(const Selectable::TYPE type, Selectable *const selection);
	void layerChanged(LAYER_TYPE type);
	void areaChanged(Area *const area);
	void levelChanged(Level *const level);
	void projectChanged(Project *const project);

	void areaAdded(Project *const project, Level *const level, Area *const area);
	void areaRenamed(const QString oldName, const QString newName);
	void levelAdded(Project *const project, Level *const level);
	void levelRenamed(const QString oldName, const QString newName);
	void projectAdded(Project *const project);
	void entityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity);

	void areaRemoved(Project *const project, Level *const level, Area *const area);
	void levelRemoved(Project *const project, Level *const level);
	void projectRemoved(Project *const project);
	void entityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity);
	void componentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void componentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);

	void projectRemoveUI();
private slots:
	void onChildProjectActivating(Project *const project, Level *const level, Area *const area);
	void onAreaAdded(Project *const project, Level *const level, Area *const area);
	void onAreaRenamed(const QString oldName, const QString newName);
	void onAreaRemoved(Project *const project, Level *const level, Area *const area);
	void onLevelAdded(Project *const project, Level *const level);
	void onLevelRemoved(Project *const project, Level *const level);
	void onLevelRenamed(const QString oldName, const QString newName);
	void onEntityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity);
	void onEntityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity);
	void onComponentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void onComponentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);

private:
    static Project *m_project;

    //Current state
    static Area *_currentArea;
    static Level *_currentLevel;
    static Selectable *_currentSelection;
	static Assets::LAYER_TYPE _currentLayer;
	static bool _layerVisible[Assets::LAYER_MAX];
    static Selectable::TYPE _currentSelectionType;

	void connectSlots(Project *const project);
};


#endif // ASSETMANAGER_H
