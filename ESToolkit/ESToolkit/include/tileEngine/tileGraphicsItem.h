#ifndef TILE_GRAPHICS_ITEM_H
#define TILE_GRAPHICS_ITEM_H

#include <QGraphicsRectItem>
#include <gyro.h>

class SourceView;

template<typename T>
class DestView;

template<typename T>
class TileSelection;

template<typename T>
class TileGraphicsItem: public QGraphicsRectItem {
public:
	enum TYPE { TYPE_SOURCE, TYPE_DEST };
	enum DRAW_STYLE { DRAW_NORMAL, DRAW_SELECTING, DRAW_SELECTION, DRAW_FILLING };
protected:
	unsigned _x, _y;

	union {
		const SourceView *src;
		const DestView<T> *dst;
	} _view;

	TYPE _type;
	bool _renderTile0;
	bool _flipRotBits;


	bool _isTileSelecting(const unsigned x, const unsigned y) const;
	bool _isTileFilling(const unsigned x, const unsigned y) const;
	bool _isTileSelection(const unsigned x, const unsigned y) const;

	DRAW_STYLE _getTileDrawStyle(const unsigned x, const unsigned y) const;
	T _getTileEntryFromDrawStyle(const unsigned x, const unsigned y, const DRAW_STYLE style) const;

	void _renderTile(QPainter *const painter, const T entry, const DRAW_STYLE style, const unsigned x, const unsigned y, const bool decorate);
	void _renderFlipRotPixmap(QPainter *const painter, const QPixmap &pixmap, const GY_RECT_TEX_ORIENT orient);

	//IMPLEMENT ME!
	virtual bool _isSelectedWithinView(const unsigned x=0, const unsigned y=0) const=0;
	virtual const QPixmap _getTilePixmap(const T entry, const bool selection=false) const=0;
	virtual const TileSelection<T> *_getTileSelection(void) const=0;

public:

	TileGraphicsItem(const TYPE type);

	inline void setView(const SourceView *const view) { _view.src = view; }
	inline void setTilePosition(const unsigned x, const unsigned y) {
		_x = x; _y = y;
		setPos(x*32.0f, y*32.0f);
	}

	inline unsigned getTileX(void) const { return _x; }
	inline unsigned getTileY(void) const { return _y; }
	inline void useFlipRotBits(const bool val=true) { _flipRotBits = val; }
	inline void renderTile0(const bool val=true) { _renderTile0 = val; }

	virtual QRectF boundingRect(void) const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


	//IMPLEMENT ME!
	//virtual uint16_t getIndex(const unsigned x=0, const unsigned y=0) const=0;
	//virtual GY_RECT_TEX_ORIENT getOrient(const unsigned x=0, const unsigned y=0) const=0;
	//virtual TileEntry &getEntry(const unsigned x=0, const unsigned y=0) const=0;
	virtual T getTileEntry(const unsigned x=0, const unsigned y=0) const = 0;
	//virtual void setTileEntry(const T entry) = 0;
};

#include "tileGraphicsItem.inl"
#endif // TILE_GRAPHICS_ITEM_H
