#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QTime>

#include "level.h"
#include "project.h"
#include "elysian_debug.hpp"
#include "tileSheetUtilities.h"
#include "sheetViewTileGraphicsItem.h"
#include "tileImage.h"
#include "area.h"
#include "fileUtilities.h"
#include "propertyStandardItem.h"
#include "checkeredAlphaRectGraphicsItem.h"
#include "elysian_level_system.hpp"

//====STATIC====

QGraphicsRectItem *TkLevel::_sheetViewTileParent          = nullptr;
CheckeredAlphaRectGraphicsItem *::TkLevel::_bgRectItem    = nullptr;
SelectionScene *TkLevel::_sheetViewScene                  = nullptr;
Project* TkLevel::_curProj                                = nullptr;

bool TkLevel::initializeScene(const SourceView *const view, bool horiz) {
	static bool initialized = false;
    static SheetViewTileGraphicsItem ***tileItem = nullptr;

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

void TkLevel::addBgRectItem(void) {
	_sheetViewScene->addItem(_bgRectItem);
}

void TkLevel::updateScenes(void) {
	_sheetViewScene->update();
}

//====NONSTATIC====

TkLevel::TkLevel(Project *const project):
    _project(project),
    _active(false),
    _open(false),
    _validSheets(false)
{
    strcpy(dir, "untitled");
    strcpy(string, "untitled");

    for(unsigned i = 0; i < ELYSIAN_TILESHEET_TILES; ++i) _elysianTileWrappers[i].initialize(this, &tileset[i], i);

}

TkLevel::~TkLevel(void) {
	close();

	for(int i = 0; i < _area.size(); ++i) {
		delete _area[i];
    }
    _area.clear();
}

bool TkLevel::duplicate(void) {
    unsigned num = 1;
    QString name(_name);

    while(_project->getLevel(_name+QString::number(num))) ++num;

    saveAs(_location+QString::number(num));

    return _project->loadLevel(name + QString::number(num));
}


void TkLevel::populateModel(void) {

	clearStack();

	insertHeaders();

	pushLabel("Level");

    addProperty("name", new PropertyStandardItem<QString, Level>(this, &Level::getNodeName, &Level::setNodeName));
    addProperty("path", new PropertyStandardItem<QString, Level>(this, &Level::getLocation));
	addProperty("display", new PropertyStandardItem<QString, Level>(this, &Level::getString, &Level::setString));

    //addProperty("tiles", new PropertyStandardItem<unsigned char, Level>(this, &Level::getTileAmount));

	popLabel();
}


void TkLevel::setNodeName(const QString str) {
	QString newLocation;
	bool renamed = false;
	QString name = str;
	QString oldName = _name;
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

		renamed = true;
	}

    TreeNode::setNodeName(name);
	_location = newLocation;
    strcpy(dir, name.toStdString().c_str());
    strcpy(path, _location.toStdString().c_str());

	if(renamed) emit levelRenamed(oldName, name);
}

void TkLevel::setLocation(const QString loc) {
   _location = loc;

   QStringList splitList = _location.split("/");
   setNodeName(splitList.last());
}

QString TkLevel::getLocation() const {
    return _location;
}

elysian::Tile *TkLevel::getElysianTile(const uint16_t index) {
    return &tileset[index];
}


void TkLevel::connectSlots(Area *area) {
	connect(area, SIGNAL(activating(Area *const)), this, SLOT(activateByArea(Area*const)));
	connect(area, SIGNAL(entityAdded(Area*const, Entity*const)), this, SLOT(onEntityAdded(Area*const, Entity*const)));
	connect(area, SIGNAL(entityRemoved(Area*const, Entity*const)), this, SLOT(onEntityRemoved(Area*const, Entity*const)));
	connect(area, SIGNAL(componentAttached(Area*const,Entity*const,Component*const)), this, SLOT(onComponentAttached(Area*const,Entity*const,Component*const)));
	connect(area, SIGNAL(componentRemoved(Area*const,Entity*const,Component*const)), this, SLOT(onComponentRemoved(Area*const,Entity*const,Component*const)));
	connect(area, SIGNAL(renamed(QString,QString)), this, SLOT(onAreaRenamed(QString,QString)));
    connect(area, SIGNAL(mapChanged(Area*const,const QVector<MapChange>&)), this, SLOT(onMapChanged(Area*const,const QVector<MapChange>&)));
}

/*
void Level::OnChildAreaActivating(Area *const area) {
    Activate(area);
}*/

bool TkLevel::activate(void) {
	_sheetViewScene->addItem(_bgRectItem);
    return activateByArea(nullptr);
}

