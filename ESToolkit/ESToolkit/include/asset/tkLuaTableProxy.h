#ifndef TK_LUA_TABLEPROXY_H
#define TK_LUA_TABLEPROXY_H

#include "elysian_lua_table_proxy_base.hpp"
#include  <QObject>

/*
 * Had to move QObject signal/slot functionality to a separate class,
 * because Qt's MOC reests on class templates...
 */
class TkLuaTableProxyQObject: public QObject {
    Q_OBJECT
signals:
    //Notifies Qt's GUI models to update when Lua table fields change
    void fieldChanged(const QString key);
    void fieldAdded(const QString key);
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
    using               elysian::LuaManager::_mainState;
    mutable int         _luaRef = LUA_NOREF;

    //CRTP overridden from engine
    void        _onFieldAdded(const char*const key);
    void        _onFieldChanged(const char*const key);

    static void _callPropertyProxyMethod(const bool write, const char*const key);
    static void _addMetamethods(lua_State* l);

    //Tk-Specific metamethods
    static int  _getQObjectLightUserdata(lua_State* l);


public:

    virtual     ~TkLuaTableProxy(void);

    //Might also need copy constructor
    const TkLuaTableProxy<P, T>&    operator=(const TkLuaTableProxy<P, T>& rhs);
    int                             getLuaTableRef(void) const;

};


// ======== INLINEZ ======


template <typename P, typename T>
inline void TkLuaTableProxy<P, T>::_onFieldAdded(const char*const key) { emit fieldAdded(key); }

template <typename P, typename T>
inline void TkLuaTableProxy<P, T>::_onFieldChanged(const char*const key) { emit fieldChanged(key); }

template <typename P, typename T>
inline void TkLuaTableProxy<P, T>::_callPropertyProxyMethod(const bool write, const char*const key) {
    T* obj;
    if(write) {
        lua_pushvalue(*_mainState, 1);
        OOLUA::pull(*_mainState, obj);
    }
    ParentType::_callPropertyProxyMethod(write, key);
    if(write) {
        //static_cast<P*>(obj)->fieldChanged(key);
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
inline int TkLuaTableProxy<P, T>::_getQObjectLightUserdata(lua_State *l) {
    T* obj;
    OOLUA::pull(l, obj);
    assert(obj);
    lua_pushlightuserdata(l, static_cast<TkLuaTableProxyQObject*>(obj));
    return 1;
}

template<typename P, typename T>
inline void TkLuaTableProxy<P, T>::_addMetamethods(lua_State *l) {
    //Push our QObject retrieval function
    lua_pushcfunction(l, &_getQObjectLightUserdata);
    lua_setfield(l, -2, "__getQObject");

    ParentType::_addMetaMethods(l);
}

#endif // TK_LUA_TABLE_PROXY_H
