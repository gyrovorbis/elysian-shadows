#ifndef TK_LUA_TABLEPROXY_H
#define TK_LUA_TABLEPROXY_H

#include "elysian_lua_table_proxy_base.hpp"
#include "luaTableStackModel.h"

template <typename P, typename T=P>
class TkLuaTableProxy:
        public elysian::LuaTableProxyBase<P, T>
{
protected:
    void _onFieldAdded(const char*const key);
    void _onFieldChanged(const char*const key);

public:


signals:

    void fieldChanged(const QString key);
    void fieldAdded(const QString key);
};



// ======== INLINEZ ======


template <typename P, typename T>
inline void TkLuaTableProxy<P, T>::_onFieldAdded(const char*const key) { emit fieldAdded(key); }

template <typename P, typename T>
inline void TkLuaTableProxy<P, T>::_onFieldChanged(const char*const key) { emit fieldChanged(key); }


#endif // TK_LUA_TABLE_PROXY_H
