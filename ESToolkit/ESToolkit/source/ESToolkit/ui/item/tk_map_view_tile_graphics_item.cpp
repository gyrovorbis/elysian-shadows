#include <ESToolkit/ui/item/tk_map_view_tile_graphics_item.h>
#include <ESToolkit/AssetIO/tk_area.h>

MapViewTileGraphicsItem::MapViewTileGraphicsItem(void):
    TileGraphicsItem<TileEntry>::TileGraphicsItem(TileGraphicsItem<TileEntry>::TYPE_DEST),
    _currentLayer(Assets::LAYER_TILE1)
{
    TileGraphicsItem<TileEntry>::setFlag(QGraphicsItem::ItemIsSelectable, false);
}


TileEntry MapViewTileGraphicsItem::_getTileEntryFromDrawStyle(const unsigned x, const unsigned y, const DRAW_STYLE style) const {
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
        //mouseLoc = _view.src->getSceneMousePos();
        //return _getTileSelection()->getEntry(x, y, mouseLoc);
    case DRAW_SELECTING:
    case DRAW_NORMAL:
        return getTileEntry(x, y);
    }
}

QRectF MapViewTileGraphicsItem::boundingRect(void) const {
    return getCurrentArea() ? QRectF(-16.0f, -16.0f, getCurrentArea()->getMapWidth()*32.0f, getCurrentArea()->getMapHeight()*32.0f)
                            : QRectF(-16.0f, -16.0f, 32.0f, 32.0f) ;
}

QRect MapViewTileGraphicsItem::getVisibleTileRegion(void) const {
    //Calculate the visible sceneRect region
    QPointF topLeft = this->_view.dst->mapToScene(0,0);
    QPointF bottomRight = this->_view.dst->mapToScene(this->_view.dst->viewport()->width(), this->_view.dst->viewport()->height());
    QRectF visibleSceneRect(topLeft, bottomRight);

    //Calculate the terrain render region based on the sceneRect
    QRect renderRegion;
    renderRegion.setTop((((int)visibleSceneRect.top())+16)/32-1);
    renderRegion.setLeft((((int)visibleSceneRect.left())+16)/32-1);
    renderRegion.setRight((((int)visibleSceneRect.right())+16)/32+1);
    renderRegion.setBottom((((int)visibleSceneRect.bottom())+16)/32+1);

    //Bind the region
    if(renderRegion.left() < 0) renderRegion.setLeft(0);
    if(renderRegion.right() > getCurrentArea()->getMapWidth()-1) renderRegion.setRight(getCurrentArea()->getMapWidth()-1);
    if(renderRegion.top() < 0) renderRegion.setTop(0);
    if(renderRegion.bottom() > getCurrentArea()->getMapHeight()-1) renderRegion.setBottom(getCurrentArea()->getMapHeight()-1);

    return renderRegion;
}

/*
  This shit is the heart of all terrain rendering for the map.
  Exercise caution before fucking with this. --FG
*/
void MapViewTileGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {
    if(!getCurrentLevel() || !getCurrentArea()) return;

    //I'm culling this shit manually, so enabling this should actually be slower... --FG
    //painter->setClipRect(option->exposedRect);

    QRect renderRegion = getVisibleTileRegion();

    //Move the painter to the beginning of the renderRegion
    painter->translate(32*renderRegion.left(), 32*renderRegion.top());
    //Check if the layer this Item represents is even enabled
        //Iterate over entire visible region
        for(int j = renderRegion.top(); j <= renderRegion.bottom(); ++j) {
            for(int i = renderRegion.left(); i <= renderRegion.right(); ++i) {
                  /*Optimization -- If we aren't the current layer, then there is
                  no reason we will even have a special render decoration.
                  Skip all of the bullshit logic.*/
                if(getAssets()->getCurrentLayer() != _currentLayer) {
                    _renderTile(painter, getTileEntry(i,j), this->DRAW_NORMAL, i, j, false);
                } else {
                    TileGraphicsItem<TileEntry>::DRAW_STYLE style = this->_getTileDrawStyle(i, j);
                    TileEntry entry = _getTileEntryFromDrawStyle(i, j, style);
                    this->_renderTile(painter, entry, style, i, j, true);
                }
                //advance to next column
                painter->translate(32, 0);
            }
            //advance to first column of next row
            painter->translate(-32*(renderRegion.width()), 32);
        }
}
