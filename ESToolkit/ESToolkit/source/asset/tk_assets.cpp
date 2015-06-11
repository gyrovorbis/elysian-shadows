#include <QDir>
#include <QMessageBox>
#include "tk_assets.h"
#include <tk_project.h>
#include "elysian_level.hpp"
#include "tk_area.h"
#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "elysian_engine.hpp"
#include "elysian_level_system.hpp"
#include "elysian_map_system.hpp"
#include "elysian_map_system.hpp"
#include "tk_entity_selectable.h"
#include "elysian_party_system.hpp"

//====STATIC MEMBERS====

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

        Q_EMIT selectionChanged(_currentSelectionType, _currentSelection);

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
    return _curArea->getUndoStackCount();
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

    QObject::connect(project, &Project::inactiveEntityAdded, [&](Project*const project, Level*const level, Area*const area, Entity*const entity) {
        Q_EMIT inactiveEntityAdded(project, level, area, entity);
    });

    QObject::connect(project, &Project::inactiveEntityRemoved, [&](Project*const project, Level*const level, Area*const area, Entity*const entity) {
        Q_EMIT inactiveEntityRemoved(project, level, area, entity);
    });
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
        Q_EMIT AreaChanged(_currentArea);
        return true;
    }
    Debug::Log(Debug::DBG_WARNING, "Could not set current area to %s!\n", name.toStdString().c_str());
    return false;*/
    if(!_curLevel) {
        setCurrentArea(nullptr);
        return false;
    }
    Area *area = _curLevel->getArea(name);
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
    //_currentArea = area;
    _engine->getLevelSys()->switchArea(area? area->getPath() : nullptr);
    if(area) {
        //area->activate();
        Q_EMIT areaChanged(_curArea);
        //if(area->getFirstEntity()) setCurrentSelection(new EntitySelectable(area->getFirstEntity()));
        setCurrentSelection((EntitySelectable*)nullptr);
    }
    else {
        Q_EMIT areaChanged(_curArea);
        setCurrentSelection((EntitySelectable*)nullptr);
    }
}


void Assets::setCurrentLayer(const LAYER_TYPE type) {
    _currentLayer=type;
    Q_EMIT layerChanged(type);
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
  //  _currentLevel = level;

    _engine->getLevelSys()->switchLevel(level? level->getPath() : nullptr);
    if(level) {
    //	level->activate();
        if(level->getFirstArea()) setCurrentArea(level->getFirstArea()->getNodeName());
        else setCurrentArea(nullptr);
    }
    else setCurrentArea(nullptr);
    //Debug::Log(Debug::DBG_VERBOSE, "!!!!!!EMIT CURRENT SEVERITY " +( (level)? level->getName() : "NULL"));
    Q_EMIT levelChanged(level);
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
    //Debug::Log(Debug::DBG_VERBOSE, "!!!!!EMIT CURRENT PROJECT - " + ((project)? project->getName() : "NULL"));
    Q_EMIT projectChanged(project);
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
  //  makeGLContextCurrent();
   // qDebug() << "LOADPROJECT" << QOpenGLContext::currentContext();

   // if(!validateProjectPath(directory)) return nullptr;

    Project *proj = new Project;
    connectSlots(proj);

    gyTimerInit();

    elysian::Behavior::staticConstructor();
    elysian::Debug::getInstance().setEngine(*_engine);

    {
        DbgEvent event("Initializing ESGamma engine");
        _engine = new elysian::Engine;
        elysian::Debug::setEngine(*_engine);


        _engine->loadAssets();
        elysian::Debug::getInstance().sandboxInit();
        _engine->setEngineState(elysian::System::ENGINE_ESTK_STOP);
    }

    Q_EMIT engineInitialize();

    //_engine->getMapSys()->makeCurrent();


    if(proj->load(directory)) {
        //ConnectSlots(proj);
        m_project = proj;
        Q_EMIT projectAdded(proj);

       // _engine->getMapSys()->show();
//        doneGLContextCurrent();
        return proj;
    }

   // doneGLContextCurrent();
    delete proj;
    Debug::Logf(Debug::DBG_VERBOSE, "Could not load project %s!", directory.toStdString().c_str());
    return nullptr;
}

bool Assets::saveProject(const QString name) const {
    if(m_project->getNodeName() == name) return m_project->save();
    Debug::Logf(Debug::DBG_VERBOSE, "Project %s does not exist!", name.toStdString().c_str());
    return false;
}

