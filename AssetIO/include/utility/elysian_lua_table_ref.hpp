#ifndef ELYSIAN_LUA_TABLE_REF_HPP
#define ELYSIAN_LUA_TABLE_REF_HPP

#include <utility/elysian_lua_variant.hpp>
#include <crtp/elysian_lua_manager.hpp>


namespace elysian {




#if 0

    //QStandardItem representing a Lua k,v pair
    class LuaTableFieldItem:
        public PropertyStandardItem<QVariant, LuaTableFieldItem, true>,
        public elysian::LuaManager,
        public AssetManager
    {
        friend struct LuaTableTableFieldData;
    protected:
        mutable LuaTableTableFieldData* _tableData      = nullptr;
        QStandardItem*                  _labelItem      = nullptr;
        const int                       _luaRef         = LUA_NOREF;
        const QVariant                  _key;

    public:
                                LuaTableFieldItem(const int ref, const QVariant key, QStandardItem*const labelItem);
        virtual                 ~LuaTableFieldItem(void);
        virtual void            insertingModelItem(void) override;

        QVariant                getValue(void) const;
        void                    setValue(const QVariant val);
        virtual QStyledItemDelegate* getStyledItemDelegate(void) const override;

        void                    setDirty(const bool val=true);
        LuaTableTableFieldData* getTableData(void) const;

        int                     getValueLuaType(void) const;
        int                     getKeyLuaType(void) const;
        virtual int type(void) const override { return (int)ESTkStandardItemType::LuaTable; }
    };


    //Aggregating "Table" data away, so primitive-type field items don't have all this overhead.
    struct LuaTableTableFieldData: public QObject, public LuaManager {
        Q_OBJECT
        friend class LuaTableFieldItem;
    public:
        QHash<QVariant, LuaTableFieldItem*> _fieldItems;
        int                                 _luaRef = LUA_NOREF;
        QStandardItem*                      _parentItem = nullptr;

                                            LuaTableTableFieldData(const int tableRef);
        virtual                             ~LuaTableTableFieldData(void) override;

        void                                insertTableFieldItem(const QVariant key);
        void                                insertTableFieldItems(QStandardItem* parentItem);

    public Q_SLOTS:
        void                                onFieldChanged(const QVariant key);
        void                                onFieldAdded(const QVariant key);
      //vod                                 onFieldRemoved(const QVariant key); same shit as onFieldChanged to nil!

    };


    class LuaTableStackModel: public PropertyStackModel, public LuaManager {
        friend class LuaTableItem;

        LuaTableTableFieldData*     _tableData      = nullptr;
        QStandardItem*              _parentItem     = nullptr;
        int                         _luaRef         = LUA_NOREF;
        bool                        _populated      = false;

    public:
                        LuaTableStackModel(const int ref);

        void            setLuaRef(const int ref);
        virtual void    populateModel(void) override;
        void            setLabelItemText(const QString string);

    };




#endif


    //constructor to copy all values from stack and into lua array (variable argument types)
    class LuaTableRef: public LuaManager {
    public:

        template<bool is_const_iterator=true>

        class const_noconst_iterator : public std::iterator<std::forward_iterator_tag, LuaVariant>, public LuaManager {
            friend class const_noconst_iterator<true>;
            friend class LuaTableRef;

            typedef typename std::conditional<is_const_iterator, const LuaTableRef*, LuaTableRef*>::type DataStructurePointerType;
            typedef typename std::conditional<is_const_iterator, const LuaTableRef&, LuaTableRef>::type ValueReferenceType;

            const_noconst_iterator(DataStructurePointerType pointer_to_datastructure, const int funcRef, const LuaPair initialValue):
                _pair(initialValue),
                _tableRef(pointer_to_datastructure),
                _nextFuncRef(funcRef)
            {


            }

            /**
            * Copy constructor. Allows for implicit conversion from a regular iterator to a const_iterator
            */
            const_noconst_iterator(const const_noconst_iterator<false>& other):
              _pair(other._pair),
              _tableRef(other._tableRef),
                _nextFuncRef(other._nextFuncRef){

            }

