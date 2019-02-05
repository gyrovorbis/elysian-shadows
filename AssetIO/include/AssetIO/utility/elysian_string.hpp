#ifndef ELYSIAN_STRING_HPP
#define ELYSIAN_STRING_HPP

#include <iostream>
#include <cstring>
#include <cassert>
#include <AssetIO/utility/elysian_generic.hpp>

// Default templated argument for string's internal stack size
#define DD_STRING_STACK_SIZE_DEFAULT            30

// Amount of extra space allocated when the string grows on the heap
#ifndef DD_STRING_GROW_SIZE_PADDING
#  define DD_STRING_GROW_SIZE_PADDING           10
#endif

// Define this to force a shrunk string back onto the stack if it can fit
#define DD_STRING_AUTO_SHRINK_TO_STACK

#define DD_NO_ALLOCATOR_USE_C_STD_LIB

#define DD_ASSERT(b) assert(b)

namespace elysian
{

    template<typename A, size_t S>
    class String;

    template<typename A, size_t S1, size_t S2, size_t S3 = S1 + S2>
    inline String<A, S3>  operator+(const String<A, S1>& lhs, const String<A, S2>& rhs);


    // Templated std::string replacement offering a high-level abstraction with
    // 1. custom allocators
    // 2. small-string stack optimizations
    // 3. move semantics
    // 4. C++11 STL-compatible iterators and reverse iterators
    // 5. utility functions for inserting, finding, erasing, swapping, etc.
    // 6. convenience overloaded operators for indexing and concatenating
    // 7. const char* compatible without any conversion overhead
    // 8. optional macro to force small strings to shrink back onto the stack
    //
    // Abstraction is basically a union around a char[] array allocated on the
    // stack and a pointer that can either point to the internal stack or to
    // heap data. Dynamically resizes onto the heap if the given stack space is
    // not large enough. A C-compatible NULL-terminated const char* array can
    // be requested at any time via the getCArray() method. Note, because of
    // this, the GetLength() operation is having to rely on strlen(), which
    // makes the operation O(N), so please cache length if it remains constant
    // in a tight loop.

    template<typename A = std::allocator<char>, size_t S = DD_STRING_STACK_SIZE_DEFAULT>
    class String
    {
        // Friend all instantiatiations of this template for internals.
        template<typename Q, size_t W>
        friend class String;

    private:
        char        m_stackStr[S]	= { '\0' };
        char*       m_pStrPtr		= m_stackStr;
        A*          m_pAllocator	= nullptr;
        size_t      m_capacity		= S;

        // Internal utility wrapper around allocation mechanism, should allocator
        // semantics ever change. Equivalent to malloc, except might allocate
        // additional space for padding (returned via newSize).
        void*       _alloc(size_t size, size_t* newSize);

        // Internal utility wrapper around reallocation mechanism, should allocator
        // semantics ever change. Equivalent to realloc, except might allocate
        // additional space for padding (return via newSize).
        void*       _realloc(void* ptr, size_t size, size_t* newSize);

        // Internal utility wrapper around mechanism to free heap data, should
        // allocator semantics ever change.
        void        _free(void* ptr);

    public:

        // Public type-aliases for STL-compatible iterators and const/reverse iterator types
        using ptr_type					= String<A, S>*;
        using const_ptr_type			= const String<A, S>*;
        using iterator					= ContiguousArrayIterator<ptr_type>;
        using const_iterator			= ContiguousArrayIterator<const_ptr_type>;
        using reverse_iterator			= std::reverse_iterator<iterator>;
        using const_reverse_iterator	= std::reverse_iterator<const_iterator>;

        // Similar to C++'s npos, used to signal an end or invalid string position
        const static constexpr size_t npos = static_cast<size_t>(-1);

                        String(std::nullptr_t);
                        String(A* allocator = nullptr);
                        String(const char* cArray, A* allocator = nullptr);

        // Catch-all for all lvalue assignments or assignments between disjoint
        // String objects with different templated types
        template<typename T>
                        String(const T& lRef, A* allocator);

        // Standard copy constructor, grabs allocator of other object (of same type)
        template<size_t N>
                        String(const String<A, N>& lRef);

        // Move semantics only available between Strings with same allocator type + instance.
        template<size_t N>
                        String(String<A, N>&& rRef, A* allocator);

        //Perform a move and assume rRef's allocator as well as buffer
        template<size_t N>
                        String(String<A, N>&& rRef);

                        ~String();

