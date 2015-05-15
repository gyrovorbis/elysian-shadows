#include <QTreeWidgetItem>
#include <QDir>
#include <QMessageBox>

#include <tk_project.h>
#include "elysian_level.hpp"
#include "elysian_debug.hpp"
#include <tk_file_utilities.h>
#include "elysian_engine.hpp"
#include "elysian_level_system.hpp"
#include <elysian_entity.hpp>

Project::Project(void): _active(false) {}

Project::~Project(void) {
	for(int i = 0; i < _level.size(); ++i) {
		delete _level[i];
    }
    _level.clear();
}

void Project::connectSlots(TkLevel *const level) {
	connect(level, SIGNAL(activating(Level *const, Area *const)), this, SLOT(activateByLevel(Level *const, Area *const)));
	connect(level, SIGNAL(areaAdded(Level *const, Area *const)), this, SLOT(onAreaAdded(Level *const, Area *const)));
	connect(level, SIGNAL(areaRenamed(QString,QString)), this, SLOT(onAreaRenamed(QString,QString)));
	connect(level, SIGNAL(areaRemoved(Level*const,Area*const)), this, SLOT(onAreaRemoved(Level*const,Area*const)));
	connect(level, SIGNAL(levelRenamed(QString,QString)), this, SLOT(onLevelRenamed(QString,QString)));
	connect(level, SIGNAL(entityAdded(Level*const,Area*const,Entity*const)), this, SLOT(onAddEntity(Level*const,Area*const,Entity*const)));
	connect(level, SIGNAL(entityRemoved(Level*const,Area*const,Entity*const)), this, SLOT(onRemoveEntity(Level*const,Area*const,Entity*const)));
	connect(level, SIGNAL(componentAttached(Level*const,Area*const,Entity*const,Component*const)), this, SLOT(onComponentAttached(Level*const,Area*const,Entity*const,Component*const)));
	connect(level, SIGNAL(componentRemoved(Level*const,Area*const,Entity*const,Component*const)), this, SLOT(onComponentRemoved(Level*const,Area*const,Entity*const,Component*const)));
    connect(level, SIGNAL(mapChanged(Area*const,const QVector<MapChange>&)), this, SLOT(onMapChanged(Area*const,const QVector<MapChange>&)));
    connect(level, SIGNAL(entityParented(Entity*const,Entity*const)), this, SLOT(onEntityParented(Entity*const,Entity*const)));

    connect(level, &Level::inactiveEntityAdded, [&](Level*const level, Area*const area, Entity*const entity) {
        Q_EMIT inactiveEntityAdded(this, level, area, entity);
    });

    connect(level, &Level::inactiveEntityRemoved, [&](Level*const level, Area*const area, Entity*const entity) {
        Q_EMIT inactiveEntityRemoved(this, level, area, entity);
    });
}

void Project::populateModel(void) {

}

QAbstractItemModel* Project::getModel(void) const { return const_cast<Project*>(this); }

Level *Project::newLevel(const QString n) {
    QString name(n);
    unsigned int num = 1;
    TkLevel *lev = new TkLevel(this);
	connectSlots(lev);

    while(getLevel(name+QString::number(num))) {
        ++num;
    }

    name += QString::number(num);
    lev->setLocation(_location+"/level/"+name);
	_level.push_back(lev);
	lev->saveAll();
    getAssets()->getEngine()->getLevelSys()->addLevel(lev);
    _curNodeParent->addChild(lev);
    Q_EMIT levelAdded(this, lev);
    return lev;
}

Level *Project::loadLevel(const QString name) {
    QString directory(_curNodeParent->getNodePath());
    directory.append("/");
    directory.append(name);

	Debug::Log(Debug::VERBOSE, QString("Project::LoadLevel("+directory+")"));
    Debug::push();

    Level::_curProj = this;

    Level *lev = getLevelSys()->loadLevel(directory.toStdString().c_str());

    //connectSlots(lev);
    if(lev) {
        connectSlots(lev);
		_level.push_back(lev);
        _curNodeParent->addChild(lev);
        Q_EMIT levelAdded(this, lev);
        Debug::pop();
		return lev;
    }

	delete lev;
    Debug::pop();
    return nullptr;
}

//This looks like a mistake...
//Project::Save() invokes level->Save
//Project::Load() invokes Project::LoadLevel()
//unorthogonal
bool Project::saveLevel(const QString name) {
	Level *lev = getLevel(name);
	if(lev) return lev->saveAll();
	Debug::Logf(Debug::CRITICAL, "Level %s does not exist!\n", name.toStdString().c_str());
    return false;
}

