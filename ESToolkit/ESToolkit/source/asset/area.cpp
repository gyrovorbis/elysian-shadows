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
SelectionScene *Area::editorScene                               = nullptr;
unsigned int Area::_currentSceneWidth                           = 0;
unsigned int Area::_currentSceneHeight                          = 0;
bool Area::inactiveTransparency                                 = false;
MapViewDepthBufferGraphicsItem	*Area::_depthBuffGraphicsItem   = nullptr;

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

TkArea::TkArea(Level *const level):
    _level(level),
    _active(false),
    _open(false)
{

    width = height = 50;
	//Must allocate a map in case this map is to be saved before it is loaded.
    allocMap();
    memset(tiles, 0, sizeof(elysian::TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS);
    //memset(_elysianArea.lights, 0, sizeof(elysian::LightEntry)*_elysianArea.width*_elysianArea.height);
    strcpy(dir, "untitled");
    strcpy(string, "untitled");
    connect(&_undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoCleanStateChanged(bool)));
}

TkArea::~TkArea(void) {
	close();
}

void Area::resizeEditorScene(void) {
	_depthBuffGraphicsItem->prepareResize();
}

void Area::populateModel(void) {
	clearStack();

	insertHeaders();

	pushLabel("Area");

    addProperty("name", new PropertyStandardItem<QString, Area, true>(this, &Area::getNodeName, &Area::setNodeName, UndoItem::AREA_RENAME, &_undoStack));
	addProperty("path", new PropertyStandardItem<QString, Area>(this, &Area::getLocation));
	addProperty("display", new PropertyStandardItem<QString, Area, true>(this, &Area::getString, &Area::setString, UndoItem::AREA_STRING, &_undoStack));

	pushLabel("size");
		addProperty("width", new PropertyStandardItem<unsigned short, Area, true>(this, &Area::getMapWidth, &Area::setMapWidth, UndoItem::AREA_WIDTH, &_undoStack));
		addProperty("height", new PropertyStandardItem<unsigned short, Area, true>(this, &Area::getMapHeight, &Area::setMapHeight, UndoItem::AREA_HEIGHT, &_undoStack));
	popLabel();

	popLabel();

}

void Area::setNodeName(const QString str) {
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

    TreeNode::setNodeName(name);
	_location = newLocation;
    strcpy(dir, name.toStdString().c_str());
    strcpy(path, _location.toStdString().c_str());

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
    setNodeName(splitList.last());
}

void Area::setString(const QString str) {
	if(str.isEmpty()) return;
    strcpy(string, str.toStdString().c_str());
}

QString Area::getString(void) const {
    return QString(string);
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
    _curArea = this;
	Debug::Logf(Debug::VERBOSE, "Area::Load(%s)", dir.toStdString().c_str());
	Debug::Push();

    setNodeName(QDir(dir).dirName());
	_location = dir;

	//Allocated a dummy map upon construction, have to free before loading.
    freeMap();
    success &= AreaBase::load(dir.toStdString().c_str(), (elysian::Level*)_level->getElysianLevel());

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
    success &= AreaBase::save(dir.toStdString().c_str());

    Debug::Pop();
	return success;
}

bool Area::activate(void) {
	if(!_open) open();
	if(!_active) {
        _curArea = this;

		editorScene->addItem(_bgRect);
        resizeEditorScene();
        editorScene->setSceneRect(QRect(-48,-48,32*width+64, 32*height+64));

		for(size_t i = 0; i < entities.size(); ++i) editorScene->addItem(entities[i]);
		//editorScene->addItem(_entityParent);
		_active = true;
	}
	emit activating(this);
	return true;
}

bool Area::deactivate(void) {
	if(!_active) return false;

	editorScene->removeItem(_bgRect);

	for(size_t i = 0; i < entities.size(); ++i) editorScene->removeItem(entities[i]);
	//editorScene->removeItem(_entityParent);
	_active = false;
	return true;
}

bool Area::open(void) {
	if(_open) return false;

	resizeEditorScene();

	_bgRect = new CheckeredAlphaRectGraphicsItem;
    _bgRect->setRect(QRect(-16, -16, 32*width, 32*height));
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

//can't really be called unless it's being added to the current Area? Guess that's cool?
Entity* Area::newEntity(void) {
    auto* entity = new Entity;

    QString name("Untitled");
    unsigned int num = 1;

    entity->activate();
    connectSlots(entity);
    while(findEntity(QString(name+QString::number(num)).toStdString().c_str())) {
        ++num;
    }
    name += QString::number(num);
    entity->setName(name);

    if(_active) editorScene->addItem(entity);
    emit entityAdded(this, entity);
    return entity;
}

bool Area::deleteEntity(const QString name) {
	for(size_t i = 0; i < entities.size(); ++i) {
        if(entities[i]->getName() == name) return deleteEntity(entities[i]);
	}
	return false;
}

bool Area::deleteEntity(Entity *const pentity) {
	for(size_t i = 0; i < entities.size(); ++i) {
        if(entities[i] == pentity) {
			emit entityRemoved(this, pentity);

            removeEntity(entities[i]);
			delete pentity;
			return true;
		}
	}
	return false;
}

void Area::setMapWidth(const unsigned short w) {
    const unsigned short _width = (w <= ELYSIAN_AREA_MAP_WIDTH)? (w < 30)? 30 : w : ELYSIAN_AREA_MAP_WIDTH;
	//_elysianArea.width = width;
    resizeMap(_width, height);
	if(isActive()) { //We have to resize the scene for the current area
		resizeEditorScene();
	}
    _bgRect->setRect(QRect(-16, -16, 32*width, 32*height));
    editorScene->setSceneRect(QRect(-16,-16,32*width, 32*height));
}

void Area::setMapHeight(const unsigned short h) {
    const unsigned short _height = (h <= ELYSIAN_AREA_MAP_HEIGHT)? (h < 30)? 30 : h : ELYSIAN_AREA_MAP_HEIGHT;
    resizeMap(width, _height);
	if(isActive()) { //We have to resize the scene for the current area
		resizeEditorScene();
	}
    _bgRect->setRect(QRect(-16, -16, 32*width, 32*height));
    editorScene->setSceneRect(QRect(-16,-16,32*width, 32*height));
}

Entity* Area::getFirstEntity(void) const {
    return (entities.empty())? nullptr : entities[0];
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

elysian::Area& Area::getElysianArea(void) {
    return *this;
}


