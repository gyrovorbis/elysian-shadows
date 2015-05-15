#ifndef TK_LUA_QT_UTILS_H
#define TK_LUA_QT_UTILS_H

#include <oolua.h>
#include <QHash>
#include <QVariant>
#include <QString>

namespace OOLUA {

    namespace STRING {
        //Template specialization allowing OOLUA to push a QString to Lua as a character array
        template<>
        inline bool push<QString>(lua_State* state, QString const& string) {
            return OOLUA::push(state, string.toStdString().c_str());
        }

        //Template specialization allowing OOLUA to pull a character array from Lua as a QString
        template<>
        inline bool pull<QString>(lua_State* state, QString& string) {
            string = luaL_tolstring(state, -1, nullptr); //stringify ANY type
            lua_pop(state, 2);
            return (string.isNull())? true : false;
        }
    }

    template<>
    inline bool push<QVariant>(lua_State* state, QVariant const& variant) {

        switch(variant.type()) {
        case QMetaType::Bool:
            return push(state, variant.toBool());
        case QMetaType::QString:
            return push(state, variant.toString());
        default:
            return push(state, variant.toFloat());
        }
    }

    template<>
    inline bool pull<QVariant>(lua_State* state, QVariant& variant) {
        QString     stringVal;
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

//Overload this bitch for  Lua QVariant-indexed hashtables
inline uint qHash( const QVariant & variant ) {
    switch(variant.type()) {
    case QMetaType::Bool:
        return qHash(variant.toBool());
    default:
        if(variant.canConvert(QMetaType::Float)) {
            return qHash(variant.toFloat());
        } else {
            return qHash(variant.toString());
        }
    }
}



#endif // TK_LUA_QT_UTILS_H