            /**
            * Equals comparison operator
            */
            bool operator== (const const_noconst_iterator& other) const {
                return _tableRef == other._tableRef && _pair == other._pair;
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
                lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _tableRef);
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
            DataStructurePointerType    _tableRef;
            int                         _nextFuncRef    = LUA_NOREF;

        };


       using iterator       = const_noconst_iterator<false>;
       using const_iterator = const_noconst_iterator<true>;

    protected:
        int             _luaRef = LUA_NOREF;


    public:

                        LuaTableRef(void) = default;
                        LuaTableRef(const int ref);
                        LuaTableRef(const OOLUA::Lua_table_ref& ref);

                        ~LuaTableRef(void) = default;

        void            insert(const LuaPair& pair);
        void            insert(const LuaVariant& key, const LuaVariant& value=LuaVariant());
        void            insert(const char*const key, const LuaVariant& value=LuaVariant()); //override for performance

        bool            isNilRef(void) const;

        //since these can be created from the basic types, won't they cover everything?
        LuaVariant      getValue(const LuaVariant& key);
        LuaVariant      operator[](const LuaVariant& key);

        operator        LuaVariant(void);
        operator        OOLUA::Lua_table_ref(void);

        LuaTableRef     getMetatable(void);
        bool            load(const char*const filename);
        bool            save(const char*const filename);

        //C++11-style iterators
        iterator        begin(void);
        const_iterator  begin(void) const;
        const_iterator  cbegin(void) const;
        iterator        end(void);
        const_iterator  end(void) const;
        const_iterator  cend(void) const;


        static LuaTableRef&& createFromString(const char*const str);
        static LuaTableRef&& createFromFile(const char*const filename);



    //C++11 iterators

    public:
       // LuaTableRef
        //to OOLUA type
        //conversions
        //load/save from file
        //invoke generic methods
        //C++/Lua virtuals

        //UNION REPRESENTING ALL LUA "VALUE" TYPES

        //REPRESENT C++-side of VIRTUALS VIA LAMBDA-VTABLE

    };

    inline bool LuaTableRef::isNilRef(void) const { return _luaRef == LUA_NOREF; }

    inline LuaTableRef::LuaTableRef(const int ref): _luaRef(ref) {}

    inline LuaTableRef&& LuaTableRef::createFromString(const char * const str) {
        LuaTableRef ref;
        if(runString(str)) {
            ref._luaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
            if(ref._luaRef == LUA_NOREF) {
                //HOUSTON, SHIT'S REESTED
            }
        }
        return std::move(ref);

    }

    inline LuaTableRef::iterator LuaTableRef::begin(void) {
        const auto& it = cbegin();
        return iterator(this,
                        it._nextFuncRef,
                        it._pair);
    }

    inline LuaTableRef::const_iterator LuaTableRef::begin(void) const {

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

    inline LuaTableRef::const_iterator LuaTableRef::cbegin(void) const {
        return begin();
    }


    inline LuaTableRef::iterator LuaTableRef::end(void) {
        return iterator(this, LUA_NOREF, LuaPair());
    }


    inline LuaTableRef::const_iterator LuaTableRef::end(void) const {
        return const_iterator(this, LUA_NOREF, LuaPair());
    }

    inline LuaTableRef::const_iterator LuaTableRef::cend(void) const {
        return const_iterator(this, LUA_NOREF, LuaPair());
    }

    inline void LuaTableRef::insert(const LuaPair &pair) {
        insert(pair.first, pair.second);
    }

    inline void LuaTableRef::insert(const LuaVariant &key, const LuaVariant &value) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
        OOLUA::push(*_mainState, key);
        OOLUA::push(*_mainState, value);
        lua_settable(*_mainState, -3);
        lua_pop(*_mainState, 1);
    }

    inline void LuaTableRef::insert(const char * const key, const LuaVariant &value) {
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
        OOLUA::push(*_mainState, value);
        lua_setfield(*_mainState, -2, key);
        lua_pop(*_mainState, 1);
    }


}




#endif // ELYSIAN_LUA_TABLE_REF_HPP

