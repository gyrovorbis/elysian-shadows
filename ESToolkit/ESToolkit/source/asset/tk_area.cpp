#include <QDir>
#include <QMessageBox>
#include <proxy/elysian_area_proxy.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <asset/tk_area.h>
#include "elysian_debug.hpp"
#include "tk_property_standard_item.h"
#include <tk_checkered_alpha_rect_graphics_item.h>
#include <tk_source_view.h>
#include "tk_map_view_depth_buffer_graphics_item.h"
#include <tk_file_utilities.h>
#include <QOpenGLContext>
#include <tk_engine_gl_scene_widget.h>
#include <tk_selection_scene.h>
#include <model/tk_property_item_delegate.h>
#include <tk_lua_wrappers.h>


ELYSIAN_LUA_PROXY_FIELDS_EXPORT(TkArea)


//====STATIC====
SelectionScene*                 Area::_editorScene              = nullptr;
unsigned int                    Area::_currentSceneWidth        = 0;
unsigned int                    Area::_currentSceneHeight       = 0;
bool                            Area::_inactiveTransparency     = false;
MapViewDepthBufferGraphicsItem* Area::_depthBuffGraphicsItem    = nullptr;

bool Area::staticConstructor(const SourceView *const view) {
    _editorScene = new SelectionScene;
    _editorScene->setSceneRect(0,0,2,2);

    //Allocate a MapViewTerrainItem for 4 different layers
    _depthBuffGraphicsItem = new MapViewDepthBufferGraphicsItem;
    _depthBuffGraphicsItem->setView(view);

    resizeEditorScene();
    //editorScene->addItem(_depthBuffGraphicsItem);

    return true;
}

void Area::updateScene(void) {
    _editorScene->update();
}

//====NONSTATIC====

TkArea::TkArea(Level *const lev) {
    this->level = lev;
    width = height = 50;
    //Must allocate a map in case this map is to be saved before it is loaded.
    allocMap();
    memset(tiles, 0, sizeof(elysian::TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS);
    //memset(_elysianArea.lights, 0, sizeof(elysian::LightEntry)*_elysianArea.width*_elysianArea.height);
    strcpy(dir, "untitled");
    strcpy(string, "untitled");
    connect(&_undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoCleanStateChanged(bool)));
}

QAbstractItemModel* TkArea::getModel(void) const {
    if(!_luaModel) {
        _luaModel = new LuaTableStackModel(const_cast<TkArea*>(this));
        _luaModel->populateModel();
    }
    return _luaModel;
}

TkArea::~TkArea(void) {
    //deactivate();
    //close();
}

void Area::resizeEditorScene(void) {
    _depthBuffGraphicsItem->prepareResize();
}

/*
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

}*/

void Area::setNodeName(const QString str) {
    QString newLocation;
    bool _renamed = false;
    QString oldName = getNodeName();
    QString name = str;
    name.replace(QRegExp("[\\\\/ ]"), QString("_"));

    if(getNodeName() == name) return;

    QStringList splitList = _location.split("/");
    for(int i = 0; i < splitList.size()-1; ++i) {
        newLocation.append(splitList[i]+"/");
    }
    newLocation.append(name);

    //Already has a name, so we have to rename it
    if(!getNodeName().isEmpty())  {

        if(level->getArea(name)) {
            QMessageBox::critical(nullptr, tr("Error!"), tr("An area with that name already exists in the current level!\n"));
            return;
        }

        QString areaDir = level->getLocation();
        areaDir.append("/area");

        QDir dir(areaDir);
        if(!dir.rename(getNodeName(), name)) {
            QMessageBox::critical(nullptr, tr("Error!"), tr("Unable to rename the directory!\n"));
            return;
        }

        _renamed = true;
    }

    TreeNode::setNodeName(name);
    _location = newLocation;
    strcpy(dir, name.toStdString().c_str());
    strcpy(path, _location.toStdString().c_str());

    if(_renamed) Q_EMIT renamed(oldName, name);
}

bool Area::duplicate(void) {
    DbgEvent event("Duplicating Area [%s].", Q_CSTR(getNodePath()));
    unsigned num = 1;
    QString path(getNodePath());

    while(level->getArea(getNodePath()+QString::number(num))) ++num;

    if(!FileUtilities::copyDir(path, path+QString::number(num))) {
        QMessageBox::critical(nullptr, "Error!", "Could not copy area directory!");
        return false;
    } else {
        dbgprintf("Successfully copied area directory [%s].", Q_CSTR((getNodePath()+QString::number(num))));
    }
    //saveAs(_location+QString::number(num));

    return level->loadArea(path + QString::number(num));
}

void Area::setNodePath(const QString path) {
    _location = path;
    strcpy(elysian::AreaBase::path, Q_CSTR(path));
    TreeNode::setNodePath(path);
}


