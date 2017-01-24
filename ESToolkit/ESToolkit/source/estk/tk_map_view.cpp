#include <QMouseEvent>
#include <QDebug>
#include <QMenu>
#include <QPainterPath>
#include <QScrollBar>

#include "tk_map_view.h"
#include "tk_selection_item_group.h"
#include "tk_tile_selectable.h"
#include <asset/tk_area.h>
#include "tk_map_view_place_tile_group_undo_item.h"
#include "tk_assets.h"
#include "elysian_engine.hpp"
#include "crtp/elysian_camera_system.hpp"
#include "tk_engine_gl_scene_widget.h"
#include <asset/tk_collidable.h>
#include <asset/tk_entity.h>
#include "tk_entity_selectable.h"
#include <asset/tk_renderable.h>
#include <asset/tk_level.h>
#include <core/tk_mime_data.h>
#include <proxy/elysian_entity_proxy.hpp>
#include <core/tk_mime_drop_manager.h>
#include <core/tk_file_utilities.h>
#include <QFileInfo>


MapView::MapView(QWidget *parent): DestView<TileEntry>::DestView(parent) {
    setMouseTracking(true);
    setAcceptDrops(true);
    setEventMask(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK);

    _curUndoItem = new MapViewPlaceTileGroupUndoItem();

    _registerDropEventCallbacks();

    connect(getAssets(), SIGNAL(areaChanged(Area*const)), this, SLOT(onAreaChange(Area*const)));
    connect(getAssets(), SIGNAL(areaChanging(Area*const)), this, SLOT(onAreaChanging(Area*const)));
    connect(getAssets(), SIGNAL(layerChanged(LAYER_TYPE)), this, SLOT(onLayerChange(LAYER_TYPE)));
    connect(getAssets(), SIGNAL(engineInitialize()), this, SLOT(onEngineInitialize()));
    connect(getAssets(), SIGNAL(mapChanged(Area*const,const QVector<MapChange>&)), this, SLOT(onMapChange(Area*const,const QVector<MapChange>&)));
}

MapView::~MapView(void) {
    delete _mimeDropManager;
}

void MapView::onLayerChange(Assets::LAYER_TYPE type) {
    //update current undoGroupItem
    _curUndoItem->setLayer(type);

    if(type == Assets::LAYER_ENTITY) {
        getAssets()->getEngine()->getCollisionSys()->registerContactEventMonitor(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK, this);
    } else {
        getAssets()->getEngine()->getCollisionSys()->unregisterContactEventMonitor(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK);
    }
}

void MapView::onAreaChanging(Area * const /*area*/) {
    if(getAssets()->getCurrentArea()) getAssets()->getCurrentArea()->removeEntity(_cursorEntity);
}

void MapView::onAreaChange(Area * const area) {
    if(area && area->getLevel()->validSheets()) {
        _curUndoItem->setArea(area);
    }

    if(area) {
        area->addEntity(_cursorEntity);
        _updateScrollBars();
    }

    _entityClearSceneSelection();
    _underCursorEntity = nullptr;
}

void MapView::onMapChange(Area*const area, const QVector<MapChange>& changes) {
    area->setEmitMapChange(false);
    if(getAssets()->getCurrentArea() == area) {
        _curUndoItem->onMapChanged(changes);
    }
    area->setEmitMapChange(true);
}

void MapView::onEngineInitialize(void) {
    _cursorEntity = new TkEntity(false);
    _cursorEntity->setPlaneType(GY_RECT_PLANE_BOTTOM);
    auto* collid = _cursorEntity->addCollidable();
    auto* colAttrib = new elysian::CollidableAttrib();
    auto* render = _cursorEntity->addRenderable();
    auto* sprAttrib = new elysian::SpriteAttrib;
    auto* mat = new elysian::Material;
    mat->setEmissiveColor(GYVector4 {4.0f, 0.0f, 0.0f, 0.0f});
    sprAttrib->setMaterial(mat);
    sprAttrib->setColor(GYColor4{ 255, 0, 0, 255});
    render->setSpriteAttrib(sprAttrib);
    colAttrib->getFlags()->setType(0);
    colAttrib->getFlags()->getType().setValue(elysian::CollisionFlags::CURSOR);
    colAttrib->getFlags()->setTypeAgainst(0);
    colAttrib->getFlags()->getTypeAgainst().setValue(elysian::CollisionFlags::CURSOR);
    collid->setAttrib(colAttrib);
    collid->setWorldSpace(false);
    collid->setResolvePenetration(false);
    collid->setContactEventMonitorMask(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK);
    _cursorEntity->getTransform().setSize(2.0f, 2.0f, 2.0f);
    _cursorEntity->setName("_ESTkCursor");
}

