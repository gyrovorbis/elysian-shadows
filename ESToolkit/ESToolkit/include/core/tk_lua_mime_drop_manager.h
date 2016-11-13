#ifndef TK_LUA_MIME_DROP_MANAGER_H
#define TK_LUA_MIME_DROP_MANAGER_H

#include <core/tk_mime_drop_manager.h>
#include <crtp/elysian_lua_variant.hpp>
#include <QMap>


class LuaMimeDropManager: public MimeDropManager {
public:
    using   LuaDropCbType            = std::function<bool(LuaVariant, Qt::DropAction, QDropEvent*, int, int, const QModelIndex&)>;
    using   LuaCanDropCbType         = std::function<bool(LuaVariant, Qt::DropAction, QDragEnterEvent*, int, int, const QModelIndex&)>;

protected:
    struct LuaHandler {
        LuaDropCbType       dropCallback;
        LuaCanDropCbType    canDropCallback = LuaCanDropCbType();
        QString             typeName;
        Qt::DropActions     actions;
    };

    QMap<QString, LuaHandler>   _luaHandlers;

    bool                    _dropCallback(const QMimeData* data, Qt::DropAction action, QDropEvent* event, int row, int col, const QModelIndex& parent);
    bool                    _canDropCallback(const QMimeData* data, Qt::DropAction action, QDragEnterEvent* event, int row, int col, const QModelIndex& parent);


public:
                            LuaMimeDropManager(void) = default;
                            LuaMimeDropManager(QVector<Handler>&& handlers);

    void                    addLuaTypeHandler(QString typeName, LuaDropCbType dropCallback, LuaCanDropCbType canDropCallback, Qt::DropActions actions);
    void                    removeLuaTypeHandler(QString typeName);

    static bool             mimeDataHasLuaTypeName(QString typeName, const QMimeData* data);

};


inline void LuaMimeDropManager::addLuaTypeHandler(QString typeName, LuaDropCbType dropCallback, LuaCanDropCbType canDropCallback, Qt::DropActions actions) {
}

inline void LuaMimeDropManager::removeLuaTypeHandler(QString typeName) {

}






#endif // TK_LUA_MIME_DROP_MANAGER_H
