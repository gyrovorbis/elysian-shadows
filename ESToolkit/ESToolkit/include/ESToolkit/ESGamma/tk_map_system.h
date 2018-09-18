#ifndef TK_MAP_SYSTEM_H
#define TK_MAP_SYSTEM_H

#include <ESGamma/system/elysian_map_system_base.hpp>
#include <ESToolkit/ui/item/tk_map_view_tile_graphics_item.h>
#include <ESGamma/system/elysian_gui_system_base.hpp>
#include <ESToolkit/core/tk_assets.h>

class TkMapSystem:  public MapViewTileGraphicsItem,
                    public elysian::MapSystemBase
{
    friend class ESToolkit;
    typedef elysian::MapSystemBase MapSystemBase;
public:
    enum TILE_DECOR_TYPE { TILE_DECOR_NONE, TILE_DECOR_INDEX, TILE_DECOR_ELEV, TILE_DECOR_ORIENT, TILE_DECOR_PLANE, TILE_DECOR_MAX };

protected:
    TILE_DECOR_TYPE             _tileDecor              = TILE_DECOR_NONE;
    bool                        _planeDecor             = false;
    static bool                 _renderMapBackground;
    static elysian::Material*   _selMat;
    static elysian::Material*   _bgMat;

    void        _renderCustomPass(const unsigned count, const TkMapDepthBuffEntryImpl array[], const elysian::NORMAL_TYPE normalType) const;
    void        _renderMapBorders(void) const;

    bool        _useCustomPass(void) const;
public:
    using       elysian::MapSystemBase::update;

    static void staticInitialize(void);
    static void staticUninitialize(void);

    //static polymorphic elysian::MapDepthBuffer CRTP overrides
    void        renderCustomPassImmediate(void);
    void        renderCustomPassAtElev(const int elev, const elysian::NORMAL_TYPE normalType) const;
    void        getObjectLayerEntry(const unsigned x, const unsigned y, TkMapDepthBuffEntryImpl &entry);

    void        setTileDecor(const TILE_DECOR_TYPE type);
    void        setPlaneDecor(const bool val);
    static void setRenderMapBackground(const bool val);
    static bool getRenderMapBackground(void);
};

// ====== INLINEZ =====

inline bool TkMapSystem::_useCustomPass(void) const {
    return (_getTileDrawState() != DRAW_NORMAL || _tileDecor != TILE_DECOR_NONE);

}

inline void TkMapSystem::renderCustomPassImmediate(void) {
    int count = 0;
    if(!_useCustomPass()) return;

    for(int j = _scrStartY; j <= _scrEndY; ++j) {
        for(int i = _scrStartX; i <= _scrEndX; ++i) {
            for(unsigned k = 0; k < 3; ++k) {
                TkMapDepthBuffEntryImpl curMapEntry;
                curMapEntry.x = i;
                curMapEntry.y = j;
                curMapEntry.layer = k;
                getObjectLayerEntry(i, j, curMapEntry);

                int elevation = curMapEntry.mapEntry.getElev();//-1;

                if(elevation == 0 && !curMapEntry.mapEntry.getPlane()) { //render tile immediately at elevation -1 (below entities)
                    //Immediate render, or buffer if it requires multiple passes
                    _objectBuffImmediate[++count] = curMapEntry;
                }
            }
        }
    }
    if(count)
        _renderCustomPass(count, _objectBuffImmediate, elysian::NORMAL_FLAT);
}

inline void TkMapSystem::renderCustomPassAtElev(const int elev, const elysian::NORMAL_TYPE normalType) const {
    MapSystemBase::renderCustomPassAtElev(elev, normalType);
    if(elev != -1 && _useCustomPass()) _renderCustomPass(_objectCount[elev], _objectBuff[elev], normalType);
    else           _renderMapBorders();
}

/*
 *   GYVector4  { x*32.0f-16.0f, y*32.0f - 16.0f +tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }

                */


