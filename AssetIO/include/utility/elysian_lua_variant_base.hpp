#ifndef ELYSIAN_LUA_VARIANT_BASE_HPP
#define ELYSIAN_LUA_VARIANT_BASE_HPP

#include <initializer_list>
#include <tuple>
#include <cstdlib>
#include <crtp/elysian_lua_manager.hpp>
#include <utility/elysian_functional.hpp>

#define ELYSIAN_LUA_VARIANT_DEBUG               ELYSIAN_LUA_DEBUG

#define ELYSIAN_LUA_VARIANT_TYPE_BIT_POS        0
#define ELYSIAN_LUA_VARIANT_TYPE_BIT_MASK       0x0f
#define ELYSIAN_LUA_VARIANT_WEAK_REF_BIT_POS    31



//Function-type constructor, get?, set, assignment operator
//overloaded stream operators
//check for memory leaks
//pass some shit internally by const reference
//are anonyous functions' lightuserdata std::function void* ptrs being deleted correctly?


//other comparison operators >, <, >=, <=
//mathematical operators? Stream operators? Postincrement, preincrement?
//Fix iteration mechanism
//doesn't handle unsigned integers

namespace elysian {
    template<bool is_const_iterator>
    class const_noconst_iterator;

    class LuaFieldRef;
    class LuaVariantBase;
    using LuaPair = std::pair<LuaVariantBase, LuaVariantBase>;

    //add function operator
    //support C functions/closures and Lua functions


    //- support integers as numbers
    //support conversions
    //support calling arbitrary functions
    //support storing arbitrary C functions/closures/lambdas in Lua (use upvalues for stateful storage)
    //- support table references

    class LuaVariantBase: public LuaManager {
        using iterator       =      const_noconst_iterator<false>;
        using const_iterator =      const_noconst_iterator<true>;

        friend bool                 OOLUA::push<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase const& variant);
        friend bool                 OOLUA::pull<elysian::LuaVariantBase>(lua_State* state, elysian::LuaVariantBase& variant);
        friend                      iterator;
        friend                      const_iterator;

    protected:
        uint32_t                    _type       = LUA_TNIL; //only least significant 4 bits represent LUA_TYPE, rest is metadata
        union Union {
            bool                    boolean;
            lua_Number              number;
            int                     ref         = LUA_REFNIL;
            const char*             string;
            void*                   luserdata;
        } _value;

        template<typename L>
        LuaVariantBase              _convertType(const L converter) const;
        template<typename L>
        LuaVariantBase&             _convertType(const L converter);

        static bool                 _lightConvertToBool(const LuaVariantBase& source);
        static LuaVariantBase&      _convertToBool(const LuaVariantBase& source, LuaVariantBase& dest);
        static lua_Number           _lightConvertToNumber(const LuaVariantBase& source);
        static LuaVariantBase&      _convertToNumber(const LuaVariantBase& source, LuaVariantBase& dest);

        void                        _setWeakRef(const bool val);
        void                        _setType(const int luaType);
        void                        _setRefDirect(const int luaRef);



    public:


                                    LuaVariantBase(void) = default;
                                    LuaVariantBase(const std::initializer_list<LuaPair>& pairs);
        template<typename T>
                                    LuaVariantBase(T*const userdata);
                                    LuaVariantBase(std::nullptr_t null);
                                    LuaVariantBase(const LuaVariantBase& rhs);
                                    LuaVariantBase(LuaVariantBase&& rhs);
                                    LuaVariantBase(const int number);
                                    LuaVariantBase(const unsigned number);
                                    LuaVariantBase(const float number);
                                    LuaVariantBase(const double number);
                                    LuaVariantBase(const bool val);
                                    LuaVariantBase(const char*const string);
                                    LuaVariantBase(void*const usd);
                                    LuaVariantBase(const lua_CFunction funcPtr);
                                    LuaVariantBase(const OOLUA::Lua_table_ref& ref);
                                    LuaVariantBase(const OOLUA::Lua_func_ref& ref);
        template <typename Ret, typename... Args>
                                    LuaVariantBase(const std::function<Ret(Args...)>& closure);
        template<typename T>
                                    LuaVariantBase(const T fun);


                                    ~LuaVariantBase(void);

        LuaVariantBase&             operator=(const std::initializer_list<LuaPair>& pairs);
        template<typename T>
        LuaVariantBase&             operator=(const T*const ud);
        template<typename T>
        LuaVariantBase&             operator=(T*const ud);
        LuaVariantBase&             operator=(const LuaVariantBase& rhs);
        LuaVariantBase&             operator=(LuaVariantBase&& rhs);
        LuaVariantBase&             operator=(const std::nullptr_t);
        LuaVariantBase&             operator=(const bool val);
        LuaVariantBase&             operator=(const int num);
        LuaVariantBase&             operator=(const float num);
        LuaVariantBase&             operator=(const double num);
        LuaVariantBase&             operator=(const char*const string);
        LuaVariantBase&             operator=(void*const ptr);
        LuaVariantBase&             operator=(const lua_CFunction funcPtr);
        LuaVariantBase&             operator=(const OOLUA::Lua_func_ref& ref);
        LuaVariantBase&             operator=(const OOLUA::Lua_table_ref& ref);
        template <typename Ret, typename... Args>
        LuaVariantBase&             operator=(const std::function<Ret(Args...)>& closure);
        template<typename T>
        LuaVariantBase&             operator=(const T func);


