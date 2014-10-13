#include <QDir>
#include <QMessageBox>

#include "elysian_entity.hpp"

#include "area.h"
#include "level.h"
#include "debug.h"
#include "entity.h"
#include "propertyStandardItem.h"
#include "checkeredAlphaRectGraphicsItem.h"
#include "sourceView.h"
#include "mapViewDepthBufferGraphicsItem.h"

//====STATIC====
SelectionScene *Area::editorScene = NULL;
unsigned int Area::_currentSceneWidth = 0;
unsigned int Area::_currentSceneHeight = 0;
bool Area::inactiveTransparency = false;
MapViewDepthBufferGraphicsItem	*Area::_depthBuffGraphicsItem = NULL;

bool Area::staticConstructor(const SourceView *const view) {
	editorScene = new SelectionScene;
	editorScene->setSceneRect(0,0,2,2);

    //Allocate a MapViewTerrainItem for 4 different layers
	_depthBuffGraphicsItem = new MapViewDepthBufferGraphicsItem;
	_depthBuffGraphicsItem->setView(view);

	resizeEditorScene();
	editorScene->addItem(_depthBuffGraphicsItem);

	return true;
}

void Area::updateScene(void) {
	editorScene->update();
}

//====NONSTATIC====

Area::Area(Level *const level):
    _level(level),
    _active(false),
    _open(false)
{

	_elysianArea.width = _elysianArea.height = 50;
	//Must allocate a map in case this map is to be saved before it is loaded.
	_elysianArea.allocMap();
	memset(_elysianArea.tiles, 0, sizeof(elysian::TileEntry)*_elysianArea.width*_elysianArea.height*ELYSIAN_AREA_MAP_LAYERS);
    //memset(_elysianArea.lights, 0, sizeof(elysian::LightEntry)*_elysianArea.width*_elysianArea.height);
	strcpy(_elysianArea.name, "untitled");
    strcpy(_elysianArea.string, "untitled");
    connect(&_undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoCleanStateChanged(bool)));
}

Area::~Area(void) {
	close();
}

void Area::resizeEditorScene(void) {
	_depthBuffGraphicsItem->prepareResize();
}

void Area::populateModel(void) {
	clearStack();

	insertHeaders();

	pushLabel("Area");

	addProperty("name", new PropertyStandardItem<QString, Area, true>(this, &Area::getName, &Area::setName, UndoItem::AREA_RENAME, &_undoStack));
	addProperty("path", new PropertyStandardItem<QString, Area>(this, &Area::getLocation));
	addProperty("display", new PropertyStandardItem<QString, Area, true>(this, &Area::getString, &Area::setString, UndoItem::AREA_STRING, &_undoStack));

	pushLabel("size");
		addProperty("width", new PropertyStandardItem<unsigned short, Area, true>(this, &Area::getMapWidth, &Area::setMapWidth, UndoItem::AREA_WIDTH, &_undoStack));
		addProperty("height", new PropertyStandardItem<unsigned short, Area, true>(this, &Area::getMapHeight, &Area::setMapHeight, UndoItem::AREA_HEIGHT, &_undoStack));
	popLabel();

	popLabel();

}

void Area::setName(const QString str) {
	QString newLocation;
	bool _renamed = false;
	QString oldName = _name;
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

		if(_level->getArea(name)) {
			QMessageBox::critical(NULL, tr("Error!"), tr("An area with that name already exists in the current level!\n"));
			return;
		}

		QString areaDir = _level->getLocation();
		areaDir.append("/area");

		QDir dir(areaDir);
		if(!dir.rename(_name, name)) {
			QMessageBox::critical(NULL, tr("Error!"), tr("Unable to rename the directory!\n"));
			return;
		}

		_renamed = true;
	}

	TreeNode::setName(name);
	_location = newLocation;
	strcpy(_elysianArea.name, name.toStdString().c_str());
	strcpy(_elysianArea.path, _location.toStdString().c_str());

	if(_renamed) emit renamed(oldName, name);
}

bool Area::duplicate(void) {
    unsigned num = 1;
    QString name(_name);

    while(_level->getArea(_name+QString::number(num))) ++num;

    saveAs(_location+QString::number(num));

    return _level->loadArea(name + QString::number(num));
}


void Area::setLocation(const QString loc) {
	_location = loc;

	QStringList splitList = _location.split("/");
	setName(splitList.last());
}

void Area::setString(const QString str) {
	if(str.isEmpty()) return;
	strcpy(_elysianArea.string, str.toStdString().c_str());
}

QString Area::getString(void) const {
	return QString(_elysianArea.string);
}

int Area::getUndoStackCount(void) {
	return _undoStack.count();
}

QString Area::getLocation(void) const {
	return _location;
}