void MapView::onEngineUpdated(void) {
    _zoomLabel->setText(QString::number((int)(getAssets()->getEngine()->getCameraSys()->getKeyFrame().getScale().x*getAssets()->getEngine()->getCameraSys()->getStaticScale()*100.0f))+QString("%"));
}

TileSelection<TileEntry> *MapView::_getTileSelection(void) const {
    return static_cast<TileSelectable*> (
                (getCurrentSelectionType() == Selectable::TILE)? getCurrentSelection() : nullptr
    );
}

void MapView::_entitySceneSelectionUpdate(void) {
    TkEntity* entity;

    for(auto &it: _selectionColliders) {
        getCurrentArea()->removeEntity(it);
    }

    while(_selectionColliders.size() < _tileSelectionRects.size()) {
        entity = new TkEntity(false);
        entity->setPlaneType(GY_RECT_PLANE_BOTTOM);
        entity->setName("_ESTkEntitySelection");
        auto* collid = entity->addCollidable();
        auto* render = entity->addRenderable();
        auto* sprAttrib = new elysian::SpriteAttrib;
        sprAttrib->setColor(GYColor4{ 0, 255, 255, 25});
        render->setSpriteAttrib(sprAttrib);
        auto* mat = new elysian::Material;
        mat->setEmissiveColor(GYVector4{0.0f, 1.0f, 1.0f, 0.0f});
        sprAttrib->setMaterial(mat);

        auto* const colAttrib = new elysian::CollidableAttrib;

        colAttrib->getFlags()->setType(0);
        colAttrib->getFlags()->getType().setValue(elysian::CollisionFlags::CURSOR);
        colAttrib->getFlags()->setTypeAgainst(0);
        colAttrib->getFlags()->getTypeAgainst().setValue(elysian::CollisionFlags::CURSOR);


        collid->setAttrib(colAttrib);
        collid->setWorldSpace(false);
        collid->setResolvePenetration(false);
        collid->setContactEventMonitorMask(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK);
        _selectionColliders.push_back(entity);
    }

    for(int k = 0; k < _tileSelectionRects.size(); ++k) {
        auto& rect = _tileSelectionRects[k];
        auto midPt = rect.bottomRight() + rect.topLeft();
        midPt /= 2.0f;

        entity = _selectionColliders[k];
        entity->getTransform().setPos(midPt.x(), midPt.y(), 0.0f);
        entity->getTransform().setSize(rect.width(), rect.height(), rect.height()+600.0f);
        getCurrentArea()->addEntity(_selectionColliders[k]);
    }
    _entityDrag = false;

}

void MapView::_entitySelectableCreate(void) {
    if(_sceneSelectedEntities.size() > 0) {
        auto* select = new EntitySelectable;
        for(auto it = _sceneSelectedEntities.begin(); it != _sceneSelectedEntities.end(); ++it) {
            select->addEntity(*it);
        }
        _entityClearSceneSelection();

        getAssets()->setCurrentSelection(select);
    } else {
        getAssets()->setCurrentSelection(nullptr);
    }

    for(auto &it: _selectionColliders) {
        getCurrentArea()->removeEntity(it);
    }
    //_selectionColliders.clear();
}

