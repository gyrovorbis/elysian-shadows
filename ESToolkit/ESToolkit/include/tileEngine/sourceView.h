#ifndef SOURCE_VIEW_H
#define SOURCE_VIEW_H

#include <QGraphicsView>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>
#include <gyro_matrix_api.h>

class SourceView : public QGraphicsView {
	Q_OBJECT
protected:
	static Qt::MouseButton _panMouseButton;
	static Qt::MouseButton _selMouseButton;

	QPoint _viewMousePos;
	QPointF _sceneMousePos;
    //pivot of overall selection region(s)
	QPointF _fixedPivot;
    //pivot of current rectangular region within selection
	QPointF _currentPivot;
    bool _selecting                     = false;

    //Panning
    GYVector3   _baseTranslation          = { 0.0f, 0.0f, 0.0f };
    bool        _panning                  = false;
    QPoint      _panPivot;

	QPointF _currentCenterPoint;

    bool _autoScrollEnabled             = true;
    QTimer *_autoScrollTimer            = new QTimer(this);
    QMouseEvent _lastMouseEvent         = QMouseEvent(QEvent::None, QPoint(),
                                              Qt::LeftButton,
                                              Qt::RightButton,
                                              Qt::ControlModifier);

    QLabel *_sceneMousePosLabel         = new QLabel;
    QLabel *_sceneMouseTileLabel        = new QLabel;
    QLabel *_sceneSelectionSizeLabel    = new QLabel;
    QLabel *_zoomLabel                  = new QLabel;

	void _updateSceneMousePos(void);

public:
    explicit SourceView(QWidget *parent = nullptr);

	inline QLabel *getSceneMousePosLabel(void) const { return _sceneMousePosLabel; }
	inline QLabel *getSceneMouseTileLabel(void) const { return _sceneMouseTileLabel; }
	inline QLabel *getSceneSelectionSizeLabel(void) const { return _sceneSelectionSizeLabel; }
	inline QLabel *getZoomLabel(void) const { return _zoomLabel; }

	void defaultTransform(void);

	 static QRectF getRect(const QPointF &p1, const QPointF &p2);
	 static QRect getRect(const QPoint &p1, const QPoint &p2);

	 inline static void setMouseButtons(const Qt::MouseButton sel, const Qt::MouseButton pan) {
		_selMouseButton = sel;
		_panMouseButton = pan;
	 }

     QPointF        getSceneMousePos(void) const { return _sceneMousePos; }
     void           setCenter(const QPointF &centerPoint);
	 inline QPointF getCenter(void) const { return _currentCenterPoint; }

     inline bool    isSelecting(void) const { return _selecting; }
	 inline QPointF getSelectionFixedPivot(void) const { return _fixedPivot; }
	 inline QPointF getSelectionCurrentPivot(void) const { return _currentPivot; }

	 inline void autoScrollToggle(const bool enabled) { _autoScrollEnabled = enabled; }

     virtual void drawForeground(QPainter *painter, const QRectF &rect) override;

     virtual void mousePressEvent(QMouseEvent *event) override;
     virtual void keyReleaseEvent(QKeyEvent *event) override;
     virtual void mouseMoveEvent(QMouseEvent *event) override;
     virtual void mouseReleaseEvent(QMouseEvent *event) override;
     virtual void wheelEvent(QWheelEvent *event) override;


Q_SIGNALS:
	 void releaseFocus(void);
	 void exitPanMode(void);

protected Q_SLOTS:
    void autoScrollLogic(void);

protected:

     QPainterPath _selectedPath;
     QPainterPath _selectingPath;

	 //SELECTION EVENT PROCESSING
	 void _startRegionSelection(QMouseEvent *event);
	 void _endRegionSelection(QMouseEvent *event);
     void _updateRegionSelection(QMouseEvent *event);

     /* INTERNAL SELECTION
       These complex-as-shit functions are for abstracting the actual selection "mechanism" away
       from each scene. The default implementation uses a QPainterPath. The only view that overrides
       this behavior is MapView, because it uses a custom, optimized-to-shit selection mechanism.
     */

     virtual QPointF _mapViewPosToScenePos(const QPoint viewPos) const;
     virtual void    _startPan(void);
     virtual void    _endPan(void);
     virtual void    _forceSceneUpdate(void);

     //adds a rectangular region to the current scene selection
     virtual void _sceneSelectionAddRect(const QRectF &rect);
     //updates the latest-added rectangular region within the current scene selection
     virtual void _sceneSelectionUpdateLastRect(const QRectF &rect);
     //clears internal scene selection
     virtual void _sceneSelectionClear(void);

     /* EXTERNAL SELECTION
      These two functions are for abstracting the external selection mechanism away
      from the internal selection mechanism. The default implementation converts in-scene
      selections to Toolkit terrain selections. To use some other selection container (SheetManager),
      override these.
      */

     //logic for converting an internal "scene selection" to an external selection container
	 virtual void _externalSelectionCreate(void) = 0;
     //logic for clearing external selection container
	 virtual void _externalSelectionClear(void) = 0;

};

#endif // TILESELECTIONVIEW_H