        // Sets the current string to a portion of the given C string (or to entire
        // string if no arguments are specified)
        void            SetCString(const char* cArray, size_t pos = 0, size_t length = npos);
        bool            IsHeapAllocated() const;

        char&           Back();
        const char&     Back() const;
        char&           Front();
        const char&     Front() const;
        char&           At(size_t index);
        const char&     At(size_t index) const;

        // Appends the given string to the end of the current string, starting
        // at the given position (or beginning of string by default), appending
        // length # of bytes or the whole thing if npos is given.
        bool            Append(const char* str, size_t pos = 0, size_t length = npos);
        template<typename T>
        bool            Append(const T& str, size_t pos = 0, size_t length = npos);
        template<typename I>
        bool            Append(I beginIt, I endIt);

        // Requests that the string be large enough to accomodate the given
        // length. Returns true if this actually required a resize.
        bool            Resize(size_t length);

        // Reduces the capacity of the given string to only exactly what is needed
        // for storage, moving it back onto the stack if possible.
        bool            ShrinkToFit();

        // Calculates string length. O(n) lookup, performs a strlen call
        // internally
        size_t          GetLength() const;

        // Returns size of internal buffer to store string, which must be at
        // least as large as the length of the string.
        size_t          GetCapacity() const;

        // Returns a standard NULL-terminated C string from internal buffer.
        const char*     GetCString() const;

        String<A, S>&   Erase(size_t index, size_t len = npos);
        iterator        Erase(iterator p);
        iterator        Erase(iterator first, iterator last);

        // Swaps the value of the given string with another, uses move
        // semantic-like pointer swapping internally if the given strings
        // share the same allocator.
        void            Swap(String<A, S>& lRef);
        template<typename T>
        void            Swap(T& lRef);

        // Inserts a portion of another string (or entire thing with default
        // arguments) into the given string, resizing if necessary
        template<typename T>
        String<A, S>&   Insert(size_t index, const T& str, size_t len = npos);
        template<typename T>
        String<A, S>&   Insert(size_t index, const char* str, size_t len = npos);

        // Copies the string given by the range of the two iterators
        // into the current string, resizing if necessary
        template<typename I>
        String<A, S>&   Insert(iterator pos, I itFirst, I itLast);

        // Fills the specified range with the given character.
        String<A, S>&   Insert(iterator pos, char c, size_t n = 1);

        // Replaces the given range of the string with a range of another string
        template<typename T>
        String<A, S>    Replace(size_t pos, size_t len, const T& substr,
                                size_t subpos = 0, size_t sublen = npos);

        String<A, S>&   Replace(size_t pos, size_t len, const char* str,
                                size_t subpos = 0, size_t sublen = npos);

        String<A, S>&   Replace(iterator i1, iterator i2, const char* str, size_t length);

        template<typename I>
        String<A, S>&   Replace(iterator dstFirst, iterator dstLast, I srcFirst, I srcLast);

        // Returns the first position of the given substring, if found, otherwise
        // returns npos.
        template<typename T>
        size_t			Find(const T& substr, size_t index = 0, size_t len=npos) const;
        size_t          Find(const char* str, size_t index = 0, size_t len=npos) const;

        // Returns a new string object, copying the values of the current string
        // over the range given
        String<A, S>   Substr(size_t index = 0, size_t len = npos) const;

        // Overloaded subscript operators to allow treating the string like a C array
        const char&		operator[](size_t index) const;
        char&			operator[](size_t index);

        bool			operator==(const char* str) const;
        bool			operator!=(const char* str) const;
        String<A, S>&	operator+=(const char* str);
        String<A, S>&	operator =(const char* str);

        template<typename T>
        bool			operator==(const T& rhs) const;
        template<typename T>
        bool			operator!=(const T& rhs) const;
        template<typename T>
        String<A, S>&	operator+=(const T& rhs);
        template<typename T>
        String<A, S>&   operator=(const T& rhs);

        // Only do move semantics for string of this allocator type.
        template<size_t N>
        String<A, S>&   operator=(String<A, N>&& rhs);

        // Returns standard C++11/STL compatible iterators
        iterator        begin();
        const_iterator  begin() const;
        const_iterator  cbegin() const;
        iterator        end();
        const_iterator  end() const;
        const_iterator  cend() const;

        // Returns the requested C++11/STL compatible iterators
        auto            rbegin() -> reverse_iterator;
        auto            rbegin() const -> const_reverse_iterator;
        auto            crbegin() const -> const_reverse_iterator;
        auto            rend() -> reverse_iterator;
        auto            rend() const -> const_reverse_iterator;
        auto            crend() const -> const_reverse_iterator;

