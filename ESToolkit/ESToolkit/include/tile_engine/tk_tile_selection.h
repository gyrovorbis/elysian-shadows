#ifndef TILE_SELECTION_H
#define TILE_SELECTION_H

#include <QList>
#include <QPointF>
#include <gyro_video_api.h>
#include <tk_tile_graphics_item.h>

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
    //TileSelection(TileSelection &&rval);
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




template<typename T>
T TileSelection<T>::getEntry(const unsigned x, const unsigned y, const QPointF &mousePos) const {
    int selX = x;
    int selY = y;
    if(!mousePos.isNull()) _mapToSelectionCoords(x, y, mousePos, selX, selY);
    return _entry[selY][selX];
}


template<typename T>
T TileSelection<T>::getFillEntry(const int x, const int y, const QPointF &pivot, const QPointF &mousePos) const {
    unsigned fillX, fillY;
    _mapToFillCoords(x, y, pivot, mousePos, fillX, fillY);
    return _entry[fillY][fillX];
}

//-- Construction, destruction, copy construction, move construction

//Construction from a list of selected QGraphicsItems (Qt's selection engine)
template<typename T>
TileSelection<T>::TileSelection(const QList<QGraphicsItem *> &items):
    TileSelection()
{
    TileGraphicsItem<T> *terItem;
    //Tile offset
    int xOffset = INT_MAX;
    int yOffset = INT_MAX;

    Q_ASSERT(items.size());

    //Determine width and height of selection region
    for(int i = 0; i < items.size(); ++i) {
        terItem = static_cast<TileGraphicsItem<T> *>(items.at(i));

        if((signed)terItem->getTileX() < xOffset) xOffset = terItem->getTileX();
        if((signed)terItem->getTileY() < yOffset) yOffset = terItem->getTileY();
        if(terItem->getTileX() > _width) _width = terItem->getTileX();
        if(terItem->getTileY() > _height) _height = terItem->getTileY();
    }

    _width -= xOffset;
    _height -= yOffset;
    ++_width;
    ++_height;

    if(_width <= 0 || _height <= 0) {
        Q_ASSERT(0);
        return; //Wheeeeell! -- DOESNT HELP ANYBODY!!! NEED TO NOT SELECT THEN!
    }

    _allocArrays(_entry, _inSelection, _width, _height);

    //Populate arrays based on terrain values
    for(int i = 0; i < items.size(); ++i) {
        terItem = static_cast<TileGraphicsItem<T> *>(items.at(i));
        _inSelection[terItem->getTileY()-yOffset][terItem->getTileX()-xOffset] = true;
        //setSelectionEntry(_entry[terItem->getTileY()-yOffset][terItem->getTileX()-xOffset], terItem);
        _entry[terItem->getTileY()-yOffset][terItem->getTileX()-xOffset] = terItem->getTileEntry();
    }
}

template<typename T>
TileSelection<T>::TileSelection(const TileSelection &rval) {
    _width = rval._width;
    _height = rval._height;
    _allocArrays(_entry, _inSelection, _width, _height);

    for(unsigned i = 0; i < _height; ++i) {
        //memcpy(_entry[i], rval._entry[i], sizeof(_entry[0][0])*_width);
        for(unsigned j = 0; j < _width; ++j) _entry[i][j] = rval._entry[i][j];
        memcpy(_inSelection[i], rval._inSelection[i], sizeof(_inSelection[0][0])*_width);
    }
}

/*
template<typename T>
TileSelection<T>::TileSelection(TileSelection &&rval):
    _width(rval._width),
    _height(rval._height),
    _inSelection(rval._inSelection),
    _entry(rval._entry)
{
    rval._inSelection = rval._entry = nullptr;
}
*/
template<typename T>
TileSelection<T>::~TileSelection(void) {
    _freeArrays(_entry, _inSelection, _width, _height);
}


//---- MEMORY MANAGEMENT ----

template<typename T>
void TileSelection<T>::_allocArrays(T **&entry, bool **&selection, unsigned width, unsigned height) {
    //Allocate arrays for selected region and terrain values

    selection = new bool *[height];
    entry = new T *[height];

    for(unsigned y = 0; y < height; ++y){
        selection[y] = new bool[width];
        entry[y] = new T[width];
    }

    //Initialize selected region to empty
    for(unsigned y = 0; y < height; ++y) {
        for(unsigned x = 0; x < width; ++x) {
            selection[y][x] = false;
            //Clear entry out. An explicit default constructor is not required (or encouraged). <-you're a dipshit.
            //memset(&entry[y][x], 0, sizeof(T));
        }
    }
}

