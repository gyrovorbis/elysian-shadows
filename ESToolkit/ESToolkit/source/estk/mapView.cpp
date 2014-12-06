#include <QMouseEvent>
#include <QDebug>
#include <QPainterPath>
#include <QScrollBar>

#include "mapView.h"
#include "selectionItemGroup.h"
#include "tileSelectable.h"
#include "area.h"
#include "mapViewPlaceTileGroupUndoItem.h"
#include "assets.h"
#include "elysian_engine.hpp"
#include "elysian_camera_system.hpp"
#include "engineGLSceneWidget.h"
#include "elysian_collidable.hpp"
#include "entity.h"
#include "entitySelectable.h"


MapView::MapView(QWidget *parent): DestView<TileEntry>::DestView(parent) {
    setMouseTracking(true);
	setEventMask(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK);

	_curUndoItem = new MapViewPlaceTileGroupUndoItem();

	connect(getAssets(), SIGNAL(areaChanged(Area*const)), this, SLOT(onAreaChange(Area*const)));
	connect(getAssets(), SIGNAL(layerChanged(LAYER_TYPE)), this, SLOT(onLayerChange(LAYER_TYPE)));
}

void MapView::onLayerChange(Assets::LAYER_TYPE type) {
	//update current undoGroupItem
	_curUndoItem->setLayer(type);
}

void MapView::onAreaChange(Area * const area) {
	//update current undoGroupItem
	_curUndoItem->setArea(area);
}

TileSelection<TileEntry> *MapView::_getTileSelection(void) const {
	return static_cast<TileSelectable*> (
                (getCurrentSelectionType() == Selectable::TILE)? getCurrentSelection() : nullptr
	);
}

void MapView::_entitySelectionUpdate(void) {
	TkEntity* entity;

	for(auto &it: _selectionColliders) {
		getCurrentArea()->removeEntity(it);
	}

	while(_selectionColliders.size() < _tileSelectionRects.size()) {
		entity = new TkEntity;
		auto* collid = entity->addCollidable();
		collid->setContactEventMonitorMask(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_TK_MAPVIEW_MASK);
		_selectionColliders.push_back(entity);
	}

	for(int k = 0; k < _tileSelectionRects.size(); ++k) {
		auto& rect = _tileSelectionRects[k];
		auto midPt = rect.bottomRight() - rect.topLeft();
		midPt /= 2.0f;

		entity = _selectionColliders[k];
		entity->getTransform().setPos(midPt.x(), midPt.y(), 10.0f);
		entity->getTransform().setSize(rect.width(), rect.height());
		getCurrentArea()->addEntity(_selectionColliders[k]);
	}

}

void MapView::_entitySelectableCreate(void) {
	auto* select = new EntitySelectable;
	for(auto it = _selectedEntities.begin(); it != _selectedEntities.end(); ++it) {
		select->addEntity(*it);
	}
	_selectedEntities.clear();
	getAssets()->setCurrentSelection(select);
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

		//sync array with rectangular selection
		for(int j = startY; j <= endY; ++j) {
			for(int i = startX; i <= endX; ++i) {
				_tileSelectionArray[j][i] = true;
			}
		}

		if(isFillingRegion()) {
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
		}

		if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) {
			_entitySelectionUpdate();
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
	memset(_tileSelectionArray, 0, sizeof(_tileSelectionArray));
    //scene()->update(scene()->sceneRect());
    _forceSceneUpdate();
}

void MapView::_externalSelectionCreate(void) {
	if(_tileSelectionRects.size() <= 0) return;

	int startX = ELYSIAN_AREA_MAP_WIDTH;
	int endX = 0;
	int startY = ELYSIAN_AREA_MAP_HEIGHT;
	int endY = 0;

	for(int i = 0; i < _tileSelectionRects.size(); ++i) {
		if(startX > (_tileSelectionRects[i].topLeft().x()+16)/32)	startX =  (_tileSelectionRects[i].topLeft().x()+16)/32;
		if(endX < (_tileSelectionRects[i].bottomRight().x()+16)/32) endX = (_tileSelectionRects[i].bottomRight().x()+16)/32;
		if(startY > (_tileSelectionRects[i].topLeft().y()+16)/32)	startY = (_tileSelectionRects[i].topLeft().y()+16)/32;
		if(endY < (_tileSelectionRects[i].bottomRight().y()+16)/32) endY = (_tileSelectionRects[i].bottomRight().y()+16)/32;
    }
	if(startX < 0) startX = 0;
	if(startY < 0) startY = 0;

	TileSelectable *selection = new TileSelectable(QRect(startX, startY, endX-startX+1, endY-startY+1), _tileSelectionArray, getCurrentArea(), getCurrentLevel(), getAssets()->getCurrentLayer());
    setCurrentSelection(selection);
}