void MapView::_tileSelectionArrayUpdate(void) {

    //clear selection
    for(unsigned j = 0; j < getCurrentArea()->getMapHeight(); ++j) {
        for(unsigned i = 0; i < getCurrentArea()->getMapWidth(); ++i) {
            _tileSelectionArray[j][i] = false;
        }
    }

    //iterate through rectangles and create selection
    for(int k = 0; k < _tileSelectionRects.size(); ++k) {
        int startX = (_tileSelectionRects[k].topLeft().x()+16)/32;
        int startY = (_tileSelectionRects[k].topLeft().y()+16)/32;
        int endX = (_tileSelectionRects[k].bottomRight().x()+16)/32;
        int endY = (_tileSelectionRects[k].bottomRight().y()+16)/32;

        //clamp region to map boundaries
        if(startX < 0) startX = 0;
        else if(startX >= getCurrentArea()->getMapWidth()) startX = getCurrentArea()->getMapWidth()-1;
        if(startY < 0) startY = 0;
        else if(startY >= getCurrentArea()->getMapHeight()) startY = getCurrentArea()->getMapHeight()-1;

        if(endX < 0) endX = 0;
        else if(endX >= getCurrentArea()->getMapWidth()) endX = getCurrentArea()->getMapWidth()-1;
        if(endY < 0) endY = 0;
        else if(endY >= getCurrentArea()->getMapHeight()) endY = getCurrentArea()->getMapHeight()-1;


        //sync array with rectangular selection
        for(int j = startY; j <= endY; ++j) {
            for(int i = startX; i <= endX; ++i) {
                _tileSelectionArray[j][i] = true;
            }
        }

        if(isFillingRegion()) {
            getAssets()->getCurrentArea()->setEmitMapChange(false);
            _curUndoItem->beginUpdateEntries();
            for(int j = startY; j <= endY; ++j) {
                for(int i = startX; i <= endX; ++i) {
                    if(_tileSelectionArray[j][i]) {
                        auto* sel = static_cast<TileSelectable*>(_getTileSelection());
                        auto newTileEntry = sel->getFillEntry(i, j, _fillFixedPivot, getSceneMousePos());
                        if(sel->isWithinSelectionFill(i, j, _fillFixedPivot, getSceneMousePos()))
                        _curUndoItem->addTile(QPoint(i, j), newTileEntry);
                    }
                }
            }
            _curUndoItem->endUpdateEntries();
            getAssets()->getCurrentArea()->setEmitMapChange(true);
        }

        if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) {
            _entitySceneSelectionUpdate();
        }
    }
}

void MapView::_sceneSelectionAddRect(const QRectF& rect) {
    //If it's too small, don't select (to fall through into pan mode)
    if(rect.right() - rect.left() < 2 &&
            rect.bottom() - rect.top() < 2) return;

    _tileSelectionRects.push_back(rect);
    _tileSelectionArrayUpdate();
    //scene()->update(scene()->sceneRect());
    _forceSceneUpdate();
}

void MapView::_sceneSelectionUpdateLastRect(const QRectF& rect) {
    if(_tileSelectionRects.size() >= 1) _tileSelectionRects.last() = rect;
    else _tileSelectionRects.push_back(rect);
    _tileSelectionArrayUpdate();
    //scene()->update(scene()->sceneRect());
    _forceSceneUpdate();
}

void MapView::_sceneSelectionClear(void) {
    //if(_getTileSelection()) static_cast<TileSelectable*>(_getTileSelection())->clearSceneSelection();
    _tileSelectionRects.clear();
    _entityClearSceneSelection();
    memset(_tileSelectionArray, 0, sizeof(_tileSelectionArray));
    //scene()->update(scene()->sceneRect());
    getAssets()->getCurrentArea()->setEmitMapChange(false);
    _curUndoItem->clearEntries();
    getAssets()->getCurrentArea()->setEmitMapChange(true);
    _forceSceneUpdate();
}

void MapView::_externalSelectionCreate(void) {
    if(_tileSelectionRects.size() <= 0) return;
    if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) return _entitySelectableCreate();

    int startX = ELYSIAN_AREA_MAP_WIDTH-1;
    int endX = 0;
    int startY = ELYSIAN_AREA_MAP_HEIGHT-1;
    int endY = 0;

    for(int i = 0; i < _tileSelectionRects.size(); ++i) {
        if(startX > (_tileSelectionRects[i].topLeft().x()+16)/32)	startX =  (_tileSelectionRects[i].topLeft().x()+16)/32;
        if(endX < (_tileSelectionRects[i].bottomRight().x()+16)/32) endX = (_tileSelectionRects[i].bottomRight().x()+16)/32;
        if(startY > (_tileSelectionRects[i].topLeft().y()+16)/32)	startY = (_tileSelectionRects[i].topLeft().y()+16)/32;
        if(endY < (_tileSelectionRects[i].bottomRight().y()+16)/32) endY = (_tileSelectionRects[i].bottomRight().y()+16)/32;
    }
    if(startX < 0) startX = 0;
    if(startY < 0) startY = 0;
    if(endX >= getAssets()->getCurrentArea()->width) endX = getAssets()->getCurrentArea()->width;
    if(endY >= getAssets()->getCurrentArea()->height) endY = getAssets()->getCurrentArea()->height;

    TileSelectable *selection = new TileSelectable(QRect(startX, startY, endX-startX+1, endY-startY+1), _tileSelectionArray, getCurrentArea(), getCurrentLevel(), getAssets()->getCurrentLayer());
    setCurrentSelection(selection);
}