        template <typename T>
        bool                        operator==(const T*const ud) const;
        bool                        operator==(const LuaVariantBase& rhs) const;
        bool                        operator==(const std::nullptr_t) const;
        bool                        operator==(const bool val) const;
        bool                        operator==(const int num) const;
        bool                        operator==(const float num) const;
        bool                        operator==(const double num) const;
        bool                        operator==(void*const ptr) const;
        bool                        operator==(const char*const str) const;

        template<typename T>
        bool                        operator!=(const T*const ud) const;
        bool                        operator!=(const LuaVariantBase& rhs) const;
        bool                        operator!=(const std::nullptr_t) const;
        bool                        operator!=(const bool val) const;
        bool                        operator!=(const int num) const;
        bool                        operator!=(const float num) const;
        bool                        operator!=(const double num) const;
        bool                        operator!=(void*const ptr) const;
        bool                        operator!=(const char*const str) const;


        template <typename... Args>
        LuaVariantBase              operator()(Args&&... args) const;

                                    operator OOLUA::Lua_table_ref(void) const;

        //returns new QVariant with the desired type
        LuaVariantBase              asBool(void) const;
        LuaVariantBase              asNumber(void) const;
        LuaVariantBase              asString(void) const;
        LuaVariantBase              asUserdata(void) const;


        //doesn't do conversions, so type better be known statically
        template<typename T>
        T                           getValue(void) const;
        int                         getRef(void) const;


        void                        setNil(void);
        void                        setRef(const int ref);

        template<typename T>
        void                        setValue(T*const ud);
        void                        setValue(const std::nullptr_t null);
        void                        setValue(const int number);
        void                        setValue(const unsigned number);
        void                        setValue(const float number);
        void                        setValue(const double number);
        void                        setValue(const char*const str);
        void                        setValue(const bool val);
        void                        setValue(void*const ptr);
        void                        setValue(const OOLUA::Lua_table_ref& ooluaRef);
        void                        setValue(const OOLUA::Lua_func_ref& ooluaRef);
        void                        setValue(const lua_CFunction funcPtr);
        template <typename Ret, typename... Args>
        void                        setValue(const std::function<Ret(Args...)>& closure);
        template<typename T>
        void                        setValue(const T func);
        void                        setValue(const std::initializer_list<LuaPair>& pairs);

        bool                        isWeakRef(void) const;

        bool                        isNil(void) const;
        bool                        isNilRef(void) const;
        bool                        isRefType(void) const;
        bool                        isIndexable(void) const;
        bool                        isValueType(void) const;
        bool                        isUserdataType(void) const;
        int                         getType(void) const;
        const char*                 getTypeString(void) const;
        void                        setMetatable(const LuaVariantBase& mt) const;
        LuaVariant                  getMetatable(void) const;
        int                         getLength(void) const;

        bool                        push(void) const;
        bool                        pull(void);

        LuaVariantBase              getValue(const LuaVariantBase& key) const;
        LuaFieldRef                 operator[](const LuaVariantBase& key) const;


        //C++11-style iterators
        iterator                    begin(void);
        const_iterator              begin(void) const;
        const_iterator              cbegin(void) const;
        iterator                    end(void);
        const_iterator              end(void) const;
        const_iterator              cend(void) const;


