#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QTime>

#include "level.h"
#include "project.h"
#include "debug.h"
#include "tileSheetUtilities.h"
#include "sheetViewTileGraphicsItem.h"
#include "tileImage.h"
#include "area.h"
#include "fileUtilities.h"
#include "propertyStandardItem.h"
#include "checkeredAlphaRectGraphicsItem.h"

//====STATIC====

QGraphicsRectItem *Level::_sheetViewTileParent = NULL;
CheckeredAlphaRectGraphicsItem *::Level::_bgRectItem = NULL;
SelectionScene *Level::_sheetViewScene = NULL;

bool Level::initializeScene(const SourceView *const view, bool horiz) {
	static bool initialized = false;
	static SheetViewTileGraphicsItem ***tileItem = NULL;

	if(initialized) {
		//This should clean up everything in the scene
		delete _sheetViewScene;
		delete[] *tileItem;
		delete[] tileItem;
	}

	const unsigned tileWidth = (horiz)? 32 : 16;
	const unsigned tileHeight = (horiz)? 16 : 32;

	_sheetViewScene = new SelectionScene;
    _sheetViewScene->setSceneRect(QRect(-16, -16, 32*tileWidth, 32*tileHeight));


	//Allocate SheetViewTileGraphicsItems for tile/object sheets.
	tileItem = new SheetViewTileGraphicsItem**[tileHeight];

	for(unsigned int i = 0; i < tileHeight; ++i) {
		tileItem[i] = new SheetViewTileGraphicsItem*[tileWidth];
	}

	//alloc empty parent for all tiles
	_sheetViewTileParent = new QGraphicsRectItem;
	_sheetViewTileParent->setZValue(0);
	_sheetViewTileParent->setFlag(QGraphicsItem::ItemHasNoContents, true);
	_sheetViewTileParent->setRect(QRect(0, 0, 32*tileWidth, 32*tileHeight));

	_bgRectItem = new CheckeredAlphaRectGraphicsItem;
	_bgRectItem->setRect(QRect(-16, -16, 32*tileWidth, 32*tileHeight));
	_bgRectItem->setZValue(-1);

	//Initialize TileGraphicsItems
	if(!horiz) {
		for(unsigned int i = 0; i < tileHeight; ++i) {
			for(unsigned int j = 0; j < tileWidth; ++j) {
				tileItem[i][j] = new SheetViewTileGraphicsItem;
				tileItem[i][j]->setTilePosition(j, i);
				tileItem[i][j]->setView(view);
				tileItem[i][j]->setZValue(0);
				tileItem[i][j]->setValue((i*tileWidth)+j);
				tileItem[i][j]->setParentItem(_sheetViewTileParent);
			}
		}
	} else {
		/* We don't want to just invert the bitches for horizontal, because that
		 * looks completely fucking reested. We want to lay the two sections of
		 * the image beside each other. */
		for(unsigned int i = 0; i < 16; ++i) {
			for(unsigned int j = 0; j < 16; ++j) {
				tileItem[i][j] = new SheetViewTileGraphicsItem;
				tileItem[i][j]->setTilePosition(j, i);
				tileItem[i][j]->setView(view);
				tileItem[i][j]->setZValue(0);
				tileItem[i][j]->setValue((i*16)+j);
				tileItem[i][j]->setParentItem(_sheetViewTileParent);
			}
		}
		for(unsigned int i = 0; i < 16; ++i) {
			for(unsigned int j = 16; j < 32; ++j) {
				tileItem[i][j] = new SheetViewTileGraphicsItem;
				tileItem[i][j]->setTilePosition(j, i);
				tileItem[i][j]->setView(view);
				tileItem[i][j]->setZValue(0);
				tileItem[i][j]->setValue(i*16+(j-16)+256);
				//tileItem[i][j].setValue(1);
				tileItem[i][j]->setParentItem(_sheetViewTileParent);
			}
		}
	}

	_sheetViewScene->addItem(_sheetViewTileParent);
	initialized = true;
	return true;
}

void Level::addBgRectItem(void) {
	_sheetViewScene->addItem(_bgRectItem);
}

void Level::updateScenes(void) {
	_sheetViewScene->update();
}

//====NONSTATIC====