void MapView::_externalSelectionClear(void) {
    getAssets()->getCurrentArea()->setEmitMapChange(false);
    _curUndoItem->clearEntries();
    getAssets()->getCurrentArea()->setEmitMapChange(true);
    setCurrentSelection(nullptr);
}

void MapView::_setTileSelection(TileSelection<TileEntry>* const sel) {
    if(sel && getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) getAssets()->setCurrentLayer(Assets::LAYER_TILE1);
    setCurrentSelection(static_cast<TileSelectable *>(sel));
}

QUndoStack* MapView::_getUndoStack(void) const {
    return &getCurrentArea()->getUndoStack();
}

PlaceTileGroupUndoItem<TileEntry>* MapView::_createPlaceTileGroup(void) {
     auto* curGroup = _curUndoItem;
     _curUndoItem = new MapViewPlaceTileGroupUndoItem(getAssets()->getCurrentArea(), getAssets()->getCurrentLayer());
    return curGroup;
}

TileSelection<TileEntry>* MapView::_copyConstructTileSelection(const TileSelection<TileEntry>& rval) const {
    return new TileSelectable(*static_cast<const TileSelectable*>(&rval));
}


void MapView::timerEvent(QTimerEvent* event) {
    if(event->timerId() == _refreshTimerHandle) {
        _forceSceneUpdate();
    }
}

void MapView::keyPressEvent(QKeyEvent* event) {
    DestView<TileEntry>::keyPressEvent(event);

    //Check for MapView-specific key events
    if(_selItemGroup) {
        TileSelectable *terSel = static_cast<TileSelectable*>(_getTileSelection());
        if(terSel) {
            switch(event->key()) {
            case Qt::Key_Equal:
                terSel->elevateFromBase();
                //createSelectionItemGroup(); //force update region
                break;
            case Qt::Key_Plus:
                terSel->elevateUp();
                //createSelectionItemGroup(); //force update region
                break;
            case Qt::Key_Minus:
                terSel->elevateDown();
                //createSelectionItemGroup(); //force update region
                break;
            case Qt::Key_Z:
                //terSel->toggleZTransitional();
                //createSelectionItemGroup();
                break;
            case Qt::Key_N:
                terSel->toggleNormalFace();
                //createSelectionItemGroup();
                break;
            }
            mouseMoveEvent(nullptr);
        }
    } else if(getCurrentSelectionType() == Selectable::ENTITY) {
        auto* entitySel = static_cast<EntitySelectable*>(getCurrentSelection());

        switch(event->key()) {
        case Qt::Key_E:
            entitySel->setPosition(_sceneMousePos.x(), _sceneMousePos.y());
            break;
        }
#if 0
        for(auto it = entitySel->begin(); it != entitySel->end(); ++it) {
            switch(event->key()) {
            case Qt::Key_Left:
              //  (*it)->_transform.rotate(-0.1f);
                break;
            case Qt::Key_Right:
             //   (*it)->_transform.rotate(0.1f);
                break;

            }
        }
#endif
    }
}

bool MapView::_isEntityRegionSelecting(void) const { return _selectionColliders.size() && _selectionColliders[0]->isActive(); }

void MapView::wheelEvent(QWheelEvent* event) {
    const float delta = (event->delta() > 0)? 1.05f : 0.95f;
    //setTransformationAnchor();
    //scale(delta, delta);
    getAssets()->getEngine()->getCameraSys()->scale(delta);
    //_scheduleMousePosUpdate();
    //_zoomLabel->setText(QString::number((int)(getAssets()->getEngine()->getCameraSys()->getKeyFrame().getScale()*getAssets()->getEngine()->getCameraSys()->getStaticScale()*100.0f))+QString("%"));
    mouseMoveEvent(nullptr);

    event->ignore();
}

