#ifndef ELYSIAN_COLOR_HPP
#define ELYSIAN_COLOR_HPP

#include <gyro_video_api.h>
#include <crtp/elysian_lua_table_proxy.hpp>

namespace elysian {

    template<typename P>
    class StructValueRef: protected P {
    protected:
        P*                  _ref = nullptr;

    public:
                            StructValueRef(void);
                            StructValueRef(P* ref);
                            StructValueRef(const P& other);
                            StructValueRef(const StructValueRef<P>& other);

        bool                isRef(void) const;

        operator            P*(void);
        operator            const P&(void);

        StructValueRef&     operator=(P* ref);
        StructValueRef&     operator=(const P& rhs);
        StructValueRef&     operator=(const StructValueRef<P>& rhs);
    };

    template<typename P>
    inline StructValueRef<P>::StructValueRef(void) {}

    template<typename P>
    inline StructValueRef<P>::StructValueRef(P* ref): _ref(ref) {}

    template<typename P>
    inline StructValueRef<P>::StructValueRef(const P& other): _ref(this) {
        *this = other;
    }

    template<typename P>
    inline StructValueRef<P>::StructValueRef(const StructValueRef<P> &other) {
        *this = other;
    }

    template<typename P>
    inline StructValueRef<P>::operator P*(void) {
        return _ref;
    }

    template<typename P>
    inline StructValueRef<P>::operator const P&(void) {
        return *_ref;
    }

    template<typename P>
    inline StructValueRef<P>& StructValueRef<P>::operator=(P* ref) {
        _ref = ref;
        return *this;
    }

    template<typename P>
    inline StructValueRef<P>& StructValueRef<P>::operator=(const P& rhs) {
        *static_cast<P*>(this) = rhs;
        _ref = this;
    }

    template<typename P>
    inline StructValueRef<P>& StructValueRef<P>::operator=(const StructValueRef<P>& rhs) {
        if(rhs.isRef()) {
            _ref = rhs._ref;
        } else {
            *this = *static_cast<const P*>(rhs);
            _ref = this;
        }
    }

    template<typename P>
    inline bool StructValueRef<P>::isRef(void) const { return _ref != this; }




    class Color4:
            public GYColor4,
            public LuaTableProxy<Color4>
    {
   //     friend Color4&      operator=   (Color4& lhs, const GYColor4& rhs);
   //     friend GYColor4&    operator=   (GYColor4& lhs, const Color4& rhs);
    //    friend bool         operator==  (const Color4& lhs, const GYColor4& rhs);
     //   friend bool         operator!=  (const Color4& lhs, const GYColor4& rhs);


        GYColor4*       _ptr;

    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(r, getR, setR)
            ELYSIAN_LUA_PROXY_FIELD(g, getG, setG)
            ELYSIAN_LUA_PROXY_FIELD(b, getB, setB)
            ELYSIAN_LUA_PROXY_FIELD(a, getA, setA)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        Color4(void);
                        Color4(GYColor4* color);
                        Color4(const GYColor4& other);
                        Color4(const Color4& other);
                        Color4(GYColor4&& other);
                        Color4(unsigned char c);
                        Color4(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

                        ~Color4(void) = default;

        unsigned char   getA(void) const;
        unsigned char   getR(void) const;
        unsigned char   getG(void) const;
        unsigned char   getB(void) const;

        void            setA(const unsigned char a) const;
        void            setR(const unsigned char r) const;
        void            setG(const unsigned char g) const;
        void            setB(const unsigned char b) const;

        bool            isRef(void) const;

        operator        GYColor4*(void);
        operator        const GYColor4*(void) const;
        //operator        const GYColor4&(void);

        Color4&         operator=(const Color4& rhs);
        Color4&         operator=(GYColor4& rhs);
        Color4&         operator=(GYColor4&& rhs);
        bool            operator==(const GYColor4& rhs) const;
        bool            operator!=(const GYColor4& rhs) const;

        Color4&         operator*=(const GYColor4& rhs);
        Color4&         operator*=(float scalar);
        Color4&         operator/=(const GYColor4& rhs);
        Color4&         operator/=(float scalar);
        Color4&         operator+=(const GYColor4& rhs);
        Color4&         operator-=(const GYColor4& rhs);


    };








//======= INLINEZ ==========

inline Color4::Color4(void): GYColor4 { 255, 255, 255, 255 }, _ptr(this) {}
inline Color4::Color4(unsigned char c): GYColor4 { c, c, c, c }, _ptr(this) {}
inline Color4::Color4(GYColor4* color): _ptr(color) {}
inline Color4::Color4(const GYColor4& other): GYColor4 { other.r, other.g, other.b, other.a }, _ptr(this) {}
inline Color4::Color4(GYColor4 &&other): GYColor4 { other.r, other.g, other.b, other.a }, _ptr(this) {}
inline Color4::Color4(const Color4& color) : LuaTableProxy<Color4>() {
    *this = color;
}
inline Color4::Color4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a): GYColor4 { r, g, b, a }, _ptr(this) {}

inline bool Color4::isRef(void) const { return _ptr != this; }

inline Color4& Color4::operator =(const Color4& rhs) {
    if(rhs._ptr != &rhs) {
        _ptr = rhs._ptr;
    } else {
        *(GYColor4*)this = *(const GYColor4*)&rhs;
        _ptr = this;
    }
    return *this;
}

inline Color4& Color4::operator =(GYColor4& rhs) {
    _ptr = &rhs;
    return *this;
}

inline Color4& Color4::operator =(GYColor4&& rhs) {
    *(GYColor4*)this = rhs;
    _ptr = this;
    return *this;
}

inline bool Color4::operator ==(const GYColor4& rhs) const {
    return (_ptr->r == rhs.r && _ptr->g == rhs.g && _ptr->b == rhs.b && _ptr->a == rhs.a);
}

inline bool Color4::operator !=(const GYColor4& rhs) const {
    return !(*this == rhs);
}

inline unsigned char Color4::getA(void) const { return _ptr->a; }
inline unsigned char Color4::getR(void) const { return _ptr->r; }
inline unsigned char Color4::getG(void) const { return _ptr->g; }
inline unsigned char Color4::getB(void) const { return _ptr->b; }

inline void Color4::setA(const unsigned char a) const { _ptr->a = a; }
inline void Color4::setR(const unsigned char r) const { _ptr->r = r; }
inline void Color4::setG(const unsigned char g) const { _ptr->g = g; }
inline void Color4::setB(const unsigned char b) const { _ptr->b = b; }

inline Color4::operator GYColor4*(void) { return _ptr; }
inline Color4::operator const GYColor4*(void) const { return _ptr; }


}

#endif // ELYSIAN_COLOR_HPP
