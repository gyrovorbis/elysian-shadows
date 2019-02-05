#ifndef ELYSIAN_BIT_MASK_HPP
#define ELYSIAN_BIT_MASK_HPP

#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>

namespace elysian {

    template<typename T>
    struct BitMask;

    //Base parent implementation (const shit used by const and non const)
    template<typename T, class B = BitMask<T>>
    class BitMaskBitRefBase {
        B*      _parent = nullptr;
        int     _index    = -1;
    public:

        constexpr                   BitMaskBitRefBase(B* parent, int index):
                                    _parent(parent),
                                    _index(index) {}

        const BitMaskBitRefBase*    operator&(void) const = delete;
        constexpr                   operator bool(void) const {
                return _parent->getBit(_index);
         }
    };

    //SFINAEd const base template
    template<typename T,
             class B = BitMask<T>,
             class Enable = void>
    class BitMaskBitRef:
            public BitMaskBitRefBase<B> {};

    //Non-const specialization
    template<typename T, class B>
    class BitMaskBitRef<T,
                        B,
                        typename std::enable_if<std::is_const<B>::value, int>::type>:
            public BitMaskBitRefBase<BitMask<T>>
    {

        BitMaskBitRef<BitMask<T>>*  operator&(void) = delete;
        bool                        operator=(bool val) {
            BitMaskBitRefBase<T, BitMask<T>>::_parent->setBit(BitMaskBitRefBase<T, BitMask<T>>::_index, val);
        }
    };


    template<typename T>
    struct BitMask {
        T                   _value   = 0;

        constexpr           BitMask(void) = default;
        constexpr           BitMask(const T val);

        constexpr T         getValue(void) const;
        void                setValue(const T val);

        T                   getSubvalue(const unsigned pos, const unsigned bitCount) const;

        constexpr T         getSubMaskValue(const T pos, const T mask) const;
        void                setSubMaskValue(const T pos, const T mask, const T value);
        void                clearSubMaskBits(const T mask);

        //constexpr           operator T(void);

        constexpr unsigned  getBitCount(void) const;
        constexpr bool                getBit(const unsigned index) const;
        void                setBit(const unsigned index, const bool val);


        constexpr auto      operator[](int index) const -> BitMaskBitRef<T, const BitMask>;
        constexpr auto      operator[](int index) ->  BitMaskBitRef<T, BitMask>;

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
    inline constexpr BitMask<T>::BitMask(const T value): _value(value) {}

    template<typename T>
    inline constexpr T BitMask<T>::getValue(void) const {
        return _value;
    }

    template<typename T>
    constexpr auto BitMask<T>::operator[](int index) const -> BitMaskBitRef<T, const BitMask> {
        return BitMaskBitRef<T, const BitMask>(this, index);
    }

    template<typename T>
    constexpr auto BitMask<T>::operator[](int index) -> BitMaskBitRef<T, BitMask> {
        return BitMaskBitRef<T, BitMask>(this, index);
    }

#if 0
    template<typename T>
    inline constexpr BitMask<T> BitMask<T>::getMask(const unsigned pos, const unsigned bitCount) const {
        return ((_value >> pos) & ELYSIAN_##t##_ENTRY_##a##_BIT_value);
    }
    do { \
                                                        value &= ~(ELYSIAN_##t##_ENTRY_##a##_BIT_value << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
                                                        (value) |= ((unsigned)(v) << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
                                                    } while(0)
#endif


    template<typename T>
    inline constexpr T BitMask<T>::getSubMaskValue(const T pos, const T mask) const {
        return ((_value & mask) >> pos);
    }

    template<typename T>
    inline void BitMask<T>::setSubMaskValue(const T pos, const T mask, const T value) {
        clearSubMaskBits(mask);
        _value |= (((value) << pos) & mask);
    }

    template<typename T>
    inline void BitMask<T>::clearSubMaskBits(const T mask) {
        _value &= ~mask;
    }

    template<typename T>
    inline void BitMask<T>::setValue(const T value) {
        _value = value;
    }

    template<typename T>
    inline constexpr unsigned BitMask<T>::getBitCount(void) const {
        return sizeof(T)*8;
    }

    template<typename T>
    inline constexpr bool BitMask<T>::getBit(const unsigned index) const {
        T one = 1;
        return (_value&(one<<index));
    }

    template<typename T>
    inline void BitMask<T>::setBit(const unsigned index, const bool val) {
        T one = 1;
        if(val) _value |= (one<<index);
        else _value &= ~(one<<index);
    }

    template<typename T>
    inline LuaBitMask<T>::LuaBitMask(const T val): BitMask<T>(val) {}


    template<typename T>
    bool                                    LuaBitMask<T>::_proxyRegistered = false;

    template<typename T>
    constexpr elysian::LuaTableFieldProxy	LuaBitMask<T>::_proxyFields[];

}


#endif // ELYSIAN_BIT_MASK_HPP