inline void TkMapSystem::_renderCustomPass(const unsigned count, const TkMapDepthBuffEntryImpl array[], const elysian::NORMAL_TYPE normalType) const {
    if(getLightSys()->isShadowPass() || !count) return;
    //DRAW_NORMAL, DRAW_SELECTING, DRAW_SELECTION, DRAW_FILLING, DRAW_MAX

    static const GYColor4 selectionDecorColors[DRAW_MAX] = {
        { 0, 0, 0, 0 },
        { 0, 255, 255, 100 },
        { 255, 255, 0, 100},
        { 255, 0, 255, 100}
    };
//    enum TILE_DECOR_TYPE { TILE_DECOR_NONE, TILE_DECOR_INDEX, TILE_DECOR_ELEV, TILE_DECOR_ORIENT, TILE_DECOR_PLANE, TILE_DECOR_MAX };
    /*
    static const GYColor4 elevDecorColors[TILE_DECOR_MAX] = {
        { 0, 0, 0, 0 },
        { },
        {},
        {},
        {}
    };*/

    static const char* orientDecorStrs[GY_RECT_TEX_ORIENT_MAX] = {
        "", "H", "V", "R90", "R180", "R270", "HR90", "HR270"
    };

    static const char* planeDecorStrs[GY_RECT_PLANE_MAX] = {
        "B", "F", "L", "R", "F45", "L45", "R45"
    };

    gyVidVertexBatchFlush();
    gyVidVertexBatchEnd();

    uint16_t            tileIndex;
    GY_RECT_PLANE_TYPE  tilePlane;
    GY_RECT_TEX_ORIENT  tileOrient;
    unsigned char       tileElev;
    GY_RECT_PLANE_TYPE  planeType = normalType? GY_RECT_PLANE_FRONT : GY_RECT_PLANE_BOTTOM;

    gyVidRectPlaneType(planeType);

    if(_tileDecor != TILE_DECOR_NONE) {
        gyVidDepthTest(false);
        gyVidAlphaLevel(GY_ALPHA_NONE);
        auto& twcen = getGuiSys()->pragmata18;
        twcen.setColor(255, 255, 255, 255);
        twcen._transform.setSize(16.0f, 32.0f, 32.0f);
        for(unsigned i = 0; i < count; ++i) {
            auto& entry = array[i];
            if(!entry.mapEntry.getIndex() || entry.mapEntry.getPlane() != planeType || entry.layer != getAssets()->getCurrentLayer()) continue;
            if(normalType == elysian::NORMAL_FLAT) twcen._transform.setPos(entry.x*32.0f/*+16.0*/, entry.y*32.0f + entry.mapEntry.getElev()*32.0f+1.0f/*+8.0f*/, entry.mapEntry.getElev()*32.0f+1.0f);
            else twcen._transform.setPos(entry.x*32.0f/*+16.0*/, entry.y*32.0f + entry.mapEntry.getElev()*32.0f/*+16.0f*/, entry.mapEntry.getElev()*32.0f-8.0f);

            switch(_tileDecor) {

            case TILE_DECOR_INDEX:
                tileIndex = entry.mapEntry.getIndex();
                if(tileIndex) twcen.drawStringf(elysian::Font::C_ALIGN, "%d", tileIndex);
                break;
            case TILE_DECOR_ELEV:
                tileElev = entry.mapEntry.getElev();
                if(tileElev) twcen.drawStringf(elysian::Font::C_ALIGN, "%d", tileElev);
                break;
            case TILE_DECOR_ORIENT:
                tileOrient = entry.mapEntry.getOrient();
                if(tileOrient != GY_RECT_TEX_ORIENT_NORMAL) twcen.drawString(elysian::Font::C_ALIGN, orientDecorStrs[(int)tileOrient]);
                break;
            case TILE_DECOR_PLANE:
                tilePlane = entry.mapEntry.getPlane();
                if(tilePlane != GY_RECT_PLANE_BOTTOM) twcen.drawString(elysian::Font::C_ALIGN, planeDecorStrs[(int)tilePlane]);
                break;
            default:
                break;
            }
        }
    }

   /*
   font._transform.setPos(X_COORD, Y_COORD1+ yCoord2, Z_COORD);
   font.setColor(r, g, b, 255);
   if(i == _startIndex) {
       float tempAlpha = 1.0f - _fadeCounter;
       font.setAlpha(tempAlpha*_baseAlpha*255.0f);
   }
   else font.setAlpha(_baseAlpha*255.0f);
   font.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, "%s", _log[i]);
*/

    //twcen._transform.setSize();


    //Tile selection decoration
    auto drawStyle = _getTileDrawState();
    if(drawStyle != DRAW_NORMAL) {
        gyVidTexBind(nullptr);
        gyVidNormalMapBind(nullptr);
        gyVidSpecularMapBind(nullptr);
        gyVidAlphaLevel(GY_ALPHA_FULL);
        gyVidBlendMode(GY_BLEND_SRC_ALPHA, GY_BLEND_ONE_MINUS_SRC_ALPHA);
        gyVidDepthTest(false);
        gyVidDepthTest(false);
        gyVidMaterialBind(&_selMat[drawStyle]);

        const GYColor4& color = selectionDecorColors[drawStyle];
        //GYVector4 { x*32.0f+16.0f, y*32.0f+ 16.0f +  tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },

        //WTF?
        gyVidQuadListBegin(&color);
        for(unsigned i = 0; i < count; ++i) {
            auto& entry = array[i];
            //int plane = entry.mapEntry.getPlane();
            //int elev = entry.mapEntry.getElev();
            if(entry.drawStyle == DRAW_NORMAL || entry.mapEntry.getPlane() != planeType/* || !entry.mapEntry.getIndex()*/) continue;
            gyVidRectPlaneType(entry.mapEntry.getPlane());
            gyMatPush();
            gyMatLoadIdentity();
            if(normalType == elysian::NORMAL_FLAT)
                //Had to add the offset of +3.0f to get the decoration above tiles. Could be potentially reested.
                gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f + entry.mapEntry.getElev()*32.0f, entry.mapEntry.getElev()*32.0f+10.0, 32.0f, 32.0f, 32.0f);
            else gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f + entry.mapEntry.getElev()*32.0f, entry.mapEntry.getElev()*32.0f+10.0, 32.0f, 32.0f, 32.0f);
            gyVidRect(-1);
            gyMatPop();

        }
        gyVidQuadListEnd();
        gyVidVertexBatchFlush();
        gyVidVertexBatchEnd();
        gyVidMaterialBind(nullptr);
    }
}


