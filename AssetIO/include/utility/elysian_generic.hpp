#ifndef ELYSIAN_GENERIC_HPP
#define ELYSIAN_GENERIC_HPP
/* @brief Small, generic utility functions */
#include <vector>
#include <algorithm> /* for std::remove */
namespace elysian {

    /* @brief Iterates through haystack, erase()s all items from haystack
    whose its memory address is the same as needle's.
    @returns true if item is found and erased, false otherwise */
    template <class T, class U>
    inline bool containerEraseOnMatch(const T needle, U& haystack) {
        /* A modified erase/remove idiom */
        auto old_last = std::end(haystack);
        auto new_last = std::remove(std::begin(haystack),
                                    old_last,
                                    needle);
        if (old_last == new_last) {
            return false;
        }
        haystack.erase(new_last, old_last);
        return true;
    }

    template<typename T>
    inline T incrementEnum(T& val) {
        unsigned intVal = (unsigned) val;
        ++intVal;
        val = static_cast<T>(intVal);
        return val;
    }





    /*No No, No No No No, No No No No, No No there's a limit! */
    #	define OOLUA_INTERNAL_60_N \
            N, N, N, N, N, N, N, N, N, N \
            , N, N, N, N, N, N, N, N, N, N \
            , N, N, N, N, N, N, N, N, N, N \
            , N, N, N, N, N, N, N, N, N, N \
            , N, N, N, N, N, N, N, N, N, N \
            , N, N, N, N, N, N, N, N, N, N


    #	define OOLUA_NARG(...)\
            OOLUA_NARG_DO(__VA_ARGS__, OOLUA_RSEQ_N)

    #	define OOLUA_ARG_N(\
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10 \
            , _11, _12, _13, _14, _15, _16, _17, _18, _19, _20 \
            , _21, _22, _23, _24, _25, _26, _27, _28, _29, _30 \
            , _31, _32, _33, _34, _35, _36, _37, _38, _39, _40 \
            , _41, _42, _43, _44, _45, _46, _47, _48, _49, _50 \
            , _51, _52, _53, _54, _55, _56, _57, _58, _59, _60 \
            , _61, _62, _63, N, ...) N

    #	define OOLUA_RSEQ_N\
            63, 62, 61, 60 \
            , 59, 58, 57, 56, 55, 54, 53, 52, 51, 50 \
            , 49, 48, 47, 46, 45, 44, 43, 42, 41, 40 \
            , 39, 38, 37, 36, 35, 34, 33, 32, 31, 30 \
            , 29, 28, 27, 26, 25, 24, 23, 22, 21, 20 \
            , 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 \
            , 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#		define OOLUA_NARG_DO(...) \
            OOLUA_ARG_N(__VA_ARGS__)

#define _ELYSIAN_VA_EXPAND_0(COMMA, OPERATION)
#define _ELYSIAN_VA_EXPAND_1(COMMA, OPERATION, PARAM1) OPERATION(PARAM1)
#define _ELYSIAN_VA_EXPAND_2(COMMA, OPERATION, PARAM1, PARAM2)  _ELYSIAN_VA_EXPAND_1(COMMA, OPERATION, PARAM1) COMMA() OPERATION(PARAM2)
#define _ELYSIAN_VA_EXPAND_3(COMMA, OPERATION, PARAM1, PARAM2, PARAM3) _ELYSIAN_VA_EXPAND_2(COMMA, OPERATION, PARAM1, PARAM2)  COMMA() OPERATION(PARAM3)
#define _ELYSIAN_VA_EXPAND_4(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4) _ELYSIAN_VA_EXPAND_3(COMMA, OPERATION, PARAM1, PARAM2, PARAM3)  COMMA() OPERATION(PARAM4)
#define _ELYSIAN_VA_EXPAND_5(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5) _ELYSIAN_VA_EXPAND_4(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4) COMMA()  OPERATION(PARAM5)
#define _ELYSIAN_VA_EXPAND_6(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6) _ELYSIAN_VA_EXPAND_5(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5) COMMA()  OPERATION(PARAM6)
#define _ELYSIAN_VA_EXPAND_7(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7) _ELYSIAN_VA_EXPAND_6(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6) COMMA()  OPERATION(PARAM7)
#define _ELYSIAN_VA_EXPAND_8(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8) _ELYSIAN_VA_EXPAND_7(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7) COMMA()  OPERATION(PARAM8)
#define _ELYSIAN_VA_EXPAND_9(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9) _ELYSIAN_VA_EXPAND_8(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8) COMMA()  OPERATION(PARAM9)
#define _ELYSIAN_VA_EXPAND_10(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10) _ELYSIAN_VA_EXPAND_9(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9) COMMA()  OPERATION(PARAM10)
#define _ELYSIAN_VA_EXPAND_11(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11) _ELYSIAN_VA_EXPAND_10(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10) COMMA()  OPERATION(PARAM11)
#define _ELYSIAN_VA_EXPAND_12(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12) _ELYSIAN_VA_EXPAND_11(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11) COMMA()  OPERATION(PARAM12)
#define _ELYSIAN_VA_EXPAND_13(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12, PARAM13) _ELYSIAN_VA_EXPAND_12(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12) COMMA()  OPERATION(PARAM13)
#define _ELYSIAN_VA_EXPAND_14(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12, PARAM13, PARAM14) _ELYSIAN_VA_EXPAND_13(COMMA, OPERATION, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12, PARAM13) COMMA()  OPERATION(PARAM14)