void Area::connectSlots(Entity *const entity) {
	connect(entity, SIGNAL(componentAttached(Entity*const,Component*const)), this, SLOT(onComponentAttached(Entity*const,Component*const)));
	connect(entity, SIGNAL(componentRemoved(Entity*const,Component*const)), this, SLOT(onComponentRemoved(Entity*const,Component*const)));
}

bool Area::load(const QString dir) {
	bool success = true;
	elysian::Area *tempCurArea = _curArea;
	_curArea = &_elysianArea;
	Debug::Logf(Debug::VERBOSE, "Area::Load(%s)", dir.toStdString().c_str());
	Debug::Push();

	setName(QDir(dir).dirName());
	_location = dir;

	//Allocated a dummy map upon construction, have to free before loading.
	_elysianArea.freeMap();
    success &= _elysianArea.load(dir.toStdString().c_str(), _level->getElysianLevel());

	Debug::Log(Debug::VERBOSE, "Creating Toolkit Entities from elysian::Entities.");
	Debug::Push();
#if 0
    for(unsigned int i = 0; i < _elysianArea.entities.size(); ++i) {
		Debug::Logf(Debug::VERBOSE, "Entity #%d - %s.", i, _elysianArea.entities[i]->getName());
		Entity *tempEntity = new Entity(*_elysianArea.entities[i]);
		connectSlots(tempEntity);
		_entities.push_back(tempEntity);
		//tempEntity->setParentItem(_entityParent);
	}
#endif
	Debug::Pop();

	_curArea = tempCurArea;
	Debug::Pop();
	return true;//success;
}

bool Area::save(void) {
	bool success = true;
	Debug::Log(Debug::VERBOSE, QString("Area::Save("+_location+")"));
	Debug::Push();
	//Save parent level
	success &= _level->save();
	//export to previous save location
	success &= saveAs(_location);
	_undoStack.setClean();
	Debug::Pop();
	return success;
}

