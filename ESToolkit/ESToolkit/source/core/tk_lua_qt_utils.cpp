#include <core/tk_lua_qt_utils.h>
#include <crtp/elysian_lua_variant.hpp>

namespace OOLUA {

    template<>
    bool push<QVariant>(lua_State* state, QVariant const& variant) {

        bool boolean = false;
        bool boolVal;
        QString str = variant.toString();
        if(variant.canConvert(QMetaType::QString)) {
            if(str == "true") {
                boolean = true;
                boolVal = true;
            } else if(str == "false") {
                boolean = true;
                boolVal = false;
            }
        }

        if(boolean) {
            lua_pushboolean(state, boolVal);
            return true;
        } else if(variant.canConvert(QMetaType::Int) || variant.canConvert(QMetaType::Float)) {
            return push(state, variant.toFloat());
        } else if(variant.canConvert(QMetaType::QString)) {
            if(str == "nil") {
                lua_pushnil(state);
                return true;
            } else return push(state, str);
        } else {
            Q_ASSERT("Trying to push unknown QVariant type to Lua!");
            return false;
        }
    }

    template<>
    bool pull<QVariant>(lua_State* state, QVariant& variant) {
        QString     stringVal;
        LuaVariant v;
        float       numberVal;
        bool        boolVal;
        int         luaType = lua_type(state, -1);
        bool        success;

        //QVariant is dynamic, so figure out what the fuck we want to pull
        switch(luaType) {
        case LUA_TBOOLEAN:
            success = OOLUA::pull(state, boolVal);
            variant.setValue(boolVal);
            break;
        case LUA_TNUMBER:
            success = OOLUA::pull(state, numberVal);
            variant.setValue(numberVal);
            break;
        default:
            success = OOLUA::pull(state, stringVal);
            variant.setValue(stringVal);

        }
        return success;
    }

}
