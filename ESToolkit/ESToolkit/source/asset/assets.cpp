#include <QDir>
#include <QMessageBox>
#include "assets.h"
#include "project.h"
#include "elysian_level.hpp"
#include "tkArea.h"
#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "elysian_engine.hpp"
#include "elysian_level_system.hpp"
#include "elysian_map_system.hpp"
#include "elysian_map_system.hpp"
#include "entitySelectable.h"

//====STATIC MEMBERS====

Area *Assets::_currentArea = nullptr;
Level *Assets::_currentLevel = nullptr;
Project *Assets::m_project = nullptr;
elysian::Engine* Assets::_engine = nullptr;
Selectable *Assets::_currentSelection = nullptr;
Selectable::TYPE Assets::_currentSelectionType = Selectable::EMPTY;
Assets::LAYER_TYPE Assets::_currentLayer = Assets::LAYER_TILE1;
bool Assets::_layerVisible[Assets::LAYER_MAX] = { true };

//====INSTANCE MEMBERS====

//General case
void Assets::setCurrentSelection(Selectable *const selection) {
	Selectable *const oldSelection = _currentSelection;

    if(oldSelection != selection) {

        _currentSelectionType = (!selection)? Selectable::EMPTY : selection->getType();
        _currentSelection = selection;

        if(oldSelection) {
            oldSelection->setCurrentSelection(false, selection);
        }

        if(_currentSelection) {
            _currentSelection->setCurrentSelection(true, oldSelection);
        }

        emit selectionChanged(_currentSelectionType, _currentSelection);

        //prevent fucking memory leaks!
        //Must be done after, in case shit is still using this data before change.
        if(oldSelection && oldSelection->isTemporary()) delete oldSelection;

    }
}

//Use a regular expression instead!
QString Assets::formatFileName(const QString string) {
    QString text=string.simplified();
    for(int i=0;i<text.size();i++) {
        if  ( (text.data()[i].unicode() < 48 && text.data()[i].unicode()!=32)
            || (text.data()[i].unicode() < 65 && text.data()[i].unicode() > 57)
            || (text.data()[i].unicode() < 97 && text.data()[i].unicode() > 90 && text.data()[i].unicode()!=95)
            || (text.data()[i].unicode() < 127 && text.data()[i].unicode() > 122) ) {
            text.data()[i]='X';
        } else if(text.data()[i].unicode()==32) {
            text.data()[i]='_';
        }
    }
    return text;
}

QString Assets::getProjectDir(void)
{
    if(m_project != nullptr) {
        return m_project->getLocation();
    }

    else {
        return "";
    }
}

int Assets::getUndoStackCount(void)
{
    return _currentArea->getUndoStackCount();
}


