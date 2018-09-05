#ifndef ELYSIAN_BIT_MASK_PROXY_HPP
#define ELYSIAN_BIT_MASK_PROXY_HPP

#include <AssetIO/utility/elysian_bit_mask.hpp>

namespace OOLUA {
    using BitMask32 = elysian::BitMask32;
    using BitMask64 = elysian::BitMask64;
}

//Proxy over arithmetic operators as bitwise operators (* and +)

OOLUA_PROXY(BitMask32)
    OOLUA_MEM_FUNC(void, setValue, uint32_t)
    OOLUA_MEM_FUNC_CONST(uint32_t, getValue)
    OOLUA_MEM_FUNC_CONST(bool, getBit, unsigned)
    OOLUA_MEM_FUNC(void, setBit, unsigned, bool)
    OOLUA_MEM_FUNC_CONST(unsigned, getBitCount)
OOLUA_PROXY_END

OOLUA_PROXY(BitMask64)
    OOLUA_MEM_FUNC(void, setValue, uint64_t)
    OOLUA_MEM_FUNC_CONST(uint64_t, getValue)
    OOLUA_MEM_FUNC_CONST(bool, getBit, unsigned)
    OOLUA_MEM_FUNC(void, setBit, unsigned, bool)
    OOLUA_MEM_FUNC_CONST(unsigned, getBitCount)
OOLUA_PROXY_END

#endif // ELYSIAN_BIT_MASK_PROXY_HPP

