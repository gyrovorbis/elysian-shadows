#ifndef ELYSIAN_BLEND_MODE_BASE_HPP
#define ELYSIAN_BLEND_MODE_BASE_HPP

#include <AssetIO/crtp/elysian_color.hpp>

ELYSIAN_ASSET_TYPEDEF(BlendMode)

namespace elysian {

    class BlendModeBase:
        public LuaTableProxy<BlendModeBase, BlendMode> {
    private:
        Color4          _constColor;
        uint8_t         _srcFactor      = 0;
        uint8_t         _dstFactor      = 0;

    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(constColor, getConstColor,  setConstColor)
            ELYSIAN_LUA_PROXY_FIELD(srcFactor,  getSrcFactor,   setSrcFactor)
            ELYSIAN_LUA_PROXY_FIELD(dstFactor,  getDstFactor,   setDstFactor)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        BlendModeBase(void) = default;
                        BlendModeBase(GY_BLEND_MODE srcFactor, GY_BLEND_MODE dstFactor, Color4 constColor=Color4());
        GY_BLEND_MODE   getSrcFactor(void) const;
        GY_BLEND_MODE   getDstFactor(void) const;
        void            setSrcFactor(GY_BLEND_MODE factor);
        void            setDstFactor(GY_BLEND_MODE factor);
        Color4          getConstColor(void);
        void            setConstColor(Color4 color);

    };



    //===== INLINEZ ======

    inline BlendModeBase::BlendModeBase(GY_BLEND_MODE srcFactor, GY_BLEND_MODE dstFactor, Color4 constColor):
        _constColor(constColor),
        _srcFactor(srcFactor),
        _dstFactor(dstFactor)
    {}

    inline GY_BLEND_MODE BlendModeBase::getSrcFactor(void) const { return (GY_BLEND_MODE)_srcFactor; }
    inline GY_BLEND_MODE BlendModeBase::getDstFactor(void) const { return (GY_BLEND_MODE)_dstFactor; }
    inline void BlendModeBase::setSrcFactor(GY_BLEND_MODE factor) { _srcFactor = factor; }
    inline void BlendModeBase::setDstFactor(GY_BLEND_MODE factor) { _dstFactor = factor; }
    inline Color4 BlendModeBase::getConstColor(void) { return &_constColor; }
    inline void BlendModeBase::setConstColor(Color4 color) { _constColor = std::move(color); }

}

#endif // ELYSIAN_BLEND_MODE_HPP
