#ifndef TK_DEPTH_BUFF_ENTRY_H
#define TK_DEPTH_BUFF_ENTRY_H

#include <level/elysian_depth_buff_entry_base.hpp>
#include <tile_engine/tk_tile_graphics_item.h>

struct TkMapDepthBuffEntryImpl: public elysian::MapDepthBuffEntryImplBase {
    TileGraphicsItem<TileEntry>::DRAW_STYLE drawStyle;
};

struct TkEntityDepthBuffEntryImpl: public elysian::EntityDepthBuffEntryImplBase {};



#endif // TK_DEPTH_BUFF_ENTRY_H