TkLevel *Project::getFirstLevel() const {
    return (_level.size() > 0)? _level.first() : nullptr;
}

TkLevel *Project::getLevel(const QString name)  {
	for(int i = 0; i < _level.size(); ++i) {
        if(_level[i]->getNodeName() == name) return _level[i];
    }
    return nullptr;
}

unsigned int Project::getLevelAmount(void) const { return _level.size(); }

TkLevel *Project::getLevelByIndex(const unsigned int index) {
    return (index < (unsigned)_level.size())? _level[index] : nullptr;
}

bool Project::removeLevel(const QString name, const bool deleteFolder) {
    bool success = true;

	for(int i = 0; i < _level.size(); ++i) {
        if(_level[i]->getNodeName() == name) {
			Level *temp = _level[i];
            getAssets()->getEngine()->getLevelSys()->removeLevel(temp);
			_level.erase(_level.begin() + i);
            if(deleteFolder && !FileUtilities::removeDir(temp->getLocation())) {
				Debug::Logf(Debug::CRITICAL, "! - Could not delete directory '%s'\n", temp->getLocation().toStdString().c_str());
                success = false;
            }
            Q_EMIT levelRemoved(this, temp);
            delete temp;
            return success;
        }
    }
	Debug::Logf(Debug::CRITICAL, "Level %s could not be removed!\n", name.toStdString().c_str());
    return false;
}

bool Project::load(const QString loc) {
	Debug::Log(Debug::VERBOSE, QString("Project::Load(" + loc + ")"));
    Debug::push();

    QDir dir(loc);
    bool success = true;
    _curNodeParent = this;

    if(!dir.isReadable()) {
		Debug::Log(Debug::CRITICAL, "Directory is not readable!");
        Debug::pop();
		return false;
    }

    QString prevWorkingDir = QDir::current().absolutePath();
    QDir::setCurrent(loc);

    //Set Project name equal to project directory name
    setName(dir.dirName());
    _location = loc;

    if(!dir.cd("level")) {
        Debug::Logf(Debug::CRITICAL, "No 'level' folder!", getNodeName().toStdString().c_str());
        QDir::setCurrent(prevWorkingDir);
        Debug::pop();
        return false;
    }

    setNodePath(dir.absolutePath());

    std::function<void(const QString)> scanDirectory = [&](const QString path) {
        QDir dir(path);

        if(!dir.isReadable()) {
            Debug::printf(Debug::CRITICAL, "%s is not readable!", path.toStdString().c_str());
            return;
        }

        QStringList folders = dir.entryList(QDir::AllDirs);

        //check if folder is a level based on whether it has subdirs containing level data
        if(folders.contains(ESTK_LEVEL_TILE_SUBDIR) || folders.contains(ESTK_LEVEL_METADATA_SUBDIR) || folders.contains(ESTK_LEVEL_AREA_SUBDIR)) {
            success &= loadLevel(dir.dirName())? true : false;

        //assume regular subdir and recurse
        } else {
            Debug::printf(Debug::VERBOSE, "Entering %s subdirectory, scanning for levels.", path.toStdString().c_str());

            TreeNode* subDir = new TreeNode;
            subDir->setNodeName(dir.dirName());
            subDir->setNodePath(path);
            _subDirs[path] = subDir;

            TreeNode* oldNodeParent = _curNodeParent;
            _curNodeParent->addChild(subDir);
            _curNodeParent = subDir;

            //Skip '.' and '..' virtual directories
            for(int i = 2; i < folders.size(); ++i) {
                scanDirectory(path+QString("/")+folders[i]);
            }

            _curNodeParent = oldNodeParent;
        }
    };

    scanDirectory(getNodePath()); //begin recursive scan

    QDir::setCurrent(prevWorkingDir);
    Debug::pop();
	return true;//success;
}

bool Project::save(void) {
	Debug::Log(Debug::VERBOSE, QString("Project::Save(" + _location + ")"));
    Debug::push();
	//Saving is just exporting to the same location.
	bool success = saveAs(_location);
    Debug::pop();
	return success;
}

Level *Project::importLevel(const QString dir) {
    if(getLevel(QStringList(dir.split("/")).last())) {
        Debug::Log(Debug::CRITICAL, "A level of the same name already exists within the current project!");
        return nullptr;
    }

    TkLevel *level = new TkLevel(this);
	connectSlots(level);
	level->load(dir);
    level->setLocation(_location + "/level/" + QStringList(dir.split("/")).last());
	level->saveAll();
	_level.push_back(level);
    Q_EMIT levelAdded(this, level);
    return level;
}

