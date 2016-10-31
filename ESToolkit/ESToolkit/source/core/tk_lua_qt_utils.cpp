#include <core/tk_lua_qt_utils.h>
#include <crtp/elysian_lua_variant.hpp>

namespace OOLUA {

    template<>
    bool push<QVariant>(lua_State* state, QVariant const& variant) {

        switch(variant.type()) {
        case QMetaType::Bool:
            return push(state, variant.toBool());
        case QMetaType::QString:
            if(variant.toString() == "nil") {
                lua_pushnil(state);
                return true;
            }
             else return push(state, variant.toString());
        default:
            return push(state, variant.toFloat());
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
