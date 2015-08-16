#include <proxy/elysian_bit_mask_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(BitMask32,
                       setValue,
                       setBit)
OOLUA_EXPORT_FUNCTIONS_CONST(BitMask32,
                             getValue,
                             getBit,
                             getBitCount)

OOLUA_EXPORT_FUNCTIONS(BitMask64,
                       setValue,
                       setBit)
OOLUA_EXPORT_FUNCTIONS_CONST(BitMask64,
                             getValue,
                             getBit,
                             getBitCount)

