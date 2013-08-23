#include <QMouseEvent>
#include <QPainterPath>
#include <QScrollBar>

#include "sourceView.h"
#include <QDebug>

//===== STATIC =====

//This shit doesn't even work, because QT's drag system only works with the left button
//Keeping it in case we want to do it manually and make it configurable in the future.
Qt::MouseButton SourceView::_panMouseButton = Qt::LeftButton;
Qt::MouseButton SourceView::_selMouseButton = Qt::RightButton;

//utility function. Converts two QPointF's to a
//QRectF with x,y being the region's top left.
QRectF SourceView::getRect(const QPointF &p1, const QPointF &p2) {
	const float x = qMin(p1.x(), p2.x());
	const float y = qMin(p2.y(), p1.y());
	const float w = qAbs(p1.x()-p2.x());
	const float h = qAbs(p1.y()-p2.y());
	return QRectF(x,y,w,h);
}

QRect SourceView::getRect(const QPoint &p1, const QPoint &p2) {
	const int x = qMin(p1.x(), p2.x());
	const int y = qMin(p2.y(), p1.y());
	const int w = qAbs(p1.x()-p2.x());
	const int h = qAbs(p1.y()-p2.y());
	return QRect(x,y,w,h);
}

//==== INSTANCE ====

SourceView::SourceView(QWidget *parent):
    QGraphicsView(parent)
{
	setDragMode(NoDrag);
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
	viewport()->setMouseTracking(true);
	_sceneMousePosLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	_sceneMouseTileLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	_sceneSelectionSizeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	_zoomLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	_zoomLabel->setText(QString::number((int)(transform().m11()*100.0f))+QString("%"));

	connect(_autoScrollTimer, SIGNAL(timeout()), this, SLOT(autoScrollLogic()));
	_autoScrollTimer->start(200);
}

void SourceView::_updateSceneMousePos(void) {
	_sceneMousePos = mapToScene(_viewMousePos);
    _sceneMousePosLabel->setText(QString::number((int)_sceneMousePos.x()) + QString(", ") + QString::number((int)_sceneMousePos.y()));
    _sceneMouseTileLabel->setText(QString::number(((int)_sceneMousePos.x()+16)/32) + QString(", ") + QString::number(((int)_sceneMousePos.y()+16)/32)
        + QString(" [") + QString::number((int)((_sceneMousePos.x()+16)/32)*32) + QString(", ") + QString::number((int)((_sceneMousePos.y()+16)/32)*32) + QString("]"));
}

void SourceView::setCenter(const QPointF &centerPoint) {
	QRectF visibleArea = mapToScene(rect()).boundingRect();

		//Get the scene area
		QRectF sceneBounds = sceneRect();

		const double boundX = visibleArea.width() / 2.0;
		const double boundY = visibleArea.height() / 2.0;
		const double boundWidth = sceneBounds.width() - 2.0 * boundX;
		const double boundHeight = sceneBounds.height() - 2.0 * boundY;

		//The max boundary that the centerPoint can be to
		QRectF bounds(boundX, boundY, boundWidth, boundHeight);

		if(bounds.contains(centerPoint)) {
			//We are within the bounds
			_currentCenterPoint = centerPoint;
		} else {
			//We need to clamp or use the center of the screen
			if(visibleArea.contains(sceneBounds)) {
				//Use the center of scene ie. we can see the whole scene
				_currentCenterPoint = sceneBounds.center();
			} else {

				_currentCenterPoint = centerPoint;

				//We need to clamp the center. The centerPoint is too large
				if(centerPoint.x() > bounds.x() + bounds.width()) {
					_currentCenterPoint.setX(bounds.x() + bounds.width());
				} else if(centerPoint.x() < bounds.x()) {
					_currentCenterPoint.setX(bounds.x());
				}

				if(centerPoint.y() > bounds.y() + bounds.height()) {
					_currentCenterPoint.setY(bounds.y() + bounds.height());
				} else if(centerPoint.y() < bounds.y()) {
					_currentCenterPoint.setY(bounds.y());
				}

			}
		}

		//Update the scrollbars
		centerOn(_currentCenterPoint);
}

void SourceView::defaultTransform() {
	resetTransform();
	_zoomLabel->setText(QString::number((int)(transform().m11()*100.0f))+QString("%"));
}

void SourceView::autoScrollLogic(void) {
	if(!underMouse() || !isEnabled()) return;

	if(_autoScrollEnabled && dragMode() == NoDrag) {

#ifndef Q_OS_MAC
		if(verticalScrollBar()->underMouse() ||
				horizontalScrollBar()->underMouse()) return;
#endif

		static const int MAX_SCROLL_AMOUNT_PER_FRAME = 32;
		//Widget's width/height measurements INCLUDE the scrollbar!
		//Viewport's doesn't, apparently.
		const int innerWidth = viewport()->size().width();
		const int innerHeight = viewport()->size().height();
		//if(verticalScrollBar()) innerWidth -= verticalScrollBar()->width();
		//if(horizontalScrollBar()) innerHeight -= horizontalScrollBar()->height();
		const int scrollAreaX = innerWidth/30;
		const int scrollAreaY = innerHeight/30;
		const int rightBoundary = innerWidth-scrollAreaX;
		const int bottomBoundary = innerHeight-scrollAreaY;
		const float xScrollMultiplier = MAX_SCROLL_AMOUNT_PER_FRAME/scrollAreaX;
		const float yScrollMultiplier = MAX_SCROLL_AMOUNT_PER_FRAME/scrollAreaY;

		bool didMove = false;

		if(_viewMousePos.x() > ( rightBoundary )) {
			const float xDelta = (_viewMousePos.x() - rightBoundary)*xScrollMultiplier;
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() + xDelta);
			didMove = true;
		} else if(_viewMousePos.x() < ( scrollAreaX ) ) {
			const float xDelta = (scrollAreaX - _viewMousePos.x())*xScrollMultiplier;
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() - xDelta);
			didMove = true;
		}

		if(_viewMousePos.y() > ( bottomBoundary )) {
			const float yDelta = (_viewMousePos.y() - bottomBoundary)*yScrollMultiplier;
			verticalScrollBar()->setValue(verticalScrollBar()->value() + yDelta);
			didMove = true;
		} else if(_viewMousePos.y() < ( scrollAreaY ) ) {
			const float yDelta = (scrollAreaY - _viewMousePos.y())*yScrollMultiplier;
			verticalScrollBar()->setValue(verticalScrollBar()->value() - yDelta);
			didMove = true;
		}

		if(didMove) mouseMoveEvent(&_lastMouseEvent);
	}
}