Level::Level(Project *const project):
    _project(project),
    _active(false),
    _open(false),
    _validSheets(false)
{
    strcpy(_elysianLevel.name, "untitled");
    strcpy(_elysianLevel.string, "untitled");
}

Level::~Level() {
	close();

	for(int i = 0; i < _area.size(); ++i) {
		delete _area[i];
    }
    _area.clear();
}

bool Level::duplicate(void) {
    unsigned num = 1;
    QString name(_name);

    while(_project->getLevel(_name+QString::number(num))) ++num;

    saveAs(_location+QString::number(num));

    return _project->loadLevel(name + QString::number(num));
}


void Level::populateModel(void) {

	clearStack();

	insertHeaders();

	pushLabel("Level");

	addProperty("name", new PropertyStandardItem<QString, Level>(this, &Level::getName, &Level::setName));
    addProperty("path", new PropertyStandardItem<QString, Level>(this, &Level::getLocation));
	addProperty("display", new PropertyStandardItem<QString, Level>(this, &Level::getString, &Level::setString));

    //addProperty("tiles", new PropertyStandardItem<unsigned char, Level>(this, &Level::getTileAmount));

	popLabel();
}


void Level::setName(const QString str) {
	QString newLocation;
	QString name = str;
	name.replace(QRegExp("[\\\\/ ]"), QString("_"));

	if(_name == name) return;

	QStringList splitList = _location.split("/");
	for(int i = 0; i < splitList.size()-1; ++i) {
		newLocation.append(splitList[i]+"/");
	}
	newLocation.append(name);

	//Already has a name, so we have to rename it
	if(!_name.isEmpty())  {

		if(_project->getLevel(name)) {
			QMessageBox::critical(NULL, tr("Error!"), tr("A level with that name already exists in the current project!\n"));
			return;
		}

		QString levDir = _project->getLocation();
		levDir.append("/level");

		QDir dir(levDir);
		if(!dir.rename(_name, name)) {
			QMessageBox::critical(NULL, tr("Error!"), tr("Unable to rename the directory!\n"));
			return;
		}
	}

	TreeNode::setName(name);
	_location = newLocation;
	strcpy(_elysianLevel.name, name.toStdString().c_str());
	strcpy(_elysianLevel.path, _location.toStdString().c_str());
}

void Level::setLocation(const QString loc) {
   _location = loc;

   QStringList splitList = _location.split("/");
   setName(splitList.last());
}

QString Level::getLocation() const {
    return _location;
}

elysian::Tile *Level::getElysianTile(const uint16_t index) {
	return &_elysianLevel.tileset[index];
}


void Level::connectSlots(Area *area) {
	connect(area, SIGNAL(activating(Area *const)), this, SLOT(activateByArea(Area*const)));
	connect(area, SIGNAL(entityAdded(Area*const, Entity*const)), this, SLOT(onEntityAdded(Area*const, Entity*const)));
	connect(area, SIGNAL(entityRemoved(Area*const, Entity*const)), this, SLOT(onEntityRemoved(Area*const, Entity*const)));
	connect(area, SIGNAL(componentAttached(Area*const,Entity*const,Component*const)), this, SLOT(onComponentAttached(Area*const,Entity*const,Component*const)));
	connect(area, SIGNAL(componentRemoved(Area*const,Entity*const,Component*const)), this, SLOT(onComponentRemoved(Area*const,Entity*const,Component*const)));
	connect(area, SIGNAL(renamed(QString,QString)), this, SLOT(onAreaRenamed(QString,QString)));
}

/*
void Level::OnChildAreaActivating(Area *const area) {
    Activate(area);
}*/

bool Level::activate(void) {
	_sheetViewScene->addItem(_bgRectItem);
	return activateByArea(NULL);
}

bool Level::activateByArea(Area *const area) {
	 //Debug::Log(Debug::VERBOSE, "Level[%s].Activate()\n", getName().toStdString().c_str());
	if(!_open) open();
    if(!_active) {
        _curLevel = &_elysianLevel;
    }
	emit activating(this, area);
    /* If there is anything that is not persistent to be added to the
       object/tilesheet scenes, do it now.
    */

	//What the fuck was I doing here? Maybe this was forcing an update?
	//objectPanelScene->addItem(_bgRectObjects);
	//tilePanelScene->addItem(_bgRectTiles);
    return true;
}