void Assets::connectSlots(Project *const project) {
	connect(project, SIGNAL(activating(Project *const, Level *const, Area *const)), this, SLOT(onChildProjectActivating(Project *const, Level *const, Area *const)));
	connect(project, SIGNAL(areaAdded(Project * const, Level *const, Area *const)), this, SLOT(onAreaAdded(Project *const, Level *const, Area *const)));
	connect(project, SIGNAL(areaRenamed(QString,QString)), this, SLOT(onAreaRenamed(QString,QString)));
	connect(project, SIGNAL(areaRemoved(Project*const,Level*const,Area*const)), this, SLOT(onAreaRemoved(Project*const,Level*const,Area*const)));
	connect(project, SIGNAL(levelAdded(Project *const, Level *const)), this, SLOT(onLevelAdded(Project *const, Level *const)));
	connect(project, SIGNAL(levelRemoved(Project*const,Level*const)), this, SLOT(onLevelRemoved(Project*const,Level*const)));
	connect(project, SIGNAL(levelRenamed(QString,QString)), this, SLOT(onLevelRenamed(QString,QString)));
	connect(project, SIGNAL(entityAdded(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(onEntityAdded(Project*const,Level*const,Area*const,Entity*const)));
	connect(project, SIGNAL(entityRemoved(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(onEntityRemoved(Project*const,Level*const,Area*const,Entity*const)));
	connect(project, SIGNAL(componentAttached(Project*const,Level*const,Area*const,Entity*const,Component*const)), this, SLOT(onComponentAttached(Project*const,Level*const,Area*const,Entity*const,Component*const)));
	connect(project, SIGNAL(componentRemoved(Project*const,Level*const,Area*const,Entity*const,Component*const)), this, SLOT(onComponentRemoved(Project*const,Level*const,Area*const,Entity*const,Component*const)));
    connect(project, SIGNAL(mapChanged(Area*const,const QVector<MapChange>&)), this, SLOT(onMapChanged(Area*const,const QVector<MapChange>&)));
    connect(project, SIGNAL(entityParented(Entity*const,Entity*const)), this, SLOT(onEntityParented(Entity*const,Entity*const)));
}

//====NONSTATIC====

Assets::Assets(void) {
	for(unsigned i = 0; i < Assets::LAYER_MAX; ++i) _layerVisible[i] = true;
}

Assets::~Assets(void) {
    //for(QMap<QString, Project *>::iterator it = _project.begin(); it != _project.end(); ++it) {
      //  delete it.value();
    //}
    if(m_project) {
        delete m_project;
        m_project->clear();
    }
    //_project.clear();
}

void Assets::setLayerVisible(const Assets::LAYER_TYPE type, bool flag) {
    //_layerVisible[type] = flag;
	if(type <= 3) _engine->getMapSys()->setRenderTileLayer(type, flag);
    _layerVisible[type] = flag;
}

bool Assets::getLayerVisible(const Assets::LAYER_TYPE type) const {
    return (type <= 2)? _engine->getMapSys()->getRenderTileLayer(type) : _layerVisible[type];
}

bool Assets::setCurrentArea(const QString name) {
/*    if(_currentArea) _currentArea->Deactivate();
    if(_currentLevel->getArea(name)) {
        _currentArea = _currentLevel->getArea(name);
        _currentArea->Activate();
        if(_currentArea->entity.size() > 0) setCurrentEntity(_currentArea->entity.first());
        else setCurrentEntity(NULL);
        emit AreaChanged(_currentArea);
        return true;
    }
    Debug::Log(Debug::CRITICAL, "Could not set current area to %s!\n", name.toStdString().c_str());
    return false;*/
    if(!_currentLevel) {
        setCurrentArea(nullptr);
        return false;
    }
    Area *area = _currentLevel->getArea(name);
    if(!area) {
        setCurrentArea(nullptr);
        return false;
    }
    setCurrentArea(area);
    return true;
}

void Assets::setCurrentArea(Area *const area) {
   // if(_currentArea) _currentArea->deactivate();
    //if(_currentArea == area) return;
    _currentArea = area;
    _engine->getLevelSys()->switchArea(area? area->getPath() : nullptr);
    if(area) {
        //area->activate();
		emit areaChanged(_currentArea);
        //if(area->getFirstEntity()) setCurrentSelection(new EntitySelectable(area->getFirstEntity()));
        setCurrentSelection((EntitySelectable*)nullptr);
    }
    else {
		emit areaChanged(_currentArea);
        setCurrentSelection((EntitySelectable*)nullptr);
    }
}


void Assets::setCurrentLayer(const LAYER_TYPE type) {
    _currentLayer=type;
	emit layerChanged(type);
}

bool Assets::setCurrentLevel(const QString name) {

    if(!m_project) return false;
    Level *level = m_project->getLevel(name);
    if(!level) return false;
    setCurrentLevel(level);
    return true;
}

void Assets::setCurrentLevel(Level *const level) {
   // if(_currentLevel) _currentLevel->deactivate();
    //if(_currentLevel == level) return;
    _currentLevel = level;

    _engine->getLevelSys()->switchLevel(level? level->getPath() : nullptr);
    if(level) {
    //	level->activate();
        if(level->getFirstArea()) setCurrentArea(level->getFirstArea()->getNodeName());
        else setCurrentArea(nullptr);
    }
    else setCurrentArea(nullptr);
	//Debug::Log(Debug::VERBOSE, "!!!!!!EMIT CURRENT LEVEL " +( (level)? level->getName() : "NULL"));
	emit levelChanged(level);
}

bool Assets::setCurrentProject(const QString name) {

    Project *project = getProject(name);
    if(!project) return false;
    setCurrentProject(project);
    return true;
}

void Assets::setCurrentProject(Project *const project) {
    if(m_project != project) {
        //m_project->Deactivate();
        m_project = project;
    }

    if(project) {
		project->activate();
        if(project->getFirstLevel()) setCurrentLevel(project->getFirstLevel()->getNodeName());
        else setCurrentLevel(nullptr);
        QDir::setCurrent(m_project->getLocation());
    }

    else {
        setCurrentLevel(nullptr);
    }
	//Debug::Log(Debug::VERBOSE, "!!!!!EMIT CURRENT PROJECT - " + ((project)? project->getName() : "NULL"));
	emit projectChanged(project);
}

Project *Assets::getProject(const QString /*name*/) const {
    return m_project;
}

Project *Assets::loadProject(const QString directory) {
    //for(QMap<QString, Project *>::iterator it = _project.begin(); it != _project.end(); ++it) {
    if(m_project != nullptr) {
        if(m_project->getLocation() == directory) {
            QMessageBox::critical(nullptr, tr("Error!"), tr("The project is already open!\n"));
            return nullptr;
        }
    }

    Project *proj = new Project;
	connectSlots(proj);

    gyTimerInit();

    elysian::Behavior::staticConstructor();
    elysian::Debug::getInstance().setEngine(*_engine);

    _engine = new elysian::Engine;
    elysian::Debug::setEngine(*_engine);


    _engine->loadAssets();
    elysian::Debug::getInstance().sandboxInit();
    _engine->setEngineState(elysian::System::ENGINE_ESTK_STOP);

    emit engineInitialize();

    //_engine->getMapSys()->makeCurrent();


	if(proj->load(directory)) {
        //ConnectSlots(proj);
        m_project = proj;
        emit projectAdded(proj);

       // _engine->getMapSys()->show();
        return proj;
    }


    delete proj;
	Debug::Logf(Debug::VERBOSE, "Could not load project %s!\n", directory.toStdString().c_str());
    return nullptr;
}

bool Assets::saveProject(const QString name) const {
    if(m_project->getNodeName() == name) return m_project->save();
	Debug::Logf(Debug::VERBOSE, "Project %s does not exist!", name.toStdString().c_str());
    return false;
}

Project *Assets::getFirstProject(void) const {
    return m_project;
}


bool Assets::removeProject(const QString name) {
    //for(QMap<QString, Project *>::iterator it = _project.begin(); it != _project.end(); ++it) {
        if(m_project->getNodeName() == name) {
            setCurrentProject(nullptr);
			emit projectRemoved(m_project);
			emit projectRemoveUI();

            Project *temp = m_project;
            //_project.erase(it);
            delete temp;


            return true;
        }

    //}
	Debug::Logf(Debug::VERBOSE, "Project %s could not be removed!\n", name.toStdString().c_str());
    return false;
}

void Assets::onChildProjectActivating(Project *const project, Level *const level, Area *const area) {
    if(m_project != project) setCurrentProject(project->getNodeName());
    if(level && _currentLevel != level) setCurrentLevel(level->getNodeName());
    if(area && _currentArea != area) setCurrentArea(area->getNodeName());
}

void Assets::onAreaAdded(Project *const project, Level *const level, Area *const area) {
	emit areaAdded(project, level, area);
}

void Assets::onAreaRemoved(Project *const project, Level *const level, Area *const area) {
	emit areaRemoved(project, level, area);
}

void Assets::onLevelAdded(Project *const project, Level *const level) {
	emit levelAdded(project, level);
}

void Assets::onLevelRemoved(Project *const project, Level *const level) {
	emit levelRemoved(project, level);
}

void Assets::onEntityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity) {
	emit entityAdded(project, level, area, entity);
}

void Assets::onEntityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity) {
	emit entityRemoved(project, level, area, entity);
}

void Assets::onComponentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp) {
	emit componentAttached(project, level, area, entity, comp);
}

void Assets::onComponentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp) {
	emit componentRemoved(project, level, area, entity, comp);
}

