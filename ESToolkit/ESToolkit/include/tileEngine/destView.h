#ifndef DEST_VIEW_H
#define DEST_VIEW_H

#include <gyro.h>
#include "sourceView.h"

class QUndoStack;
template<typename T>
class TileSelection;
template<typename T>
class PlaceTileGroupUndoItem;

template<typename T>
class DestView : public SourceView  {
protected:
	QGraphicsItemGroup *_selItemGroup;
	TileSelection<T> *_selClipboard;

	QVector<QRectF> _tileSelectionRects;
	QPainterPath _fillPath;
	QPointF _fillFixedPivot;
	QPointF _fillCurrentPivot;
	bool _fillingRegion;

	//Undo/redo extra data to retreive ID dynamically
	bool _placedTileGroup;
	int _placeTileGroupID;

	//Tile selection placement state
	int _prevXOffset, _prevYOffset;
	bool _selectionChanged;

	//Tile Region Filling Logic
	void _startRegionFill(QMouseEvent *event);
	void _updateRegionFill(QMouseEvent *event);
	void _endRegionFill(QMouseEvent *event);

	bool _populatePlaceTileGroup(const TileSelection<T> *const terSel, PlaceTileGroupUndoItem<T> *const groupCmd) const;
	bool _populatePlaceTileGroupRegionFill(const TileSelection<T> *const terSel, PlaceTileGroupUndoItem<T> *const groupCmd) const;

	//Overridden SourceView functionality
	virtual void _sceneSelectionAddRect(const QRectF &rect);
	virtual void _sceneSelectionUpdateLastRect(const QRectF &rect);
	virtual void _sceneSelectionClear(void);

	//Overridable DestView functionality
	virtual TileSelection<T> *_getTileSelection(void) const = 0;
	virtual void _setTileSelection(TileSelection<T> *const sel) = 0;
	virtual QUndoStack *_getUndoStack(void) const = 0;
	virtual PlaceTileGroupUndoItem<T> *_createPlaceTileGroup(void) = 0;
	virtual TileSelection<T> *_copyConstructTileSelection(const TileSelection<T> &rval) const = 0;

public:
	explicit DestView(QWidget *parent = 0);

	inline QPointF getFillFixedPivot(void) const { return _fillFixedPivot; }
	inline QPointF getFillCurrentPivot(void) const { return _fillCurrentPivot; }
	inline bool isFillingRegion(void) const { return _fillingRegion; }
	inline void setSelectionChanged(const bool val) { _selectionChanged = val; }

	/* Needs to be invoked whenever a terrain selection is made to
	   create the selection in the view. */
	void createSelectionItemGroup(void);
	void deleteSelectionItemGroup(void);
	void updateSelectionItemGroup(void);

	void placeSelection(void);
	void disableSelectionPlacementMerge(void);

	void selectionCut(void);
	void selectionPaste(void);
	void selectionFlipHorizontal(void);
	void selectionFlipVertical(void);
	void selectionRotateC(void);
	void selectionRotateCC(void);

	//reimplemented QT functionality

	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void scrollContentsBy(int dx, int dy);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void drawForeground(QPainter *painter, const QRectF &rect);
};

#include "destView.inl"

#endif // DESTVIEW_H