bool Level::deactivate(void) {
	Debug::Logf(Debug::VERBOSE, "Level[%s].Deactivate()\n", getName().toStdString().c_str());
    /* If there is anything that is not persistent to be removed from the
       object/tilesheet scenes, do it now.
    */

	_sheetViewScene->removeItem(_bgRectItem);

    return true;
}

// Will ALWAYS return false if valid sheets are already there.
// To regenerate sheets, first delete the contents of
//_objectSheet and _tileSheet, set valid sheets to false, then call this.
bool Level::generateTileSheet(const QString location){
	bool success = true;

    if(_validSheets) {
		Debug::Log(Debug::CRITICAL, "Tile sheet generation request when sheet is already valid! Returning false. ");
        return false;
    }

    //chop up/allocate tile sheets
	if(!TileSheetUtilities::DivideTileSheet(location, 512, 1024, 32, 32, _tileSheet)) {
		Debug::Log(Debug::CRITICAL, "Failure cutting tilesheet!");
        success = false;
    }

	if(!_tileSheet.isEmpty()) _validSheets = true;

    return success;
}

void Level::clearAllStacks(void) {
    for(unsigned int a = 0; a < getAreaAmount(); ++a) {
		getAreaByIndex(a)->getUndoStack().clear();
    }
}

bool Level::open(void) {
	//Debug::Log(Debug::VERBOSE, "Level[%s].Open()\n", getName().toStdString().c_str());
    bool success = true;
	if(_open) close();

	if(!TileSheetUtilities::DivideTileSheet(_location+ELYSIAN_TILESHEET_PATH,
											ELYSIAN_TILESHEET_WIDTH,
											ELYSIAN_TILESHEET_HEIGHT,
											ELYSIAN_TILE_WIDTH,
											ELYSIAN_TILE_HEIGHT,
											_tileSheet))
	{
        Debug::Log(Debug::CRITICAL, "Failure cutting tilesheet!\n");
        success = false;
    }

    _open = true;
    return success;
}

bool Level::close(void) {
    if(!_open) return true;
	if(_active) deactivate();
	for(int i = 0; i < _tileSheet.size(); ++i) delete _tileSheet[i];
	_tileSheet.clear();
    _open = false;
    return true;
}

Area *Level::newArea(void) {
	QString name("new_area");
    unsigned int num = 1;

	Area *ar = new Area(this);
	connectSlots(ar);
    while(getArea(name+QString::number(num))) {
        ++num;
    }
    name += QString::number(num);
    ar->setLocation(_location+"/area/"+name);
	_area.push_back(ar);
	ar->save();
	emit areaAdded(this, ar);
    return ar;
}

Area *Level::newArea(const QString n) {
    QString name(n);
    unsigned int num = 1;

	Area *ar = new Area(this);
	connectSlots(ar);
    while(getArea(name+QString::number(num))) {
        ++num;
    }
    name += QString::number(num);
    ar->setLocation(_location+"/area/"+name);
	_area.push_back(ar);
	ar->save();
	emit areaAdded(this, ar);
    return ar;
}



Area *Level::getArea(const QString name) {
	for(int i = 0; i < _area.size(); ++i) {
		if(_area[i]->getName() == name) return _area[i];
    }
    return NULL;
}

unsigned int Level::getAreaAmount(void) const { return _area.size(); }

Area *Level::getAreaByIndex(const unsigned int index) {
    return (index < (unsigned)_area.size())? _area[index] : NULL;
}

Area *Level::loadArea(const QString name) {
    QString directory(_location);
    directory.append("/area/");
    directory.append(name);

	Debug::Log(Debug::VERBOSE, QString("Level::LoadArea("+directory+")"));
	Debug::Push();

	Area *area = new Area(this);
	connectSlots(area);
	if(area->load(directory)) {
		_area.push_back(area);
		emit areaAdded(this, area);
		Debug::Pop();
        return area;
    }

	delete area;
	Debug::Pop();
    return NULL;
}

bool Level::removeArea(const QString name) {
    bool success = true;

	for(int i = 0; i < _area.size(); ++i) {
		if(_area[i]->getName() == name) {
			Area *temp = _area[i];
			_area.erase(_area.begin() + i);
			if(!FileUtilities::RemoveDir(temp->getLocation())) {
				Debug::Logf(Debug::CRITICAL, "! - Could not delete directory '%s'\n", temp->getLocation().toStdString().c_str());
                success = false;
            }
			emit areaRemoved(this, temp);
            delete temp;
            return success;
        }
    }
	Debug::Logf(Debug::CRITICAL, "Area %s could not be removed!\n", getName().toStdString().c_str());
    return false;
}