bool Project::saveAs(const QString dir) {
    bool success = true;
    bool newLoc = (dir != _location)? true : false;
    QDir qdir;

	Debug::Log(Debug::VERBOSE, QString("Project::Save("+dir+")"));
    Debug::push();

    if(newLoc && qdir.exists(dir)) { //Are we overriding another existing folder for a save as?
        if(QMessageBox::critical(NULL, tr("Warning!"), tr("A directory of the same name already exists at the specified location. Overwrite?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if(!qdir.rmdir(dir)) {
				Debug::Log(Debug::CRITICAL, "Could not remove existing directory!\n");
                Debug::pop();
				return false;
            }
        }
        else {
			Debug::Log(Debug::VERBOSE, "Overwrite canceled.");
            Debug::pop();
			return true; //They didn't want to overwrite, not an error.
        }
    }
   if(!qdir.exists(dir)) { //generate main project directory if it doesn't already exist
        if(!qdir.mkdir(dir)) {
			Debug::Log(Debug::CRITICAL, "Could not create root directory!");
            Debug::pop();
			return false;
        }
    }

    if(!qdir.exists(dir + "/level")) {
        if(!qdir.mkdir(dir + "/level")) {
			Debug::Log(Debug::CRITICAL, "Could not create 'level' directory!");
            Debug::pop();
			return false;
        }
    }

	for(int i = 0; i < _level.size(); ++i) {
        success &= _level[i]->saveAs(dir+"/level/"+_level[i]->getNodeName());
    }

    Debug::pop();
    return success;
}

void Project::clearUndoStacks(void)
{
    for(unsigned int l = 0; l < getLevelAmount(); ++l) {
		getLevelByIndex(l)->clearAllStacks();
    }
}

bool Project::activate(void) {
    return activateByLevel(nullptr, nullptr);
}

bool Project::activateByLevel(Level *const level, Area *const area) {
	//Debug::Log(Debug::VERBOSE, "Activating Project %s", getName().toStdString().c_str());
    if(!_active) {
        _active = true;
    }
    Q_EMIT activating(this, level, area);
    return true;
}

void Project::onAddEntity(Level *const level, Area *const area, Entity *const entity) {
    Q_EMIT entityAdded(this, level, area, entity);
}

void Project::onRemoveEntity(Level *const level, Area *const area, Entity *const entity) {
    Q_EMIT entityRemoved(this, level, area, entity);
}

bool Project::deactivate(void) {
    _active = false;
	//Debug::Log(Debug::VERBOSE, "Deactivating Project %s", getName().toStdString().c_str());
    _active = false;
    return true;
}

bool Project::isActive(void) { return _active; }


void Project::onAreaAdded(Level *const level, Area *const area) {
    Q_EMIT areaAdded(this, level, area);
}

void Project::onAreaRemoved(Level *const level, Area *const area) {
    Q_EMIT areaRemoved(this, level, area);
}

void Project::setName(const QString name) {
    QString newLocation;
    TreeNode::setNodeName(name);

    //Update folder location
    QStringList splitList = _location.split("/");
	for(int i = 0; i < splitList.size()-1; ++i) {
        newLocation.append(splitList[i]+"/");
    }
    newLocation.append(name);
    _location = newLocation;

}

void Project::setLocation(const QString loc) {
   _location = loc;

   QStringList splitList = _location.split("/");
   TreeNode::setNodeName(splitList.last());
}

QString Project::getLocation(void) const {
    if(!QString(_location).isNull()) {
        return _location;
    }

    else {
        return "";
    }
}

TkLevel* Project::reloadLevel(TkLevel *level) {
    QString dirName = level->getDirName();
    removeLevel(dirName, false);
    return loadLevel(dirName);
}

void Project::onComponentAttached(Level *const level, Area *const area, Entity *const entity, Component *const comp) {
    Q_EMIT componentAttached(this, level, area, entity, comp);
}

void Project::onComponentRemoved(Level *const level, Area *const area, Entity *const entity, Component *const comp) {
    Q_EMIT componentRemoved(this, level, area, entity, comp);
}

void Project::onAreaRenamed(const QString oldName, const QString newName) {
    Q_EMIT areaRenamed(oldName, newName);
}

void Project::onLevelRenamed(const QString oldName, const QString newName) {
    Q_EMIT levelRenamed(oldName, newName);
}

void Project::onMapChanged(Area * const area, const QVector<MapChange>& changes) {
    Q_EMIT mapChanged(area, changes);
}

void Project::onEntityParented(Entity * const child, Entity * const parent) {
    Q_EMIT entityParented(child, parent);
}