#define _ELYSIAN_VA_COMMA()         ,
#define _ELYSIAN_VA_NO_COMMA()

#define _ELYSIAN_VA_EXPAND(COMMA, OPERATION, ...)  \
    _ELYSIAN_VA_CONCAT(_ELYSIAN_VA_EXPAND_, OOLUA_NARG(__VA_ARGS__)) (COMMA, OPERATION, __VA_ARGS__)

#define ELYSIAN_VA_EXPAND(OPERATION, ...)  \
    _ELYSIAN_VA_EXPAND(_ELYSIAN_VA_NO_COMMA, OPERATION, __VA_ARGS__)

#define ELYSIAN_VA_EXPAND_LIST(OPERATION, ...) \
    _ELYSIAN_VA_EXPAND(_ELYSIAN_VA_COMMA, OPERATION, __VA_ARGS__)

#	define _ELYSIAN_VA_CONCAT2(A, B) A##B
#	define _ELYSIAN_VA_CONCAT(A, B) _ELYSIAN_VA_CONCAT2(A, B)
#	define ELYSIAN_VA_CONCAT(A, B) DO_VA_CONCAT(A, B)



#if 0
#	define _ELYSIAN_VA_CONCAT3(A, B, C) COMMA##NAME##POST
#	define _ELYSIAN_VA_CONCAT(A, B, C) _ELYSIAN_VA_CONCAT3(A, B, C)
#	define ELYSIAN_VA_CONCAT(A, B, C) _ELYSIAN_VA_CONCAT(A, B, C)
#endif

#if 0
    struct Type_list
    {
    private:
        typedef typename Type_list
            <
                 T02, T03, T04, T05, T06, T07, T08, T09, T10,
            T11, T12, T13, T14, T15, T16, T17, T18, T19, T20,
            T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
            T31, T32, T33, T34, T35, T36, T37, T38, T39, T40,
            T41, T42, T43, T44, T45, T46, T47, T48, T49, T50
            >
            ::type TailResult;

    public:
        typedef Type_node<T01, TailResult> type;
    };

    template<>
    struct Type_list<>
    {
        typedef TYPE::Null_type type;
    };


    template<typename T>struct lua_return_count;

    template <>
    struct lua_return_count<TYPE::Null_type>
    {
        enum { out = 0 };
    };

    template<typename T, typename T1>
    struct lua_return_count< Type_node<T, T1> >
    {
        enum { out = T::out + lua_return_count<T1>::out };
    };

    namespace TYPE
    {
        struct Null_type{};
    }

#endif
    template<typename P, class... _Types> struct GenericMemberData;

    template<typename P>
    struct GenericMemberData<P> {

        template<typename T>
        T& getGenericData(void) { }

        template<typename T>
        void setGenericData(const T&/* data*/) {}

    };

    template<typename P, class _This,
             class... _Rest>
      struct GenericMemberData<P, _This, _Rest...>
      : public GenericMemberData<P, _Rest...>
    {

    public:
        typedef GenericMemberData<P, _This, _Rest...> GenericMemberDataType;
/*
        template<_This>
        _This& getGenericData(void) { return _genericDataElement; }

        template<_This>
        void setGenericData(const _This& value) { _genericDataElement = value; }
*/
    protected:

        _This _genericDataElement;

    };


#if 0
      template<typename P, typename I, class _This,
               class... _Rest>
        struct GenericMemberData<P, std::pair<I, _This>, _Rest...>
        : public GenericMemberData<P, _Rest...>
      {

      public:
          typedef GenericMemberData<P, _This, _Rest...> GenericMemberDataType;

          template<I>
          _This& getGenericData(void) { return _genericDataElement; }

          template<I>
          void setGenericData(const _This& value) { _genericDataElement = value; }

      protected:

          _This _genericDataElement;

      };

#endif


    // tuple_element
    template<size_t _Index, class _GenericMemberData> struct GenericMemberDataElement;

    // select first element
    template<typename P, class _This, class... _Rest>
    struct GenericMemberDataElement<0, GenericMemberData<P, _This, _Rest...>>
    {
      typedef _This& type;
      typedef GenericMemberData<P, _This, _Rest...> _Ttype;
    };

    // recursive tuple_element definition
    template <size_t _Index, class P, class _This, class... _Rest>
    struct GenericMemberDataElement<_Index, GenericMemberData<P, _This, _Rest...>>
      : public GenericMemberDataElement<_Index - 1, GenericMemberData<P, _Rest...> >
    {
    };


    // get reference to _Index element of tuple
    template<size_t _Index, typename P, class... _Types> inline
      typename GenericMemberDataElement<_Index, GenericMemberData<P, _Types...>>::type
      getGenericData(GenericMemberData<P, _Types...>& _memberData)
    {
      typedef typename GenericMemberDataElement<_Index, GenericMemberData<P, _Types...>>::_Ttype _Ttype;
      return (((_Ttype&) _memberData)._genericDataElement);
    }

}

#endif
