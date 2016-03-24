#ifndef ELYSIAN_COLOR_HPP
#define ELYSIAN_COLOR_HPP

#include <gyro_video_api.h>
#include <crtp/elysian_lua_table_proxy.hpp>

namespace elysian {

    class Color4:
            public GYColor4,
            public LuaTableProxy<Color4>
    {
        GYColor4* _ptr;

    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(r, getR, setR)
            ELYSIAN_LUA_PROXY_FIELD(g, getG, setG)
            ELYSIAN_LUA_PROXY_FIELD(b, getB, setB)
            ELYSIAN_LUA_PROXY_FIELD(a, getA, setA)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        Color4(void);
        Color4(GYColor4* color);
        Color4(const Color4& other);
        Color4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

        ~Color4(void) = default;

        unsigned char getA(void) const;
        unsigned char getR(void) const;
        unsigned char getG(void) const;
        unsigned char getB(void) const;

        void setA(const unsigned char a) const;
        void setR(const unsigned char r) const;
        void setG(const unsigned char g) const;
        void setB(const unsigned char b) const;

        operator GYColor4*(void);

    };








//======= INLINEZ ==========

inline Color4::Color4(void): GYColor4 { 255, 255, 255, 255 }, _ptr(this) {}
inline Color4::Color4(GYColor4* color): _ptr(color) {}
inline Color4::Color4(const Color4 &color) : LuaTableProxy<Color4>() {
    if(color._ptr != &color) {
        _ptr = color._ptr;
    } else {
        *(GYColor4*)this = *(const GYColor4*)&color;
        _ptr = this;
    }
}
inline Color4::Color4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a): GYColor4 { r, g, b, a }, _ptr(this) {}

inline unsigned char Color4::getA(void) const { return _ptr->a; }
inline unsigned char Color4::getR(void) const { return _ptr->r; }
inline unsigned char Color4::getG(void) const { return _ptr->g; }
inline unsigned char Color4::getB(void) const { return _ptr->b; }

inline void Color4::setA(const unsigned char a) const { _ptr->a = a; }
inline void Color4::setR(const unsigned char r) const { _ptr->r = r; }
inline void Color4::setG(const unsigned char g) const { _ptr->g = g; }
inline void Color4::setB(const unsigned char b) const { _ptr->b = b; }

inline Color4::operator GYColor4*(void) { return _ptr; }


}

#endif // ELYSIAN_COLOR_HPP
