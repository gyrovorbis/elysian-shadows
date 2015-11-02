#ifndef ELYSIAN_LUA_VARIANT_BASE_HPP
#define ELYSIAN_LUA_VARIANT_BASE_HPP

#include <initializer_list>
#include <cstdlib>
#include <crtp/elysian_lua_manager.hpp>

#define ELYSIAN_LUA_VARIANT_DEBUG   ELYSIAN_LUA_DEBUG

#define ELYSIAN_LUA_VARIANT_TYPE_BIT_POS	0
#define ELYSIAN_LUA_VARIANT_TYPE_BIT_MASK	0x0f

template<unsigned...> struct index_tuple{};

template<unsigned I, typename IndexTuple, typename... Types>
struct make_indices_impl;

template<unsigned I, unsigned... Indices, typename T, typename... Types>
struct make_indices_impl<I, index_tuple<Indices...>, T, Types...>
{
  typedef typename
    make_indices_impl<I + 1,
                      index_tuple<Indices..., I>,
                      Types...>::type type;
};

template<unsigned I, unsigned... Indices>
struct make_indices_impl<I, index_tuple<Indices...> >
{
  typedef index_tuple<Indices...> type;
};

template<typename... Types>
struct make_indices
  : make_indices_impl<0, index_tuple<>, Types...>
{};


namespace elysian {


    class LuaFieldRef;

    //add function operator
    //support C functions/closures and Lua functions


    //- support integers as numbers
    //support conversions
    //support calling arbitrary functions
    //support storing arbitrary C functions/closures/lambdas in Lua (use upvalues for stateful storage)
    //- support table references

/*
 *
 *         friend     bool push<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase const& variant);
        friend     bool pull<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase const& variant);
        */
    class LuaVariantBase: public LuaManager {
        friend     bool OOLUA::push<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase const& variant);
        friend     bool OOLUA::pull<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase& variant);

    protected:
        uint32_t                _type       = LUA_TNIL; //thas a lot of bits .. Type only needs 4, rest can be internal state data
        union Union {
            bool                boolean;
            lua_Number          number;
            int                 ref         = LUA_REFNIL;
            const char*         string;
            void*               userdata;
        } _value;

        template<typename L>
        LuaVariantBase              _convertType(const L converter) const;
        template<typename L>
        LuaVariantBase&             _convertType(const L converter);

        static bool             _lightConvertToBool(const LuaVariantBase& source);
        static LuaVariantBase&      _convertToBool(const LuaVariantBase& source, LuaVariantBase& dest);
        static lua_Number       _lightConvertToNumber(const LuaVariantBase& source);
        static LuaVariantBase&      _convertToNumber(const LuaVariantBase& source, LuaVariantBase& dest);


        template <unsigned... Indices, class... Args, class Ret>
        static Ret forward_impl(index_tuple<Indices...>,
                         std::tuple<Args...> tuple,
                         const std::function<Ret(Args...)>& closure)
        {
          return closure(std::get<Indices>(tuple)...);
        }

        template<typename Ret, class... Args>
        static Ret forward(std::tuple<Args...> tuple, const std::function<Ret(Args...)>& closure)
        {
           typedef typename make_indices<Args...>::type Indices;
           return forward_impl(Indices(), tuple, closure);
        }

        template<std::size_t> struct int_{};

        template <class Tuple, size_t Pos>
        static void pull_tuple(Tuple& t, int_<Pos> ) {
          OOLUA::pull(*_mainState, std::get< std::tuple_size<Tuple>::value-Pos >(t));
          return pull_tuple(t, int_<Pos-1>());
        }

        template <class Tuple>
        static void pull_tuple(Tuple& t, int_<1> ) {
          OOLUA::pull(*_mainState, std::get<std::tuple_size<Tuple>::value-1>(t));
        }


        //Assumes actual function is already at the top of the stack!
        template<typename Ret, typename... Args>
        static int _callLambda(lua_State*vm) {
            std::tuple<Args...> argTuple;

            pull_tuple(argTuple, int_<sizeof...(Args)>());

            //MEMORY LEAK!!!!!
            //change this to full-userdata, set a metatable with a _gc metamethod so this context can be deleted...
            auto* closure = static_cast<const std::function<Ret(Args...)>*>( lua_topointer(vm, lua_upvalueindex(1)) );

            const Ret retVal = forward<Ret>(argTuple, *closure);

            OOLUA::push(*_mainState, retVal);
            return 1;
        }



    public:
