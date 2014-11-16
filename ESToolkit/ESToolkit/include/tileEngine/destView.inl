#include <QUndoStack>
#include <QMouseEvent>
#include "destView.h"
#include "selectionItemGroup.h"
#include "tileGroupUndoItem.h"
#include "tileSelection.h"
#include "elysian_zcoords.hpp"

template<typename T>
DestView<T>::DestView(QWidget *parent): SourceView(parent), _selItemGroup(NULL), _selClipboard(NULL),
	_fillingRegion(false), _placedTileGroup(false), _prevXOffset(0), _prevYOffset(0), _selectionChanged(true) {
}

template<typename T>
void DestView<T>::_sceneSelectionAddRect(const QRectF &rect) {
	_selectingPath = _selectedPath;
	_selectingPath.addRect(rect);
	_selectedPath = _selectingPath;
	_tileSelectionRects.push_back(rect);
	scene()->setSelectionArea(_selectingPath, Qt::IntersectsItemBoundingRect);
}

template<typename T>
void DestView<T>::_sceneSelectionUpdateLastRect(const QRectF &rect) {
	_selectingPath = _selectedPath;
	_selectingPath.addRect(rect);
	if(_tileSelectionRects.size() >= 1) _tileSelectionRects.last() = rect;
	else _tileSelectionRects.push_back(rect);
	scene()->setSelectionArea(_selectingPath, Qt::IntersectsItemBoundingRect);

}

template<typename T>
void DestView<T>::_sceneSelectionClear(void) {
	_selectingPath = _selectedPath = QPainterPath();
	_tileSelectionRects.clear();
	scene()->clearSelection();
}

