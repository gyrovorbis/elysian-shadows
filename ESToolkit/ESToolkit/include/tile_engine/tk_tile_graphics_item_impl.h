#ifndef TK_TILE_GRAPHICS_ITEM_IMPL_H
#define TK_TILE_GRAPHICS_ITEM_IMPL_H

#include <tk_dest_view.h>
#include <tk_tile_selection.h>

// ======== TEMPLATE IMPLEMENTATION

template<typename T>
TileGraphicsItem<T>::TileGraphicsItem(const TYPE type): _type(type), _renderTile0(false), _flipRotBits(true) {
    _view.src = nullptr;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
}

//tile is currently being selected for placement
template<typename T>
bool TileGraphicsItem<T>::_isTileSelecting(const unsigned x, const unsigned y) const {
    return ((_type == TYPE_SOURCE || (_type == TYPE_DEST && !_view.dst->isFillingRegion())) && _isSelectedWithinView(x, y));
}

//tile is currently being selected for region fill
template<typename T>
bool TileGraphicsItem<T>::_isTileFilling(const unsigned x, const unsigned y) const {
    if(_type != TYPE_DEST || !_view.dst->isFillingRegion()) return false;
    const TileSelection<T> *tileSel = _getTileSelection();
    QPointF fillPivot = _view.dst->getFillFixedPivot();

    Q_ASSERT(tileSel); //This should never happen
    return (_isSelectedWithinView(x, y) && tileSel->isWithinSelectionFill(x, y, fillPivot, _view.dst->getSceneMousePos()));
}

//tile has selection hovering over it
template<typename T>
bool TileGraphicsItem<T>::_isTileSelection(const unsigned x, const unsigned y) const {
    if(_type == TYPE_SOURCE) return false;
    if(_type == TYPE_DEST && _view.dst->isFillingRegion()) return false;

    const TileSelection<T> *tileSel = _getTileSelection();
    QPointF mouseLoc = _view.src->getSceneMousePos();

    if(tileSel) {
        return tileSel->isWithinSelection(x, y, mouseLoc);
    }
    return false;
}

template<typename T>
QRectF TileGraphicsItem<T>::boundingRect(void) const {
    return QRectF(-16.0f, -16.0f, 32.0f, 32.0f);
}


template<typename T>
typename TileGraphicsItem<T>::DRAW_STYLE TileGraphicsItem<T>::_getTileDrawState(void) const {
    //Uglier and faster
    const TileSelection<T> *tileSel = _getTileSelection();
    //const QPointF mouseLoc = _view.src->getSceneMousePos();

    if(tileSel) {
        //Region Fill
        if(_view.dst->isFillingRegion()) {
            return DRAW_FILLING;
        //Selection
        } else {
            return DRAW_SELECTION;
        }
    }

    //Selecting
    if(_view.src->isSelecting()) {
        return DRAW_SELECTING;
    }

    return DRAW_NORMAL;
}

template<typename T>
typename TileGraphicsItem<T>::DRAW_STYLE TileGraphicsItem<T>::_getTileDrawStyle(const unsigned x, const unsigned y) const {
#if 0
    //Prettier and slower
    if(_isTileSelection(x, y)) {
        return DRAW_SELECTION;
    } else if(_isTileSelecting(x, y)) {
        return DRAW_SELECTING;
    } else if(_isTileFilling(x, y)) {
        return DRAW_FILLING;
    }
    return DRAW_NORMAL;
#else
    //Uglier and faster
    const TileSelection<T> *tileSel = _getTileSelection();
    const QPointF mouseLoc = _view.src->getSceneMousePos();

    if(tileSel) {
        //Region Fill
        if(_view.dst->isFillingRegion()) {
            if(_isSelectedWithinView(x, y)) {
                if(tileSel->isWithinSelectionFill(x, y, _view.dst->getFillFixedPivot(), mouseLoc)) {
                    return DRAW_FILLING;
                }
            }
        //Selection
        } else {
            if(tileSel->isWithinSelection(x, y, mouseLoc)) {
                return DRAW_SELECTION;
            }
        }
        return DRAW_NORMAL;
    }

    //Selecting
    if(_view.src->isSelecting()) {
        if(_isSelectedWithinView(x, y)) {
            return DRAW_SELECTING;
        }
    }

    return DRAW_NORMAL;
#endif
}