bool Area::saveAs(const QString dir) {
	bool success = true;
	bool newLoc = (_location != dir)? true : false;
	QDir qdir;

	Debug::Log(Debug::VERBOSE, QString("Area::Export("+dir+")"));
	Debug::Push();

	//Check Save As and directory already exists
	if(newLoc && qdir.exists(dir)) {
		Debug::Log(Debug::VERBOSE, "Directory already exists!");
		Debug::Push();
		if(QMessageBox::critical(NULL, tr("Warning!"), tr("A directory of the same name already exists at the specified location. Overwrite?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			if(!qdir.rmdir(dir)) {
				Debug::Log(Debug::CRITICAL, "Could not delete existing directory!");
				Debug::Pop();
				Debug::Pop();
				return false;
			}
		}
		else {
			Debug::Log(Debug::VERBOSE, "Directory overwrite canceled.");
			Debug::Pop();
			Debug::Pop();
			return true; //User canceled, still a success
		}
		Debug::Pop();
	}

	//create main area directory if it doesn't already exist
	if(!qdir.exists(dir)) {
		Debug::Log(Debug::VERBOSE, "Creating root directory");
		Debug::Push();
		if(!qdir.mkdir(dir)) {
			Debug::Log(Debug::CRITICAL, "Could not create directory!");
			Debug::Pop();
			Debug::Pop();
			return false; //no point in proceeding
		}
		Debug::Pop();
	}

/*
	Any instance of tile index 0 on the map should not have other attributes. Rendering code checks for tile value 0 to skip render calls,
	and applying other attributes to tile entry with index 0 will reest this. The Toolkit should be preventing this from happening,
	but this is just in case and to fix legacy levels.
*/
#ifdef AREA_SAVE_MAP_TILE_0_REEST_CHECK
	bool passed = true;
	qDebug() << "PERFORMING MAP TILE #0 REEST CHECK:";
	for(unsigned i = 0; i <= Assets::LAYER_LIGHT; ++i) {
		for(unsigned y = 0; y < getMapHeight(); ++y) {
			for(unsigned x = 0; x < getMapWidth(); ++x) {
				if(getTileIndex(QPoint(x, y), (Assets::LAYER_TYPE)i) == 0 && getTileValue(QPoint(x, y), (Assets::LAYER_TYPE)i) != 0) {
					qDebug() << "\tDereesting Map Loc - <" << x << ", " << y << ">";
					setTileValue(QPoint(x, y), (Assets::LAYER_TYPE)i, 0);
					passed = false;
				}
			}
		}
	}
	qDebug() << QString("\t") << QString((passed)? "PASSED" : "FAILED");
#endif

	//save elysian::Area
	success &= _elysianArea.save(dir.toStdString().c_str());

    Debug::Pop();
	return success;
}

bool Area::activate(void) {
	if(!_open) open();
	if(!_active) {
		_curArea = &_elysianArea;

		editorScene->addItem(_bgRect);
        resizeEditorScene();
        editorScene->setSceneRect(QRect(-48,-48,32*_elysianArea.width+64, 32*_elysianArea.height+64));

		for(int i = 0; i < _entities.size(); ++i) editorScene->addItem(_entities[i]);
		//editorScene->addItem(_entityParent);
		_active = true;
	}
	emit activating(this);
	return true;
}

bool Area::deactivate(void) {
	if(!_active) return false;

	editorScene->removeItem(_bgRect);

	for(int i = 0; i < _entities.size(); ++i) editorScene->removeItem(_entities[i]);
	//editorScene->removeItem(_entityParent);
	_active = false;
	return true;
}

bool Area::open(void) {
	if(_open) return false;

	resizeEditorScene();

	_bgRect = new CheckeredAlphaRectGraphicsItem;
	_bgRect->setRect(QRect(-16, -16, 32*_elysianArea.width, 32*_elysianArea.height));
	_bgRect->setZValue(-1);

	//Populate Entities
	_open = true;
	return true;
}

bool Area::close(void) {
	//Debug::Log(Debug::VERBOSE, "Area[%s].Close()\n", getName().toStdString().c_str());
	if(!_open) return true;
	if(_active) deactivate();

	delete _bgRect;

	//Remove them from the _entityParent, so they aren't deleted with it.
	//  for(unsigned int i = 0; i < entity.size(); ++i) entity[i]->setParent(NULL);


	_open = false;
	return true;
}

Entity *Area::newEntity(void) {
	elysian::Entity *editorEntity = new elysian::Entity;
	_elysianArea.addEntity(editorEntity);
	Entity *toolkitEntity = new Entity(*editorEntity);
	connectSlots(toolkitEntity);
	_entities.push_back(toolkitEntity);
	//_elysianArea.entity.push_back(editorEntity);
	//toolkitEntity->setParentItem(_entityParent);
	if(_active) editorScene->addItem(toolkitEntity);
	emit entityAdded(this, toolkitEntity);
	return toolkitEntity;
}

bool Area::deleteEntity(const QString name) {
	for(int i = 0; i < _entities.size(); ++i) {
		if(_entities[i]->getName() == name) return deleteEntity(_entities[i]);
	}
	return false;
}

bool Area::deleteEntity(Entity *const pentity) {
	for(int i = 0; i < _entities.size(); ++i) {
		if(_entities[i] == pentity) {
			emit entityRemoved(this, pentity);

			for(unsigned int j = 0; j < _elysianArea.entities.size(); ++j) {
				if(_entities[i]->getElysianEntity() == _elysianArea.entities[j]) {
					_elysianArea.entities.erase(_elysianArea.entities.begin()+j);
				}
			}
			_entities.erase(_entities.begin()+i);
			delete pentity;
			return true;
		}
	}
	return false;
}

void Area::setMapWidth(const unsigned short w) {
	const unsigned short width = (w <= ELYSIAN_AREA_MAP_WIDTH)? (w < 30)? 30 : w : ELYSIAN_AREA_MAP_WIDTH;
	//_elysianArea.width = width;
	_elysianArea.resizeMap(width, _elysianArea.height);
	if(isActive()) { //We have to resize the scene for the current area
		resizeEditorScene();
	}
	_bgRect->setRect(QRect(-16, -16, 32*_elysianArea.width, 32*_elysianArea.height));
	editorScene->setSceneRect(QRect(-16,-16,32*_elysianArea.width, 32*_elysianArea.height));
}

void Area::setMapHeight(const unsigned short h) {
	const unsigned short height = (h <= ELYSIAN_AREA_MAP_HEIGHT)? (h < 30)? 30 : h : ELYSIAN_AREA_MAP_HEIGHT;
	_elysianArea.resizeMap(_elysianArea.width, height);
	if(isActive()) { //We have to resize the scene for the current area
		resizeEditorScene();
	}
	_bgRect->setRect(QRect(-16, -16, 32*_elysianArea.width, 32*_elysianArea.height));
	editorScene->setSceneRect(QRect(-16,-16,32*_elysianArea.width, 32*_elysianArea.height));
}

bool Area::isOpen(void) const { return _open; }
bool Area::isActive(void) const { return _active; }

Entity *Area::getFirstEntity(void) const {
	return (_entities.empty())? NULL : _entities[0];
}

void Area::onComponentAttached(Entity *const entity, Component *const comp) {
	emit componentAttached(this, entity, comp);
}

void Area::onComponentRemoved(Entity *const entity, Component *const comp) {
	emit componentRemoved(this, entity, comp);
}

void Area::onUndoCleanStateChanged(bool clean) {
    setChanged(clean);
}

QUndoStack &Area::getUndoStack(void) {
	return _undoStack;
}

elysian::Area &Area::getElysianArea(void) {
	return _elysianArea;
}