        //static constructors
        static LuaVariantBase       fromStack(const int index=-1, const bool remove=true);
        static LuaVariantBase       fromRef(const int luaRef);
        static LuaVariantBase       fromWeakRef(const int ref);
        static LuaVariantBase       fromArray(const std::initializer_list<LuaVariant>& array);
        template<typename C>
        static LuaVariantBase       fromSequenceContainer(const C& cont);
        static LuaVariantBase       makeWeakRef(const LuaVariant& variant);
        static LuaVariantBase       newTable(void);

    };

    class LuaFieldRef: public LuaManager {
        friend class                LuaVariantBase;
        friend class                const_noconst_iterator<false>;
        friend class                const_noconst_iterator<true>;
    private:
        LuaVariantBase              _tableRef;
        LuaVariantBase              _key;
        mutable LuaVariantBase      _cachedValue;

    public:

                                    LuaFieldRef(LuaVariantBase tableRef, LuaVariantBase key, LuaVariantBase val=LuaVariantBase());
                                    LuaFieldRef(const LuaFieldRef& rhs);
                                    LuaFieldRef(LuaFieldRef&& rhs);

        const LuaVariantBase&       getVariant(void) const;
        const LuaVariantBase&       getCachedVariant(void) const;
        void                        setCachedVariant(const LuaVariantBase& variant);
        void                        setVariant(const LuaVariantBase& variant);
        const LuaVariantBase&       getKey(void) const;
        void                        setKey(const LuaVariantBase& key);
        const LuaVariantBase&       getTableRef(void) const;
        void                        setTableRef(const LuaVariantBase& ref);


        LuaFieldRef                 operator[](const LuaVariantBase& key) const;

        LuaFieldRef&                operator=(const LuaFieldRef& rhs);
        LuaFieldRef&                operator=(LuaFieldRef&& rhs);
        LuaFieldRef&                operator=(const LuaVariantBase& rhs);
        const LuaVariantBase*       operator->(void) const;
        const LuaVariantBase&       operator*(void) const;

        bool                        operator==(const LuaFieldRef& rhs) const;
                                    operator LuaVariantBase(void) const;
    };



    template<bool is_const_iterator=true>
    class const_noconst_iterator : public std::iterator<std::forward_iterator_tag, LuaVariantBase>, public LuaManager {
        friend class const_noconst_iterator<true>;
        friend class LuaVariantBase;

        typedef typename std::conditional<is_const_iterator, const LuaVariantBase*, LuaVariantBase*>::type DataStructurePointerType;
        typedef typename std::conditional<is_const_iterator, const LuaVariantBase&, LuaVariantBase&>::type ValueReferenceType;
public:
        const_noconst_iterator(const LuaFieldRef& fieldRef, const LuaVariant& funcRef):
            _fieldRef(fieldRef),
            _nextFuncRef(funcRef)
        {}

        /**
        * Copy constructor. Allows for implicit conversion from a regular iterator to a const_iterator
        */
        const_noconst_iterator(const const_noconst_iterator<false>& other):
            _fieldRef(other._fieldRef),
            _nextFuncRef(other._nextFuncRef)
        {}

        /**
        * Equals comparison operator
        */
        bool operator== (const const_noconst_iterator& other) const {
            return _fieldRef == other._fieldRef;
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
        LuaFieldRef& operator*(void) {
            return _fieldRef;
        }

        LuaFieldRef* operator->(void) {
            return &_fieldRef;
        }

        LuaVariantBase first(void) const {
            return _fieldRef.getKey();
        }

        LuaVariantBase second(void) const {
            return _fieldRef.getCachedVariant();
        }

        const_noconst_iterator operator++(void);
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
        LuaFieldRef                 _fieldRef;
        LuaVariant                  _nextFuncRef;
    };




        // ======== INLINEZ ==========

    inline LuaFieldRef::LuaFieldRef(LuaVariantBase tableRef, LuaVariantBase key, LuaVariantBase value):
        _tableRef(std::move(tableRef)),
        _key(std::move(key)),
        _cachedValue(std::move(value))
    {}

    inline LuaFieldRef::LuaFieldRef(const LuaFieldRef &rhs) { *this = rhs; }

    inline LuaFieldRef::LuaFieldRef(LuaFieldRef &&rhs) { *this = rhs; }


    inline LuaFieldRef& LuaFieldRef::operator=(const LuaFieldRef& rhs) {
        _tableRef       = rhs._tableRef;
        _key            = rhs._key;
        _cachedValue    = rhs._cachedValue;
        return *this;
    }

    inline LuaFieldRef& LuaFieldRef::operator=(LuaFieldRef&& rhs) {
        _tableRef       = std::move(rhs._tableRef);
        _key            = std::move(rhs._key);
        _cachedValue    = std::move(rhs._cachedValue);
        return *this;
    }

    inline LuaFieldRef& LuaFieldRef::operator=(const LuaVariantBase& rhs) {
        setVariant(rhs);
        return *this;
    }

    inline const LuaVariantBase* LuaFieldRef::operator->(void) const {
        getVariant();
        return &_cachedValue;
    }

    inline const LuaVariantBase& LuaFieldRef::operator*(void) const {
        getVariant();
        return _cachedValue;
    }

    inline bool LuaFieldRef::operator==(const LuaFieldRef& rhs) const {
        return _tableRef == rhs._tableRef && _key == rhs._key;
    }

    inline const LuaVariantBase& LuaFieldRef::getKey(void) const {
        return _key;
    }

    inline void LuaFieldRef::setKey(const LuaVariantBase& key) {
        _key = key;
    }

    inline const LuaVariantBase& LuaFieldRef::getTableRef(void) const {
        return _tableRef;
    }

    inline void LuaFieldRef::setTableRef(const LuaVariantBase& ref) {
        _tableRef = ref;
    }

    inline const LuaVariantBase& LuaFieldRef::getCachedVariant(void) const {
        return _cachedValue;
    }

    inline void LuaFieldRef::setCachedVariant(const LuaVariantBase& variant) {
        _cachedValue = variant;
    }

    inline const LuaVariantBase& LuaFieldRef::getVariant(void) const {
        bool success = true;
        if(!_tableRef.isNil()) {
            success &= _tableRef.push();
            success &= _key.push();
            GY_ASSERT(success);
            lua_gettable(*_mainState, -2);
            lua_remove(*_mainState, -2);
            success &= _cachedValue.pull();
        }
        return _cachedValue;
    }

    inline void LuaFieldRef::setVariant(const LuaVariantBase& variant) {
        bool success = true;
        success &= _tableRef.push();
        success &= _key.push();
        success &= variant.push();
        GY_ASSERT(success);
        lua_settable(*_mainState, -3);
        lua_pop(*_mainState, 1);
    }


    inline LuaFieldRef LuaFieldRef::operator[](const LuaVariantBase& key) const {
        //forward through the variant's index operator
        return getVariant()[key];
    }












    template<typename T>
    inline LuaVariantBase::LuaVariantBase(T*const ref) {
        if(ref) {
            OOLUA::push(*_mainState, ref);
            pull();
        } else {
            _setType(LUA_TNIL);
        }
    }

    inline LuaVariantBase::LuaVariantBase(const std::initializer_list<LuaPair>& pairs) {
        *this = pairs;
    }

    template <typename Ret, typename... Args>
    LuaVariantBase::LuaVariantBase(const std::function<Ret(Args...)>& closure) {
        setValue<Ret, Args...>(closure);
    }

    inline LuaVariantBase::LuaVariantBase(const LuaVariantBase& rhs) { *this = rhs; }
    inline LuaVariantBase::LuaVariantBase(std::nullptr_t /*null*/): LuaVariantBase() {}
    inline LuaVariantBase::LuaVariantBase(LuaVariantBase&& rhs) {
        *this = rhs;
    }
    inline LuaVariantBase& LuaVariantBase::operator=(const LuaVariantBase& rhs) {
        if(!rhs.isWeakRef()) { //Gracefully obtain copy of data for strong references
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
                setValue(rhs._value.luserdata);
                break;
            default:
                setRef(rhs.getRef());
            }
        } else { //Simply assume raw values, otherwise
            _setType(LUA_TNIL);
            _type   = rhs._type;
            _value  = rhs._value;

        }
        return *this;
    }

    inline LuaVariantBase& LuaVariantBase::operator=(LuaVariantBase&& rhs) {
        _setType(LUA_TNIL); //clear out old shit
        _type           = rhs._type;
        _value          = rhs._value;
        if(!rhs.isWeakRef()) { //only steal references if this was a strong reference
            rhs._type       = LUA_TNIL;
            rhs._value.ref  = LUA_REFNIL;
        }
        return *this;
    }

    inline LuaVariantBase::LuaVariantBase(const char * const string) { setValue(string); }
    inline LuaVariantBase::LuaVariantBase(const bool val) { setValue(val); }
    inline LuaVariantBase::LuaVariantBase(const float number) { setValue(number); }
    inline LuaVariantBase::LuaVariantBase(const double number) { setValue(number); }
    inline LuaVariantBase::LuaVariantBase(const int num) { setValue(num); }
    inline LuaVariantBase::LuaVariantBase(const unsigned num) { setValue((int)num); }
    inline LuaVariantBase::LuaVariantBase(void*const ptr) { setValue(ptr); }
    inline LuaVariantBase::LuaVariantBase(const OOLUA::Lua_table_ref& ref) { setRef(ref.ref()); }
    inline LuaVariantBase::LuaVariantBase(const OOLUA::Lua_func_ref &ref) { setRef(ref.ref()); }
    inline LuaVariantBase::LuaVariantBase(const lua_CFunction funcPtr) { setValue(funcPtr); }
    template<typename T>
    inline LuaVariantBase::LuaVariantBase(const T fun) {
        setValue(fun);
    }


    inline LuaVariantBase::~LuaVariantBase(void) { setNil(); }

    template<typename T>
    inline LuaVariantBase& LuaVariantBase::operator=(const T*const rhs) {
        setValue(rhs);
        return *this;
    }

    template<typename T>
    inline LuaVariantBase& LuaVariantBase::operator=(T*const rhs) {
        setValue(rhs);
        return *this;
    }

    inline LuaVariantBase& LuaVariantBase::operator=(const bool val) { setValue(val); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const std::nullptr_t) { setValue(nullptr); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const int num) { setValue(num); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const float num) { setValue((lua_Number)num); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const double num) { setValue((lua_Number)num); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const char*const string) { setValue(string); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(void*const ptr) { setValue(ptr); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const OOLUA::Lua_table_ref& ref) { setValue(ref); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const OOLUA::Lua_func_ref& ref) { setValue(ref); return *this; }
    inline LuaVariantBase& LuaVariantBase::operator=(const lua_CFunction funcPtr) { setValue(funcPtr); return *this; }
    template <typename Ret, typename... Args>
    LuaVariantBase& LuaVariantBase::operator=(const std::function<Ret(Args...)>& closure) {
        setValue(closure);
        return *this;
    }
    template<typename T>
    LuaVariantBase& LuaVariantBase::operator=(const T func) {
        setValue(make_function(func));
        return *this;
    }

    inline bool LuaVariantBase::operator==(const LuaVariantBase& rhs) const {
        const auto type = getType();
        if(type != rhs.getType()) return false;
        switch(type) {
        case LUA_TNIL:
            return true;
        case LUA_TBOOLEAN:
            return _value.boolean == rhs._value.boolean;
        case LUA_TNUMBER:
            return _value.number == rhs._value.number;
        case LUA_TSTRING:
            return *this == rhs._value.string;
        case LUA_TLIGHTUSERDATA:
            return _value.luserdata == rhs._value.luserdata;
        default: //let Lua handle the comparison for complex types
            push();
            rhs.push();
            const bool equality = lua_compare(*_mainState, -1, -2, LUA_OPEQ);
            lua_pop(*_mainState, 2);
            return equality;
        }
    }

    template<typename T>
    inline bool LuaVariantBase::operator==(const T*const ud) const {
        //Purposely defer comparison to Lua, to honor overloaded comparison metamethods
        bool equal = false;
        if(push()) {
            if(OOLUA::push(*_mainState, ud)) {
                equal = lua_compare(*_mainState, -1, -2, LUA_OPEQ);
                lua_pop(*_mainState, 1);
            }
            lua_pop(*_mainState, 1);
        }
        return equal;
    }

    inline bool LuaVariantBase::operator==(const std::nullptr_t) const { return isNil() || (getType() == LUA_TLIGHTUSERDATA && _value.luserdata == nullptr); }
    inline bool LuaVariantBase::operator==(const bool val) const { return getType() == LUA_TBOOLEAN && _value.boolean == val; }
    inline bool LuaVariantBase::operator==(const int num) const { return getType() == LUA_TNUMBER && (int)_value.number == (int)num; }
    inline bool LuaVariantBase::operator==(const float num) const { return getType() == LUA_TNUMBER && _value.number == (lua_Number)num; }
    inline bool LuaVariantBase::operator==(const double num) const { return getType() == LUA_TNUMBER && _value.number == (lua_Number)num; }
    inline bool LuaVariantBase::operator==(void*const ptr) const { return ptr? isUserdataType() && _value.luserdata == ptr : isNil(); }
    inline bool LuaVariantBase::operator==(const char*const str) const { return (_type == LUA_TSTRING && (strcmp(_value.string, str) == 0)); };


    template<typename T>
    inline bool LuaVariantBase::operator!=(const T*const ud) const {
        return !(*this == ud);
    }
    inline bool LuaVariantBase::operator!=(const LuaVariantBase& rhs) const { return !(*this == rhs); }
    inline bool LuaVariantBase::operator!=(const std::nullptr_t) const { return !(*this == nullptr); }
    inline bool LuaVariantBase::operator!=(const bool val) const { return !(*this == val); }
    inline bool LuaVariantBase::operator!=(const int num) const { return !(*this == num); }
    inline bool LuaVariantBase::operator!=(const float num) const { return !(*this == num); }
    inline bool LuaVariantBase::operator!=(const double num) const { return !(*this == num); }
    inline bool LuaVariantBase::operator!=(void*const ptr) const { return !(*this == ptr); }
    inline bool LuaVariantBase::operator!=(const char*const str) const { return !(*this == str); }

    inline LuaVariantBase::operator OOLUA::Lua_table_ref(void) const {
        OOLUA::Lua_table_ref ref;

        //Don't even bother if this isn't even a valid reference type.
        if(isRefType()) {
            //Have to get a fresh reference for OOLUA, or it's going to delete ours
            if(push()) {
                OOLUA::pull(*_mainState, ref);
            }
        }
        return ref;
    }

    //inline LuaVariantBase::operator LuaTableRef(void) const;
    //inline LuaVariantBase::operator OOLUA::Lua_table_ref(void);
    inline bool LuaVariantBase::isNil(void) const { return _type == LUA_TNIL; }
    inline bool LuaVariantBase::isNilRef(void) const { return _type == LUA_TNIL || _value.ref == LUA_REFNIL; }
    inline bool LuaVariantBase::isRefType(void) const { return !(_type == LUA_TBOOLEAN || _type == LUA_TNUMBER || _type == LUA_TNIL || _type == LUA_TLIGHTUSERDATA|| _type == LUA_TSTRING); }
    inline bool LuaVariantBase::isIndexable(void) const { return (_type == LUA_TUSERDATA || _type == LUA_TTABLE); }
    inline bool LuaVariantBase::isValueType(void) const { return _type == LUA_TNIL || !isRefType(); }
    inline bool LuaVariantBase::isUserdataType(void) const { return _type == LUA_TUSERDATA || _type == LUA_TLIGHTUSERDATA; }
    inline bool LuaVariantBase::isWeakRef(void) const {
        return ((_type >> ELYSIAN_LUA_VARIANT_WEAK_REF_BIT_POS) & 0x1);
    }

    inline void LuaVariantBase::_setWeakRef(const bool value) {
        _type &= ~(0x1 << ELYSIAN_LUA_VARIANT_WEAK_REF_BIT_POS);
        _type |= ((uint32_t)(value) << ELYSIAN_LUA_VARIANT_WEAK_REF_BIT_POS);
    }

    inline void LuaVariantBase::_setType(const int luaType) {
        if(!isWeakRef()) { //only delete shit if we have ownership of it (are a strong reference)
            if(isRefType()) {
                luaL_unref(*_mainState, LUA_REGISTRYINDEX, _value.ref);
            } else if(_type == LUA_TSTRING) {
                free((void*)_value.string);
            } else if(_type == LUA_TNIL) {
                _value.ref = LUA_REFNIL;
            }
        }
        _type &= ~(ELYSIAN_LUA_VARIANT_TYPE_BIT_MASK);
        _type |= ((uint32_t)luaType);
    }

    inline void LuaVariantBase::setNil(void) {
        _setType(LUA_TNIL);
        _value.ref = LUA_REFNIL;
    }

    template<typename T>
    inline void LuaVariantBase::setValue(T*const ud) {
#if 0
        if(OOLUA::push(*_mainState, ud)) pull();
#else
        LuaManager::push(ud);
        pull();
#endif
    }

    inline void LuaVariantBase::setValue(const std::initializer_list<LuaPair>& pairs) {
        LuaManager::push(pairs);
        pull();
    }

    inline void LuaVariantBase::setValue(std::nullptr_t /*null*/) {
        setNil();
    }

    inline void LuaVariantBase::setValue(const int number) {
        _setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const unsigned number) {
        _setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const float number) {
        _setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const double number) {
        _setType(LUA_TNUMBER);
        _value.number = (lua_Number)number;
    }

    inline void LuaVariantBase::setValue(const char*const str) {
        if(!str) setValue(nullptr);

        const int size = std::strlen(str);

        if(_type != LUA_TSTRING) {
            _setType(LUA_TSTRING);
            _value.string = (const char*)malloc(size*sizeof(char)+1);
        } else {
            const int curSize = std::strlen(_value.string); //only realloc if we actually need more space... fuck fragmentation
            if(curSize < size) _value.string = (const char*)realloc((void*)_value.string, size*sizeof(char)+1);
        }

        std::strcpy((char*)_value.string, str);
    }

    inline void LuaVariantBase::setValue(const bool val) {
        _setType(LUA_TBOOLEAN);
        _value.boolean = val;
    }

    inline void LuaVariantBase::setValue(void*const ptr) {
        if(ptr) {
            _setType(LUA_TLIGHTUSERDATA);
            _value.luserdata = ptr;
        } else _setType(LUA_TNIL);
    }

    inline void LuaVariantBase::setValue(const OOLUA::Lua_table_ref &ooluaRef) {
        if(ooluaRef.push(*_mainState)) pull();
    }

    inline void LuaVariantBase::setValue(const OOLUA::Lua_func_ref &ooluaRef) {
        if(ooluaRef.push(*_mainState)) pull();
    }

    inline void LuaVariantBase::setValue(const lua_CFunction funcPtr) {
        lua_pushcfunction(*_mainState, funcPtr);
        pull();
    }

    template<typename T>
    inline T LuaVariantBase::getValue(void) const {
        T ud = nullptr;
        push();
        OOLUA::pull(*_mainState, ud);
        return ud;
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
    inline unsigned LuaVariantBase::getValue<unsigned>(void) const { return (unsigned)_value.number; }

    template<>
    inline  void* LuaVariantBase::getValue<void*>(void) const { return _value.luserdata; }

    template<>
    inline OOLUA::Lua_table_ref LuaVariantBase::getValue<OOLUA::Lua_table_ref>(void) const {
        OOLUA::Lua_table_ref ref;
        if(push()) {
            ref.pull(*_mainState);
        }
        return ref;
    }

    template<>
    inline OOLUA::Lua_func_ref LuaVariantBase::getValue<OOLUA::Lua_func_ref>(void) const {
        OOLUA::Lua_func_ref ref;
        if(push()) {
            ref.pull(*_mainState);
        }
        return ref;
    }

    inline  int LuaVariantBase::getType(void) const {
        return (_type & ELYSIAN_LUA_VARIANT_TYPE_BIT_MASK);
    }

    inline const char* LuaVariantBase::getTypeString(void) const {
        const int luaType = getType();
        if(luaType != LUA_TUSERDATA && luaType != LUA_TLIGHTUSERDATA) {
            return lua_typename(*_mainState, luaType);
        } else {
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _value.ref);
            lua_getfield(*_mainState, -1, ELYSIAN_LUA_PROXY_TYPE_METAFIELD);
            if(!lua_isnil(*_mainState, -1)) {
                const char* name = lua_tostring(*_mainState, -1);
                lua_pop(*_mainState, 2);
                return name;
            } else {
                return lua_typename(*_mainState, luaType);
            }
        }
    }

    inline int LuaVariantBase::getRef(void) const {
        return isRefType()? _value.ref : LUA_NOREF;
    }

    inline void LuaVariantBase::setRef(const int ref) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
        _setType(lua_type(*_mainState, -1));
        _value.ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
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
        switch(source.getType()) {
        case LUA_TBOOLEAN:
            return source._value.boolean;
        case LUA_TNIL:
            return false;
        case LUA_TSTRING:
            return (source == "true")? true : false;
        case LUA_TNUMBER:
            return (source._value.number >= 1)? true : false;
        case LUA_TLIGHTUSERDATA:
            return (source._value.luserdata)? true : false;
        default:
            return false;
        }
    }

    inline LuaVariantBase& LuaVariantBase::_convertToBool(const LuaVariantBase &source, LuaVariantBase &dest) {
        switch(source.getType()) {
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
        switch(source.getType()) {
        case LUA_TNUMBER:
            return source._value.number;
        case LUA_TBOOLEAN:
            return source._value.boolean? 1 : 0;
        case LUA_TNIL:
            return 0;
        case LUA_TLIGHTUSERDATA:
            return source._value.luserdata? 1 : 0;
        default:
            return 0;
        }
    }

    inline LuaVariantBase& LuaVariantBase::_convertToNumber(const LuaVariantBase &source, LuaVariantBase &dest) {
        switch(source.getType()) {
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
        return (getType() == LUA_TBOOLEAN)? *this : _convertToBool(*this, variant);
    }

    inline LuaVariantBase LuaVariantBase::asNumber(void) const {
        LuaVariantBase variant;
        return (getType() == LUA_TNUMBER)? *this :
            _convertToNumber(*this, variant);
    }

    inline LuaVariantBase LuaVariantBase::asString(void) const {
        return (getType() == LUA_TSTRING)? *this :
            _convertType( [](void) { luaL_tolstring(*_mainState, -1, nullptr); lua_remove(*_mainState, -2); } );
    }

    inline LuaVariantBase LuaVariantBase::asUserdata(void) const {
        if(getType() == LUA_TLIGHTUSERDATA) {
            return *this;
        } else {
            LuaVariantBase variant;
            variant._setType(LUA_TLIGHTUSERDATA);
            variant._value.luserdata = nullptr;
            return variant;
        }

    }

    inline int LuaVariantBase::getLength(void) const {
        int len = 0;
        if(push()) {
            lua_len(*_mainState, -1);
            len = lua_tointeger(*_mainState, -1);
            lua_pop(*_mainState, 2);
        }
        return len;
    }

    inline void LuaVariantBase::setMetatable(const LuaVariantBase &mt) const {
        GY_ASSERT(isRefType() && mt.isRefType());
        if(push()) {
            if(mt.push()) {
                lua_setmetatable(*_mainState, -2);
            }
            lua_pop(*_mainState, 1);
        }
    }

    inline LuaVariant LuaVariantBase::getMetatable(void) const {
        GY_ASSERT(isRefType());
        push();
        lua_getmetatable(*_mainState, -1);
        lua_remove(*_mainState, -2);
        return LuaVariant::fromStack(-1);
    }

    inline LuaVariantBase LuaVariantBase::newTable(void) {
        lua_newtable(*_mainState);
        return LuaVariantBase::fromStack();
    }

    inline LuaVariantBase LuaVariantBase::fromStack(const int index, const bool remove) {
        LuaVariantBase variant;
        lua_pushvalue(*_mainState, index);
        if(remove) lua_remove(*_mainState, (index > 0)? index : index-1);
        variant.pull();
        return variant;
    }

    inline LuaVariantBase LuaVariantBase::fromRef(const int luaRef) {
        LuaVariantBase variant;
        variant.setRef(luaRef);
        return variant;
    }

    inline LuaVariantBase LuaVariantBase::fromWeakRef(const int luaRef) {
        LuaVariant v;
#if 0 //WRONG AS FUCK! NOT WEAK AT ALL!

        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, luaRef);
        v.pull();
#else
        v._setRefDirect(luaRef);
#endif
        v._setWeakRef(true);
        return v;
    }

    inline LuaVariantBase LuaVariantBase::fromArray(const std::initializer_list<LuaVariant>& array) {
        int index = 1;
        lua_newtable(*_mainState);
        for(const auto& it : array) {
            lua_pushinteger(*_mainState, index++);
            const bool success = it.push();
            GY_ASSERT(success);
            lua_settable(*_mainState, -3);
        }
        return fromStack();
    }

    template<typename C>
    inline LuaVariantBase LuaVariantBase::fromSequenceContainer(const C& cont) {
        LuaVariant array;
        unsigned index = 1;
        for(auto it: cont) {
            array[index++] = it;

        }
        return array;
    }

    inline LuaVariantBase LuaVariantBase::makeWeakRef(const LuaVariant& src) {
        LuaVariant dest;
        dest._type      = src._type;
        dest._value     = src._value;
        dest._setWeakRef(true);
        return dest;
    }

    template <typename... Args>
    inline LuaVariantBase LuaVariantBase::operator()(Args&&... args) const {
        push();
       return LuaManager::_callFunction<LuaVariantBase>(nullptr, std::forward<Args>(args)...);
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
        return LuaFieldRef(*this, key);
    }

    //Assumes ownership of reference rather than duplicating!
    inline void LuaVariantBase::_setRefDirect(const int luaRef) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, luaRef);
        _setType(lua_type(*_mainState, -1));
        lua_pop(*_mainState, 1);
        _value.ref = luaRef;
    }

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
#if 0
            variant.setRef(luaL_ref(state, LUA_REGISTRYINDEX));