#if 0
        template<bool is_const_iterator=true>

        class const_noconst_iterator : public std::iterator<std::forward_iterator_tag, LuaVariantBase>, public LuaManager {
            friend class const_noconst_iterator<true>;
            friend class LuaVariantBase;

            typedef typename std::conditional<is_const_iterator, const LuaVariantBase*, LuaVariantBase*>::type DataStructurePointerType;
            typedef typename std::conditional<is_const_iterator, const LuaVariantBase&, LuaVariantBase&>::type ValueReferenceType;

            const_noconst_iterator(DataStructurePointerType pointer_to_datastructure, const int funcRef, const LuaPair initialValue):
                _pair(initialValue),
                _variant(pointer_to_datastructure),
                _nextFuncRef(funcRef)
            {


            }

            /**
            * Copy constructor. Allows for implicit conversion from a regular iterator to a const_iterator
            */
            const_noconst_iterator(const const_noconst_iterator<false>& other):
              _pair(other._pair),
              _variant(other._variant),
                _nextFuncRef(other._nextFuncRef){

            }

            /**
            * Equals comparison operator
            */
            bool operator== (const const_noconst_iterator& other) const {
                return _variant == _variant && _pair == other._pair;
            }

            /**
            * Not-equals comparison operator
            * @see operator==(const const_noconst_iterator&) const
            */
            bool operator!= (const const_noconst_iterator& other) const {
            return !(*this == other);
            }

            /**
            * Dereference operator
            * @return the value of the element this iterator is currently pointing at
            */
            ValueReferenceType operator*(void) {
                return _pair.second();
            }


            /**
            * Prefix increment operator (e.g., ++it)
            */
            const_noconst_iterator &operator++(void){
                lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _nextFuncRef);
                lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _variant._value.ref);
                OOLUA::push(_pair.first());

                _protectedCall(2, 2, nullptr);

                OOLUA::pull(*_mainState, _pair);

            }

            /**
            * Postfix increment operator (e.g., it++)
            */
            const_noconst_iterator operator++(int){
            // Use operator++()
            const const_noconst_iterator old(*this);
            ++(*this);
            return old;
            }

        private:
            LuaPair                     _pair;
            DataStructurePointerType    _variant;
            int                         _nextFuncRef    = LUA_NOREF;

        };


       using iterator       = const_noconst_iterator<false>;
       using const_iterator = const_noconst_iterator<true>;
#endif



                                LuaVariantBase(void) = default;
                                template<typename T>
                                LuaVariantBase(const std::initializer_list<T>& list);
                                template<typename T>
                                LuaVariantBase(T*const userdata);
                                LuaVariantBase(std::nullptr_t null);
                                LuaVariantBase(const LuaVariantBase& rhs);
                                LuaVariantBase(LuaVariantBase&& rhs);
                                LuaVariantBase(const int ref);
                                LuaVariantBase(const float number);
                                LuaVariantBase(const double number);
                                LuaVariantBase(const bool val);
                                LuaVariantBase(const char*const string);
                                LuaVariantBase(void*const userdata);
                                ~LuaVariantBase(void);

        LuaVariantBase&             operator=(const LuaVariantBase& rhs);
        LuaVariantBase&             operator=(LuaVariantBase&& rhs);
        LuaVariantBase&             operator=(const std::nullptr_t);
        LuaVariantBase&             operator=(const bool val);
        LuaVariantBase&             operator=(const int num);
        LuaVariantBase&             operator=(const float num);
        LuaVariantBase&             operator=(const double num);
        LuaVariantBase&             operator=(const char*const string);
        LuaVariantBase&             operator=(void*const ptr);


        bool                    operator==(const LuaVariantBase& rhs) const;
        bool                    operator==(const std::nullptr_t) const;
        bool                    operator==(const bool val) const;
        bool                    operator==(const int num) const;
        bool                    operator==(const float num) const;
        bool                    operator==(const double num) const;
        bool                    operator==(void*const ptr) const;
        bool                    operator==(const char*const str) const;

        bool                    operator!=(const LuaVariantBase& rhs) const;
        bool                    operator!=(const std::nullptr_t) const;
        bool                    operator!=(const bool val) const;
        bool                    operator!=(const int num) const;
        bool                    operator!=(const float num) const;
        bool                    operator!=(const double num) const;
        bool                    operator!=(void*const ptr) const;
        bool                    operator!=(const char*const str) const;


        template <typename... Args>
        LuaVariantBase              operator()(Args&& ... args);

        template <typename Ret, typename... Args>
        void                    setClosure(std::function<Ret(Args...)> closure);

        //does not attempt conversion, so you better know what the fuck type you are using at compile-time
        explicit                operator bool(void) const;
        explicit                operator int(void) const;
        explicit                operator float(void) const;
        explicit                operator double(void) const;
        explicit                operator const char*(void) const; //FUCK YOU, FUCKING AMBIGUOUS INDEX OPERATOR
                                operator void*(void) const;

        //returns new QVariant with the desired type
        LuaVariantBase              asBool(void) const;
        LuaVariantBase              asNumber(void) const;
        LuaVariantBase              asString(void) const;
        LuaVariantBase              asUserdata(void) const;

        //conversion functions - converts and returns converted value
        bool                    toBool(void);
        lua_Number              toNumber(void);
        const char*             toString(void);
        void*                   toUserdata(void);


        //doesn't do conversions, so type better be known statically
        template<typename T>
        T                       getValue(void) const;
        int                     getRef(void) const;


        void                    setNil(void);
        void                    setRef(const int ref);

        //template<typename T>
        //void                    setValue(T*const ud);
        void                    setValue(const std::nullptr_t null);
        void                    setValue(const int ref);
        void                    setValue(const float number);
        void                    setValue(const double number);
        void                    setValue(const char*const str);
        void                    setValue(const bool val);
        void                    setValue(void*const ptr);
        void                    setValue(const OOLUA::Lua_table_ref& ref);

        bool                    isNil(void) const;
        bool                    isNilRef(void) const;
        bool                    isRefType(void) const;
        bool                    isIndexable(void) const;
        bool                    isValueType(void) const;
        bool                    isUserdataType(void) const;
        int                     getType(void) const;
        const char*             getTypeString(void) const;
        void                    setType(const int luaType);

        bool                    push(void) const;
        bool                    pull(void);

        //Table Indexing methods