void SourceView::drawForeground(QPainter *painter, const QRectF &rect) {
/*
	This is where we would be drawing an outline around the selection WHILE IT IS BEING MADE.
	Problem is that I'm not sure of a good way to do it yet. ;)

	if(!_selecting) return;
	QPen pen(QColor(251, 225, 202), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter->strokePath(_selectingPath, pen);
*/
	QGraphicsView::drawForeground(painter, rect);
}

void SourceView::mousePressEvent(QMouseEvent *event){
	//setFocus(Qt::MouseFocusReason);
	if(event->button() == _selMouseButton) {
		setDragMode(NoDrag);
		_startRegionSelection(event);
		emit exitPanMode();
	}
    else if(event->button() == _panMouseButton) setDragMode(ScrollHandDrag);

	event->ignore();
	QGraphicsView::mousePressEvent(event);
}

void SourceView::keyReleaseEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Control) {
		_endRegionSelection(NULL);
	}
   //else QGraphicsView::keyReleaseEvent(event);
	event->ignore();
}


void SourceView::mouseMoveEvent(QMouseEvent *event) {
	_lastMouseEvent = QMouseEvent(*event);
	_viewMousePos = event->pos();
	_updateSceneMousePos();

	if(event->buttons() & _selMouseButton) {
		_updateRegionSelection(event);
	}

	event->ignore();
	QGraphicsView::mouseMoveEvent(event);
}

void SourceView::mouseReleaseEvent(QMouseEvent *event) {
	if(event->button() == _selMouseButton) {
		_endRegionSelection(event);
		/*It's VERY important that we don't release focus while CTRL is down!
		  Staggered selection will reest! --FG */
		if(!(event->modifiers() &= Qt::ControlModifier)) emit releaseFocus();
	}
	event->ignore();
	QGraphicsView::mouseReleaseEvent(event);
}

void SourceView::wheelEvent(QWheelEvent *event) {
	const float delta = (event->delta() > 0)? 1.1f : 0.9f;
	//setTransformationAnchor();
	scale(delta, delta);
	_updateSceneMousePos();
	_zoomLabel->setText(QString::number((int)(transform().m11()*100.0f))+QString("%"));
	event->ignore();
}

void SourceView::_startRegionSelection(QMouseEvent * event) {
	if(!scene()) return;

	if(!_selecting) { //Begin a new selection
        _externalSelectionClear();
		_selecting = true;

        _fixedPivot = _currentPivot = mapToScene(event->pos());

        //Prime the rect stack.
		//_sceneSelectionAddRect(getRect(QPointF(-100.0f, -100.0f), QPointF(-100.0f, -100.0f)));

	}
	else { //Continue a new selection -- move pivot (staggered selection)
        //_selectedPath.moveTo(mapToScene(event->pos()));
        _currentPivot = mapToScene(event->pos());
    }
}

void SourceView::_updateRegionSelection(QMouseEvent * event) {
    if(_selecting) {
		QPointF scenePos = mapToScene(event->pos());
		_sceneSelectionUpdateLastRect(getRect(_currentPivot, scenePos));

		//Update StatusBar label
		// Can't just use the FP rect region here, because of loss after conversion
		QPoint intScenePos((scenePos.x()+16)/32, (scenePos.y()+16)/32);
		QPoint intPivot((_currentPivot.x()+16)/32, (_currentPivot.y()+16)/32);
		QRect rect = getRect(intScenePos, intPivot);

		_sceneSelectionSizeLabel->setText(QString::number(rect.width()+1) + QString(", ") + QString::number(rect.height()+1));
    }
}

void SourceView::_endRegionSelection(QMouseEvent *event) {
    if(!_selecting) return; //Not that this should ever happen...

	if(event) {
        _sceneSelectionAddRect(getRect(_currentPivot, mapToScene(event->pos())));

        if((event->modifiers() &= Qt::ControlModifier)) {
			return; //Continue selection for staggered selection
		}
	}

    _externalSelectionCreate();
	_selecting = false;
    _sceneSelectionClear();
}


////////////////////////////////////////////////////////
//========DEFAULT VIRTUAL IMPLEMENTATIONS==============
////////////////////////////////////////////////////////

// ===== INTERNAL SELECTION =====

void SourceView::_sceneSelectionAddRect(const QRectF &rect) {
    _selectingPath = _selectedPath;
    _selectingPath.addRect(rect);
    _selectedPath = _selectingPath;
    scene()->setSelectionArea(_selectingPath, Qt::IntersectsItemBoundingRect);
}

void SourceView::_sceneSelectionUpdateLastRect(const QRectF &rect) {
    _selectingPath = _selectedPath;
	_selectingPath.addRect(rect);
    scene()->setSelectionArea(_selectingPath, Qt::IntersectsItemBoundingRect);

}

void SourceView::_sceneSelectionClear(void) {
    _selectingPath = _selectedPath = QPainterPath();
    scene()->clearSelection();
}