#else
            variant._setRefDirect(luaL_ref(state, LUA_REGISTRYINDEX));
#endif
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

    namespace INTERNAL {

        //Force OOLua to treat LuaVariant as a built-in type and not a proxy
        template<>
        struct is_false_integral<elysian::LuaVariantBase>
        {
            typedef typename LVD::raw_type<elysian::LuaVariantBase>::type raw;
            enum {value = 1 };
        };

        namespace LUA_CALLED
        {
    #if 0
            inline void get(lua_State* const vm, int idx, LuaVariant& v) {
                v = LuaVariant::fromStack(idx, false);
            }
    #endif

            //Specialized get function used by OOLua to retrieve item from stack without removing it
            template<>
            struct get_basic_type<elysian::LuaVariantBase, 0, 0> {
                static void get(lua_State* const /*vm*/, int idx, elysian::LuaVariantBase& v) {
                    v = elysian::LuaVariantBase::fromStack(idx, false);
                }
            };
        }
    }
}

namespace elysian {

    inline bool LuaVariantBase::push(void) const {
        return OOLUA::push(*_mainState, *this);
    }

    inline bool LuaVariantBase::pull(void) {
        return OOLUA::pull(*_mainState, *this);
    }

    inline LuaVariantBase& LuaVariantBase::operator=(const std::initializer_list<LuaPair>& pairs) {
#if 0
        lua_newtable(*_mainState);
        for(auto it : pairs) {
            it.first.push();
            it.second.push();
            lua_settable(*_mainState, -3);
        }
        pull();
#else
        setValue(pairs);
        return *this;
#endif
    }