//        void            insert(const LuaPair& pair);
//        void            insert(const LuaVariantBase& key, const LuaVariantBase& value=LuaVariantBase());
 //       void            insert(const char*const key, const LuaVariantBase& value=LuaVariantBase()); //override for performance

        LuaVariantBase      getValue(const LuaVariantBase& key) const;
        LuaFieldRef     operator[](const LuaVariantBase& key) const;

        //LuaTableRef     getMetatable(void);
        //bool            load(const char*const filename);
        //bool            save(const char*const filename);

        //C++11-style iterators

 //       iterator        begin(void);
  //      const_iterator  begin(void) const;
   //     const_iterator  cbegin(void) const;
    //    iterator        end(void);
   //     const_iterator  end(void) const;
   //     const_iterator  cend(void) const;


        static LuaVariantBase createFromString(const char*const str);
        static LuaVariantBase createFromFile(const char*const filename);


        static LuaVariantBase       fromStack(const int index=-1);

    };

    using LuaPair = std::pair<LuaVariantBase, LuaVariantBase>;



    class LuaFieldRef: public LuaManager {
        friend class LuaVariantBase;
    private:
        int             _luaRef = LUA_REFNIL;
        LuaVariantBase      _key;

                        LuaFieldRef(const int ref, const LuaVariantBase key);
                        LuaFieldRef(const LuaFieldRef& rhs);
                        LuaFieldRef(LuaFieldRef&& rhs);

    public:
                        ~LuaFieldRef(void);

        LuaVariantBase      getValue(void) const;
        void            setValue(const LuaVariantBase variant);


        LuaFieldRef&    operator=(const LuaFieldRef& rhs);
        LuaFieldRef&    operator=(LuaFieldRef&& rhs);
        LuaFieldRef&    operator=(const LuaVariantBase& rhs);
        //LuaFieldRef&    operator =(LuaVariantBase&& rhs);

        LuaVariantBase      operator->(void);
        LuaVariantBase      operator*(void);
                        operator LuaVariantBase(void) const;
    };













    inline LuaFieldRef::LuaFieldRef(const int ref, const LuaVariantBase key): _key(key) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
        _luaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    }

    inline LuaFieldRef::LuaFieldRef(const LuaFieldRef &rhs) { *this = rhs; }

    inline LuaFieldRef::LuaFieldRef(LuaFieldRef &&rhs) { *this = rhs; }

    inline LuaVariantBase LuaFieldRef::operator->(void) { return getValue(); }

    inline LuaVariantBase LuaFieldRef::operator*(void) { return getValue(); }

    inline LuaFieldRef& LuaFieldRef::operator=(const LuaFieldRef& rhs) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, rhs._luaRef);
        _luaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
        _key = rhs._key;
        return *this;
    }

    inline LuaFieldRef& LuaFieldRef::operator=(LuaFieldRef&& rhs) {
        _luaRef     = rhs._luaRef;
        _key        = rhs._key;
        rhs._luaRef = LUA_REFNIL;
        return *this;
    }

    inline LuaFieldRef& LuaFieldRef::operator=(const LuaVariantBase& rhs) {
        setValue(rhs);
        return *this;
    }

    inline LuaFieldRef::~LuaFieldRef(void) {
        luaL_unref(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    }

    inline LuaVariantBase LuaFieldRef::getValue(void) const {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
        _key.push();
        lua_gettable(*_mainState, -2);
        lua_remove(*_mainState, -2);
        return LuaVariantBase::fromStack();
    }











    // ======== INLINEZ ==========


    template<typename T>
    inline LuaVariantBase::LuaVariantBase(T*const ref) {
        OOLUA::push(*_mainState, ref);
        pull();
    }
    template<typename T>
    inline LuaVariantBase::LuaVariantBase(const std::initializer_list<T>& list) {
        const auto size = list.size();
        if(size == 1) {
            setValue(list[0]);
        } else {
            lua_newtable(*_mainState);
            for(unsigned i = 0; i < size; ++i) {
                OOLUA::push(list[i]);
                lua_rawseti(*_mainState, -2, i+1);
            }
            pull();
        }
    }


    inline LuaVariantBase::LuaVariantBase(const LuaVariantBase& rhs) { *this = rhs; }
    inline LuaVariantBase::LuaVariantBase(std::nullptr_t /*null*/): LuaVariantBase() {}
    inline LuaVariantBase::LuaVariantBase(LuaVariantBase&& rhs) { *this = rhs; }
    inline LuaVariantBase& LuaVariantBase::operator=(const LuaVariantBase& rhs) {
        switch(rhs._type) {
        case LUA_TNIL:
            setNil();
            break;
        case LUA_TBOOLEAN:
            setValue(rhs._value.boolean);
            break;
        case LUA_TNUMBER:
            setValue(rhs._value.number);
            break;
        case LUA_TSTRING:
            setValue(rhs._value.string);
            break;
        case LUA_TLIGHTUSERDATA:
            setValue(rhs._value.userdata);
            break;
        default:
            setRef(rhs.getRef());
        }
        return *this;
    }
    inline LuaVariantBase::LuaVariantBase(const char * const string) { setValue(string); }
    inline LuaVariantBase::LuaVariantBase(const bool val) { setValue(val); }
    inline LuaVariantBase::LuaVariantBase(const float number) { setValue(number); }
    inline LuaVariantBase::LuaVariantBase(const double number) { setValue(number); }
    inline LuaVariantBase::LuaVariantBase(const int num) { setValue(num); }
    inline LuaVariantBase::LuaVariantBase(void*const ptr) { setValue(ptr); }

    inline LuaVariantBase::~LuaVariantBase(void) { setNil(); }

    inline LuaVariantBase& LuaVariantBase::operator=(LuaVariantBase&& rhs) {
        switch(rhs._type) {
        case LUA_TNIL:
            setNil();
            break;
        case LUA_TBOOLEAN:
            setValue(rhs._value.boolean);
            break;
        case LUA_TNUMBER:
            setValue(rhs._value.number);
            break;
        case LUA_TSTRING: //steal heap-allocated string
            _type           = LUA_TSTRING;
            _value.string   = rhs._value.string;
            rhs._type       = LUA_TNIL;
            rhs._value.ref  = LUA_REFNIL;
            break;
        case LUA_TLIGHTUSERDATA:
            setValue(rhs._value.userdata);
            break;
        default:
            _type           = rhs._type;
            _value          = rhs._value;
            rhs._type       = LUA_TNIL;
            rhs._value.ref  = LUA_REFNIL;
        }
        return *this;
    }
    inline LuaVariantBase& LuaVariantBase::operator=(const bool val) { setValue(val); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const std::nullptr_t) { setValue(nullptr); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const int num) { setValue(num); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const float num) { setValue((lua_Number)num); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const double num) { setValue((lua_Number)num); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const char*const string) { setValue(string); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(void*const ptr) { setValue(ptr); return *this; }

    inline bool LuaVariantBase::operator==(const LuaVariantBase& rhs) const {
        if(_type != rhs._type) return false;
        switch(_type) {
        case LUA_TNIL:
            return true;
        case LUA_TBOOLEAN:
            return _value.boolean == rhs._value.boolean;
        case LUA_TNUMBER:
            return _value.number == rhs._value.number;
        case LUA_TSTRING:
            return *this == rhs._value.string;
        case LUA_TLIGHTUSERDATA:
            return _value.userdata == rhs._value.userdata;
        default: //let Lua handle the comparison for complex types
            push();
            rhs.push();
            const bool equality = lua_compare(*_mainState, -1, -2, LUA_OPEQ);
            lua_pop(*_mainState, 2);
            return equality;
        }
    }

    inline bool LuaVariantBase::operator==(const std::nullptr_t) const { return isNil() || (_type == LUA_TLIGHTUSERDATA && _value.userdata == nullptr); }
    inline bool LuaVariantBase::operator==(const bool val) const { return _type == LUA_TBOOLEAN && _value.boolean == val; }
    inline bool LuaVariantBase::operator==(const int num) const { return _type == LUA_TNUMBER && (int)_value.number == (int)num; }
    inline bool LuaVariantBase::operator==(const float num) const { return _type == LUA_TNUMBER && _value.number == (lua_Number)num; }
    inline bool LuaVariantBase::operator==(const double num) const { return _type == LUA_TNUMBER && _value.number == (lua_Number)num; }
    inline bool LuaVariantBase::operator==(void*const ptr) const { return ptr? isUserdataType() && _value.userdata == ptr : isNil(); }
    inline bool LuaVariantBase::operator==(const char*const str) const { return (_type == LUA_TSTRING && (strcmp(_value.string, str) == 0)); };

    inline bool LuaVariantBase::operator!=(const LuaVariantBase& rhs) const { return !(*this == rhs); }
    inline bool LuaVariantBase::operator!=(const std::nullptr_t) const { return !(*this == nullptr); }
    inline bool LuaVariantBase::operator!=(const bool val) const { return !(*this == val); }
    inline bool LuaVariantBase::operator!=(const int num) const { return !(*this == num); }
    inline bool LuaVariantBase::operator!=(const float num) const { return !(*this == num); }
    inline bool LuaVariantBase::operator!=(const double num) const { return !(*this == num); }
    inline bool LuaVariantBase::operator!=(void*const ptr) const { return !(*this == ptr); }
    inline bool LuaVariantBase::operator!=(const char*const str) const { return !(*this == str); }

    inline LuaVariantBase::operator bool(void) const { return _lightConvertToBool(*this); }
    inline LuaVariantBase::operator int(void) const { return _lightConvertToNumber(*this); }
    inline LuaVariantBase::operator float(void) const { return _lightConvertToNumber(*this); }
    inline LuaVariantBase::operator double(void) const { return _lightConvertToNumber(*this); }
    inline LuaVariantBase::operator const char*(void) const { return _type == LUA_TSTRING? _value.string : lua_typename(*_mainState, _type); }
    inline LuaVariantBase::operator void*(void) const { return _type == LUA_TLIGHTUSERDATA? _value.userdata : nullptr; }

    //inline LuaVariantBase::operator LuaTableRef(void) const;
    //inline LuaVariantBase::operator OOLUA::Lua_table_ref(void);
    inline bool LuaVariantBase::isNil(void) const { return _type == LUA_TNIL; }
    inline bool LuaVariantBase::isNilRef(void) const { return _type == LUA_TNIL || _value.ref == LUA_REFNIL; }
    inline bool LuaVariantBase::isRefType(void) const { return !(_type == LUA_TBOOLEAN || _type == LUA_TNUMBER || _type == LUA_TNIL || _type == LUA_TLIGHTUSERDATA|| _type == LUA_TSTRING); }
    inline bool LuaVariantBase::isIndexable(void) const { return (_type == LUA_TUSERDATA || _type == LUA_TTABLE); }
    inline bool LuaVariantBase::isValueType(void) const { return _type == LUA_TNIL || !isRefType(); }
    inline bool LuaVariantBase::isUserdataType(void) const { return _type == LUA_TUSERDATA || _type == LUA_TLIGHTUSERDATA; }

    inline void LuaVariantBase::setType(const int luaType) {
        if(isRefType()) {
            luaL_unref(*_mainState, LUA_REGISTRYINDEX, _value.ref);
        } else if(_type == LUA_TSTRING) {
            free((void*)_value.string);
        } else if(_type == LUA_TNIL) {
            _value.ref = LUA_REFNIL;
        }
        _type = luaType;
    }

    inline void LuaVariantBase::setNil(void) {
        setType(LUA_TNIL);
        _value.ref = LUA_REFNIL;
    }
/*
    template<typename T>
    inline void LuaVariantBase::setValue(T*const ud) {
        OOLUA::push(*_mainState, ud);
        _type = lua_type(*_mainState, -1);
        _value.ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    }
*/
    inline void LuaVariantBase::setValue(std::nullptr_t /*null*/) {
        setNil();
    }

    inline void LuaVariantBase::setValue(const int number) {
        setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const float number) {
        setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const double number) {
        setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const char*const str) {
        if(!str) setValue(nullptr);

        const int size = std::strlen(str);

        if(_type != LUA_TSTRING) {
            setType(LUA_TSTRING);
            _value.string = (const char*)malloc(size*sizeof(char)+1);
        } else {
            _value.string = (const char*)realloc((void*)_value.string, size*sizeof(char)+1);
        }

        std::strcpy((char*)_value.string, str);
    }

    inline void LuaVariantBase::setValue(const bool val) {
        setType(LUA_TBOOLEAN);
        _value.boolean = val;
    }

    inline void LuaVariantBase::setValue(void*const ptr) {
        if(ptr) {
            setType(LUA_TLIGHTUSERDATA);
            _value.userdata = ptr;
        } else setType(LUA_TNIL);
    }

    template<>
    inline  bool LuaVariantBase::getValue<bool>(void) const { return _value.boolean; }

    template<>
    inline  float LuaVariantBase::getValue<float>(void) const { return _value.number; }

    template<>
    inline double LuaVariantBase::getValue<double>(void) const { return _value.number; }

    template<>
    inline  const char* LuaVariantBase::getValue<const char*>(void) const { return _value.string; }

    template<>
    inline  int LuaVariantBase::getValue<int>(void) const { return _value.number; }

    template<>
    inline  void* LuaVariantBase::getValue<void*>(void) const { return _value.userdata; }

    inline  int LuaVariantBase::getType(void) const { return _type; }

    inline const char* LuaVariantBase::getTypeString(void) const {
        if(_type != LUA_TUSERDATA && _type != LUA_TLIGHTUSERDATA) {
            return lua_typename(*_mainState, _type);
        } else {
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _value.ref);
            lua_getfield(*_mainState, -1, ELYSIAN_LUA_PROXY_TYPE_METAFIELD);
            if(!lua_isnil(*_mainState, -1)) {
                const char* name = lua_tostring(*_mainState, -1);
                lua_pop(*_mainState, 2);
                return name;
            } else {
                return lua_typename(*_mainState, _type);
            }
        }
    }

    inline int LuaVariantBase::getRef(void) const {
        return isRefType()? _value.ref : LUA_NOREF;
    }

    inline void LuaVariantBase::setRef(const int ref) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
        setType(lua_type(*_mainState, -1));
        //if(ownsRef) {
        //    _value.ref = ref;
        //    lua_pop(*_mainState, 1);
       // } else {
            _value.ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
        //}
    }

    template<typename L>
    LuaVariantBase LuaVariantBase::_convertType(const L converter) const {
        push();
        converter();
        return fromStack(-1);
    }

    template<typename L>
    LuaVariantBase& LuaVariantBase::_convertType(const L converter) {
        push();
        converter();
        pull();
        return *this;
    }

    inline bool LuaVariantBase::_lightConvertToBool(const LuaVariantBase &source) {
        switch(source._type) {
        case LUA_TBOOLEAN:
            return source._value.boolean;
        case LUA_TNIL:
            return false;
        case LUA_TSTRING:
            return (source == "true")? true : false;
        case LUA_TNUMBER:
            return (source._value.number >= 1)? true : false;
        case LUA_TLIGHTUSERDATA:
            return (source._value.userdata)? true : false;
        default:
            return false;
        }
    }

    inline LuaVariantBase& LuaVariantBase::_convertToBool(const LuaVariantBase &source, LuaVariantBase &dest) {
        switch(source._type) {
        case LUA_TBOOLEAN:
        case LUA_TNIL:
        case LUA_TSTRING:
        case LUA_TNUMBER:
        case LUA_TLIGHTUSERDATA:
            dest = _lightConvertToBool(source);
            break;
        default:
            dest = source._convertType( [](void) { lua_toboolean(*_mainState, -1); } );
        }
        return dest;
    }

    inline lua_Number LuaVariantBase::_lightConvertToNumber(const LuaVariantBase &source) {
        switch(source._type) {
        case LUA_TNUMBER:
            return source._value.number;
        case LUA_TBOOLEAN:
            return source._value.boolean? 1 : 0;
        case LUA_TNIL:
            return 0;
        case LUA_TLIGHTUSERDATA:
            return source._value.userdata? 1 : 0;
        default:
            return 0;
        }
    }

    inline LuaVariantBase& LuaVariantBase::_convertToNumber(const LuaVariantBase &source, LuaVariantBase &dest) {
        switch(source._type) {
        case LUA_TNUMBER:
        case LUA_TBOOLEAN:
        case LUA_TNIL:
        case LUA_TLIGHTUSERDATA:
            dest = _lightConvertToNumber(source);
            break;
        case LUA_TSTRING:
        default:
             dest = source._convertType( [](void) { lua_pushnumber(*_mainState, lua_tonumber(*_mainState, -1)); lua_remove(*_mainState, -2); } )._value.number;
        }
        return dest;
    }


    //same as above, doesn't change actual value
    inline LuaVariantBase LuaVariantBase::asBool(void) const {
        LuaVariantBase variant;
        return (_type == LUA_TBOOLEAN)? *this : _convertToBool(*this, variant);
    }

    inline LuaVariantBase LuaVariantBase::asNumber(void) const {
        LuaVariantBase variant;
        return (_type == LUA_TNUMBER)? *this :
            _convertToNumber(*this, variant);
    }

    inline LuaVariantBase LuaVariantBase::asString(void) const {
        return (_type == LUA_TSTRING)? *this :
            _convertType( [](void) { luaL_tolstring(*_mainState, -1, nullptr); lua_remove(*_mainState, -2); } );
    }

    inline LuaVariantBase LuaVariantBase::asUserdata(void) const {
        if(_type == LUA_TLIGHTUSERDATA) {
            return *this;
        } else {
            LuaVariantBase variant;
            variant._type = LUA_TLIGHTUSERDATA;
            variant._value.userdata = nullptr;
            return variant;
        }

    }

    inline bool LuaVariantBase::toBool(void) {
         return (_type == LUA_TBOOLEAN)? _value.boolean : _convertToBool(*this, *this)._value.boolean;
    }

    inline lua_Number LuaVariantBase::toNumber(void) {
         return (_type == LUA_TNUMBER)? _value.number : _convertToNumber(*this, *this)._value.number;
    }

    inline const char* LuaVariantBase::toString(void) {
        return (_type == LUA_TSTRING)? _value.string :
            _convertType( [](void) { luaL_tolstring(*_mainState, -1, nullptr); lua_remove(*_mainState, -2); } )._value.string;
    }

    inline void* LuaVariantBase::toUserdata(void) {
        if(_type == LUA_TLIGHTUSERDATA) {
            return _value.userdata;
        } else {
            setType(LUA_TLIGHTUSERDATA);
            _value.userdata = nullptr;
            return nullptr;
        }
    }

    inline LuaVariantBase LuaVariantBase::fromStack(const int index) {
        LuaVariantBase variant;
        lua_pushvalue(*_mainState, index);
        lua_remove(*_mainState, (index > 0)? index : index-1);
        variant.pull();
        return variant;
    }


    template <typename... Args>
    inline LuaVariantBase LuaVariantBase::operator()(Args&& ... args) {
        push();
        return LuaManager::_callFunction<LuaVariantBase>(nullptr, args...);
    }


    template <typename Ret, typename... Args>
    void LuaVariantBase::setClosure(std::function<Ret(Args...)> closure) {
        lua_pushlightuserdata(*_mainState, new std::function<Ret(Args...)>(closure));
        lua_CFunction func = (lua_CFunction)&_callLambda<Ret, Args...>;

        lua_pushcclosure(*_mainState, func, 1);
        _type = lua_type(*_mainState, -1);
        _value.ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);

    }

    inline LuaVariantBase LuaVariantBase::getValue(const LuaVariantBase &key) const {
        if(!isIndexable()) { //gracefully return nil variant
            return LuaVariantBase();
        }
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _value.ref);
        key.push();
        lua_gettable(*_mainState, -2);
        lua_remove(*_mainState, -2);
        return LuaVariantBase::fromStack();
    }

    inline LuaFieldRef LuaVariantBase::operator[](const LuaVariantBase& key) const {
        return LuaFieldRef(isIndexable()? _value.ref : LUA_REFNIL, key);
    }