template<typename T>
void DestView<T>::drawForeground(QPainter *painter, const QRectF &rect)  {
	SourceView::drawForeground(painter, rect);

	if(_selItemGroup && !_fillingRegion) {
		SelectionItemGroup* group = static_cast<SelectionItemGroup*>(_selItemGroup);

		QPen pen(QColor(225, 202, 251), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		painter->setBrushOrigin(group->pos());
		painter->translate(group->pos());
		painter->strokePath(group->path, pen);
	}
	//Doesn't snap to a grid, so it isn't quite pristine enough to use.
	/*else if(_fillingRegion) {
		QPen pen(QColor(251, 225, 202), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		painter->strokePath(_fillPath, pen);

	}*/
}

template<typename T>
void DestView<T>::placeSelection(void) {
	//if(!getCurrentArea()) return; Shouldn't be necessary?
	TileSelection<T> *terSel = static_cast<TileSelection<T> *>(_getTileSelection());
	if(!terSel) return;

	/*
		Check to see if we are making a placement in the same location without having changed the selection.
		This logic is reesty with regionFilling, so we only use it for normal placement. --Falco
	 */
	if(!_fillingRegion) {
		int new_x_offset = (int(_sceneMousePos.x()+16)/32);
		int new_y_offset = (int(_sceneMousePos.y()+16)/32);
		if(new_x_offset == _prevXOffset && new_y_offset == _prevYOffset && !_selectionChanged)
			return; //we are most likely trying to lay the same shit.

		//Update previous state variables
		_prevXOffset = new_x_offset;
		_prevYOffset = new_y_offset;
		_selectionChanged = false;
	}

	PlaceTileGroupUndoItem<T> *groupCmd = _createPlaceTileGroup();

	//Cache the bitch so we know the QUndoCommand type for when we want to cancel merging later
	if(!_placedTileGroup) {
		_placeTileGroupID = groupCmd->id();
		_placedTileGroup = true;
	}

	bool tilesAdded = (!_fillingRegion)? _populatePlaceTileGroup(terSel, groupCmd) :
		_populatePlaceTileGroupRegionFill(terSel, groupCmd);

	//Only push the undo command if there were tiles placed
	if(tilesAdded) _getUndoStack()->push(groupCmd);
	else delete groupCmd;

	scene()->update();
}

/*
  Used to prevent the next PlaceTerrainGroup UndoItem from being merged with the last.
*/
template<typename T>
void DestView<T>::disableSelectionPlacementMerge(void) {
	const QUndoCommand *command = _getUndoStack()->command(_getUndoStack()->count()-1);

	if(command && _placedTileGroup && command->id() == _placeTileGroupID) {
		PlaceTileGroupUndoItem<T> *groupCmd = static_cast<PlaceTileGroupUndoItem<T> *>(const_cast<QUndoCommand *>(command));
		groupCmd->setCombine(false);
	}
}

template<typename T>
bool DestView<T>::_populatePlaceTileGroup(const TileSelection<T> *const terSel, PlaceTileGroupUndoItem<T> *const groupCmd) const {
	bool tilesAdded = false;
	unsigned width, height;
	terSel->getSelectionArea(width, height);

	for(unsigned y = 0; y < height; ++y) {
		for(unsigned x = 0; x < width; ++x) {
			int x_index=0, y_index=0;

			if(!terSel->isWithinSelection(x, y)) continue;
			x_index = x + _prevXOffset;
			y_index = y + _prevYOffset;

			//if(x_index >= getCurrentArea()->_elysianArea.width || x_index < 0 ||
			//  y_index >= getCurrentArea()->_elysianArea.height || y_index < 0 ) continue;

			T data = terSel->getEntry(x, y);
			tilesAdded |= groupCmd->addTile(QPoint(x_index, y_index), data);
		}
	}
	return tilesAdded;
}

template<typename T>
bool DestView<T>::_populatePlaceTileGroupRegionFill(const TileSelection<T> *const terSel, PlaceTileGroupUndoItem<T> *const groupCmd) const {
	bool tilesAdded = false;
	for(int k = 0; k < _tileSelectionRects.size(); ++k) {
		const QRectF &rect = _tileSelectionRects[k];

		int startX = (rect.topLeft().x()+16)/32;
		int startY = (rect.topLeft().y()+16)/32;
		int endX = (rect.bottomRight().x()+16)/32;
		int endY = (rect.bottomRight().y()+16)/32;

		for(int y = startY; y <= endY; ++y) {
			for(int x = startX; x <= endX; ++x) {

				//If selection is blank at this location
				if(!terSel->isWithinSelectionFill(x, y, _fillFixedPivot, _sceneMousePos)) continue;

				//Check map boundaries
				//if(x >= getCurrentArea()->_elysianArea.width || x < 0 ||
				//	y >= getCurrentArea()->_elysianArea.height || y < 0 ) continue;

				//Add location/value to Undo/Redo command
				T data = terSel->getFillEntry(x, y, _fillFixedPivot, _sceneMousePos);

				tilesAdded |= groupCmd->addTile(QPoint(x, y), data);

			}
		}
	}
	return tilesAdded;
}

//Deletes the fucker if it already exists for reallocation!
template<typename T>
void DestView<T>::createSelectionItemGroup(void) {
    if(_selItemGroup){ delete _selItemGroup; _selItemGroup = nullptr; /*return;*/}

	TileSelection<T> *tileSel = _getTileSelection();
	if(!tileSel) return;

	_selectionChanged = true;

	QPainterPath path;
	QGraphicsRectItem *item;


	SelectionItemGroup *castedItemGroup = new SelectionItemGroup;
	_selItemGroup=castedItemGroup;

	unsigned width, height;
	tileSel->getSelectionArea(width, height);
	for(unsigned y = 0; y < height; ++y) {
		for(unsigned x = 0; x < width; ++x) {

			if(!tileSel->isWithinSelection(x, y)) continue;

			item = new QGraphicsRectItem(QRectF(0.0f, 0.0f, 32.0f, 32.0f));

			item->setX(x*32);
			item->setY(y*32);
			item->setZValue(elysian::ZCoordConstants::fg1);
			item->setVisible(false);

			path.addRect(x*32,y*32,32,32);
			_selItemGroup->addToGroup(item);

		}
	}

	 castedItemGroup->path = path.simplified();

	 _selItemGroup->setZValue(elysian::ZCoordConstants::fg1);

	 _selItemGroup->setPos(SelectionItemGroup::clampToTileLoc(_sceneMousePos));
	 _selItemGroup->update();
	 scene()->addItem(castedItemGroup);

	 _sceneSelectionSizeLabel->setText(QString::number(width) + QString(", ") + QString::number(height));

}

template<typename T>
void DestView<T>::deleteSelectionItemGroup(void) {
	_fillingRegion = false;
	scene()->clearSelection();
	delete _selItemGroup;
    _selItemGroup = nullptr;
	scene()->update();

	_sceneSelectionSizeLabel->setText(QString("0, 0"));
}

template<typename T>
void DestView<T>::updateSelectionItemGroup(void) {
	if(_selItemGroup){
		_selItemGroup->setPos(_sceneMousePos);
		_selItemGroup->update();
	}
}

template<typename T>
void DestView<T>::wheelEvent(QWheelEvent *event) {
	/*float delta = (event->delta() > 0.0f)? 1.1f : 0.9f;
	//scale(delta, delta);
	_sceneMousePos *= delta;*/
	SourceView::wheelEvent(event);
	updateSelectionItemGroup();
	event->ignore();
}

template<typename T>
void DestView<T>::mousePressEvent(QMouseEvent *event) {

	if(event->button() == Qt::LeftButton && !(event->modifiers() &= Qt::ControlModifier)) {
		if(_selItemGroup) placeSelection();
		else setDragMode(ScrollHandDrag);
		//event->ignore();
		//return;
	}
	else if(event->button() == Qt::MidButton) {
		QMouseEvent fooledya(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
		setDragMode(ScrollHandDrag);
		QGraphicsView::mousePressEvent(&fooledya);
	} else QGraphicsView::mousePressEvent(event);

	if(event->buttons() & Qt::LeftButton && (event->modifiers() &= Qt::ControlModifier)) {
		_startRegionFill(event);
		event->ignore();
		return;
	}

	if(event->button() == Qt::RightButton) {
		if(_selItemGroup) {
			delete _selItemGroup;
			_selItemGroup=NULL;
		}
		setDragMode(NoDrag);

		_startRegionSelection(event);
		//event->accept();
		return;
	}

	//event->ignore();
	QGraphicsView::mousePressEvent(event);
}

template<typename T>
void DestView<T>::mouseReleaseEvent(QMouseEvent *event) {
	if(event->button() == Qt::RightButton) {
		_endRegionSelection(event);
	}

	if(event->button() == Qt::MidButton) {
		QMouseEvent fooledya(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
		QGraphicsView::mouseReleaseEvent(&fooledya);
	} else QGraphicsView::mouseReleaseEvent(event);

	if(event->button() == Qt::LeftButton && _fillingRegion) {
		_endRegionFill(event);
	}

	if( !(event->modifiers() &= Qt::ControlModifier) ) {
		disableSelectionPlacementMerge();
	}

	event->ignore();
	QGraphicsView::mouseReleaseEvent(event);
}

template<typename T>
void DestView<T>::keyPressEvent (QKeyEvent *event)  {

	//Check for tile flip/rotation
	if(_selItemGroup) {
		switch(event->key()) {
		case Qt::Key_Up:
			selectionFlipHorizontal();
			break;
		case Qt::Key_Down:
			selectionFlipVertical();
			break;
		case Qt::Key_Left:
			selectionRotateCC();
			break;
		case Qt::Key_Right:
			selectionRotateC();
			break;
		}
	}
	event->ignore();
}

template<typename T>
void DestView<T>::keyReleaseEvent (QKeyEvent *event)  {
	if(event->key() == Qt::Key_Control) {
		if(isSelecting()) _endRegionSelection(NULL);
		else if(isFillingRegion()) _endRegionFill(NULL);
	}
	//else QGraphicsView::keyReleaseEvent(event);

	event->ignore();
}

template<typename T>
void DestView<T>::mouseMoveEvent(QMouseEvent *event) {
	_lastMouseEvent = QMouseEvent(*event);
	//_curMousePos = mapToScene(event->pos());
	_viewMousePos = event->pos();
	//_sceneMousePos = mapToScene(_viewMousePos);
	_updateSceneMousePos();

/*	if(_autoScrollEnabled && dragMode() != ScrollHandDrag) {
		autoScrollLogic(event);
	}*/

	if(event->buttons()&Qt::RightButton && !_fillingRegion) {
		_updateRegionSelection(event);
	}

	if(event->buttons() == Qt::LeftButton && _selItemGroup && !_fillingRegion) {
		placeSelection();
	}

	if(event->buttons()&Qt::LeftButton) {
		_updateRegionFill(event);
	}

	updateSelectionItemGroup();

	//event->ignore();
	QGraphicsView::mouseMoveEvent(event);
}


template<typename T>
void DestView<T>::scrollContentsBy(int dx, int dy) {
	//necessary to prevent fragmenting on scroll,
	//& to allow selection to continue to follow mouse (don't think this part works with scale)
	/*if(_selItemGroup){
		//_curMousePos -= mapToScene(QPoint(dx, dy));
		_curMousePos -= QPoint(dx, dy);

		_selItemGroup->setPos(QPointF(_curMousePos.x(),_curMousePos.y()));
		_selItemGroup->update();
	}*/
	//updateSelectionItemGroup();
	QGraphicsView::scrollContentsBy(dx, dy);
	if(scene()) scene()->update();
}

template<typename T>
void DestView<T>::_startRegionFill(QMouseEvent *event) {
	if(isSelecting() || !_getTileSelection()) return;

	if(!_fillingRegion) {
		_fillingRegion = true;

		_fillFixedPivot = _fillCurrentPivot = mapToScene(event->pos());
	}
	else {
		_fillCurrentPivot = mapToScene(event->pos());
	}

}

template<typename T>
void DestView<T>::_updateRegionFill(QMouseEvent *event) {
	if(_fillingRegion) {
		_sceneSelectionUpdateLastRect(getRect(_fillCurrentPivot, mapToScene(event->pos())));
	}
}

template<typename T>
void DestView<T>::_endRegionFill(QMouseEvent *event) {
	if(!_fillingRegion) return;

	if(event) {
		_sceneSelectionAddRect(getRect(_fillCurrentPivot, mapToScene(event->pos())));

		/* Unfortunately, the new, correct region-fill cannot handle staggered region filling!
		  Because of laying on different sides of dynamic pivot
		  Force it to choose a side depending on initial direction?
		if((event->modifiers() &= Qt::ControlModifier)) {
			return; //Continue selection for staggered selection
		}
		*/
	}

	placeSelection();
	//Do NOT fucking merge region fill commands!!!
	disableSelectionPlacementMerge();

	_fillingRegion = false;
	_sceneSelectionClear();
}

template<typename T>
void DestView<T>::selectionFlipHorizontal(void) {
	TileSelection<T> *terSel = _getTileSelection();
	if(terSel) {
		terSel->flipHorizontal();
		createSelectionItemGroup();
	}
}

template<typename T>
void DestView<T>::selectionFlipVertical(void) {
	TileSelection<T> *terSel = _getTileSelection();
	if(terSel) {
		terSel->flipVertical();
		createSelectionItemGroup();
	}
}

template<typename T>
void DestView<T>::selectionRotateC(void) {
	TileSelection<T> *terSel = _getTileSelection();
	if(terSel) {
		terSel->rotateC();
		createSelectionItemGroup();
	}
}

template<typename T>
void DestView<T>::selectionRotateCC(void) {
	TileSelection<T> *terSel = _getTileSelection();
	if(terSel) {
		terSel->rotateCC();
		createSelectionItemGroup();
	}
}

template<typename T>
void DestView<T>::selectionCut(void) {
	TileSelection<T> *terSel = _getTileSelection();
	if(terSel) {
		delete _selClipboard;
		_selClipboard = _copyConstructTileSelection(*terSel);
		deleteSelectionItemGroup();
		_setTileSelection(NULL);
	}
}

template<typename T>
void DestView<T>::selectionPaste(void) {
	if(_selClipboard) {
		_setTileSelection(_selClipboard);
		_selClipboard = NULL;
	}
}