        A*              GetAllocator();
        const A*        GetAllocator() const;

        // Static member functions.
        static size_t   GetStackSize(void);
        static size_t   GetDefaultStackSize(void);

        static size_t   GetGrowSizePadding(void);

        // Creates a new string object from a C-style printf/va_arg
        // format string and a variable argument list. Can stringify
        // String objects as if they're native C strings with the "%s"
        // formatter in the argument list.
        template<typename... T>
        static String<A, S> format(A* allocator, const char* str, T... args);
    };


    namespace StringUtil {

        template<typename T>
        struct StringConverter
        {
            static const T& ToCVarArg(const T& val)
            {
                return val;
            }
        };

        template<typename A, size_t S>
        struct StringConverter<String<A, S>>
        {
            static const char* ToCVarArg(const String<A, S>& str)
            {
                return str.GetCString();
            }
        };

   }

    template<typename A, size_t S>
    template<typename... T>
    inline String<A, S> String<A, S>::format(A* allocator, const char* str, T... args)
    {
        DD_ASSERT(str);
        String<A, S> newStr(allocator);
        snprintf(newStr.m_pStrPtr, S, str, StringUtil::StringConverter<T>::ToCVarArg(args)...);
        return newStr;
    }

    // Global overloaded addition operator, only defined for two strings using the
    // same alloctor type, creates a new string (using the allocator of the
    // first), concatenating the two and returns it by value.
    template<typename A, size_t S1, size_t S2, size_t S3>
    inline String<A, S3>  operator+(const String<A, S1>& lhs, const String<A, S2>& rhs)
    {
        String<A, S3> str = lhs;
        str.Append(rhs);
        return str;
    }

    template<typename A, size_t S>
    inline String<A, S>::String(std::nullptr_t):
        String(static_cast<const char*>(nullptr), static_cast<A*>(nullptr))
    {
    }

    template<typename A, size_t S>
    String<A, S>::String(A* allocator) :
        m_pAllocator(allocator)
    {
    }

    template<typename A, size_t S>
    inline String<A, S>::String(const char* cArray, A* allocator) :
        m_pAllocator(allocator)
    {
        SetCString(cArray);
    }

    template<typename A, size_t S>
    template<size_t N>
    inline String<A, S>::String(const String<A, N>& lRef):
        m_pAllocator(lRef.m_pAllocator)
    {
        SetCString(lRef.GetCString());
    }

    template<typename A, size_t S>
    template<typename T>
    inline String<A, S>::String(const T& lRef, A* allocator) :
        m_pAllocator(allocator)
    {
        SetCString(lRef.GetCString());
    }

    /* Move semantics are ONLY used provided
     * 1) the two objects use the same allocator type
     * 2) the source string is allocated on the heap
     * 3) the destination object's stack is too small to contain the source string
     */
    template<typename A, size_t S>
    template<size_t N>
    inline String<A, S>::String(String<A, N>&& rRef, A* allocator) :
        m_pAllocator(allocator)
    {
        if (rRef.IsHeapAllocated()
                && allocator == rRef.GetAllocator()
                && GetCapacity() < rRef.GetLength())
        {
            // "Steal" pointer
            m_pStrPtr = rRef.m_pStrPtr;
            rRef.m_pStrPtr = nullptr;
            m_capacity = rRef.m_capacity;
        }
        else
        {
            SetCString(rRef.GetCString());
        }
    }

    template<typename A, size_t S>
    template<size_t N>
    inline String<A, S>::String(String<A, N>&& rRef):
        String(std::move(rRef), rRef.GetAllocator())
    {
    }

    template<typename A, size_t S>
    inline String<A, S>::~String()
    {
        if (IsHeapAllocated())
        {
            _free(m_pStrPtr);
        }
    }

    template<typename A, size_t S>
    inline const char* String<A, S>::GetCString() const
    {
        return m_pStrPtr;
    }

    template<typename A, size_t S>
    inline char& String<A, S>::Back()
    {
        const size_t len = GetLength();
        DD_ASSERT(len);
        return m_pStrPtr[len ? len - 1 : 0];   //Protect against empty string, still undefined behavior
    }

    template<typename A, size_t S>
    inline const char& String<A, S>::Back() const
    {
        const size_t len = GetLength();
        DD_ASSERT(len);
        return m_pStrPtr[len ? len - 1 : 0];
    }

    template<typename A, size_t S>
    inline char& String<A, S>::At(size_t index)
    {
        const size_t len = GetLength();
        DD_ASSERT(len && index < len);
        return m_pStrPtr[index < len ? index : 0];   //Protect against invalid index, but still undefined behavior
    }