    inline LuaVariantBase::iterator LuaVariantBase::begin(void) {
        const auto& it = cbegin();
        return iterator(it._fieldRef,
                        it._nextFuncRef);
    }

    inline LuaVariantBase::const_iterator LuaVariantBase::begin(void) const {
        LuaVariant nextFunc;
        LuaPair pair;

        pushGlobalField("pairs");
        if(push()) {
            //call pairs(this)
            if(ELYSIAN_LUA_MANAGER_PROTECTED_CALL(1, 3)) {
                //store next() function
                lua_pushvalue(*_mainState, -3);
                if(nextFunc.pull()) {
                    GY_ASSERT(nextFunc.getType() == LUA_TFUNCTION);
                    //call next(this, k)
                    if(ELYSIAN_LUA_MANAGER_PROTECTED_CALL(2, 2, nullptr)) {
                        OOLUA::pull(*_mainState, pair);
                    } else GY_ASSERT(false);    //next(this, k) failed
                } else GY_ASSERT(false);        //no next function found
            } else GY_ASSERT(false);            //pairs(this) failed
        }
        return const_iterator(LuaFieldRef(LuaVariant::makeWeakRef(*this), pair.first, pair.second), nextFunc);
    }

    inline LuaVariantBase::const_iterator LuaVariantBase::cbegin(void) const {
        return begin();
    }


