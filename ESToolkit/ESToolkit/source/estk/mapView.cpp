#include <QMouseEvent>
#include <QDebug>
#include <QPainterPath>
#include <QScrollBar>

#include "mapView.h"
#include "selectionItemGroup.h"
#include "tileSelectable.h"
#include "area.h"
#include "mapViewPlaceTileGroupUndoItem.h"


MapView::MapView(QWidget *parent): DestView<TileEntry>::DestView(parent), _refreshTimerHandle(30) {
    startTimer(30);
	memset(_tileSelectionArray, 0, sizeof(_tileSelectionArray));
}


TileSelection<TileEntry> *MapView::_getTileSelection(void) const {
	return static_cast<TileSelectable*> (
				(getCurrentSelectionType() == Selectable::TILE)? getCurrentSelection() : NULL
	);
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
	}
}

void MapView::_sceneSelectionAddRect(const QRectF &rect) {
	//If it's too small, don't select (to fall through into pan mode)
	if(rect.right() - rect.left() < 2 &&
			rect.bottom() - rect.top() < 2) return;

	_tileSelectionRects.push_back(rect);
	_tileSelectionArrayUpdate();
    scene()->update(scene()->sceneRect());
}

void MapView::_sceneSelectionUpdateLastRect(const QRectF &rect) {
	if(_tileSelectionRects.size() >= 1) _tileSelectionRects.last() = rect;
	else _tileSelectionRects.push_back(rect);
	_tileSelectionArrayUpdate();
    scene()->update(scene()->sceneRect());
}

void MapView::_sceneSelectionClear(void) {
	_tileSelectionRects.clear();
	memset(_tileSelectionArray, 0, sizeof(_tileSelectionArray));
    scene()->update(scene()->sceneRect());
}

void MapView::_externalSelectionCreate(void) {
	if(_tileSelectionRects.size() <= 0) return;

	int startX = ELYSIAN_AREA_MAP_WIDTH;
	int endX = 0;
	int startY = ELYSIAN_AREA_MAP_HEIGHT;
	int endY = 0;

	for(int i = 0; i < _tileSelectionRects.size(); ++i) {
		if(startX > (_tileSelectionRects[i].topLeft().x()+16)/32) startX =  (_tileSelectionRects[i].topLeft().x()+16)/32;
		if(endX < (_tileSelectionRects[i].bottomRight().x()+16)/32) endX = (_tileSelectionRects[i].bottomRight().x()+16)/32;
		if(startY > (_tileSelectionRects[i].topLeft().y()+16)/32) startY = (_tileSelectionRects[i].topLeft().y()+16)/32;
		if(endY < (_tileSelectionRects[i].bottomRight().y()+16)/32) endY = (_tileSelectionRects[i].bottomRight().y()+16)/32;
    }
	if(startX < 0) startX = 0;
	if(startY < 0) startY = 0;

	TileSelectable *selection = new TileSelectable(QRect(startX, startY, endX-startX+1, endY-startY+1), _tileSelectionArray, getCurrentArea(), getCurrentLevel(), getAssets()->getCurrentLayer());
    setCurrentSelection(selection);
}

void MapView::_externalSelectionClear(void) {
	setCurrentSelection(NULL);
}

void MapView::_setTileSelection(TileSelection<TileEntry> *const sel) {
	setCurrentSelection(static_cast<TileSelectable *>(sel));
}

QUndoStack *MapView::_getUndoStack(void) const {
	return &getCurrentArea()->getUndoStack();
}

PlaceTileGroupUndoItem<TileEntry> *MapView::_createPlaceTileGroup(void) {
	return new MapViewPlaceTileGroupUndoItem(getCurrentArea(), getAssets()->getCurrentLayer());
}

TileSelection<TileEntry> *MapView::_copyConstructTileSelection(const TileSelection<TileEntry> &rval) const {
	return new TileSelectable(*static_cast<const TileSelectable *>(&rval));
}


void MapView::timerEvent(QTimerEvent *event) {
	if(event->timerId() == _refreshTimerHandle) {
        scene()->update(scene()->sceneRect());
    }
}

void MapView::keyPressEvent(QKeyEvent *event) {
	DestView<TileEntry>::keyPressEvent(event);

	//Check for MapView-specific key events
	if(_selItemGroup) {
		TileSelectable *terSel = static_cast<TileSelectable *>(_getTileSelection());
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
			}
		}
	}
}
