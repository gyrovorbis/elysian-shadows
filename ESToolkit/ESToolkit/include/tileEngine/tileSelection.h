#ifndef TILE_SELECTION_H
#define TILE_SELECTION_H

#include <QList>
#include <QPointF>
#include <gyro.h>

class QGraphicsItem;

/*
	typename T -
		Must be of "T" type
*/
template <typename T>
class TileSelection {
public:
	enum TEX_REORIENT { TEX_REORIENT_FLIP_HORIZ, TEX_REORIENT_FLIP_VERT, TEX_REORIENT_ROT_C, TEX_REORIENT_ROT_CC, TEX_REORIENT_MAX };

    TileSelection(void) = default;
    //Main constructor--based on QGraphicsItems that have been selected within the scene
	TileSelection(const QList<QGraphicsItem *> &items);
	//Copy constructor -- deep copy
	TileSelection(const TileSelection &rval);
    //Move constructor (C++11)
    TileSelection(TileSelection &&rval);
	virtual ~TileSelection(void);

	//Implemented here, so this check is not expensive.
	bool isWithinSelection(const int x, const int y, const QPointF &mousePos=QPointF()) const;
	bool isWithinSelectionFill(int x, int y, const QPointF &pivot, const QPointF &mousePos) const;

	//Selection Data
	//virtual uint16_t getIndex(const unsigned x, const unsigned y, const QPointF &mousePos) const;
	//virtual GY_RECT_TEX_ORIENT getOrient(const unsigned x, const unsigned y, const QPointF &mousePos) const;
	T getEntry(const unsigned x, const unsigned y, const QPointF &mousePos=QPointF()) const;

	//Region Fill Data
	//virtual uint16_t getFillIndex(int x, int y, const QPointF &pivot, const QPointF &mousePos) const;
	//virtual GY_RECT_TEX_ORIENT getFillOrient(int x, int y, const QPointF &pivot, const QPointF &mousePos) const;
	//x and y are relative if mousePos is supplied, absolute otherwise
	T getFillEntry(const int x, const int y, const QPointF &pivot, const QPointF &mousePos) const;

	unsigned getSize(void) const;
	void getSelectionArea(unsigned &width, unsigned &height) const;

	void flipVertical(void);
	void flipHorizontal(void);
	void rotateC(void);
	void rotateCC(void);

	static GY_RECT_TEX_ORIENT reorient(const GY_RECT_TEX_ORIENT orient, const TEX_REORIENT applied);

protected:
	static void _mapToSelectionCoords(const int mapX, const int mapY, const QPointF &mousePos, unsigned &selX, unsigned &selY);
	void _mapToFillCoords(int mapX, int mapY, const QPointF &pivot, const QPointF &mousePos, unsigned &fillX, unsigned &fillY) const;
    static void _mapToSelectionCoords(const int mapX, const int mapY, const QPointF &mousePos, int &selX, int &selY);

	static void _freeArrays(T **&entry, bool **&selection, unsigned width, unsigned height);
	static void _allocArrays(T **&entry, bool **&selection, unsigned width, unsigned height);

	static const GY_RECT_TEX_ORIENT _texOrientLut[GY_RECT_TEX_ORIENT_MAX][TEX_REORIENT_MAX];

    unsigned _width = 0;
    unsigned _height = 0;
	//Determines which indices have valid entries (sparse array)
    bool **_inSelection = nullptr;
	//Actual SPARSE SelectionEntry array
    T **_entry = nullptr;

};

//---- STATIC ----

//8x4 Lookup table for reorienting oriented textures. Flexing my devmeat!
template<typename T>
const GY_RECT_TEX_ORIENT TileSelection<T>::_texOrientLut[GY_RECT_TEX_ORIENT_MAX][TEX_REORIENT_MAX] = {
	{ GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_FLIP_VERT, GY_RECT_TEX_ORIENT_ROT_270, GY_RECT_TEX_ORIENT_ROT_90 },
	{ GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270 },
	{ GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90 },
	{ GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270, GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_ROT_180 },
	{ GY_RECT_TEX_ORIENT_FLIP_VERT, GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_ROT_270 },
	{ GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_NORMAL },
	{ GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_ROT_270, GY_RECT_TEX_ORIENT_FLIP_VERT, GY_RECT_TEX_ORIENT_FLIP_HORIZ },
	{ GY_RECT_TEX_ORIENT_ROT_270, GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_FLIP_VERT }
};

template<typename T>
GY_RECT_TEX_ORIENT TileSelection<T>::reorient(const GY_RECT_TEX_ORIENT orient, const TEX_REORIENT applied) {
	return _texOrientLut[orient][applied];
}

template<typename T>
void TileSelection<T>::_mapToSelectionCoords(const int mapX, const int mapY, const QPointF &mousePos, unsigned &selX, unsigned &selY) {
	//Subtracts mouse offset (+16, because each tile's origin is its center)
    selX = mapX-(int(mousePos.x()+16)/32);
    selY = mapY-(int(mousePos.y()+16)/32);
}

template<typename T>
void TileSelection<T>::_mapToSelectionCoords(const int mapX, const int mapY, const QPointF &mousePos, int &selX, int &selY) {
    const int offsetX = mousePos.x() < -16.0f? -16 : 16;
    const int offsetY = mousePos.y() < -16.0f? -16 : 16;
    //Subtracts mouse offset (+16, because each tile's origin is its center)
    selX = mapX-(int(mousePos.x()+offsetX)/32);
    selY = mapY-(int(mousePos.y()+offsetY)/32);
}

//---- INSTANCE ----

template<typename T>
void TileSelection<T>::_mapToFillCoords(int mapX, int mapY, const QPointF &pivot, const QPointF &mousePos, unsigned &fillX, unsigned &fillY) const {
	int pivotX = (pivot.x()+16)/32;
	int pivotY = (pivot.y()+16)/32;

	if(mousePos.x() < pivot.x()) ++pivotX;
	if(mousePos.y() < pivot.y()) ++pivotY;

	mapX += _width-(pivotX%_width);
	mapY += _height-(pivotY%_height);

	fillX = (mapX == 0)? 0 : mapX%_width; //divide-by-zero protection
	fillY = (mapX == 0)? 0 : mapY%_height;
}

template<typename T>
void TileSelection<T>::getSelectionArea(unsigned &width, unsigned &height) const {
	width = _width; height = _height;
}

template<typename T>
unsigned TileSelection<T>::getSize(void) const {
	return _width*_height;
}

template<typename T>
bool TileSelection<T>::isWithinSelection(const int x, const int y, const QPointF &mousePos) const {
    int selX = x;
    int selY = y;
	if(!mousePos.isNull()) _mapToSelectionCoords(x, y, mousePos, selX, selY);
    return (selX >= (int)_width || selY >= (int)_height || selY < 0 || selX < 0)? false : _inSelection[selY][selX];
}

template<typename T>
bool TileSelection<T>::isWithinSelectionFill(int x, int y, const QPointF &pivot, const QPointF &mousePos) const {
	unsigned fillX, fillY;
	_mapToFillCoords(x, y, pivot, mousePos, fillX, fillY);
	return _inSelection[fillY][fillX];
}

#include "tileSelection.inl"


#endif // TILE_SELECTION_H