void MapView::mouseMoveEvent(QMouseEvent *event) {

    //DIRTY-ASS HACK TO KEEP SHIT SYNCED WITH ENGINE
    //_zoomLabel->setText(QString::number((int)(getAssets()->getEngine()->getCameraSys()->getKeyFrame().getScale()*getAssets()->getEngine()->getCameraSys()->getStaticScale()*100.0f))+QString("%"));
    auto* sel = static_cast<TileSelectable*>(_getTileSelection());
    auto p1 = _panPivot-_viewMousePos;

    DestView<TileEntry>::mouseMoveEvent(event);
    if(_cursorEntity) _cursorEntity->getTransform().setPos(_sceneMousePos.x(), _sceneMousePos.y());

    if(sel && !isFillingRegion()) {

        unsigned w, h;
        sel->getSelectionArea(w, h);

        //MUST BE THE CURRENT CONTEXT, OR MAP TILES WILL NOT UPDATE BASED ON SELECTION!!!11
        //EngineGLSceneWidget::makeCurrentContext();
        getAssets()->getCurrentArea()->setEmitMapChange(false);
        _curUndoItem->beginUpdateEntries();

        for(unsigned j = 0; j < h; ++j) {
            for(unsigned i = 0; i < w; ++i) {
                auto newTileEntry = sel->getEntry(i, j);
                if(sel->isWithinSelection(i, j))
                    _curUndoItem->addTile(QPoint(i+(getSceneMousePos().x()+16)/32, j+(getSceneMousePos().y()+16)/32), newTileEntry);
            }
        }

        _curUndoItem->endUpdateEntries();
        getAssets()->getCurrentArea()->setEmitMapChange(true);
    }

    float scale = getAssets()->getEngine()->getCameraSys()->getKeyFrame().getScale().x * getAssets()->getEngine()->getCameraSys()->getStaticScale();
    auto p2 = _panPivot-_viewMousePos;
    auto delta = p2 - p1;
    delta /= scale;
    if(_panning) {
        getAssets()->getEngine()->getCameraSys()->translate(GYVector3 { (float)delta.x(), (float)delta.y(), 0.0f });
    } else if(_entityDrag) {
        delta = -delta;
        if(getAssets()->getCurrentSelectionType() == Selectable::ENTITY) {
            QPointF newPos = _sceneMousePos + _cursorDragOffset;
            static_cast<EntitySelectable*>(getAssets()->getCurrentSelection())->setPosition(newPos.x(), newPos.y());
        }
    }
}

void MapView::mouseReleaseEvent(QMouseEvent *event) {

    if(event->button() == Qt::RightButton && _underCursorEntity) {
        if(_tileSelectionRects.size() == 0 ||
                (_tileSelectionRects.size() == 1 && _tileSelectionRects[0].width() <= 2.0f && _tileSelectionRects[0].height() <= 2.0f)) {

            //create entity selection from current entity
            getAssets()->setCurrentSelection(new EntitySelectable(_underCursorEntity));
            auto* menu = _underCursorEntity->createContextMenu();
            menu->exec(mapToGlobal(_viewMousePos));
        }
    }

    //if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) {
     //   _entitySelectableCreate();
   // }
    _entityDrag = false;
    DestView<TileEntry>::mouseReleaseEvent(event);
}

void MapView::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton && getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY && !getCurrentLevel()->validSheets()) return;
    DestView<TileEntry>::mousePressEvent(event);

    if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY && event->button() == Qt::LeftButton) {
        //we're dragging an entity
        if(_underCursorEntity) {
            _panning = false;
            _entityDrag = true;
            _cursorDragOffset = QPointF(_underCursorEntity->_transform._pos.x-_sceneMousePos.x(), _underCursorEntity->_transform._pos.y-_sceneMousePos.y()-_underCursorEntity->_transform._pos.z);
            if(!_entityIsSelected(_underCursorEntity)) {
                _entityAddToSceneSelection(_underCursorEntity);
                _entitySelectableCreate();
            }
            static_cast<EntitySelectable*>(getAssets()->getCurrentSelection())->setBaseEntity(_underCursorEntity);
        }

    }
}

void MapView::_forceSceneUpdate(void) {
    //if(viewport()) static_cast<EngineGLSceneWidget*>(viewport())->updateGL();
}

inline bool MapView::_entityIsSelected(const TkEntity * const entity) const {
    if(getAssets()->getCurrentSelectionType() == Selectable::ENTITY) {
        return static_cast<const EntitySelectable*>(getAssets()->getCurrentSelection())->isInSelection(entity);
    }
    return false;
}