    template<typename A, size_t S>
    inline const char& String<A, S>::At(size_t index) const
    {
        const size_t len = GetLength();
        DD_ASSERT(len && index < len);
        return m_pStrPtr[index < len ? index : 0];
    }

    template<typename A, size_t S>
    inline char& String<A, S>::Front()
    {
        DD_ASSERT(GetLength());
        return m_pStrPtr[0];
    }

    template<typename A, size_t S>
    inline const char& String<A, S>::Front() const
    {
        DD_ASSERT(GetLength());
        return m_pStrPtr[0];
    }

    template<typename A, size_t S>
    inline size_t String<A, S>::GetLength() const
    {
        DD_ASSERT(m_pStrPtr);
        return strnlen(m_pStrPtr, GetCapacity());
    }

    template<typename A, size_t S>
    inline size_t String<A, S>::GetCapacity() const
    {
        return m_capacity;
    }

    template<typename A, size_t S>
    void String<A, S>::SetCString(const char* cArray, size_t startIndex, size_t length)
    {
        if(cArray)
        {
            cArray += startIndex; //move pointer to first element
            //entire string
            if (length == npos)
            {
                length = strlen(cArray);
            }
        }
        else
        {
            length = 0;
        }

        Resize(length + 1);

        if(length)
        {
            memcpy(m_pStrPtr, cArray, length);
        }
        m_pStrPtr[length] = '\0';
    }

    template<typename A, size_t S>
    bool String<A, S>::Append(const char* str, size_t pos, size_t length)
    {

        DD_ASSERT(str);
        DD_ASSERT(pos < strlen(str));
        DD_ASSERT(length == npos || pos + length <= strlen(str));
        str += pos;

        bool resized = false;

        const size_t len = (str && length == npos) ? strlen(str) : length;
        if (len)
        {
            resized = Resize(GetLength() + len + 1);
            strncat(m_pStrPtr, str, len);
        }
        return resized;
    }

    template<typename A, size_t S>
    template<typename T>
    bool String<A, S>::Append(const T& rhs, size_t pos, size_t length)
    {
        const size_t rhsLen = rhs.GetLength();
        bool resized = false;

        DD_ASSERT(pos < rhsLen);
        DD_ASSERT(length == npos || pos + length <= rhsLen);

        if (rhsLen)
        {
            const size_t len = (length == npos) ? rhsLen : length;
            resized = Resize(GetLength() + len + 1);
            strncat(m_pStrPtr, &rhs.GetCString()[pos], len);
        }

        return resized;
    }

    template<typename A, size_t S>
    template<typename I>
    bool String<A, S>::Append(I beginIt, I endIt)
    {
        return Append(beginIt.m_pCont->toCString(),
                      beginIt.m_index,
                      (endIt.m_index == npos)?
                          npos : endIt.m_index - beginIt.m_index);
    }

    template<typename A, size_t S>
    inline bool String<A, S>::IsHeapAllocated(void) const
    {
        return m_pStrPtr != m_stackStr;
    }

    template<typename A, size_t S>
    inline void String<A, S>::Swap(String<A, S>& lRef)
    {
        /* The only scenario where we can optimize here is
         * if one of them is on the heap, and they both
         * share the same allocator.
         */
        if (GetAllocator() == lRef.GetAllocator())
        {
            //Check whether one of them is on the heap
            if (IsHeapAllocated() && lRef.IsHeapAllocated())
            {
                char* const tempStr = m_pStrPtr;
                const size_t tempCap = m_capacity;
                m_pStrPtr = lRef.m_pStrPtr;
                m_capacity = lRef.m_capacity;
                lRef.m_pStrPtr = tempStr;
                lRef.m_capacity = tempCap;

            }
            else if (IsHeapAllocated() && !lRef.IsHeapAllocated())
            {
                char* const tempStr = m_pStrPtr;
                const size_t tempCap = m_capacity;
                m_pStrPtr = nullptr;  //make sure it doesn't get deleted on resize
                SetCString(lRef.GetCString());
                lRef.m_pStrPtr = tempStr;
                lRef.m_capacity = tempCap;

            }
            else if (!IsHeapAllocated() && lRef.IsHeapAllocated())
            {
                char* const tempStr = lRef.m_pStrPtr;
                const size_t tempCap = lRef.m_capacity;
                lRef.m_pStrPtr = nullptr;
                lRef.SetCString(GetCString());
                m_pStrPtr = tempStr;
                m_capacity = tempCap;

            }
            else
            { //Both are on the stack, no optimizations available
                Swap<String<A, S>>(lRef);
            }
        }
        else
        { //Different allocators, no optimizations available
            Swap<String<A, S>>(lRef);
        }
    }

