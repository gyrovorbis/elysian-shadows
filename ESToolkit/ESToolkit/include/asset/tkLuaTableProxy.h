#ifndef TK_LUA_TABLEPROXY_H
#define TK_LUA_TABLEPROXY_H

#include "elysian_lua_table_proxy_base.hpp"
#include  <QObject>

#define TK_LUA_TABLE_USERDATA_FIELD_KEY         "__tkUd"
#define TK_LUA_TABLE_METAMETHODSET_FIELD_KEY    "__metaset"
#define TK_LUA_TABLE_PROPERTY_TABLE_FIELD_KEY   "__prop"


/*
 * Had to move QObject signal/slot functionality to a separate class,
 * because Qt's MOC reests on class templates...
 */
class TkLuaTableUserdata: public QObject {
    Q_OBJECT

private:
    int                 _propRef    = LUA_NOREF;
    int                 _origRef    = LUA_NOREF;

    static int          _metaRef;
    static lua_State*   _lvm;
Q_SIGNALS:
    //Notifies Qt's GUI models to update when Lua table fields change
    void    fieldChanged(const QString key) const;
    void    fieldAdded(const QString key) const;

public:
            TkLuaTableUserdata(void);
    virtual ~TkLuaTableUserdata(void);
    void    emitFieldChanged(const QString key) const;
    void    emitFieldAdded(const QString key) const;

    void    setOrigRef(const int ref);
    int     getOrigRef(void) const;
    void    setPropRef(const int ref);
    int     getPropRef(void) const;

    //static
    static TkLuaTableUserdata*  inject(lua_State*const l, const int luaRef);
    //static bool                 hasTkUserdata(OOLUA::Lua_table_ref& ref);

private:
    static void                 _createMetatable(lua_State*const l);

    //metamethods
    static int                  _gcMetamethod(lua_State*const l);
    static int                  _indexMetamethod(lua_State*const l);
    static int                  _newIndexMetamethod(lua_State*const l);
    static int                  _pairsMetamethod(lua_State*const l);
    static int                  _ipairsMetamethod(lua_State*const l);
    static int                  _nextMetamethod(lua_State*const l);
    static int                  _lengthMetamethod(lua_State*const l);
    static int                  _metamethodSetMetamethod(lua_State* const l);
};


/*
 * Had to move QObject signal/slot functionality to a separate class,
 * because Qt's MOC reests on class templates...
 */
class TkLuaTableProxyQObject: public QObject {
    Q_OBJECT
Q_SIGNALS:
    //Notifies Qt's GUI models to update when Lua table fields change
    void fieldChanged(const QString key);
    void fieldAdded(const QString key);

public:
    void emitFieldChanged(const QString key) { Q_EMIT fieldChanged(key); }
    void emitFieldAdded(const QString key) { Q_EMIT fieldAdded(key); }
};

/* Compile-time override engine's LuaTableProxy implementation
 * and add signal/slot functionality using the CRTP.
 *
 * Proxies Userdata and engine-types as Lua tables for
 * consistent representation within ESTk models and Lua scripts.
 * Injects itself into __newIndex metamethod to inform Qt when
 * internal Lua data changes.
 */
template <typename P, typename T=P>
class TkLuaTableProxy:
        public elysian::LuaTableProxyBase<P, T>,
        public TkLuaTableProxyQObject
{

    typedef elysian::LuaTableProxyBase<P, T> ParentType;
    friend ParentType;
protected:
    using                       elysian::LuaManager::_mainState;
    mutable int                 _luaRef = LUA_NOREF;

    //CRTP overridden from engine
    static void _callPropertyProxyMethod(const bool write, const char*const key);
    static void _addMetamethods(lua_State* l);

    //Tk-Specific metamethods
    static int  _connectSignals(lua_State* l);

public:

    virtual     ~TkLuaTableProxy(void);

    //Might also need copy constructor
    const TkLuaTableProxy<P, T>&    operator=(const TkLuaTableProxy<P, T>& rhs);
    int                             getLuaTableRef(void) const;

};










// ======== INLINEZ ======

inline void TkLuaTableUserdata::setOrigRef(const int ref) { _origRef = ref; }
inline void TkLuaTableUserdata::setPropRef(const int ref) { _propRef = ref; }
inline int TkLuaTableUserdata::getOrigRef(void) const { return _origRef; }
inline int TkLuaTableUserdata::getPropRef(void) const { return _propRef; }
inline TkLuaTableUserdata::TkLuaTableUserdata(void): QObject(nullptr) {}
inline void TkLuaTableUserdata::emitFieldAdded(const QString key) const { Q_EMIT fieldAdded(key); }
inline void TkLuaTableUserdata::emitFieldChanged(const QString key) const { Q_EMIT fieldChanged(key); }


template <typename P, typename T>
inline void TkLuaTableProxy<P, T>::_callPropertyProxyMethod(const bool write, const char*const key) {
    T* obj;
    if(write) {
        lua_pushvalue(*_mainState, 2);
        OOLUA::pull(*_mainState, obj);
        assert(obj);
    }
    ParentType::_callPropertyProxyMethod(write, key);
    if(write) {
        obj->emitFieldChanged(key);
    }
}

/* Crazy-ass assignment operator to copy assign all internal object data MINUS inherited QObject data,
 * because Qt explicitly deleted its assignment operator and will shit itself.
 */

template<typename P, typename T>
inline const TkLuaTableProxy<P, T>& TkLuaTableProxy<P,T>::operator=(const TkLuaTableProxy<P, T>& rhs) {
    *static_cast<ParentType*>(this) = *static_cast<const ParentType*>(&rhs);
    *static_cast<T*>(this) = *static_cast<const T*>(&rhs);
    return *this;
}

template <typename P, typename T>
inline int TkLuaTableProxy<P, T>::getLuaTableRef(void) const {
    if(_luaRef == LUA_NOREF) {
        OOLUA::push(*_mainState, static_cast<const T*>(this));
        _luaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    }
    return _luaRef;
}


template <typename P, typename T>
inline TkLuaTableProxy<P, T>::~TkLuaTableProxy(void) {
    if(_luaRef != LUA_NOREF) {
        luaL_unref(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    }
}

template <typename P, typename T>
inline int TkLuaTableProxy<P, T>::_connectSignals(lua_State *l) {
    QObject* tableItem = static_cast<QObject*>(lua_touserdata(*_mainState, -1));
    assert(tableItem);
    lua_pop(*_mainState, 1);

    T* thisPtr;
    OOLUA::pull(l, thisPtr);
    assert(thisPtr);

    auto* thisQObj = static_cast<QObject*>(thisPtr);

    connect(thisQObj, SIGNAL(fieldChanged(QString)), tableItem, SLOT(onFieldChanged(QString)));

    return 0;
}

template<typename P, typename T>
inline void TkLuaTableProxy<P, T>::_addMetamethods(lua_State *l) {
    //Push our QObject retrieval function
    lua_pushcfunction(l, &_connectSignals);
    lua_setfield(l, -2, "__connectSignals");

    ParentType::_addMetamethods(l);
}


#endif // TK_LUA_TABLE_PROXY_H