void Assets::onAreaRenamed(const QString oldName, const QString newName) {
	emit areaRenamed(oldName, newName);
}

void Assets::onLevelRenamed(const QString oldName, const QString newName) {
	emit levelRenamed(oldName, newName);
}

void Assets::onMapChanged(Area * const area, const QVector<MapChange>& changes) {
    emit mapChanged(area, changes);
}

void Assets::onEntityParented(Entity * const child, Entity * const parent) {
    emit entityParented(child, parent);
}

Level* Assets::reloadLevel(Level* level) {
    if(!level) level = getCurrentLevel();
    QString levelName;
    QString areaName;
    if(getCurrentLevel() == level) {
        levelName = level->getDirName();
        areaName = getCurArea()->getDirName();
        setCurrentArea(nullptr);
        setCurrentLevel(nullptr);
    }
    level = level->getProject()->reloadLevel(level);
    if(!areaName.isNull()) {
        setCurrentLevel(level);
        setCurrentArea(level->getArea(areaName));
    } else {
        //Force views and widgets to have correct selection
        setCurrentLevel(getCurrentLevel());
    }
    return level;
}

Area* Assets::reloadArea(Area* area) {
    if(!area) area = getCurrentArea();
    QString areaName;
    if(getCurrentArea() == area) {
        setCurrentArea(nullptr);
       areaName = area->getDirName();
   }
   area = area->getLevel()->reloadArea(area);
   if(!areaName.isNull()) {
        setCurrentArea(getCurrentLevel()->getArea(areaName));
   } else {
       //Force views and widgets to have correct selection
        setCurrentArea(getCurrentArea());
   }
   return area;
}