Project *Assets::getFirstProject(void) const {
    return m_project;
}


bool Assets::removeProject(const QString name) {
    //for(QMap<QString, Project *>::iterator it = _project.begin(); it != _project.end(); ++it) {
        if(m_project->getNodeName() == name) {
            setCurrentProject(nullptr);
            Q_EMIT projectRemoved(m_project);
            Q_EMIT projectRemoveUI();

            Project *temp = m_project;
            //_project.erase(it);
            delete temp;


            return true;
        }

    //}
    Debug::Logf(Debug::DBG_VERBOSE, "Project %s could not be removed!", name.toStdString().c_str());
    return false;
}

void Assets::onChildProjectActivating(Project *const project, Level *const level, Area *const area) {
    if(project && m_project != project) setCurrentProject(project->getNodeName());
    if(level && _curLevel != level) setCurrentLevel(level->getNodeName());
    if(area && _curArea != area) setCurrentArea(area->getNodeName());
}

void Assets::onAreaAdded(Project *const project, Level *const level, Area *const area) {
    Q_EMIT areaAdded(project, level, area);
}

void Assets::onAreaRemoved(Project *const project, Level *const level, Area *const area) {
    Q_EMIT areaRemoved(project, level, area);
}

void Assets::onLevelAdded(Project *const project, Level *const level) {
    Q_EMIT levelAdded(project, level);
}

void Assets::onLevelRemoved(Project *const project, Level *const level) {
    Q_EMIT levelRemoved(project, level);
}

void Assets::onEntityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity) {
    Q_EMIT entityAdded(project, level, area, entity);
}

void Assets::onEntityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity) {
    Q_EMIT entityRemoved(project, level, area, entity);
}

void Assets::onComponentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp) {
    Q_EMIT componentAttached(project, level, area, entity, comp);
}

void Assets::onComponentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp) {
    Q_EMIT componentRemoved(project, level, area, entity, comp);
}

void Assets::onAreaRenamed(const QString oldName, const QString newName) {
    Q_EMIT areaRenamed(oldName, newName);
}

void Assets::onLevelRenamed(const QString oldName, const QString newName) {
    Q_EMIT levelRenamed(oldName, newName);
}

void Assets::onMapChanged(Area * const area, const QVector<MapChange>& changes) {
    Q_EMIT mapChanged(area, changes);
}

void Assets::onEntityParented(Entity * const child, Entity * const parent) {
    Q_EMIT entityParented(child, parent);
}

Level* Assets::reloadLevel(Level* level) {
    if(!level) level = getCurrentLevel();
    QString levelName;
    QString areaName;
    if(getCurrentLevel() == level) {
        levelName = level->getDirName();
        areaName = getCurrentArea()->getDirName();
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

Project* Assets::reloadProject(void) {
    if(!m_project) return nullptr;
    const QString areaName = _curArea->getDirName();
    const QString levelName = _curLevel->getDirName();
    const QString projPath = m_project->getLocation();
    const elysian::System::ENGINE_STATE engineState = _engine->getEngineState();
    const Entity* partyLeader = _engine->getPartySys()->getPartyLeader();
    GYVector3 playerPos;

    if(partyLeader) {
        playerPos = partyLeader->_transform._pos;
    }

    Q_EMIT projectRemoved(m_project);

    setCurrentSelection(nullptr);
    setCurrentArea(nullptr);
    setCurrentLevel(nullptr);
    setCurrentProject(nullptr);

    delete m_project;
    delete _engine;

    auto* proj = loadProject(projPath);
    Q_EMIT projectAdded(m_project);

    if(!levelName.isNull()) {
        auto* lev = proj->getLevel(levelName);
        setCurrentLevel(lev);
        if(!areaName.isNull()) {
            setCurrentArea(lev->getArea(areaName));
        }
    }

    _engine->setEngineState(engineState);
    if(partyLeader) _engine->getPartySys()->getPartyLeader()->_transform._pos = playerPos;
    return proj;
}

bool Assets::validateProjectPath(const QString path) {
    QDir dir(QDir::current());

    if(!dir.cd(path)) {
        qWarning() << "Could not enter project directory: " << path;
        return false;
    }

    if(!dir.isReadable()) {
        qWarning() << "Project path is not readable: " << path;
        return false;
    }

    return true;
}