#if 0
    inline void LuaVariantBase::insert(const LuaPair &pair) {
        if(isIndexable()) insert(pair.first, pair.second);
    }

    inline void LuaVariantBase::insert(const LuaVariantBase &key, const LuaVariantBase &value) {
        if(isIndexable()) {
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _value.ref);
            OOLUA::push(*_mainState, key);
            OOLUA::push(*_mainState, value);
            lua_settable(*_mainState, -3);
            lua_pop(*_mainState, 1);
        }
    }

    inline void LuaVariantBase::insert(const char * const key, const LuaVariantBase &value) {
        if(isIndexable()) {
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _value.ref);
            OOLUA::push(*_mainState, value);
            lua_setfield(*_mainState, -2, key);
            lua_pop(*_mainState, 1);
        }
    }
#endif

    inline LuaVariantBase LuaVariantBase::createFromString(const char * const str) {
        LuaVariantBase variant;
        if(LuaManager::runString(str)) {
            variant.pull();
        }
        return variant;
    }
#if 0
    inline LuaVariantBase::iterator LuaVariantBase::begin(void) {
        const auto& it = cbegin();
        return iterator(this,
                        it._nextFuncRef,
                        it._pair);
    }

    inline LuaVariantBase::const_iterator LuaVariantBase::begin(void) const {

        getGlobal("pairs");
      //  lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, REF);
        _protectedCall(1, 3, nullptr);

        lua_pushvalue(*_mainState, -3);
        const int funcRef = luaL_ref(*_mainState, -1);

        _protectedCall(2, 2, nullptr);

        LuaPair pair;
        OOLUA::pull(*_mainState, pair);
        return const_iterator(this, funcRef, pair);
    }

    inline LuaVariantBase::const_iterator LuaVariantBase::cbegin(void) const {
        return begin();
    }


    inline LuaVariantBase::iterator LuaVariantBase::end(void) {
        return iterator(this, LUA_NOREF, LuaPair());
    }


    inline LuaVariantBase::const_iterator LuaVariantBase::end(void) const {
        return const_iterator(this, LUA_NOREF, LuaPair());
    }

    inline LuaVariantBase::const_iterator LuaVariantBase::cend(void) const {
        return const_iterator(this, LUA_NOREF, LuaPair());
    }
