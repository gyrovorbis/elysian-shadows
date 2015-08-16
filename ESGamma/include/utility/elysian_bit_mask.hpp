#ifndef ELYSIAN_BIT_MASK_HPP
#define ELYSIAN_BIT_MASK_HPP

#include <crtp/elysian_lua_table_proxy.hpp>

namespace elysian {

    template<typename T>
    struct BitMask {

        T                   _mask   = 0;

        constexpr           BitMask(void) = default;
        constexpr           BitMask(const T val);

        constexpr T         getValue(void) const;
        void                setValue(const T val);


        constexpr unsigned  getBitCount(void) const;
        constexpr bool      getBit(const unsigned index) const;
        void                setBit(const unsigned index, const bool val);

       // operator            T(void);

    };

    template<typename T>
    struct LuaBitMask:
            public BitMask<T>,
            public LuaTableProxy<LuaBitMask<T>>
    {

        LuaBitMask(const T val);
        LuaBitMask(void) = default;

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(value, getValue)
            ELYSIAN_LUA_PROXY_ARRAY_FIELD(bitFields,    getBit, setBit, getBitCount)
        ELYSIAN_LUA_PROXY_FIELDS_END()

    };


    using BitMask32 = LuaBitMask<uint32_t>;
    using BitMask64 = LuaBitMask<uint64_t>;


    template<typename T>
    constexpr BitMask<T>::BitMask(const T value): _mask(value) {}

    template<typename T>
    inline constexpr T BitMask<T>::getValue(void) const {
        return _mask;
    }

    template<typename T>
    inline void BitMask<T>::setValue(const T value) {
        _mask = value;
    }

    template<typename T>
    inline constexpr unsigned BitMask<T>::getBitCount(void) const {
        return sizeof(T)*8;
    }

    template<typename T>
    inline constexpr bool BitMask<T>::getBit(const unsigned index) const {
        T one = 1;
        return (_mask&(one<<index));

    }

    template<typename T>
    inline void BitMask<T>::setBit(const unsigned index, const bool val) {
        T one = 1;
        if(val) _mask |= (one<<index);
        else _mask &= ~(one<<index);
    }
#if 0
    template<typename T>
    inline BitMask<T>::operator T(void) {
        return _mask;
    }
#endif

    template<typename T>
    inline LuaBitMask<T>::LuaBitMask(const T val): BitMask<T>(val) {}


    template<typename T>
    bool                                    LuaBitMask<T>::_proxyRegistered = false;

    template<typename T>
    constexpr elysian::LuaTableFieldProxy	LuaBitMask<T>::_proxyFields[];

}


#endif // ELYSIAN_BIT_MASK_HPP