    template<typename A, size_t S>
    template<typename T>
    inline void String<A, S>::Swap(T& lRef)
    {

        if (IsHeapAllocated())
        { //current is heap and other is or isn't
            char* const tempStr = m_pStrPtr;
            m_pStrPtr = nullptr;
            SetCString(lRef.GetCString());
            lRef.SetCString(m_pStrPtr);
            _free(tempStr);

        }
        else if (!IsHeapAllocated() && lRef.IsHeapAllocated())
        { //current is stack, other is heap
            char* const tempStr = lRef.m_pStrPtr;
            lRef.m_pStrPtr = nullptr;
            lRef.SetCString(GetCString());
            SetCString(tempStr);
            lRef._free(tempStr);

        }
        else
        { //Both are on the stack, no optimizations available
            char tempStr[S];
            //the following logic cannot handle either being a null string
            DD_ASSERT(GetLength() && lRef.GetLength());
            strcpy(tempStr, GetCString());
            SetCString(lRef.GetCString());
            lRef.SetCString(tempStr);

        }
    }

    /* Changes string length and maybe resizing the length of its internal buffer.
     * Returns true if the operation caused the internal buffer to resize.
     *
     * Whether the buffer resizes or not is determined by a set of rules. if
     * the buffer does not actually resize internally, the NULL terminator is
     * essentially just moved to reflect the new string length.
     *
     * BUFFER RESIZE RULES:
     *
     * A. GROWING BUFFER:
     * - Resizes (or intially allocates) internal heap buffer
     * Requirements:
     * 1) Requested size is larger than current
     * 2) Requested size is larger than stack size
     *
     * B. SHRINKING BUFFER (optional):
     * - Allows heap allocated string to shrink back onto the stack
     * - Must be enabled by defining DEV_DRIVER_STRING_AUTO_SHRINK_STACK
     * Requirements:
     * 1) Requested size is smaller than current
     * 2) Requested size is small enough to fit on stack
     * 3) Current is allocated on the heap
     */

    template<typename A, size_t S>
    bool String<A, S>::Resize(size_t length)
    {
        bool resized = false;

        const  size_t newSize = length;// + 1; //take null terminator into account

         //Heap allocation required
        if (newSize > m_capacity)
        {
            //We're already allocated on the heap
            if (IsHeapAllocated())
            {
                _realloc(m_pStrPtr, newSize, &m_capacity);
            }
            else
            { //Perform initial heap allocation
                m_pStrPtr = static_cast<char*>(_alloc(sizeof(char) * newSize, &m_capacity));

                //Copy existing stack value to heap
                strncpy(m_pStrPtr, m_stackStr, length);
            }

            resized = true;
        }
        else if (newSize < m_capacity)
        {

#ifdef DD_STRING_AUTO_SHRINK_TO_STACK
            //Move string from heap to stack and free heap memory
            if (IsHeapAllocated() && newSize <= S)
            {
                strncpy(m_stackStr, m_pStrPtr, length);
                m_capacity = S;
                _free(m_pStrPtr);
                m_pStrPtr = m_stackStr;
                resized = true;
            }
#endif
            //deliminate string based on new length
            m_pStrPtr[length] = '\0';
        }
        return resized;
    }

    template<typename A, size_t S>
    inline bool String<A, S>::ShrinkToFit()
    {
        bool shrank = false;
        const size_t newCapacity = GetLength() + 1;

        if (IsHeapAllocated() && newCapacity < GetCapacity())
        {
            //move from heap to stack
            if (newCapacity <= S)
            {
                strncpy(m_stackStr, m_pStrPtr, GetLength());
                m_stackStr[GetLength()] = '\0';
                m_pStrPtr = m_stackStr;
                m_capacity = S;
            }
            else
            { //make heap buffer smaller
                m_pStrPtr = static_cast<char*>(_realloc(m_pStrPtr, newCapacity, &m_capacity));
            }
            shrank = true;
        }
        return shrank;
    }

