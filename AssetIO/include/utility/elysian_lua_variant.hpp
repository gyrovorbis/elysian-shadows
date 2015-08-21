#ifndef ELYSIAN_LUA_VARIANT_HPP
#define ELYSIAN_LUA_VARIANT_HPP

#include <crtp/elysian_lua_manager.hpp>

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

    class LuaTableRef;

    //add function operator
    //support C functions/closures and Lua functions


    //- support integers as numbers
    //support conversions
    //support calling arbitrary functions
    //support storing arbitrary C functions/closures/lambdas in Lua (use upvalues for stateful storage)
    //- support table references


    class LuaVariant: public LuaManager {
    protected:
        uint32_t                _type       = LUA_TNIL; //thas a lot of bits .. Type only needs 4, rest can be internal state data
        union Union{
            bool                boolean;
            lua_Number          number;
            int                 ref         = LUA_REFNIL;
            const char*         string;
            void*               userdata;
        } _value;

        template<typename L>
        LuaVariant              _convertType(const L converter) const;
        template<typename L>
        LuaVariant&             _convertType(const L converter);

        static bool             _lightConvertToBool(const LuaVariant& source);
        static LuaVariant&      _convertToBool(const LuaVariant& source, LuaVariant& dest);
        static lua_Number       _lightConvertToNumber(const LuaVariant& source);
        static LuaVariant&      _convertToNumber(const LuaVariant& source, LuaVariant& dest);


        template <unsigned... Indices, class... Args, class Ret>
        static Ret forward_impl(index_tuple<Indices...>,
                         std::tuple<Args...> tuple,
                         const std::function<Ret(Args...)>& closure)
        {
          return closure(std::get<Indices>(tuple)...);
        }

        template<class... Args, class Ret>
        static Ret forward(std::tuple<Args...> tuple, const std::function<Ret(Args...)>& closure)
        {
           typedef typename make_indices<Args...>::type Indices;
           return forward_impl(Indices(), tuple, closure);
        }

        template<std::size_t> struct int_{};

        template <class Tuple, size_t Pos>
        static void pull_tuple(const Tuple& t, int_<Pos> ) {
          OOLUA::pull(*_mainState, std::get< std::tuple_size<Tuple>::value-Pos >(t));
          return pull_tuple(t, int_<Pos-1>());
        }

        template <class Tuple>
        static void pull_tuple(const Tuple& t, int_<1> ) {
          OOLUA::pull(*_mainState, std::get<std::tuple_size<Tuple>::value-1>(t));
        }


        //Assumes actual function is already at the top of the stack!
        template<typename... Args, typename Ret>
        static int _callLambda(lua_State*const vm) {
            std::tuple<Args...> argTuple;

            pull_tuple(argTuple, int_<sizeof...(Args)>());

            //MEMORY LEAK!!!!!
            //change this to full-userdata, set a metatable with a _gc metamethod so this context can be deleted...
            std::function<Ret (Args...)>* closure = lua_topointer(vm, lua_upvalueindex(1));

            const Ret retVal = forward(argTuple, closure);

            OOLUA::push(*_mainState, retVal);
            return 1;
        }



    public:

                                LuaVariant(void) = default;
                                LuaVariant(std::nullptr_t null);
                                LuaVariant(const LuaVariant& rhs);
                                LuaVariant(LuaVariant&& rhs);
                                LuaVariant(const int tableRef);
                                LuaVariant(const float number);
                                LuaVariant(const double number);
                                LuaVariant(const bool val);
                                LuaVariant(const char*const string);
                                LuaVariant(void*const userdata);
                                ~LuaVariant(void);

        LuaVariant&             operator=(const LuaVariant& rhs);
        LuaVariant&             operator=(LuaVariant&& rhs);
        LuaVariant&             operator=(const std::nullptr_t);
        LuaVariant&             operator=(const bool val);
        LuaVariant&             operator=(const int num);
        LuaVariant&             operator=(const float num);
        LuaVariant&             operator=(const double num);
        LuaVariant&             operator=(const char*const string);
        LuaVariant&             operator=(void*const ptr);


        bool                    operator==(const LuaVariant& rhs) const;
        bool                    operator==(const std::nullptr_t) const;
        bool                    operator==(const bool val) const;
        bool                    operator==(const int num) const;
        bool                    operator==(const float num) const;
        bool                    operator==(const double num) const;
        bool                    operator==(void*const ptr) const;
        bool                    operator==(const char*const str) const;

        bool                    operator!=(const LuaVariant& rhs) const;
        bool                    operator!=(const std::nullptr_t) const;
        bool                    operator!=(const bool val) const;
        bool                    operator!=(const int num) const;
        bool                    operator!=(const float num) const;
        bool                    operator!=(const double num) const;
        bool                    operator!=(void*const ptr) const;
        bool                    operator!=(const char*const str) const;


        template <typename... Args, typename Ret>
        Ret                     operator()(Args&& ... args);

        template <typename... Args, typename Ret>
        void                    setClosure(std::function<Ret(Args...)> closure);





        //does not attempt conversion, so you better know what the fuck type you are using at compile-time
                                operator bool(void) const;
                                operator int(void) const;
                                operator float(void) const;
                                operator double(void) const;
                                operator const char*(void) const;
                                operator void*(void) const;

        //returns new QVariant with the desired type
        LuaVariant              asBool(void) const;
        LuaVariant              asNumber(void) const;
        LuaVariant              asString(void) const;
        LuaVariant              asUserdata(void) const;

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
        void                    setRef(const int ref, const bool ownsRef=false);

        //template<typename T>
        //void                    setValue(T*const ud);
        void                    setValue(const std::nullptr_t null);
        void                    setValue(const int ref);
        void                    setValue(const float number);
        void                    setValue(const double number);
        void                    setValue(const char*const str);
        void                    setValue(const bool val);
        void                    setValue(void*const ptr);
        void                    setValue(const LuaTableRef*const ref);
        void                    setValue(const OOLUA::Lua_table_ref& ref);



        bool                    isNil(void) const;
        bool                    isNilRef(void) const;
        bool                    isRefType(void) const;
        bool                    isValueType(void) const;
        bool                    isUserdataType(void) const;
        int                     getType(void) const;
        const char*             getTypeString(void) const;
        void                    setType(const int luaType);

        bool                    push(void) const;
        bool                    pull(void);

        static LuaVariant       fromStack(const int index=-1);

    };

    using LuaPair = std::pair<LuaVariant, LuaVariant>;

    inline LuaVariant::~LuaVariant(void) { setNil(); }
    inline LuaVariant::LuaVariant(const LuaVariant& rhs) { *this = rhs; }
    inline LuaVariant::LuaVariant(std::nullptr_t /*null*/): LuaVariant() {}
    inline LuaVariant::LuaVariant(LuaVariant&& rhs) { *this = rhs; }
    inline LuaVariant& LuaVariant::operator=(const LuaVariant& rhs) {
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
            _type = rhs._type;
            _value = rhs._value;

        }
        return *this;
    }
    inline LuaVariant::LuaVariant(const char * const string) { setValue(string); }
    inline LuaVariant::LuaVariant(const bool val) { setValue(val); }
    inline LuaVariant::LuaVariant(const float number) { setValue(number); }
    inline LuaVariant::LuaVariant(const double number) { setValue(number); }
    inline LuaVariant::LuaVariant(const int num) { setValue(num); }
    inline LuaVariant::LuaVariant(void*const ptr) { setValue(ptr); }

    inline LuaVariant& LuaVariant::operator=(LuaVariant&& rhs) {
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
            _type = LUA_TSTRING;
            _value.string = rhs._value.string;
            rhs._type = LUA_TNIL;
            rhs._value.ref = LUA_REFNIL;
            break;
        case LUA_TLIGHTUSERDATA:
            setValue(rhs._value.userdata);
            break;
        default:
            _type = rhs._type;
            _value = rhs._value;

        }
        return *this;
    }
    inline LuaVariant& LuaVariant::operator=(const bool val) { setValue(val); return *this; }
    inline LuaVariant& LuaVariant::operator=(const std::nullptr_t) { setValue(nullptr); return *this; }
    inline LuaVariant& LuaVariant::operator=(const int num) { setValue(num); return *this; }
    inline LuaVariant& LuaVariant::operator=(const float num) { setValue((lua_Number)num); return *this; }
    inline LuaVariant& LuaVariant::operator=(const double num) { setValue((lua_Number)num); return *this; }
    inline LuaVariant& LuaVariant::operator=(const char*const string) { setValue(string); return *this; }
    inline LuaVariant& LuaVariant::operator=(void*const ptr) { setValue(ptr); return *this; }

    inline bool LuaVariant::operator==(const LuaVariant& rhs) const {
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

    inline bool LuaVariant::operator==(const std::nullptr_t) const { return isNil() || (_type == LUA_TLIGHTUSERDATA && _value.userdata == nullptr); }
    inline bool LuaVariant::operator==(const bool val) const { return _type == LUA_TBOOLEAN && _value.boolean == val; }
    inline bool LuaVariant::operator==(const int num) const { return _type == LUA_TNUMBER && (int)_value.number == (int)num; }
    inline bool LuaVariant::operator==(const float num) const { return _type == LUA_TNUMBER && _value.number == (lua_Number)num; }
    inline bool LuaVariant::operator==(const double num) const { return _type == LUA_TNUMBER && _value.number == (lua_Number)num; }
    inline bool LuaVariant::operator==(void*const ptr) const { return ptr? isUserdataType() && _value.userdata == ptr : isNil(); }
    inline bool LuaVariant::operator==(const char*const str) const { return (_type == LUA_TSTRING && (strcmp(_value.string, str) == 0)); };

    inline bool LuaVariant::operator!=(const LuaVariant& rhs) const { return !(*this == rhs); }
    inline bool LuaVariant::operator!=(const std::nullptr_t) const { return !(*this == nullptr); }
    inline bool LuaVariant::operator!=(const bool val) const { return !(*this == val); }
    inline bool LuaVariant::operator!=(const int num) const { return !(*this == num); }
    inline bool LuaVariant::operator!=(const float num) const { return !(*this == num); }
    inline bool LuaVariant::operator!=(const double num) const { return !(*this == num); }
    inline bool LuaVariant::operator!=(void*const ptr) const { return !(*this == ptr); }
    inline bool LuaVariant::operator!=(const char*const str) const { return !(*this == str); }

    inline LuaVariant::operator bool(void) const { return _lightConvertToBool(*this); }
    inline LuaVariant::operator int(void) const { return _lightConvertToNumber(*this); }
    inline LuaVariant::operator float(void) const { return _lightConvertToNumber(*this); }
    inline LuaVariant::operator double(void) const { return _lightConvertToNumber(*this); }
    inline LuaVariant::operator const char*(void) const { return _type == LUA_TSTRING? _value.string : lua_typename(*_mainState, _type); }
    inline LuaVariant::operator void*(void) const { return _type == LUA_TLIGHTUSERDATA? _value.userdata : nullptr; }

    //inline LuaVariant::operator LuaTableRef(void) const;
    //inline LuaVariant::operator OOLUA::Lua_table_ref(void);
    inline bool LuaVariant::isNil(void) const { return _type == LUA_TNIL; }
    inline bool LuaVariant::isNilRef(void) const { return _type == LUA_TNIL || _value.ref == LUA_REFNIL; }
    inline bool LuaVariant::isRefType(void) const { return !(_type == LUA_TBOOLEAN || _type == LUA_TNUMBER || _type == LUA_TNIL || _type == LUA_TLIGHTUSERDATA|| _type == LUA_TSTRING); }
    inline bool LuaVariant::isValueType(void) const { return _type == LUA_TNIL || !isRefType(); }
    inline bool LuaVariant::isUserdataType(void) const { return _type == LUA_TUSERDATA || _type == LUA_TLIGHTUSERDATA; }

    inline void LuaVariant::setType(const int luaType) {
        if(isRefType()) {
            luaL_unref(*_mainState, LUA_REGISTRYINDEX, _value.ref);
        } else if(_type == LUA_TSTRING) {
            free((void*)_value.string);
        } else if(_type == LUA_TNIL) {
            _value.ref = LUA_REFNIL;
        }
        _type = luaType;
    }

    inline void LuaVariant::setNil(void) {
        setType(LUA_TNIL);
        _value.ref = LUA_REFNIL;
    }
/*
    template<typename T>
    inline void LuaVariant::setValue(T*const ud) {
        OOLUA::push(*_mainState, ud);
        _type = lua_type(*_mainState, -1);
        _value.ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    }
*/
    inline void LuaVariant::setValue(std::nullptr_t /*null*/) {
        setNil();
    }

    inline void LuaVariant::setValue(const int number) {
        setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariant::setValue(const float number) {
        setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariant::setValue(const double number) {
        setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariant::setValue(const char*const str) {
        if(!str) setValue(nullptr);

        const int size = std::strlen(str);

        if(_type != LUA_TSTRING) {
            setType(LUA_TSTRING);
            _value.string = (const char*)malloc(sizeof(size)+1);
        } else {
            _value.string = (const char*)realloc((void*)_value.string, sizeof(size)+1);
        }

        std::strcpy((char*)_value.string, str);
    }

    inline void LuaVariant::setValue(const bool val) {
        setType(LUA_TBOOLEAN);
        _value.boolean = val;
    }

    inline void LuaVariant::setValue(void*const ptr) {
        if(ptr) {
            setType(LUA_TLIGHTUSERDATA);
            _value.userdata = ptr;
        } else setType(LUA_TNIL);
    }

    template<>
    inline  bool LuaVariant::getValue<bool>(void) const { return _value.boolean; }

    template<>
    inline  float LuaVariant::getValue<float>(void) const { return _value.number; }

    template<>
    inline double LuaVariant::getValue<double>(void) const { return _value.number; }

    template<>
    inline  const char* LuaVariant::getValue<const char*>(void) const { return _value.string; }

    template<>
    inline  int LuaVariant::getValue<int>(void) const { return _value.number; }

    template<>
    inline  void* LuaVariant::getValue<void*>(void) const { return _value.userdata; }

    inline  int LuaVariant::getType(void) const { return _type; }

    inline const char* LuaVariant::getTypeString(void) const {
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

    inline int LuaVariant::getRef(void) const {
        return isRefType()? _value.ref : LUA_NOREF;
    }

    inline void LuaVariant::setRef(const int ref, const bool ownsRef) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
        setType(lua_type(*_mainState, -1));
        if(ownsRef) {
            _value.ref = ref;
            lua_pop(*_mainState, 1);
        } else {
            _value.ref = luaL_ref(*_mainState, -1);
        }
    }

    template<typename L>
    LuaVariant LuaVariant::_convertType(const L converter) const {
        push();
        converter();
        return fromStack(-1);
    }

    template<typename L>
    LuaVariant& LuaVariant::_convertType(const L converter) {
        push();
        converter();
        pull();
        return *this;
    }

    inline bool LuaVariant::_lightConvertToBool(const LuaVariant &source) {
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

    inline LuaVariant& LuaVariant::_convertToBool(const LuaVariant &source, LuaVariant &dest) {
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

    inline lua_Number LuaVariant::_lightConvertToNumber(const LuaVariant &source) {
        switch(source._type) {
        case LUA_TNUMBER:
            return source._value.number;
        case LUA_TBOOLEAN:
            return source._value.boolean? 1 : 0;
        case LUA_TNIL:
            return 0;
        case LUA_TLIGHTUSERDATA:
            return source._value.userdata? 1 : 0;
        }
    }

    inline LuaVariant& LuaVariant::_convertToNumber(const LuaVariant &source, LuaVariant &dest) {
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
    inline LuaVariant LuaVariant::asBool(void) const {
        LuaVariant variant;
        return (_type == LUA_TBOOLEAN)? *this : _convertToBool(*this, variant);
    }

    inline LuaVariant LuaVariant::asNumber(void) const {
        LuaVariant variant;
        return (_type == LUA_TNUMBER)? *this :
            _convertToNumber(*this, variant);
    }

    inline LuaVariant LuaVariant::asString(void) const {
        return (_type == LUA_TSTRING)? *this :
            _convertType( [](void) { luaL_tolstring(*_mainState, -1, nullptr); lua_remove(*_mainState, -2); } );
    }

    inline LuaVariant LuaVariant::asUserdata(void) const {
        if(_type == LUA_TLIGHTUSERDATA) {
            return *this;
        } else {
            LuaVariant variant;
            variant._type = LUA_TLIGHTUSERDATA;
            variant._value.userdata = nullptr;
            return variant;
        }

    }

    inline bool LuaVariant::toBool(void) {
         return (_type == LUA_TBOOLEAN)? _value.boolean : _convertToBool(*this, *this)._value.boolean;
    }

    inline lua_Number LuaVariant::toNumber(void) {
         return (_type == LUA_TNUMBER)? _value.number : _convertToNumber(*this, *this)._value.number;
    }

    inline const char* LuaVariant::toString(void) {
        return (_type == LUA_TSTRING)? _value.string :
            _convertType( [](void) { luaL_tolstring(*_mainState, -1, nullptr); lua_remove(*_mainState, -2); } )._value.string;
    }

    inline void* LuaVariant::toUserdata(void) {
        if(_type == LUA_TLIGHTUSERDATA) {
            return _value.userdata;
        } else {
            setType(LUA_TLIGHTUSERDATA);
            _value.userdata = nullptr;
            return nullptr;
        }
    }

    inline LuaVariant LuaVariant::fromStack(const int index) {
        LuaVariant variant;
        lua_pushvalue(*_mainState, index);
        variant.pull();
        lua_remove(*_mainState, index);
        return variant;
    }


    template <typename... Args, typename Ret>
    inline Ret LuaVariant::operator()(Args&& ... args) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _value.ref);
        return LuaManager::_callFunction<Ret>(nullptr, args...);
    }




    template <typename... Args, typename Ret>
    void LuaVariant::setClosure(std::function<Ret(Args...)> closure) {
        lua_pushlightuserdata(*_mainState, new std::function<Ret(Args...)>(closure));
        lua_pushcclosure(*_mainState, &_callLambda<Args..., Ret>, 1);
        _type = lua_type(*_mainState, -1);
        _value.ref = luaL_ref(*_mainState, -1);

    }









}

namespace OOLUA {

    template<>
    inline bool push<elysian::LuaVariant>(lua_State* state, elysian::LuaVariant const& variant) {
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
    inline bool pull<elysian::LuaVariant>(lua_State* state, elysian::LuaVariant& variant) {
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
            variant.setValue((int)luaL_ref(state, -1));
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

    inline bool LuaVariant::push(void) const {
        return OOLUA::push(*_mainState, *this);
    }

    inline bool LuaVariant::pull(void) {
        return OOLUA::pull(*_mainState, *this);
    }

}



#endif // ELYSIAN_LUA_TABLE_VARIANT_HPP