#endif

}

namespace OOLUA {

    template<>
    inline bool push<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase const& variant) {
        switch(variant.getType()) {
        case LUA_TNIL:
            lua_pushnil(state);
            break;
        case LUA_TBOOLEAN:
            lua_pushboolean(state, variant.getValue<bool>());
            break;
        case LUA_TNUMBER:
            lua_pushnumber(state, variant.getValue<float>());
            break;
        case LUA_TSTRING:
            lua_pushstring(state, variant.getValue<const char*>());
            break;
        case LUA_TLIGHTUSERDATA:
            lua_pushlightuserdata(state, variant.getValue<void*>());
            break;
        case LUA_TTABLE:
        case LUA_TUSERDATA:
        case LUA_TFUNCTION:
        case LUA_TTHREAD:
            lua_rawgeti(state, LUA_REGISTRYINDEX, variant.getRef());
            break;
        }
        return true;
    }

    template<>
    inline bool pull<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase& variant) {
        if(!lua_gettop(state)) {
            variant.setNil();
            return false;
        }

        const int type = lua_type(state, -1);

        switch(type) {
        case LUA_TNIL:
            variant.setNil();
            break;
        case LUA_TBOOLEAN:
            variant.setValue((bool)lua_toboolean(state, -1));
            break;
        case LUA_TNUMBER:
            variant.setValue((float)lua_tonumber(state, -1));
            break;
        case LUA_TSTRING:
            variant.setValue((const char*)lua_tostring(state, -1));
            break;
        case LUA_TLIGHTUSERDATA:
            variant.setValue((void*)lua_topointer(state, -1));
            break;
        case LUA_TTABLE:
        case LUA_TUSERDATA:
        case LUA_TFUNCTION:
        case LUA_TTHREAD:
            variant.setRef(luaL_ref(state, LUA_REGISTRYINDEX));
            return true;
        }

        lua_pop(state, 1);
        return true;
    }

    template<>
    inline bool push<elysian::LuaPair>(lua_State* state, elysian::LuaPair const& pair) {
        bool success = true;
        success &= OOLUA::push(state, std::get<0>(pair));
        success &= OOLUA::push(state, std::get<1>(pair));
        return success;
    }

    template<>
    inline bool pull<elysian::LuaPair>(lua_State* state, elysian::LuaPair& pair) {
        bool success = true;
        success &= OOLUA::pull(state, std::get<1>(pair));
        success &= OOLUA::pull(state, std::get<0>(pair));
        return success;
    }


}

namespace elysian {

    inline bool LuaVariantBase::push(void) const {
        return OOLUA::push(*_mainState, *this);
    }

    inline bool LuaVariantBase::pull(void) {
        return OOLUA::pull(*_mainState, *this);
    }

}



#endif // ELYSIAN_LUA_TABLE_VARIANT_HPP