    template<typename A, size_t S>
    inline String<A, S>& String<A, S>::Erase(size_t index, size_t len)
    {
        const size_t curLen = GetLength();
        DD_ASSERT(index < curLen);
        DD_ASSERT(len == npos || len + index <= curLen);

        //a. Erase remainder of string if 'npos'
        //b. Dont allow erasing beyond string length
        if (len == npos || index + len > curLen)
        {
            len = curLen - index;
        }

        const size_t newLen = curLen - len;

        for (size_t i = index; i < newLen; ++i)
        {
            m_pStrPtr[i] = m_pStrPtr[i + len];
        }

        Resize(newLen + 1);

        return *this;
    }

    template<typename A, size_t S>
    inline auto String<A, S>::Erase(iterator p) -> iterator
    {
        const size_t len = GetLength();
        if (p.m_index < len)
        {
            Erase(p, p.m_index);
            return p;
        }
        else
        {
            return iterator(this, len);
        }
    }

    template<typename A, size_t S>
    inline auto String<A, S>::Erase(iterator first, iterator last) -> iterator
    {
        DD_ASSERT(first.m_index < GetLength());

        int count;
        const size_t len = GetLength();
        if (last.m_index >= len)
        {
            count = len - first.m_index;
        }
        else
        {
            count = last.m_index - first.m_index;
        }

        Erase(first.m_index, count);
    }


    template<typename A, size_t S>
    template<typename T>
    inline String<A, S>& String<A, S>::Insert(size_t index, const T& str, size_t len)
    {
        const size_t strLen = str.GetLength();

        Insert(index, str.GetCString(),
            (len == npos || len > strLen) ? strLen : len);
    }

    template<typename A, size_t S>
    template<typename T>
    inline String<A, S>& String<A, S>::Insert(size_t index, const char* str, size_t len)
    {
        DD_ASSERT(str);
        const size_t length = GetLength();
        DD_ASSERT(index < length);
        DD_ASSERT(len == npos || index + len <= length);

        const size_t actualNewLen = strlen(str);
        const size_t addedLen = (len == npos || len > actualNewLen) ? actualNewLen : len;
        const size_t newLen = length + addedLen;
        const char* cSubStr = str;
        Resize(newLen + 1);

        //Scoot everything over by the length of the substring to leave a hole
        for (size_t p = newLen - 1; p >= index + addedLen; --p)
        {
            m_pStrPtr[p] = m_pStrPtr[p - addedLen];
        }

        //Copy substring into new hole
        for (size_t i = 0; i < addedLen; ++i)
        {
            m_pStrPtr[index + i] = cSubStr[i];
        }

        return *this;
    }

    template<typename A, size_t S>
    template<typename I>
    inline String<A, S>& String<A, S>::Insert(iterator pos, I itFirst, I itLast)
    {
        return Insert(pos.m_index, &itFirst.m_pCont[itFirst.m_index], itLast.m_index - itFirst.m_index);
    }

    template<typename A, size_t S>
    inline String<A, S>& String<A, S>::Insert(iterator pos, char c, size_t n)
    {
        char tempBuff[n + 1];
        memset(tempBuff, c, n);
        tempBuff[n] = { '\0' };
        return Insert(pos.m_index, tempBuff, n);
    }

    template<typename A, size_t S>
    inline String<A, S> String<A, S>::Substr(size_t index, size_t len) const
    {
        const size_t curLen = GetLength();
        String string;

        DD_ASSERT(index < curLen);
        DD_ASSERT(len == npos || index + len <= curLen);
        if (len == npos || index + len > curLen)
        {
            len = curLen - index;
        }

        if (len)
        {
            string.SetCString(GetCString(), index, len);
        }
        return string;
    }

    template<typename A, size_t S>
    template<typename T>
    inline size_t String<A, S>::Find(const T& str, size_t index, size_t len) const
    {
        return Find(str.GetCString(), index, len);
    }

    template<typename A, size_t S>
    inline size_t String<A, S>::Find(const char* cstr, size_t index, size_t len) const {

        DD_ASSERT(cstr);
        const size_t subLength = (len == npos)? strlen(cstr) : len;
        const size_t length = GetLength();

        DD_ASSERT(index < length);
        DD_ASSERT(subLength < length);
        DD_ASSERT(index + subLength <= length);

        for (size_t i = index; i < length - subLength; ++i)
        {
            if (memcmp(&m_pStrPtr[i], cstr, subLength) == 0)
            {
                return i;
            }
        }
        return npos;
    }


    template<typename A, size_t S>
    inline bool String<A, S>::operator==(const char* str) const
    {
        if (!str || !GetCString())
        {
            return false;
        }
        else
        {
            return strcmp(GetCString(), str) == 0;
        }
    }

    template<typename A, size_t S>
    inline bool String<A, S>::operator!=(const char* str) const
    {
        return !(*this == str);
    }