bool TkLevel::activateByArea(Area *const area) {
	 //Debug::Log(Debug::VERBOSE, "Level[%s].Activate()\n", getName().toStdString().c_str());
	if(!_open) open();
    if(!_active) {
        _curLevel = this;
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

bool TkLevel::deactivate(void) {
    Debug::Logf(Debug::VERBOSE, "Level[%s].Deactivate()\n", getNodeName().toStdString().c_str());
    /* If there is anything that is not persistent to be removed from the
       object/tilesheet scenes, do it now.
    */

	_sheetViewScene->removeItem(_bgRectItem);

    return true;
}

// Will ALWAYS return false if valid sheets are already there.
// To regenerate sheets, first delete the contents of
//_objectSheet and _tileSheet, set valid sheets to false, then call this.
bool TkLevel::generateTileSheet(const QString location){
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

void TkLevel::clearAllStacks(void) {
    for(unsigned int a = 0; a < getAreaAmount(); ++a) {
		getAreaByIndex(a)->getUndoStack().clear();
    }
}

bool TkLevel::open(void) {
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

bool TkLevel::close(void) {
    if(!_open) return true;
	if(_active) deactivate();
	for(int i = 0; i < _tileSheet.size(); ++i) delete _tileSheet[i];
	_tileSheet.clear();
    _open = false;
    return true;
}

Area *TkLevel::newArea(void) {
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

Area *TkLevel::newArea(const QString n) {
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



Area *TkLevel::getArea(const QString name) {
	for(int i = 0; i < _area.size(); ++i) {
        if(_area[i]->getNodeName() == name) return _area[i];
    }
    return nullptr;
}

unsigned int TkLevel::getAreaAmount(void) const { return _area.size(); }

Area *TkLevel::getAreaByIndex(const unsigned int index) {
    return (index < (unsigned)_area.size())? _area[index] : nullptr;
}

Area *TkLevel::loadArea(const QString name) {
    QString directory(_location);
    directory.append("/area/");
    directory.append(name);

	Debug::Log(Debug::VERBOSE, QString("Level::LoadArea("+directory+")"));
    Debug::push();

    Level *tempCurLevel = _curLevel;
    _curLevel = this;

    Area* area = getLevelSys()->loadArea(directory.toStdString().c_str());//new Area(this);

    _curLevel = tempCurLevel;

    //connectSlots(area);
    if(area) {
		_area.push_back(area);
		emit areaAdded(this, area);
        Debug::pop();
        return area;
    }

	delete area;
    Debug::pop();
    return nullptr;
}

bool TkLevel::removeArea(const QString name) {
    bool success = true;

	for(int i = 0; i < _area.size(); ++i) {
        if(_area[i]->getNodeName() == name) {
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
    Debug::Logf(Debug::CRITICAL, "Area %s could not be removed!\n", getNodeName().toStdString().c_str());
    return false;
}

Area *TkLevel::getFirstArea(void) const { return (_area.size() > 0)? _area[0] : NULL; }

Area *TkLevel::importArea(const QString dir) {
    if(getArea(QStringList(dir.split("/")).last())) {
        Debug::Log(Debug::CRITICAL, "An area of the same name already exists within the current level!");
        return nullptr;
    }

	Area *area = new Area(this);
	connectSlots(area);
    area->load(dir.toStdString().c_str());
    area->setLocation(_location + "/area/" + QStringList(dir.split("/")).last());
	area->save();
	_area.push_back(area);
	emit areaAdded(this, area);
    return area;
}

bool TkLevel::load(const QString loc) {
    QDir dir(loc);
    bool success = true;

    _project = _curProj;
    _project->connectSlots(this);

	Debug::Logf(Debug::VERBOSE, "Level::Load(%s)", loc.toStdString().c_str());
    Debug::push();

    setNodeName(dir.dirName());
    _location = loc;

	Debug::Log(Debug::VERBOSE, "Verifying Tilesheet");
    Debug::push();
	if(!TileSheetUtilities::VerifyImageSize(loc+ELYSIAN_TILESHEET_PATH,
											ELYSIAN_TILESHEET_WIDTH,
											ELYSIAN_TILESHEET_HEIGHT)) {
		Debug::Log(Debug::CRITICAL, "tilesheet.png either could not be opened or is not 512x1024!");
        success = false;
    }
    Debug::pop();
    _validSheets = success;

    success &= LevelBase::load(dir.absolutePath().toStdString().c_str());

    success &= _loadMetaData(loc);

    if(!dir.cd("area")) {
		Debug::Log(Debug::CRITICAL, "No area directory!");
        Debug::pop();
		return false;
    }

    QStringList areas = dir.entryList(QDir::AllDirs);
	for(int j = 2; j < areas.size(); ++j) {
		success &= (loadArea(areas[j]))? true : false;
    }

    Debug::pop();
	return true;//success;
}

//Does not save children. Just attrib to current location
bool TkLevel::save(void) {
	bool success = true;
	Debug::Log(Debug::VERBOSE, QString("Level::Save(void)"));
    Debug::push();
	//Save elysian::Level
    success &= getElysianLevel()->save(_location.toStdString().c_str());
    success &= _saveMetaData(_location);
    Debug::pop();
	return success;
}


bool TkLevel::saveAll(void) {
	Debug::Log(Debug::VERBOSE, QString("Level::SaveAll("+_location+")"));
    Debug::push();
    //export to default save location
	bool success = saveAs(_location);
    Debug::pop();
	return success;
}

bool TkLevel::saveTileSheet(void) {
	return TileSheetUtilities::GenerateTileSheet(_location+ELYSIAN_TILESHEET_PATH,
												 ELYSIAN_TILESHEET_WIDTH,
												 ELYSIAN_TILESHEET_HEIGHT,
												 ELYSIAN_TILE_WIDTH,
												 ELYSIAN_TILE_HEIGHT,
												 _tileSheet);
}

bool TkLevel::saveAs(const QString dir) {
    bool success = true;
    bool newLoc = (_location != dir)? true : false;
    QDir qdir;

	Debug::Log(Debug::VERBOSE, QString("Level::Export("+dir+")"));
    Debug::push();

	//Save As and destionation already exists
	if(newLoc && qdir.exists(dir)) {
		Debug::Log(Debug::VERBOSE, "Folder already exists");
        Debug::push();
        if(QMessageBox::critical(NULL, tr("Warning!"), tr("A directory of the same name already exists at the specified location. Overwrite?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if(!qdir.rmdir(dir)) {
				Debug::Log(Debug::CRITICAL, "Could not remove existing directory!");
                Debug::pop();
                Debug::pop();
				return false;
            }
			else {
				Debug::Log(Debug::VERBOSE, "Successfully deleted existing directory.");
                Debug::pop();
			}
        }
        else {
			Debug::Log(Debug::VERBOSE, "Canceled overwrite.");
            Debug::pop();
            Debug::pop();
			return true;
        }
    }

	//Create level root if it doesn't already exist
   if(!qdir.exists(dir)) { //generate main level directory if it doesn't already exist
	   Debug::Log(Debug::VERBOSE, "Creating root directory");
       Debug::push();
        if(!qdir.mkdir(dir)) {
			Debug::Log(Debug::CRITICAL, "Could not create directory!");
            Debug::pop();
            Debug::pop();
			return false;
        }
        Debug::pop();
    }

   //Create tile directory if it doesn't already exist
	if(!qdir.exists(dir+"/tile")) {
		Debug::Log(Debug::VERBOSE, "Creating tile directory");
        Debug::push();
		if(!qdir.mkdir(dir+"/tile")) {
			Debug::Log(Debug::CRITICAL, "Could not create directory!");
            Debug::pop();
            Debug::pop();
			return false;
        }
        Debug::pop();
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
        Debug::push();
		if(!QFile::copy(_location+ELYSIAN_TILESHEET_PATH, dir+ELYSIAN_TILESHEET_PATH)) {
			Debug::Log(Debug::CRITICAL, "Could not copy terrainsheet.png");
			success = false;
        }
        Debug::pop();
	}

	//Save elysian::Level
    success &= getElysianLevel()->save(dir.toStdString().c_str());

    //Save ESTk metadata
    success &= _saveMetaData(dir);

	//Create area directory if it doesn't already exist
    if(!qdir.exists(dir+"/area")) {
		Debug::Log(Debug::VERBOSE, "Creating 'area' directory");
        Debug::push();
        if(!qdir.mkdir(dir+"/area")) {
			Debug::Log(Debug::CRITICAL, "Could not create 'area' directory!");
            Debug::pop();
            Debug::pop();
			return false;
        }
        Debug::pop();
    }

	//Save areas
	for(int i = 0; i < _area.size(); ++i) {
        success &= _area[i]->saveAs(dir+"/area/"+_area[i]->getNodeName());
    }

    Debug::pop();
    return success;
}

//VisualTile *Level::getTileSheet(const unsigned int index) const { return _tileSheet[index]; }
//VisualTile *Level::getObjectSheet(const unsigned int index) const { return _objectSheet[index]; }

bool TkLevel::reloadSheets(void) {
	Debug::Log(Debug::VERBOSE, "Reloading Sheets");
    Debug::push();
	close();
	open();
    Debug::pop();
    return true;
}

QString TkLevel::getString(void) const { return string; }
void TkLevel::setString(const QString str) {
	if(str.isEmpty()) {
		return;
	}
    strcpy(string, str.toStdString().c_str());
}


bool TkLevel::isActive(void) const { return _active; }
bool TkLevel::isOpen(void) const { return _open; }
bool TkLevel::validSheets(void) const { return _validSheets; }

void TkLevel::onEntityAdded(Area *const area, Entity *const entity) {
	emit entityAdded(this, area, entity);
}

void TkLevel::onEntityRemoved(Area *const area, Entity *const entity) {
	emit entityRemoved(this, area, entity);
}

void TkLevel::onComponentAttached(Area *const area, Entity *const entity, Component *const comp) {
	emit componentAttached(this, area, entity, comp);
}

void TkLevel::onComponentRemoved(Area *const area, Entity *const entity, Component *const comp) {
	emit componentRemoved(this, area, entity, comp);
}

void TkLevel::onAreaRenamed(const QString oldName, const QString newName) {
	emit areaRenamed(oldName, newName);
}

void TkLevel::onMapChanged(Area * const area, const QVector<MapChange>& changes) {
    emit mapChanged(area, changes);
}

bool TkLevel::_saveMetaData(const QString dir) {
    QDir qdir;
    Debug::Log(Debug::VERBOSE, "Saving level metadata");
    Debug::push();

    if(!qdir.exists(dir+"/estk")) {
        Debug::Log(Debug::VERBOSE, "Creating 'estk' directory");
        Debug::push();
        if(!qdir.mkdir(dir+"/estk")) {
            Debug::Log(Debug::CRITICAL, "Could not create 'estk' directory!");
            Debug::pop();
            Debug::pop();
            return false;
        }
        Debug::pop();
    }

    FILE *file = fopen(QString(dir+"/estk/"+ESTK_LEVEL_METADATA_TILESET).toStdString().c_str(), "wb");
    if(!file) {
        Debug::Log(Debug::CRITICAL, "Could not open estkTileset.bin for writing!");
        Debug::pop();
        return false;
    }

    fwrite(_normalMapEnabled, sizeof(_normalMapEnabled), 1, file);
    fwrite(_specularMapEnabled, sizeof(_specularMapEnabled), 1, file);

    fclose(file);


    Debug::pop();
    return true;
}

bool TkLevel::_loadMetaData(const QString dir) {
    QDir qdir;
    Debug::Log(Debug::VERBOSE, "Loading level metadata");
    Debug::push();


    if(!qdir.exists(dir+"/estk")) {
        Debug::Log(Debug::VERBOSE, "'estk' directory not present");
        Debug::pop();
        return true;
    }

    FILE *file = fopen(QString(dir+"/estk/"+ESTK_LEVEL_METADATA_TILESET).toStdString().c_str(), "rb");
    if(!file) {
        Debug::Log(Debug::CRITICAL, "Could not open estkTileset.bin for reading!");
        Debug::pop();
        return false;
    }

    fread(_normalMapEnabled, sizeof(_normalMapEnabled), 1, file);
    fread(_specularMapEnabled, sizeof(_specularMapEnabled), 1, file);

    fclose(file);

    Debug::pop();

    return true;
}

bool TkLevel::_scatterMaps(const bool normal) {
    bool success = true;
    QString path = getLocation();
    path = path+"/estk/"+QString((normal)? ESTK_LEVEL_NORMAL_MAPS_DIRECTORY : ESTK_LEVEL_SPECULAR_MAPS_DIRECTORY);

    //generate normalMaps folder
    QDir qdir;
    Debug::Log(Debug::VERBOSE, "Scattering tilesheet maps");
    Debug::push();


    if(!qdir.exists(path)) {
        Debug::Log(Debug::VERBOSE, "Creating 'maps' directory");
        Debug::push();

        if(!qdir.mkdir(path)) {
            Debug::Log(Debug::CRITICAL, "Could not create 'maps' directory!");
            Debug::pop();
            Debug::pop();

            return false;
        }
        Debug::pop();
    }

    for(unsigned i = 0; i < ELYSIAN_TILESHEET_TILES; ++i) {
        if(((normal)? _normalMapEnabled[i] : _specularMapEnabled[i])) {
            QString file = path+QString("/tile%1.png").arg(i);
            QImage image(40, 40, QImage::Format_ARGB32);
            image.fill(QColor(0, 0, 0, 0));
            QPainter painter(&image);

            painter.drawPixmap(4, 4, _tileSheet[i]->getPixmap());

            //Top and bottom borders
            for(unsigned x = 4; x <= 35; ++x) {
                //top
                QRgb pixel = image.pixel(x, 4);
                for(unsigned y = 0; y < 4; ++y) {
                    image.setPixel(x, y, pixel);
                }

                //bottom
                pixel = image.pixel(x, 35);
                for(unsigned y = 36; y < 40; ++y) {
                    image.setPixel(x, y, pixel);
                }
            }

            for(unsigned y = 4; y <= 35; ++y) {
                //left
                QRgb pixel = image.pixel(4, y);
                for(unsigned x = 0; x < 4; ++x) {
                    image.setPixel(x, y, pixel);
                }

                //right
                pixel = image.pixel(35, y);
                for(unsigned x = 36; x < 40; ++x) {
                    image.setPixel(x, y, pixel);
                }

            }

            //top left corner
            QRgb pixel = image.pixel(4, 4);
            for(unsigned y = 0; y < 4; ++y)
                for(unsigned x = 0; x < 4; ++x)
                    image.setPixel(x, y, pixel);

            //top right corner
            pixel = image.pixel(35, 4);
            for(unsigned y = 0; y < 4; ++y)
                for(unsigned x = 36; x < 40; ++x)
                    image.setPixel(x, y, pixel);

            //bottom left corner
            pixel = image.pixel(4, 35);
            for(unsigned y = 36; y < 40; ++y)
                for(unsigned x = 0; x < 4; ++x)
                    image.setPixel(x, y, pixel);

            //bottom right corner
            pixel = image.pixel(35, 35);
            for(unsigned y = 36; y < 40; ++y)
                for(unsigned x = 36; x < 40; ++x)
                    image.setPixel(x, y, pixel);

            image.save(file);
        }
    }

    Debug::pop();
    return success;
}

bool TkLevel::_gatherMaps(const bool normal) {
    bool success = true;
    QString path = getLocation();
    path = path+"/estk/"+QString((normal)? ESTK_LEVEL_NORMAL_MAPS_DIRECTORY : ESTK_LEVEL_SPECULAR_MAPS_DIRECTORY);
    QVector<TileImage *> normalMaps;
    QRect clipRect(4, 4, 32, 32);

    QPixmap blankMap(32, 32);
    blankMap.fill(normal? QColor(128, 128, 255, 255) : QColor(0, 0, 0, 255));
    TileImage blankImage(blankMap);

    //generate normalMaps folder
    QDir qdir;
    Debug::Log(Debug::VERBOSE, "Gathering tilesheet maps");
    Debug::push();

    if(!qdir.exists(path)) {
        Debug::Log(Debug::CRITICAL, "'maps' folder not found!");
        Debug::push();
        return false;
    }

    for(unsigned i = 0; i < ELYSIAN_TILESHEET_TILES; ++i) {
        if(_normalMapEnabled[i]) {
            QString file = path+((normal)? QString("/tile%1_normal.png").arg(i) : QString("/tile%1_specular.png").arg(i));
            QImage image;
            if(!image.load(file)) {
                Debug::Log(Debug::CRITICAL, QString("Could not open ")+file);
                normalMaps.push_back(&blankImage);
                success = false;

            } else {
                QPixmap pixmap = QPixmap::fromImage(image.copy(clipRect));
                TileImage* image = new TileImage(pixmap);
                normalMaps.push_back(image);
            }

        } else {
            normalMaps.push_back(&blankImage);
        }
    }

    //Divide tilesheets!!!
    success &= TileSheetUtilities::GenerateTileSheet(getLocation()+((normal)? QString("/tile/normalsheet.png") : QString("/tile/specularsheet.png")), 512, 1024, 32, 32, normalMaps);

    //clear resources!
    for(unsigned i = 0; i < ELYSIAN_TILESHEET_TILES; ++i) {
        //make fucking sure we ain't deleting the static image
        if(((normal)? _normalMapEnabled[i] : _specularMapEnabled[i]) && normalMaps[i] != &blankImage) {
            delete normalMaps[i];
        }
    }

    Debug::push();
    return success;
}


// ========= SPECULAR/NORMAL MAP GENERATION ===========

bool TkLevel::scatterTileNormalMaps(void) {
    return _scatterMaps(true);
}

bool TkLevel::gatherTileNormalMaps(void) {
   return _gatherMaps(true);
}


bool TkLevel::scatterTileSpecularMaps(void) {
    return _scatterMaps(false);
}

bool TkLevel::gatherTileSpecularMaps(void) {
    return _gatherMaps(false);
}