template <typename T>
void TileSelection<T>::_freeArrays(T **&entry, bool **&selection, unsigned /*width*/, unsigned height) {
    for(unsigned y = 0; y < height; ++y) {
        delete[] selection[y];
        delete[] entry[y];
    }
    delete[] selection;
    delete[] entry;

    selection = nullptr;
    entry = nullptr;
}

// ---- SELECTION MODIFICATION ----

template<typename T>
void TileSelection<T>::flipVertical(void) {
    for(unsigned x = 0; x < _width; ++x) {
        for(unsigned y = 0; y < _height/2; ++y) {
            T temp = _entry[y][x];
            _entry[y][x] = _entry[_height-y-1][x];
            _entry[_height-y-1][x] = temp;

            bool temb = _inSelection[y][x];
            _inSelection[y][x] = _inSelection[_height-y-1][x];
            _inSelection[_height-y-1][x] = temb;
        }
    }

    for(unsigned x = 0; x < _width; ++x) {
        for(unsigned y = 0; y < _height; ++y) {
            if(!_entry[y][x].getIndex()) continue;
            _entry[y][x].setOrient(reorient(_entry[y][x].getOrient(), TEX_REORIENT_FLIP_VERT));
        }
    }
}

template<typename T>
void TileSelection<T>::flipHorizontal(void) {
    for(unsigned y = 0; y < _height; ++y) {
        for(unsigned x = 0; x < _width/2; ++x) {
            T temp = _entry[y][x];
            _entry[y][x] = _entry[y][_width-x-1];
            _entry[y][_width-x-1] = temp;

            bool temb = _inSelection[y][x];
            _inSelection[y][x] = _inSelection[y][_width-x-1];
            _inSelection[y][_width-x-1] = temb;

        }
    }

    for(unsigned x = 0; x < _width; ++x) {
        for(unsigned y = 0; y < _height; ++y) {
            if(!_entry[y][x].getIndex()) continue;
            _entry[y][x].setOrient(reorient(_entry[y][x].getOrient(), TEX_REORIENT_FLIP_HORIZ));
        }
    }
}

template <typename T>
void TileSelection<T>::rotateC(void) {
    T **nEntry;
    bool **nSelected;
    unsigned nWidth = _height; unsigned nHeight = _width;
    _allocArrays(nEntry, nSelected, nWidth, nHeight);

    for(unsigned y = 0; y < nHeight; ++y) {
        for(unsigned x = 0; x < nWidth; ++x) {
            nEntry[y][x] = _entry[_height-x-1][y];
            nSelected[y][x] = _inSelection[_height-x-1][y];
            if(!_entry[_height-x-1][y].getIndex()) continue;
            nEntry[y][x].setOrient(reorient(_entry[_height-x-1][y].getOrient(), TEX_REORIENT_ROT_C));
        }
    }

    _freeArrays(_entry, _inSelection, _width, _height);

    _width = nWidth;
    _height = nHeight;
    _entry = nEntry;
    _inSelection = nSelected;
}

template <typename T>
void TileSelection<T>::rotateCC(void) {
    T **nEntry;
    bool **nSelected;
    unsigned nWidth = _height; unsigned nHeight = _width;
    _allocArrays(nEntry, nSelected, nWidth, nHeight);

    for(unsigned y = 0; y < nHeight; ++y) {
        for(unsigned x = 0; x < nWidth; ++x) {
            nEntry[y][x] = _entry[x][_width-y-1];
            nSelected[y][x] = _inSelection[x][_width-y-1];
            if(!_entry[x][_width-y-1].getIndex()) continue;
            nEntry[y][x].setOrient(reorient(_entry[x][_width-y-1].getOrient(), TEX_REORIENT_ROT_CC));
        }
    }

    _freeArrays(_entry, _inSelection, _width, _height);

    _width = nWidth;
    _height = nHeight;
    _entry = nEntry;
    _inSelection = nSelected;
}




#endif // TILE_SELECTION_H