    template<typename A, size_t S>
    inline String<A, S>& String<A, S>::operator+=(const char* str)
    {
        Append(str);
        return *this;
    }

    template<typename A, size_t S>
    template<typename T>
    inline bool String<A, S>::operator==(const T& rhs) const
    {
        return (GetLength() && rhs.GetLength())?
            strcmp(GetCString(), rhs.GetCString()) == 0 : false;
    }

    template<typename A, size_t S>
    template<typename T>
    inline bool String<A, S>::operator!=(const T& rhs) const
    {
        return !(*this == rhs);
    }

    template<typename A, size_t S>
    template<typename T>
    inline String<A, S>& String<A, S>::operator+=(const T& rhs)
    {
        Append(rhs.GetCString());
        return *this;
    }

    template<typename A, size_t S>
    inline String<A, S>& String<A, S>::operator=(const char* ptr)
    {
        SetCString(ptr);
        return *this;
    }

    template<typename A, size_t S>
    template<typename T>
    inline String<A, S>& String<A, S>::operator=(const T& rhs)
    {
        SetCString(rhs.GetCString());
        return *this;
    }

    //only do move semantics for string of this allocator type
    template<typename A, size_t S>
    template<size_t N>
    String<A, S>& String<A, S>::operator=(String<A, N>&& rhs)
    {
        if (GetAllocator() == rhs.GetAllocator() && rhs.IsHeapAllocated())
        {
            m_pStrPtr = rhs.m_pStrPtr;
            m_capacity = rhs.m_capacity;
            rhs.m_pStrPtr = nullptr;
            return *this;
        }
        else
        {
            return *this = rhs;
        }
    }

    template<typename A, size_t S>
    inline const char& String<A, S>::operator[](size_t index) const
    {
        DD_ASSERT(index < GetLength());
        return GetCString()[index];
    }

    template<typename A, size_t S>
    inline char& String<A, S>::operator[](size_t index)
    {
        DD_ASSERT(index < GetLength());
        return m_pStrPtr[index];
    }

    // Internal utility wrapper around allocation mechanism, should allocator
    // semantics ever change. Equivalent to malloc.
    template<typename A, size_t S>
    void* String<A, S>::_alloc(size_t size, size_t* newSize)
    {
        DD_ASSERT(newSize);
        size_t reqSize = size + DD_STRING_GROW_SIZE_PADDING;
        void* pData = nullptr;

        if(size > 0)
        {

            if (!m_pAllocator)
            {
#ifdef DD_NO_ALLOCATOR_USE_C_STD_LIB
                pData = malloc(reqSize);
#endif
            }
            else
            {
                pData = m_pAllocator->allocate(reqSize);
            }
        }

        if(pData != nullptr)
        {
            *newSize = reqSize;
        }
        else
        {
            *newSize = 0;
        }

        DD_ASSERT(size <= *newSize);
        return pData;
    }

    // Internal utility wrapper around rr conteallocation mechanism, should allocator
    // semantics ever change. Equivalent to realloc.
    template<typename A, size_t S>
    void* String<A, S>::_realloc(void* ptr, size_t size, size_t* newSize)
    {
        DD_ASSERT(ptr && newSize);
        size_t reqSize = size + DD_STRING_GROW_SIZE_PADDING;
        void* pData = nullptr;

        if(size > 0)
        {

            if (!m_pAllocator)
            {
#ifdef DD_NO_ALLOCATOR_USE_C_STD_LIB
                pData = realloc(ptr, reqSize);
#endif
            }
            else
            {
                pData = m_pAllocator->reallocate(ptr, reqSize);
            }
        }

        if(pData != nullptr)
        {
            *newSize = reqSize;
        }
        else
        {
            *newSize = 0;
        }

        DD_ASSERT(size <= *newSize);
        return pData;
    }

    // Internal utility wrapper around mechanism to free heap data, should
    // allocator semantics ever change. Equivalent to realloc.
    template<typename A, size_t S>
    inline void String<A, S>::_free(void* ptr)
    {
        if (!m_pAllocator)
        {
#ifdef DD_NO_ALLOCATOR_USE_C_STD_LIB
             free(ptr);
#else
            DD_ASSERT(false);
#endif
        }
        else
        {
            m_pAllocator->deallocate(ptr);
        }
    }