    inline LuaVariantBase::iterator LuaVariantBase::end(void) {
        return iterator(LuaFieldRef(LuaVariant::makeWeakRef(*this), LuaVariant()), LuaVariant());
    }


    inline LuaVariantBase::const_iterator LuaVariantBase::end(void) const {
        return const_iterator(LuaFieldRef(LuaVariant::makeWeakRef(*this), LuaVariant()), LuaVariant());
    }

    inline LuaVariantBase::const_iterator LuaVariantBase::cend(void) const {
        return const_iterator(LuaFieldRef(LuaVariant::makeWeakRef(*this), LuaVariant()), LuaVariant());
    }


    template<bool is_const_iterator>
    inline const_noconst_iterator<is_const_iterator> const_noconst_iterator<is_const_iterator>::operator++(void){
        bool success = true;

        success = _nextFuncRef.push();
        GY_ASSERT(success);
        success = _fieldRef._tableRef.push();
        GY_ASSERT(success);
        success = _fieldRef._key.push();
        GY_ASSERT(success);

        if(ELYSIAN_LUA_MANAGER_PROTECTED_CALL(2, 2, nullptr)) {

            success &= _fieldRef._cachedValue.pull();
            GY_ASSERT(success);
            success &= _fieldRef._key.pull();
            GY_ASSERT(success);
        } else GY_ASSERT(false);

        return *this;
    }



    template <typename Ret, typename... Args>
    inline void LuaVariantBase::setValue(const std::function<Ret(Args...)>& closure) {
        LuaManager::push(closure);
        pull();
    }

    template<typename T>
    inline void LuaVariantBase::setValue(const T lambda) {
        setValue(make_function(lambda));
        //LuaManager::push(lambda);
        //pull();
    }


}



#endif // ELYSIAN_LUA_TABLE_VARIANT_HPP

