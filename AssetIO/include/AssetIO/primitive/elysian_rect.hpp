/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_RECT_HPP
#define ELYSIAN_RECT_HPP

#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>

namespace elysian {

    template<typename T, bool LuaProxy=false>
    struct Rect {
        T       top;
        T       bottom;
        T       left;
        T       right;

                Rect(void) = default;
                Rect(T value);
                Rect(T top, T bottom, T left, T right);

        Rect&   operator=(const Rect& rhs);

        T       getTop(void) const;
        T       getBottom(void) const;
        T       getLeft(void) const;
        T       getRight(void) const;
        void    setTop(T top);
        void    setBottom(T bottom);
        void    setLeft(T left);
        void    setRight(T right);
        T       getWidth(void) const;
        T       getHeight(void) const;
        void    getMidPoint(T& x, T& y) const;
        void    translate(T x, T y);
        void    scale(T x, T y);

        void    setBoundaries(T top, T bottom, T left, T right);
        static  Rect fromPosSize(T centerX, T centerY, T sizeX, T sizeY);


    };

    template<class T>
    class Rect<T, true>:
            public Rect<T, false>,
            public LuaTableProxy<Rect<T, true>>
    {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(top,    getTop,     setTop)
            ELYSIAN_LUA_PROXY_FIELD(bottom, getBottom,  setBottom)
            ELYSIAN_LUA_PROXY_FIELD(left,   getLeft,    setLeft)
            ELYSIAN_LUA_PROXY_FIELD(right,  getRight,   setRight)
        ELYSIAN_LUA_PROXY_FIELDS_END()

            Rect(void) = default;
            Rect(T value);
            Rect(T top, T bottom, T left, T right);
    };

    using Recti = Rect<int, true>;
    using Rectf = Rect<float>;


    //===== INLINEZ =======
    template<typename T, bool LuaProxy>
    inline Rect<T, LuaProxy>::Rect(T value):
        top(value),
        bottom(value),
        left(value),
        right(value)
    {}

    template<typename T, bool LuaProxy>
    inline Rect<T, LuaProxy>::Rect(T top, T bottom, T left, T right):
        top(top),
        bottom(bottom),
        left(left),
        right(right)
    {}

    template<typename T, bool LuaProxy>
    inline T Rect<T, LuaProxy>::getTop(void) const { return top; }

    template<typename T, bool LuaProxy>
    inline T Rect<T, LuaProxy>::getBottom(void) const { return bottom; }

    template<typename T, bool LuaProxy>
    inline T Rect<T, LuaProxy>::getLeft(void) const { return left; }

    template<typename T, bool LuaProxy>
    inline T Rect<T, LuaProxy>::getRight(void) const { return right; }

    template<typename T, bool LuaProxy>
    inline T Rect<T, LuaProxy>::getWidth(void) const { return right - left; }

    template<typename T, bool LuaProxy>
    T Rect<T, LuaProxy>::getHeight(void) const { return bottom - top; }

    template<typename T, bool LuaProxy>
    void Rect<T, LuaProxy>::getMidPoint(T& x, T& y) const { x = (left + right)/(T)2; y = (top + bottom)/(T)2; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::setTop(T top) { this->top = top; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::setBottom(T bottom) { this->bottom = bottom; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::setLeft(T left) { this->left = left; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::setRight(T right) { this->right = right; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::translate(T x, T y) { left += x; right += x; top += y; bottom += y; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::scale(T sx, T sy) { left *= sx; right *= sx; top *= sy; bottom *= sy; }

    template<typename T, bool LuaProxy>
    inline void Rect<T, LuaProxy>::setBoundaries(T top, T bottom, T left, T right) {
        setTop(top); setBottom(bottom); setLeft(left); setRight(right);
    }

    template<typename T, bool LuaProxy>
    inline auto Rect<T, LuaProxy>::operator=(const Rect& rhs) -> Rect& {
        left = rhs.left; right = rhs.right; top = rhs.top; bottom = rhs.bottom;
        return *this;
    }

    template<typename T, bool LuaProxy>
    inline Rect<T, LuaProxy> Rect<T, LuaProxy>::fromPosSize(T centerX, T centerY, T sizeX, T sizeY) {
        return Rect<T, LuaProxy>(
            centerY - sizeY/(T)2,
            centerY + sizeY/(T)2,
            centerX - sizeX/(T)2,
            centerX + sizeX/(T)2
        );
    }

    template<typename T>
    inline Rect<T, true>::Rect(T value):
        Rect<T, false>(value)
    {}

    template<typename T>
    inline Rect<T, true>::Rect(T top, T bottom, T left, T right):
        Rect<T, false>(top, bottom, left, right)
    {}

    template<class T>
    bool Rect<T, true>::_proxyRegistered = false;

    template<class T>
    constexpr elysian::LuaTableFieldProxy Rect<T, true>::_proxyFields[];


}

#endif // ELYSIAN_RECT_HPP