inline void MapView::_setUnderCursorEntity(TkEntity * const entity) {
    if(!_isEntityRegionSelecting()) {
        if(_underCursorEntity && _underCursorEntity->getSelectionState() == TkEntity::SELECTION_SELECTING) {
            _underCursorEntity->setSelectionState(TkEntity::SELECTION_NONE);
        }
    }
    _underCursorEntity = entity;
    if(entity && entity->getSelectionState() != TkEntity::SELECTION_SELECTED) entity->setSelectionState(TkEntity::SELECTION_SELECTING);
}

const QPointF& MapView::_mapViewPosToScenePos(const QPoint &viewPos, QPointF& out) const {
    GYVector3 dest;
    GYVector3 source = { (float)viewPos.x(), (float)viewPos.y(), 0.0f };

    getAssets()->getEngine()->getCameraSys()->inverseTransform(&dest, &source);
    out.setX(dest.x);
    out.setY(dest.y);
    return out;
}

void MapView::_startPan(void) {
    DestView<TileEntry>::_startPan();
}

void MapView::_endPan(void) {
    DestView<TileEntry>::_endPan();
}


bool MapView::_populatePlaceTileGroup(const TileSelection<TileEntry> *const /*terSel*/, PlaceTileGroupUndoItem<TileEntry> *const /*groupCmd*/) const {
    return true;
}

bool MapView::_populatePlaceTileGroupRegionFill(const TileSelection<TileEntry> *const /*terSel*/, PlaceTileGroupUndoItem<TileEntry> *const /*groupCmd*/) const {
    return true;
}

void MapView::scrollContentsBy(int dx, int dy) {
    getAssets()->getEngine()->getCameraSys()->translate(GYVector3 { (float)dx, (float)dy, 0.0f });
}

inline void MapView::_entityClearSceneSelection(void) {
    for(auto it = _sceneSelectedEntities.begin(); it != _sceneSelectedEntities.end(); ++it) {
        (*it)->setSelectionState(TkEntity::SELECTION_NONE);
    }
    _sceneSelectedEntities.clear();
}

inline bool MapView::_entityAddToSceneSelection(TkEntity * const entity) {
  //  Entity* parent = entity->getTopParentEntity();

    for(auto it = _sceneSelectedEntities.begin(); it != _sceneSelectedEntities.end(); ++it) {
        if(entity == *it) {
            return false;
        }
    }
    _sceneSelectedEntities.push_back(entity);
    entity->setSelectionState(TkEntity::SELECTION_SELECTING);

    return true;
}

bool MapView::_entityRemoveFromSceneSelection(TkEntity * const entity) {
    for(auto it = _sceneSelectedEntities.begin(); it != _sceneSelectedEntities.end(); ++it) {
        if(entity == *it) {
            _sceneSelectedEntities.erase(it);
            if(entity->getSelectionState() != TkEntity::SELECTION_SELECTED) entity->setSelectionState(TkEntity::SELECTION_NONE);
            return true;
        }
    }
    return false;
}

void MapView::onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* /*contact*/) {
    if((bodyA->getEntity() == _cursorEntity && _isSelectionCollider(bodyB->getEntity())) || (bodyB->getEntity() == _cursorEntity && _isSelectionCollider(bodyA->getEntity()))) return;

        //cursorEntity
        if(bodyA->getEntity() == _cursorEntity) {
          //  parent = bodyB->getEntity()->getTopParentEntity();
            if(!_isEntityRegionSelecting()) _setUnderCursorEntity(bodyB->getEntity());
        } else { //region select entity
            if(!(bodyB->getContactEventMonitorMask() & ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK)) {
                if(bodyB->getAttrib()->getFlags()->getType().getValue() & elysian::CollisionFlags::CURSOR) _entityAddToSceneSelection(bodyB->getEntity());
            }
        }

}

void MapView::onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* /*contact*/) {
    if((bodyA->getEntity() == _cursorEntity && _isSelectionCollider(bodyB->getEntity())) || (bodyB->getEntity() == _cursorEntity && _isSelectionCollider(bodyA->getEntity()))) return;

    if(bodyA->getEntity() == _cursorEntity && !_underCursorEntity) {
        if(!(bodyB->getContactEventMonitorMask() & ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK))
            _setUnderCursorEntity(bodyB->getEntity());
    }
}

bool MapView::_entityIsSceneSelected(const TkEntity * const entity) const {
    for(auto it = _sceneSelectedEntities.begin(); it != _sceneSelectedEntities.end(); ++it)
        if(entity == *it) return true;
    return false;
}