Area *Level::getFirstArea(void) const { return (_area.size() > 0)? _area[0] : NULL; }

Area *Level::importArea(const QString dir) {
    if(getArea(QStringList(dir.split("/")).last())) {
        Debug::Log(Debug::CRITICAL, "An area of the same name already exists within the current level!");
        return NULL;
    }

	Area *area = new Area(this);
	connectSlots(area);
	area->load(dir);
    area->setLocation(_location + "/area/" + QStringList(dir.split("/")).last());
	area->save();
	_area.push_back(area);
	emit areaAdded(this, area);
    return area;
}

bool Level::load(const QString loc) {
    QDir dir(loc);
    bool success = true;
    elysian::Level *tempCurLevel = _curLevel;
    _curLevel = &_elysianLevel;

	Debug::Logf(Debug::VERBOSE, "Level::Load(%s)", loc.toStdString().c_str());
	Debug::Push();

    setName(dir.dirName());
    _location = loc;

	Debug::Log(Debug::VERBOSE, "Verifying Tilesheet");
	Debug::Push();
	if(!TileSheetUtilities::VerifyImageSize(loc+ELYSIAN_TILESHEET_PATH,
											ELYSIAN_TILESHEET_WIDTH,
											ELYSIAN_TILESHEET_HEIGHT)) {
		Debug::Log(Debug::CRITICAL, "tilesheet.png either could not be opened or is not 512x1024!");
        success = false;
    }
	Debug::Pop();
    _validSheets = success;

	success &= _elysianLevel.load(dir.absolutePath().toStdString().c_str());

    if(!dir.cd("area")) {
		Debug::Log(Debug::CRITICAL, "No area directory!");
		Debug::Pop();
		return false;
    }

    QStringList areas = dir.entryList(QDir::AllDirs);
	for(int j = 2; j < areas.size(); ++j) {
		success &= (loadArea(areas[j]))? true : false;
    }

    _curLevel = tempCurLevel;
	Debug::Pop();
	return true;//success;
}

//Does not save children. Just attrib to current location
bool Level::save(void) {
	bool success = true;
	Debug::Log(Debug::VERBOSE, QString("Level::Save(void)"));
	Debug::Push();
	//Save elysian::Level
	success &= _elysianLevel.save(_location.toStdString().c_str());
	Debug::Pop();
	return success;
}


bool Level::saveAll(void) {
	Debug::Log(Debug::VERBOSE, QString("Level::SaveAll("+_location+")"));
	Debug::Push();
    //export to default save location
	bool success = saveAs(_location);
	Debug::Pop();
	return success;
}

bool Level::saveTileSheet(void) {
	return TileSheetUtilities::GenerateTileSheet(_location+ELYSIAN_TILESHEET_PATH,
												 ELYSIAN_TILESHEET_WIDTH,
												 ELYSIAN_TILESHEET_HEIGHT,
												 ELYSIAN_TILE_WIDTH,
												 ELYSIAN_TILE_HEIGHT,
												 _tileSheet);
}