template<typename T>
T TileGraphicsItem<T>::_getTileEntryFromDrawStyle(const unsigned x, const unsigned y, const DRAW_STYLE style) const {
    QPointF mouseLoc;
    QPointF fillPivot;

    switch(style) {
    default:
        Q_ASSERT(0);
    case DRAW_FILLING:
        mouseLoc = _view.src->getSceneMousePos();
        fillPivot = _view.dst->getFillFixedPivot();
        return _getTileSelection()->getFillEntry(x, y, fillPivot, mouseLoc);
    case DRAW_SELECTION:
        mouseLoc = _view.src->getSceneMousePos();
        return _getTileSelection()->getEntry(x, y, mouseLoc);
    case DRAW_SELECTING:
    case DRAW_NORMAL:
        return getTileEntry(x, y);
    }
}

template<typename T>
void TileGraphicsItem<T>::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {
    DRAW_STYLE style = _getTileDrawStyle(_x, _y);
    T entry = _getTileEntryFromDrawStyle(_x, _y, style);
    _renderTile(painter, entry, style, _x, _y, true);
}

template<typename T>
void TileGraphicsItem<T>::_renderTile(QPainter *const painter, const T entry, const DRAW_STYLE style, const unsigned x, const unsigned y, const bool decorate) {
    static const QColor selectionColor(225, 202, 251, 80);
    static const QColor shadedColor(180, 251, 225, 80);
    static const QColor fillColor(250, 250, 150, 80);
    static const QPen selectionPen(selectionColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    static const QPen shadedPen(shadedColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    static const QPen fillPen(fillColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    //Don't render unecessary blank tiles
    if(entry.getIndex() != 0 || _renderTile0) {
        //Grab pixmap
        QPixmap pixmap = _getTilePixmap(entry,
                                (style == DRAW_SELECTION || style == DRAW_FILLING)? true : false);

        GY_RECT_TEX_ORIENT texOrient = entry.getOrient();

        //Render tile
        _renderFlipRotPixmap(painter, pixmap, texOrient);

        //Render custom decoration
        if(decorate) entry.renderDecoration(painter, x, y);

    }

    //Render DRAW_STYLE decoration
    switch(style) {
    default:
        Q_ASSERT(0);
        break;
    case DRAW_SELECTION:
        painter->fillRect(QRect(-16, -16, 32, 32), selectionColor);
        painter->setPen(selectionPen);
        painter->drawRect(QRect(-16,-16,31,31));
        break;
    case DRAW_FILLING:
        painter->fillRect(QRect(-16, -16, 32, 32), fillColor);
        painter->setPen(fillPen);
        painter->drawRect(QRect(-16, -16, 31, 31));
        break;
    case DRAW_NORMAL:
        break;
    case DRAW_SELECTING:
        painter->fillRect(QRect(-16, -16, 32, 32), shadedColor);
        painter->setPen(shadedPen);
        painter->drawRect(QRect(-16,-16,31,31));
        break;
    }
}

/*
 Do NOT fucking forget that the handedness of QT's coordinate system is reverse the engine's. So a 90 degree
 rotation is counterclockwise in the engine, while it's clockwise in QT. Hence we are negating rotation here.
 */
template<typename T>
void TileGraphicsItem<T>::_renderFlipRotPixmap(QPainter *const painter, const QPixmap &pixmap, const GY_RECT_TEX_ORIENT orient) {
    switch(orient) {
    case GY_RECT_TEX_ORIENT_NORMAL:
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        break;
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
        painter->scale(-1.0, 1.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->scale(-1.0, 1.0);
        break;
    case GY_RECT_TEX_ORIENT_FLIP_VERT:
        painter->scale(1.0, -1.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->scale(1.0, -1.0);
        break;
    case GY_RECT_TEX_ORIENT_ROT_90:
        painter->rotate(-90.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->rotate(90.0);
        break;
    case GY_RECT_TEX_ORIENT_ROT_180:
        painter->rotate(180.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->rotate(-180.0);
        break;
    case GY_RECT_TEX_ORIENT_ROT_270:
        painter->rotate(-270.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->rotate(270.0);
        break;
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
        painter->scale(-1.0, 1.0);
        painter->rotate(-90.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->rotate(90.0);
        painter->scale(-1.0, 1.0);
        break;
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
        painter->scale(-1.0, 1.0);
        painter->rotate(-270.0);
        painter->drawPixmap(-16, -16, 32, 32, pixmap);
        painter->rotate(270.0);
        painter->scale(-1.0, 1.0);
        break;
    default:
        Q_ASSERT(0);
    }

    //WHEEEEEEEELL!!!! SCREW THIS!!!
    //Correct our own transformation reest.
    //painter->resetTransform();

}



#endif // TK_TILE_GRAPHICS_ITEM_IMPL_H