inline void TkMapSystem::getObjectLayerEntry(const unsigned x, const unsigned y, TkMapDepthBuffEntryImpl& entry) {
#if 1
    _currentLayer   = (Assets::LAYER_TYPE)entry.layer;
    entry.lights    = getLightSys()->getLightMapEntryMapCoord(entry.y, entry.x);
    entry.drawStyle = (getAssets()->getCurrentLayer() == _currentLayer)? _getTileDrawStyle(x, y) : DRAW_NORMAL;
    entry.mapEntry  = _getTileEntryFromDrawStyle(x, y, entry.drawStyle);
    entry.renderPass = 0;
#else
    MapSystemBase<TkMapSystem, TkMapDepthBuffEntryImpl, TkEntityDepthBuffEntryImpl>::getObjectLayerEntry(x, y, entry);
    entry.drawStyle = (getAssets()->getCurrentLayer() == entry.layer)? _getTileDrawStyle(x, y) : DRAW_NORMAL;
#endif
}

inline void TkMapSystem::setTileDecor(const TILE_DECOR_TYPE type) { _tileDecor = type; }
inline void TkMapSystem::setPlaneDecor(const bool val) { _planeDecor = val; }
inline void TkMapSystem::setRenderMapBackground(const bool val) { _renderMapBackground = val; }
inline bool TkMapSystem::getRenderMapBackground(void) { return _renderMapBackground; }

#endif // TK_MAP_SYSTEM_H