void MapView::_externalSelectionClear(void) {
	_curUndoItem->clearEntries();
    setCurrentSelection(nullptr);
}

void MapView::_setTileSelection(TileSelection<TileEntry>* const sel) {
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
				createSelectionItemGroup(); //force update region
				break;
			case Qt::Key_Plus:
				terSel->elevateUp();
				createSelectionItemGroup(); //force update region
				break;
			case Qt::Key_Minus:
				terSel->elevateDown();
				createSelectionItemGroup(); //force update region
				break;
			case Qt::Key_Z:
				terSel->toggleZTransitional();
				createSelectionItemGroup();
                break;
            case Qt::Key_N:
                terSel->toggleNormalFace();
                createSelectionItemGroup();
                break;
			}
		}
	}
}

void MapView::wheelEvent(QWheelEvent* event) {
    const float delta = (event->delta() > 0)? 1.05f : 0.95f;
    //setTransformationAnchor();
    //scale(delta, delta);
    getAssets()->getEngine()->getCameraSys()->scale(delta);
    _updateSceneMousePos();
    _zoomLabel->setText(QString::number((int)(getAssets()->getEngine()->getCameraSys()->getKeyFrame().getScale()*100.0f))+QString("%"));
    event->ignore();
}

void MapView::mouseMoveEvent(QMouseEvent *event) {
    auto p1 = _panPivot-_viewMousePos;

    DestView<TileEntry>::mouseMoveEvent(event);

	if(_selItemGroup && !isFillingRegion()) {
		auto *sel = static_cast<TileSelectable*>(_getTileSelection());
		unsigned w, h;
		sel->getSelectionArea(w, h);

		_curUndoItem->beginUpdateEntries();
		for(unsigned j = 0; j < h; ++j) {
			for(unsigned i = 0; i < w; ++i) {
				auto newTileEntry = sel->getEntry(i, j);
				if(sel->isWithinSelection(i, j))
					_curUndoItem->addTile(QPoint(i+(getSceneMousePos().x()+16)/32, j+(getSceneMousePos().y()+16)/32), newTileEntry);
			}
		}

		_curUndoItem->endUpdateEntries();
	}

    if(_panning) {
        float scale = getAssets()->getEngine()->getCameraSys()->getKeyFrame().getScale();
        auto p2 = _panPivot-_viewMousePos;
        auto delta = p2 - p1;
        delta /= scale;
        getAssets()->getEngine()->getCameraSys()->translate(GYVector3 { (float)delta.x(), (float)delta.y(), 0.0f });
        _forceSceneUpdate();

        float x, y, z;
        getAssets()->getEngine()->getCameraSys()->getKeyFrame().getPos(x, y, z);
    }

}

void MapView::mouseReleaseEvent(QMouseEvent *event) {
	if(getAssets()->getCurrentLayer() == Assets::LAYER_ENTITY) {
		_entitySelectableCreate();
	}

	DestView<TileEntry>::mouseReleaseEvent(event);
}

void MapView::_forceSceneUpdate(void) {
    //if(viewport()) static_cast<EngineGLSceneWidget*>(viewport())->updateGL();
}

QPointF MapView::_mapViewPosToScenePos(const QPoint viewPos) const {
    GYVector3 dest;
    GYVector3 source = { (float)viewPos.x(), (float)viewPos.y(), 0.0f };

    getAssets()->getEngine()->getCameraSys()->inverseTransform(&dest, &source);

    return QPointF(dest.x, dest.y);
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

void MapView::scrollContentsBy(int /*dx*/, int /*dy*/) {

}

void MapView::onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) {
	_selectedEntities.push_back(bodyB->getEntity());
}

void MapView::onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) {

}

void MapView::onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) {
	for(auto it = _selectedEntities.begin(); it != _selectedEntities.end(); ++it) {
		_selectedEntities.erase(it);
	}
}
