#include <proxy/elysian_bit_mask_proxy.hpp>


/*!
    \qmltype BitMask32
    \since 1.6.0
    \inmodule ESToolkit
    \brief Represents a 32-bit packed integer of bitfields.

    BitMask32 is a convenience class for setting, extracting, and applying bitmasks to a 32-bit unsigned integer.
    Since Lua5.2 doesn't even support integers natively or have a bitmask library, ALWAYS use this class and don't
    try to do it yourself!
*/

/*!
 * \qmlproperty int BitMask32::value
 * The entire unsigned 32-bit integer field represented by the BitMask32
 * \sa getValue(), setValue()
 */
/*!
 * \qmlproperty array BitMask32::bitFields
 * Array of 32 booleans containing the values of each bit within the internal 32-bit unsigned integer.
 * \sa getBit(), setBit(), getBitCount()
 */
/*!
 * \qmlmethod void BitMask32::setValue(int value)
 *
 * Sets the value of the entire internal 32-bit unsigned integer field to \a{value}.
 * \sa getValue()
 */
/*!
 * \qmlmethod int BitMask32::getValue()
 *
 * Retrieves the value of the entire 32-bit unsigned integer field.
 * \sa setValue()
 */
/*!
 * \qmlmethod void BitMask32::setBit(int index, bool value)
 *
 * Sets the value of the single bit at location \a{index} to \a{value}.
 * \sa getBit()
 */
/*!
 * \qmlmethod bool BitMask32::getBit(int index)
 *
 * Returns the value of the single bit at location \a{index}.
 * \sa setBit()
 */
/*!
 * \qmlmethod int BitMask32::getBitCount()
 *
 * Returns the total number of bits in the internal unsiged 32-bit integer object... Better be 32.
 */

OOLUA_EXPORT_FUNCTIONS(BitMask32,
                       setValue,
                       setBit)
OOLUA_EXPORT_FUNCTIONS_CONST(BitMask32,
                             getValue,
                             getBit,
                             getBitCount)

/*!
    \qmltype BitMask64
    \since 1.6.0
    \inmodule ESToolkit
    \brief Represents a 64-bit packed integer of bitfields.

    BitMask64 is a convenience class for setting, extracting, and applying bitmasks to a 64-bit unsigned integer.
    Since Lua5.2 doesn't even support integers natively or have a bitmask library, ALWAYS use this class and don't
    try to do it yourself!
*/

/*!
 * \qmlproperty int BitMask64::value
 * The entire unsigned 64-bit integer field represented by the BitMask64
 * \sa getValue(), setValue()
 */
/*!
 * \qmlproperty array BitMask64::bitFields
 * Array of 64 booleans containing the values of each bit within the internal 64-bit unsigned integer.
 * \sa getBit(), setBit(), getBitCount()
 */
/*!
 * \qmlmethod void BitMask64::setValue(int value)
 *
 * Sets the value of the entire internal 64-bit unsigned integer field to \a{value}.
 * \sa getValue()
 */
/*!
 * \qmlmethod int BitMask64::getValue()
 *
 * Retrieves the value of the entire 64-bit unsigned integer field.
 * \sa setValue()
 */
/*!
 * \qmlmethod void BitMask64::setBit(int index, bool value)
 *
 * Sets the value of the single bit at location \a{index} to \a{value}.
 * \sa getBit()
 */
/*!
 * \qmlmethod bool BitMask64::getBit(int index)
 *
 * Returns the value of the single bit at location \a{index}.
 * \sa setBit()
 */
/*!
 * \qmlmethod int BitMask64::getBitCount()
 *
 * Returns the total number of bits in the internal unsiged 64-bit integer object... Better be 64.
 */


OOLUA_EXPORT_FUNCTIONS(BitMask64,
                       setValue,
                       setBit)
OOLUA_EXPORT_FUNCTIONS_CONST(BitMask64,
                             getValue,
                             getBit,
                             getBitCount)