void MapView::onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) {
    if((bodyA->getEntity() == _cursorEntity && _isSelectionCollider(bodyB->getEntity())) || (bodyB->getEntity() == _cursorEntity && _isSelectionCollider(bodyA->getEntity()))) return;


    //normal cursor
    if(bodyA->getEntity() == _cursorEntity) {
        if(!_isEntityRegionSelecting()) {
            if(bodyB->getEntity() == _underCursorEntity) {
                    if(_underCursorEntity->getSelectionState() != TkEntity::SELECTION_SELECTED) {

                        if(!_entityIsSceneSelected(_underCursorEntity)) _underCursorEntity->setSelectionState(TkEntity::SELECTION_NONE);
                    }
                    _setUnderCursorEntity(nullptr);
            }
        }
    //region select box
    } else {
        if(bodyB->getAttrib()->getFlags()->getType().getValue() & elysian::CollisionFlags::CURSOR)
                if(bodyB->getEntity()->getSelectionState() != TkEntity::SELECTION_SELECTED)
                        _entityRemoveFromSceneSelection(bodyB->getEntity());

    }

}

void MapView::_updateScrollBars(void) {
    //QSize viewSize = viewport()->size();
    QSize sceneSize(getAssets()->getCurrentArea()->getMapWidth() * 32.0f, getAssets()->getCurrentArea()->getMapHeight() * 32.0f);

    verticalScrollBar()->setEnabled(false);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalScrollBar()->setVisible(false);
    horizontalScrollBar()->setEnabled(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalScrollBar()->setVisible(false);
/*
    verticalScrollBar()->setPageStep(viewSize.height());
    horizontalScrollBar()->setPageStep(viewSize.width());

    verticalScrollBar()->setRange(0, sceneSize.height()-viewSize.height());
    horizontalScrollBar()->setRange(0, sceneSize.width()-viewSize.width());
*/
}

void MapView::createSelectionItemGroup(void) {
    _selectionChanged = true;
    _selItemGroup = new SelectionItemGroup;
}

void MapView::updateSelectionItemGroup(void) { }

void MapView::deleteSelectionItemGroup(void) {
    _selItemGroup = nullptr;
}

void MapView::selectionCut(void) {

    DestView<TileEntry>::selectionCut();

    if(getAssets()->getCurrentSelectionType() == Selectable::ENTITY) {
        auto* curSelection = static_cast<EntitySelectable*>(getAssets()->getCurrentSelection());

    if(_entityClipboard != curSelection) {
        if(_entityClipboard) delete _entityClipboard;
            _entityClipboard = static_cast<EntitySelectable*>(getAssets()->getCurrentSelection());
            _entityClipboard->deactivate();
            getAssets()->setCurrentSelection(nullptr);
            _cutClipboard = true;
        }
    }
}

void MapView::selectionPaste(void) {
    DestView<TileEntry>::selectionPaste();

    if(_entityClipboard) {
        _entityClipboard->activate(getAssets()->getCurrentArea());
        getAssets()->setCurrentSelection(_entityClipboard);
        _entityClipboard->setPosition(_sceneMousePos.x(), _sceneMousePos.y());
        if(_cutClipboard) {
            _entityClipboard = nullptr;
        } else {
            _entityClipboard = new EntitySelectable(_entityClipboard);
            _entityClipboard->deactivate();
            _cutClipboard = false;
        }
    }
}

void MapView::selectionCopy(void) {
    DestView<TileEntry>::selectionCopy();

    if(getAssets()->getCurrentSelectionType() == Selectable::ENTITY) {
        auto* curSelection = static_cast<EntitySelectable*>(getAssets()->getCurrentSelection());

        if(_entityClipboard != curSelection) {
            if(_entityClipboard) delete _entityClipboard;

            _entityClipboard = new EntitySelectable(curSelection);
            _entityClipboard->deactivate();
            _cutClipboard = false;
        }
    }
}

void MapView::selectAll(void) {
    const auto layer = getAssets()->getCurrentLayer();

    if(layer == Assets::LAYER_ENTITY) {
        EntitySelectable* entitySelectable = nullptr;
        auto& entities = getCurrentArea()->getEntities(); //only look at active entities!
        auto addEntity = [&](Entity* e) {
            if(!entitySelectable) entitySelectable = new EntitySelectable(e);
            else entitySelectable->addEntity(e);
        };

        if(entities.size()) {
            for(auto&& entity : entities) {
                if(entity->getName()[0] != '_') { //don't select hidden entities
                    addEntity(entity);
                }
            }

            if(entitySelectable) getAssets()->setCurrentSelection(entitySelectable);
        }
    } else {
        auto* area = getCurrentArea();
        TileSelectable* tileSelectable = new TileSelectable(QRect(0, 0, area->getMapWidth(), area->getMapHeight()),
                                                            nullptr, area, getCurrentLevel(), layer);
        getAssets()->setCurrentSelection(tileSelectable);
    }
}

void MapView::dragEnterEvent(QDragEnterEvent *event) {
    if(_mimeDropManager->canDrop(event)) event->acceptProposedAction();
}

void MapView::dropEvent(QDropEvent *event) {
    if(_mimeDropManager->drop(event)) event->acceptProposedAction();
}

void MapView::_registerDropEventCallbacks(void) {
    _mimeDropManager = new MimeDropManager(QVector<MimeDropManager::Handler> {
       {
            TkMimeData::TYPE_LUA_VARIANT,
            [=](const QMimeData* data, Qt::DropAction, QDropEvent* event, int, int, const QModelIndex&) {
                DbgEvent dbg("Drop Event: Instantiating from Entity Prefab.");
                auto* tkMimeData = qobject_cast<const TkMimeData*>(data);
                LuaVariant v = tkMimeData->getVariant()["new"].getVariant()(tkMimeData->getVariant());
                Entity* entity = v.getValue<Entity*>();
                QPointF scenePos = _mapViewPosToScenePos(event->pos(), scenePos);

                if(!entity) {
                   errprintf("Failed: Prefab constructor returned NULL!");
                   return false; //entity SHOULD be garbage collected by Lua VM when v goes out of scope...
               }
                entity->getTransform().setPosX(scenePos.x());
                entity->getTransform().setPosY(scenePos.y());
                entity->setSaveFlag(true);

                getAssets()->getCurrentArea()->addEntity(entity);
                dbgprintf("Success: Adding Entity [%s] to area.", entity->getName());

                return true;
            },
           [=](const QMimeData* data, Qt::DropAction, QDragEnterEvent*, int, int, const QModelIndex&) {
               auto* tkMimeData = qobject_cast<const TkMimeData*>(data);
                return (tkMimeData->isPrefab() && QString(tkMimeData->getVariant().getTypeString()) == QString("Entity"));
           }

       }, {
            MimeDropManager::TYPE_URLS,
            [=](const QMimeData* data, Qt::DropAction /*action*/, QDropEvent* event, int, int, const QModelIndex&) {
                QPointF scenePos = _mapViewPosToScenePos(event->pos(), scenePos);
                for(auto&& url : data->urls()) {
                    if(url.isLocalFile()) {
                        QString texturePath = url.toLocalFile();
                        QFileInfo info(texturePath);
                        if(info.suffix() == "png" || info.suffix() == "PNG") {
                            DbgEvent dbg("Drop Event: Instantiating Entity from image file [%s].", Q_CSTR(texturePath));
                            Entity* entity = new Entity;
                            entity->setName(Q_CSTR(info.fileName().split(".")[0]));
                            Renderable* render = entity->addRenderable();
                            SpriteAttrib* attrib = new SpriteAttrib();
                            attrib->setTexPath(Q_CSTR(FileUtilities::getAssetRelativePath(texturePath)));
                            GYTexture* tex = attrib->getTexture();
                            render->setSpriteAttrib(attrib);

                            entity->getTransform().setPosX(scenePos.x());
                            entity->getTransform().setPosY(scenePos.y());

                            if(tex) {
                                dbgprintf("SUCCESS: Adding Entity [%s] to area.", entity->getName());
                                entity->getTransform().setSize(tex->width, tex->height, tex->height);
                            } else {
                                errprintf("FAILURE! SpriteAttrib.texture is NULL!");
                            }

                            getAssets()->getCurrentArea()->addEntity(entity);

                        }
                    }
                }
                return true;
            },
            [=](const QMimeData* data, Qt::DropAction /*action*/, QDragEnterEvent*, int, int, const QModelIndex&) {
                if(data->hasUrls()) {
                    for(auto&& url : data->urls()) {
                       if(url.isLocalFile()) {
                           QFileInfo info(url.toLocalFile());
                           if(info.suffix() == "png" || info.suffix() == "PNG") {
                               return true;
                           }
                       }
                    }
                }
                return false;
            }
       }
   });

}