    // Replaces the given range of the string with a range of another string
    template<typename A, size_t S>
    template<typename T>
    inline String<A, S> String<A, S>::Replace(size_t pos, size_t len, const T& substr,
        size_t subpos, size_t sublen)
    {
        DD_ASSERT(subpos < substr.GetLength());
        DD_ASSERT(sublen == npos || subpos + sublen < substr.GetLength());

        return Replace(pos, len, substr.GetCString(), subpos, sublen);
    }

    template<typename A, size_t S>
    inline String<A, S>& String<A, S>::Replace(size_t pos, size_t len, const char* str,
        size_t subpos, size_t sublen)
    {
        DD_ASSERT(str);
        DD_ASSERT(pos < GetLength());
        DD_ASSERT(pos + len < GetLength());
        DD_ASSERT(subpos < strlen(str));
        DD_ASSERT(sublen == npos || subpos + sublen < strlen(str));

        Erase(pos, len);
        return Insert(pos, sublen, &str[subpos]);
    }

    template<typename A, size_t S>
    inline String<A, S>& String<A, S>::Replace(iterator i1, iterator i2, const char* str, size_t length)
    {
        return relace(i1.m_index, (i2.m_index == npos)? npos : i2.m_index - i1.m_index, str, length);
    }

    template<typename A, size_t S>
    template<typename I>
    inline String<A, S>& String<A, S>::Replace(iterator dstFirst, iterator dstLast, I srcFirst, I srcLast)
    {
        return Replace(dstFirst.m_index, (dstLast._mIndex == npos)? npos : dstLast.m_index - dstFirst.m_index,
             srcFirst.m_pCont, srcFirst.m_index, (srcLast.m_index == npos)? npos : srcLast.m_index - srcFirst.m_index);
    }

    template<typename A, size_t S>
    inline auto String<A, S>::begin() -> iterator
    {
        return iterator(this, 0);
    }

    template<typename A, size_t S>
    inline auto String<A, S>::begin() const -> const_iterator
    {
        return const_iterator(this, 0);
    }

    template<typename A, size_t S>
    inline auto String<A, S>::cbegin() const -> const_iterator
    {
        return const_iterator(this, 0);
    }

    template<typename A, size_t S>
    inline auto String<A, S>::end() -> iterator
    {
        return iterator(this, GetLength());
    }

    template<typename A, size_t S>
    inline auto String<A, S>::end() const -> const_iterator
    {
        return const_iterator(this, GetLength());
    }

    template<typename A, size_t S>
    inline auto String<A, S>::cend() const -> const_iterator
    {
        return const_iterator(this, GetLength());
    }

    template<typename A, size_t S>
    inline A* String<A, S>::GetAllocator()
    {
        return m_pAllocator;
    }

    template<typename A, size_t S>
    inline const A* String<A, S>::GetAllocator() const
    {
        return m_pAllocator;
    }

    // Returns the requested C++11/STL compatible iterators
    template<typename A, size_t S>
    inline auto String<A, S>::rbegin() -> reverse_iterator
    {
        const size_t len = GetLength();
        return reverse_iterator(iterator(this, len ? len : 0));
    }

    template<typename A, size_t S>
    inline auto String<A, S>::rbegin() const -> const_reverse_iterator
    {
        const size_t len = GetLength();
        return const_reverse_iterator(const_iterator(this, len ? len : 0));

    }

    template<typename A, size_t S>
    inline auto String<A, S>::crbegin() const -> const_reverse_iterator
    {
        const size_t len = GetLength();
        return const_reverse_iterator(const_iterator(this, len ? len : 0));
    }

    template<typename A, size_t S>
    inline auto String<A, S>::rend() -> reverse_iterator
    {
        return reverse_iterator(iterator(this, static_cast<size_t>(0)));
    }

    template<typename A, size_t S>
    inline auto String<A, S>::rend() const -> const_reverse_iterator
    {
        return const_reverse_iterator(const_iterator(this, static_cast<size_t>(0)));
    }

    template<typename A, size_t S>
    inline auto String<A, S>::crend() const -> const_reverse_iterator
    {
        return const_reverse_iterator(const_iterator(this, static_cast<size_t>(0)));
    }

    // Static member functions.
    template<typename A, size_t S>
    inline size_t String<A, S>::GetStackSize(void)
    {
        return S;
    }

    template<typename A, size_t S>
    inline size_t String<A, S>::GetDefaultStackSize(void)
    {
        return DD_STRING_STACK_SIZE_DEFAULT;
    }

    template<typename A, size_t S>
    inline size_t String<A, S>::GetGrowSizePadding(void)
    {
        return DD_STRING_GROW_SIZE_PADDING;
    }

}

#undef DD_ASSERT

#endif
