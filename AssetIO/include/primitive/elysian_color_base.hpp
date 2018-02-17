#ifndef ELYSIAN_COLOR_BASE_HPP
#define ELYSIAN_COLOR_BASE_HPP

#include <gyro_video_api.h>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <utility/elysian_generic.hpp>

ELYSIAN_ASSET_TYPEDEF(Color4)

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




    class Color4Base:
            public GYColor4,
            public VirtualArray<Color4, int, unsigned char>,
            public LuaTableProxy<Color4>
    {

        GYColor4*       _ptr;

    public:

        enum INDEX_COLOR {
            INDEX_R,
            INDEX_G,
            INDEX_B,
            INDEX_A
        };

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(r, getR, setR)
            ELYSIAN_LUA_PROXY_FIELD(g, getG, setG)
            ELYSIAN_LUA_PROXY_FIELD(b, getB, setB)
            ELYSIAN_LUA_PROXY_FIELD(a, getA, setA)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        Color4Base(void);
                        Color4Base(GYColor4* color);
                        //Color4Base(const GYColor4& other);
                        Color4Base(const Color4Base& other);
                        //Color4Base(GYColor4&& other);
                        Color4Base(unsigned char c);
                        Color4Base(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

                        ~Color4Base(void) = default;

        unsigned char   getA(void) const;
        unsigned char   getR(void) const;
        unsigned char   getG(void) const;
        unsigned char   getB(void) const;

        void            setComponents(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
        void            setA(const unsigned char a) const;
        void            setR(const unsigned char r) const;
        void            setG(const unsigned char g) const;
        void            setB(const unsigned char b) const;

        bool            isRef(void) const;

        //operator        GYColor4*(void);
        //operator        const GYColor4*(void) const;
        //operator        const GYColor4Base&(void);

        Color4Base&         operator=(const Color4Base& rhs);
        //Color4Base&         operator=(GYColor4& rhs);
        //Color4Base&         operator=(GYColor4&& rhs);
        bool                operator==(const GYColor4& rhs) const;
        bool                operator!=(const GYColor4& rhs) const;

        Color4Base&         operator*=(const GYColor4& rhs);
        Color4Base&         operator*=(float scalar);
        Color4Base&         operator/=(const GYColor4& rhs);
        Color4Base&         operator/=(float scalar);
        Color4Base&         operator+=(const GYColor4& rhs);
        Color4Base&         operator-=(const GYColor4& rhs);

        void        setValueByIndex(int index, unsigned char val);
        auto        getValueByIndex(int index) const -> unsigned char;

        static const Color4 white;
        static const Color4 black;
        static const Color4 blue;
        static const Color4 green;
        static const Color4 red;
        static const Color4 yellow;
        static const Color4 orange;
        static const Color4 gray;
        static const Color4 aqua;
        static const Color4 brown;
        static const Color4 purple;
    };








//======= INLINEZ ==========

inline Color4Base::Color4Base(void): GYColor4 { 255, 255, 255, 255 }, _ptr(this) {}
inline Color4Base::Color4Base(unsigned char c): GYColor4 { c, c, c, c }, _ptr(this) {}
inline Color4Base::Color4Base(GYColor4* color):GYColor4 { color->r, color->g, color->b, color->a }, _ptr(color) {}
//inline Color4Base::Color4Base(const GYColor4& other): GYColor4 { other.r, other.g, other.b, other.a }, _ptr(this) {}
//inline Color4Base::Color4Base(GYColor4 &&other): GYColor4 { other.r, other.g, other.b, other.a }, _ptr(this) {}
inline Color4Base::Color4Base(const Color4Base& color) : LuaTableProxy<Color4>() {
    *this = color;
}
inline Color4Base::Color4Base(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a):
    GYColor4 { r, g, b, a }, _ptr(this)
{}

inline bool Color4Base::isRef(void) const { return _ptr != this; }

inline Color4Base& Color4Base::operator =(const Color4Base& rhs) {
    if(rhs._ptr != &rhs) {
        _ptr = rhs._ptr;
        setA(rhs.getA()); setR(rhs.getR()); setG(rhs.getG()); setB(rhs.getB());
    } else {
        //*(GYColor4*)this = *(const GYColor4*)&rhs;
        _ptr = this;
        setA(rhs.getA()); setR(rhs.getR()); setG(rhs.getG()); setB(rhs.getB());
    }
    return *this;
}
#if 0
inline Color4Base& Color4Base::operator =(GYColor4& rhs) {
    *(GYColor4*)this = rhs;
    _ptr = this;
    return *this;
}

inline Color4Base& Color4Base::operator =(GYColor4&& rhs) {
    *(GYColor4*)this = rhs;
    _ptr = this;
    return *this;
}
#endif
inline bool Color4Base::operator ==(const GYColor4& rhs) const {
    return (_ptr->r == rhs.r && _ptr->g == rhs.g && _ptr->b == rhs.b && _ptr->a == rhs.a);
}

inline bool Color4Base::operator !=(const GYColor4& rhs) const {
    return !(*this == rhs);
}

inline unsigned char Color4Base::getA(void) const { return _ptr->a; }
inline unsigned char Color4Base::getR(void) const { return _ptr->r; }
inline unsigned char Color4Base::getG(void) const { return _ptr->g; }
inline unsigned char Color4Base::getB(void) const { return _ptr->b; }

inline void Color4Base::setA(const unsigned char a) const { _ptr->a = a; }
inline void Color4Base::setR(const unsigned char r) const { _ptr->r = r; }
inline void Color4Base::setG(const unsigned char g) const { _ptr->g = g; }
inline void Color4Base::setB(const unsigned char b) const { _ptr->b = b; }
inline void Color4Base::setComponents(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
    setA(a); setR(r); setG(g); setB(b);
}

//inline Color4Base::operator GYColor4*(void) { return _ptr; }
//inline Color4Base::operator const GYColor4*(void) const { return _ptr; }

inline void Color4Base::setValueByIndex(int index, unsigned char val) {
    GY_ASSERT(index >= INDEX_R && index <= INDEX_A);
    switch(index) {
    case INDEX_A:
        setA(val);
        break;
    case INDEX_R:
        setR(val);
        break;
    case INDEX_B:
        setB(val);
        break;
    default:
    case INDEX_G:
        setG(val);
        break;
    }
}

inline unsigned char Color4Base::getValueByIndex(int index) const {
    GY_ASSERT(index >= INDEX_R && index <= INDEX_A);
    switch(index) {
    case INDEX_A:
        return getA();
    case INDEX_R:
        return getR();
    case INDEX_B:
        return getB();
    default:
    case INDEX_G:
        return getG();
    }
}

}

#endif // ELYSIAN_COLOR_BASE_HPP