void Area::setLocation(const QString loc) {
    _location = loc;

    QStringList splitList = _location.split("/");
    setNodeName(splitList.last());
    setNodePath(loc);
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

void Area::_connectSlots(Entity *const entity) {
    //connect(entity, SIGNAL(componentAttached(Entity*const,Component*const)), this, SLOT(onComponentAttached(Entity*const,Component*const)));
    //connect(entity, SIGNAL(componentRemoved(Entity*const,Component*const)), this, SLOT(onComponentRemoved(Entity*const,Component*const)));
    connect(entity, SIGNAL(entityParented(Entity*const,Entity*const)), this, SLOT(onEntityParented(Entity*const,Entity*const)));
}

void Area::_disconnectSlots(Entity * const entity) {
    entity->disconnect(SIGNAL(entityParented(Entity*const,Entity*const)));
    //entity->disconnect(SIGNAL(activated()));
    //entity->disconnect(SIGNAL(deactivated()));

}

bool TkArea::load(const char*const str) {
    DbgEvent event("Indexing Area [%s]", str);
    level->connectSlots(this);
    QString dir(str);
    bool success = true;

    _location = dir;

    //Allocated a dummy map upon construction, have to free before loading.
    //freeMap();

    //level = _level;

    setNodeName(QDir(dir).dirName());
    setNodePath(QDir(dir).absolutePath());
#if 0
    Debug::Log(Debug::LOG_VERBOSE, "Creating Toolkit Entities from elysian::Entities.");
    Debug::push();

    for(unsigned int i = 0; i < _elysianArea.entities.size(); ++i) {
        Debug::Logf(Debug::LOG_VERBOSE, "Entity #%d - %s.", i, _elysianArea.entities[i]->getName());
        Entity *tempEntity = new Entity(*_elysianArea.entities[i]);
        connectSlots(tempEntity);
        _entities.push_back(tempEntity);
        //tempEntity->setParentItem(_entityParent);
    }
#endif
    return success;//success;
}

bool Area::save(void) {
    DbgEvent event("Saving Area [%s].", Q_CSTR(getNodePath()));
    bool success = true;
    //Save parent level
    success &= level->save();
    //export to previous save location
    success &= saveAs(getNodePath());
    _undoStack.setClean();
    return success;
}

bool Area::saveAs(const QString path) {
    DbgEvent event("Saving Area [%s].", Q_CSTR(path));
    bool success = true;
    bool newLoc = (_location != path)? true : false;
    QString dir = FileUtilities::getAbsoluteAssetPath(path);
    QDir qdir;

    //Check Save As and directory already exists
    if(newLoc && qdir.exists(dir)) {
        DbgEvent event("Directory already exists!");
        if(QMessageBox::critical(nullptr, tr("Warning!"), tr("A directory of the same name already exists at the specified location. Overwrite?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if(!qdir.rmdir(dir)) {
                Debug::Log(Debug::LOG_ERROR, "Could not delete existing directory!");
                return false;
            } else {
                dbgprintf("Directory successfully deleted.");
            }
        }
        else {
            Debug::Log(Debug::LOG_VERBOSE, "Directory overwrite canceled.");
            return true; //User canceled, still a success
        }
    }

    //create main area directory if it doesn't already exist
    if(!qdir.exists(dir)) {
        DbgEvent event("Creating directory.");
        if(!qdir.mkdir(dir)) {
            errprintf("Could not create directory!");
            return false; //no point in proceeding
        }
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

    //Save all entities within current area
    std::vector<Entity*> oldEntities            = entities;
    std::vector<Entity*> oldInactiveEntities    = inactiveEntities;

    auto removeTempEntities = [&](std::vector<Entity*>& list) {
        bool allActive;
        do {
            allActive = true;
            for(auto it = list.begin(); it != list.end(); ++it) {
                if(!(*it)->getSaveFlag()) {
                    list.erase(it);
                    allActive = false;
                    break;
                }

            }
        } while(!allActive);
    };

    //Remove all temporary entities, leaving only shit that should be saved with the area
    removeTempEntities(entities);
    removeTempEntities(inactiveEntities);

    //save elysian::Area
    success &= AreaBase::save(dir.toStdString().c_str());

    //Restore all temporary entities
    entities = oldEntities;
    inactiveEntities = oldInactiveEntities;

    return success;
}

bool Area::activate(void) {
    if(!_open) open();
    Q_EMIT activating(this);
    if(!_active) {
        DbgEvent event("Activating Area [%s].", Q_CSTR(_location));
       EngineGLSceneWidget::makeCurrentContext();
        AreaBase::activate();

        //EngineGLSceneWidget::doneCurrentContext();
        //editorScene->addItem(_bgRect);
        //resizeEditorScene();
        //editorScene->setSceneRect(QRect(-48,-48,32*width+64, 32*height+64));

        //for(size_t i = 0; i < entities.size(); ++i) editorScene->addItem(entities[i]);
        //editorScene->addItem(_entityParent);
        _active = true;
    }
    return true;
}

bool Area::deactivate(void) {
    if(!_active) return false;

    //editorScene->removeItem(_bgRect);

    //for(size_t i = 0; i < entities.size(); ++i) editorScene->removeItem(entities[i]);
    //editorScene->removeItem(_entityParent);
    _active = false;
    return true;
}

bool Area::open(void) {
    bool success = true;
    if(_open) return true;
    DbgEvent event("Opening Area [%s].", Q_CSTR(_location));

    EngineGLSceneWidget::makeCurrentContext();

    success &= AreaBase::load(_location.toStdString().c_str());

    //if we just got done loading them, obviously they're savable...
    for(auto& it: entities) it->setSaveFlag(true);
    for(auto& it: inactiveEntities) it->setSaveFlag(true);

    setValid(success);
    Q_EMIT validChanged(success);

    resizeEditorScene();
/*
    _bgRect = new CheckeredAlphaRectGraphicsItem;
    _bgRect->setRect(QRect(-16, -16, 32*width, 32*height));
    _bgRect->setZValue(-1);
*/
    //EngineGLSceneWidget::doneCurrentContext();

    //Populate Entities
    _open = true;
    return success;
}

bool Area::close(void) {
    //Debug::Log(Debug::LOG_VERBOSE, "Area[%s].Close()\n", getName().toStdString().c_str());
    if(!_open) return true;
    if(_active) deactivate();

    //delete _bgRect;

    //Remove them from the _entityParent, so they aren't deleted with it.
    //  for(unsigned int i = 0; i < entity.size(); ++i) entity[i]->setParent(NULL);


    _open = false;
    return true;
}

//can't really be called unless it's being added to the current Area? Guess that's cool?
Entity* Area::newEntity(void) {
    Entity* entity = new Entity;

    QString name("Untitled");
    unsigned int num = 1;

    while(findEntity(QString(name+QString::number(num)).toStdString().c_str())) {
        ++num;
    }
    name += QString::number(num);
    entity->setName(name.toStdString().c_str());
    _connectSlots(entity);

    addInactiveEntity(entity);
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
            Q_EMIT entityRemoved(this, pentity);

            removeEntity(entities[i]);
            delete pentity;
            return true;
        }
    }
    return false;
}

void Area::setMapWidth(const unsigned short w) {
    const unsigned short _width = (w <= ELYSIAN_AREA_MAP_WIDTH)? (w < 30)? 30 : w : ELYSIAN_AREA_MAP_WIDTH;

    if(w < width && QMessageBox::critical(nullptr, tr("Warning!"), tr("This operation cannot be undone! Continue?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    //_elysianArea.width = width;
    resizeMap(_width, height);
    /*if(isActive()) { //We have to resize the scene for the current area
        resizeEditorScene();
    }*/
    //_bgRect->setRect(QRect(-16, -16, 32*width, 32*height));
    //_editorScene->setSceneRect(QRect(-16,-16,32*width, 32*height));
}

void Area::setMapHeight(const unsigned short h) {
    const unsigned short _height = (h <= ELYSIAN_AREA_MAP_HEIGHT)? (h < 30)? 30 : h : ELYSIAN_AREA_MAP_HEIGHT;

    if(h < height && QMessageBox::critical(nullptr, tr("Warning!"), tr("This operation cannot be undone! Continue?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    resizeMap(width, _height);
    /*if(isActive()) { //We have to resize the scene for the current area
        resizeEditorScene();
    }*/
    //_bgRect->setRect(QRect(-16, -16, 32*width, 32*height));
    //_editorScene->setSceneRect(QRect(-16,-16,32*width, 32*height));
}

Entity* Area::getFirstEntity(void) const {
    return (entities.empty())? nullptr : entities[0];
}

void Area::onComponentAttached(Entity *const entity, Component *const comp) {
    Q_EMIT componentAttached(this, entity, comp);
}

void Area::onComponentRemoved(Entity *const entity, Component *const comp) {
    Q_EMIT componentRemoved(this, entity, comp);
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

void TkArea::onEntityParented(Entity * const child, Entity * const parent) {
    Q_EMIT entityParented(child, parent);
}

void TkArea::addEntity(Entity *entity) {
    const unsigned count = entities.size();
    elysian::AreaBase::addEntity(entity);
    if(count != entities.size()) {
        _connectSlots(entity);
         Q_EMIT entityAdded(this, entity);
    }
}

void TkArea::addInactiveEntity(Entity *entity) {
    const unsigned count = inactiveEntities.size();
    elysian::AreaBase::addInactiveEntity(entity);
    if(count != inactiveEntities.size()) {
        _connectSlots(entity);
        Q_EMIT entityAdded(this, entity);
    }
}

void TkArea::removeEntity(Entity *entity) {
    elysian::AreaBase::removeEntity(entity);
    _disconnectSlots(entity);
    Q_EMIT entityRemoved(this, entity);
}

void TkArea::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
    /*
    auto* normalDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, normalDelegate);
    lua_setfield(*_mainState, -2, "normalMap");
*/
    //auto* widthDelegate = new SpinboxPropertyItemDelegate(0, ELYSIAN_AREA_MAP_WIDTH);
    //OOLUA::push(*_mainState, widthDelegate);
    //lua_setfield(*_mainState, -2, "lkkfsdklj;fsdalk;jasdfl;kj");

    lua_pop(*_mainState, 1);

}