bool Level::saveAs(const QString dir) {
    bool success = true;
    bool newLoc = (_location != dir)? true : false;
    QDir qdir;

	Debug::Log(Debug::VERBOSE, QString("Level::Export("+dir+")"));
	Debug::Push();

	//Save As and destionation already exists
	if(newLoc && qdir.exists(dir)) {
		Debug::Log(Debug::VERBOSE, "Folder already exists");
		Debug::Push();
        if(QMessageBox::critical(NULL, tr("Warning!"), tr("A directory of the same name already exists at the specified location. Overwrite?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if(!qdir.rmdir(dir)) {
				Debug::Log(Debug::CRITICAL, "Could not remove existing directory!");
				Debug::Pop();
				Debug::Pop();
				return false;
            }
			else {
				Debug::Log(Debug::VERBOSE, "Successfully deleted existing directory.");
				Debug::Pop();
			}
        }
        else {
			Debug::Log(Debug::VERBOSE, "Canceled overwrite.");
			Debug::Pop();
			Debug::Pop();
			return true;
        }
    }

	//Create level root if it doesn't already exist
   if(!qdir.exists(dir)) { //generate main level directory if it doesn't already exist
	   Debug::Log(Debug::VERBOSE, "Creating root directory");
	   Debug::Push();
        if(!qdir.mkdir(dir)) {
			Debug::Log(Debug::CRITICAL, "Could not create directory!");
			Debug::Pop();
			Debug::Pop();
			return false;
        }
		Debug::Pop();
    }

   //Create tile directory if it doesn't already exist
	if(!qdir.exists(dir+"/tile")) {
		Debug::Log(Debug::VERBOSE, "Creating tile directory");
		Debug::Push();
		if(!qdir.mkdir(dir+"/tile")) {
			Debug::Log(Debug::CRITICAL, "Could not create directory!");
			Debug::Pop();
			Debug::Pop();
			return false;
        }
		Debug::Pop();
    }
	//Dynamically regenerate tilesheet if it's currently open or is being saved to a new location
	if(_open || (!newLoc && !QFile::exists(dir+ELYSIAN_TILESHEET_PATH))) { // Dynamically regenerate sheet for open level and new levels
		success &= TileSheetUtilities::GenerateTileSheet(dir+ELYSIAN_TILESHEET_PATH,
														 ELYSIAN_TILESHEET_WIDTH,
														 ELYSIAN_TILESHEET_HEIGHT,
														 ELYSIAN_TILE_WIDTH,
														 ELYSIAN_TILE_HEIGHT,
														 _tileSheet);
	}
	//Copy tilesheet if it hasn't been opened and already exists
	else if(!_open && newLoc) { // Copy existing sheet from previous directory to new directory
		Debug::Log(Debug::VERBOSE, "Copying terrainsheet.png");
		Debug::Push();
		if(!QFile::copy(_location+ELYSIAN_TILESHEET_PATH, dir+ELYSIAN_TILESHEET_PATH)) {
			Debug::Log(Debug::CRITICAL, "Could not copy terrainsheet.png");
			success = false;
        }
		Debug::Pop();
	}

	//Save elysian::Level
	success &= _elysianLevel.save(dir.toStdString().c_str());

	//Create area directory if it doesn't already exist
    if(!qdir.exists(dir+"/area")) {
		Debug::Log(Debug::VERBOSE, "Creating 'area' directory");
		Debug::Push();
        if(!qdir.mkdir(dir+"/area")) {
			Debug::Log(Debug::CRITICAL, "Could not create 'area' directory!");
			Debug::Pop();
			Debug::Pop();
			return false;
        }
		Debug::Pop();
    }

	//Save areas
	for(int i = 0; i < _area.size(); ++i) {
		success &= _area[i]->saveAs(dir+"/area/"+_area[i]->getName());
    }

	Debug::Pop();
    return success;
}

//VisualTile *Level::getTileSheet(const unsigned int index) const { return _tileSheet[index]; }
//VisualTile *Level::getObjectSheet(const unsigned int index) const { return _objectSheet[index]; }

bool Level::reloadSheets(void) {
	Debug::Log(Debug::VERBOSE, "Reloading Sheets");
	Debug::Push();
	close();
	open();
	Debug::Pop();
    return true;
}

QString Level::getString(void) const { return _elysianLevel.string; }
void Level::setString(const QString str) {
	if(str.isEmpty()) {
		return;
	}
	strcpy(_elysianLevel.string, str.toStdString().c_str());
}


bool Level::isActive(void) const { return _active; }
bool Level::isOpen(void) const { return _open; }
bool Level::validSheets(void) const { return _validSheets; }

void Level::onEntityAdded(Area *const area, Entity *const entity) {
	emit entityAdded(this, area, entity);
}

void Level::onEntityRemoved(Area *const area, Entity *const entity) {
	emit entityRemoved(this, area, entity);
}

void Level::onComponentAttached(Area *const area, Entity *const entity, Component *const comp) {
	emit componentAttached(this, area, entity, comp);
}

void Level::onComponentRemoved(Area *const area, Entity *const entity, Component *const comp) {
	emit componentRemoved(this, area, entity, comp);
}

void Level::onAreaRenamed(const QString oldName, const QString newName) {
	emit areaRenamed(oldName, newName);
}









