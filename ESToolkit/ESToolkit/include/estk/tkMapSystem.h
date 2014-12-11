#ifndef TK_MAP_SYSTEM_H
#define TK_MAP_SYSTEM_H

#include "elysian_map_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include "mapViewTileGraphicsItem.h"
#include "assets.h"

ELYSIAN_ASSET_TYPEDEF(MapSystem)
//ELYSIAN_ASSET_TYPEDEF(MapDepthBuffEntryImpl, struct)
//ELYSIAN_ASSET_TYPEDEF(EntityDepthBuffEntryImpl, struct)


struct TkMapDepthBuffEntryImpl: public elysian::MapDepthBuffEntryImplBase {
    TileGraphicsItem<TileEntry>::DRAW_STYLE drawStyle;
};

struct TkEntityDepthBuffEntryImpl: public elysian::EntityDepthBuffEntryImplBase {};


class TkMapSystem:  public MapViewTileGraphicsItem,
                    public elysian::MapSystemBase<TkMapSystem, TkMapDepthBuffEntryImpl, TkEntityDepthBuffEntryImpl>
{
    friend class ESToolkit;

protected:
    void        _renderCustomPass(const unsigned count, const TkMapDepthBuffEntryImpl array[], const elysian::NORMAL_TYPE normalType) const;

public:
    using       elysian::MapSystemBase<TkMapSystem, TkMapDepthBuffEntryImpl, TkEntityDepthBuffEntryImpl>::update;

    void        renderCustomPassImmediate(void);
    void        renderCustomPassAtElev(const unsigned char elev, const elysian::NORMAL_TYPE normalType) const;
    void        getObjectLayerEntry(const unsigned x, const unsigned y, TkMapDepthBuffEntryImpl &entry);

};


// ====== INLINEZ =====

inline void TkMapSystem::renderCustomPassImmediate(void) {
    for(unsigned j = _scrStartY; j <= _scrEndY; ++j) {
        for(unsigned i = _scrStartX; i <= _scrEndX; ++i) {
            for(unsigned k = 0; k < 3; ++k) {
                TkMapDepthBuffEntryImpl curMapEntry;
                curMapEntry.x = i;
                curMapEntry.y = j;
                curMapEntry.layer = k;
                getObjectLayerEntry(i, j, curMapEntry);

                int elevation = curMapEntry.mapEntry.getElev();//-1;

                if(elevation == 0 && !curMapEntry.mapEntry.getPlane()) { //render tile immediately at elevation -1 (below entities)
                    //Immediate render, or buffer if it requires multiple passes
                    _renderCustomPass(1, &curMapEntry, elysian::NORMAL_FLAT);
                }
            }
        }
    }
}

inline void TkMapSystem::renderCustomPassAtElev(const unsigned char elev, const elysian::NORMAL_TYPE normalType) const {
    MapSystemBase::renderCustomPassAtElev(elev, normalType);
    _renderCustomPass(_objectCount[elev], _objectBuff[elev], normalType);
}

/*
 *   GYVector4  { x*32.0f-16.0f, y*32.0f - 16.0f +tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }

                */


inline void TkMapSystem::_renderCustomPass(const unsigned count, const TkMapDepthBuffEntryImpl array[], const elysian::NORMAL_TYPE normalType) const {
    //DRAW_NORMAL, DRAW_SELECTING, DRAW_SELECTION, DRAW_FILLING, DRAW_MAX

    GYColor4 decorColors[DRAW_MAX] = {
        { 0, 0, 0, 0 },
        { 255, 255, 0, 100 },
        { 255, 0, 255, 100},
        { 0, 255, 255, 100}
    };

    auto drawStyle = _getTileDrawState();

    if(drawStyle != DRAW_NORMAL) {
        GYColor4 color = decorColors[drawStyle];
        //GYVector4 { x*32.0f+16.0f, y*32.0f+ 16.0f +  tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
        gyVidTexBind(nullptr);
        gyVidNormalMapBind(nullptr);
        gyVidSpecularMapBind(nullptr);
        gyVidAlphaLevel(GY_ALPHA_FULL);
        glEnable(GL_DEPTH_TEST);
        gyVidQuadListBegin(&color);
        for(unsigned i = 0; i < count; ++i) {
            auto& entry = array[i];
            if(entry.drawStyle == DRAW_NORMAL /* || !entry.mapEntry.getIndex()*/) continue;
            gyVidRectPlaneType(entry.mapEntry.getPlane());
            gyMatPush();
			gyMatLoadIdentity();
            if(normalType == elysian::NORMAL_FLAT)
                gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f + entry.mapEntry.getElev()*32.0f+1.0f, entry.mapEntry.getElev()*32.0f+1.0f, 32.0f, 32.0f, 32.0f);
            else gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f + entry.mapEntry.getElev()*32.0f+33.0f, entry.mapEntry.getElev()*32.0f+33.0f, 32.0f, 32.0f, 32.0f);
            gyVidRect(0);
            gyMatPop();
        }
        gyVidQuadListEnd();
        glDisable(GL_DEPTH_TEST);
    }
}


inline void TkMapSystem::getObjectLayerEntry(const unsigned x, const unsigned y, TkMapDepthBuffEntryImpl& entry) {
#if 1
    _currentLayer   = (Assets::LAYER_TYPE)entry.layer;
    entry.lights    = getLightSys()->getLightMapEntryMapCoord(entry.y, entry.x);
    entry.drawStyle = (getAssets()->getCurrentLayer() == _currentLayer)? _getTileDrawStyle(x, y) : DRAW_NORMAL;
    entry.mapEntry  = _getTileEntryFromDrawStyle(x, y, entry.drawStyle);
#else
    MapSystemBase<TkMapSystem, TkMapDepthBuffEntryImpl, TkEntityDepthBuffEntryImpl>::getObjectLayerEntry(x, y, entry);
    entry.drawStyle = (getAssets()->getCurrentLayer() == entry.layer)? _getTileDrawStyle(x, y) : DRAW_NORMAL;
#endif
}

#endif // TK_MAP_SYSTEM_H
