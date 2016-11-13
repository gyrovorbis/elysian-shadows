#include <core/tk_lua_mime_drop_manager.h>
#include <core/tk_mime_data.h>


LuaMimeDropManager::LuaMimeDropManager(QVector<Handler>&& handlers):
    MimeDropManager(std::move(handlers))

{
    addHandler(TkMimeData::TYPE_LUA_VARIANT,
               std::bind(&LuaMimeDropManager::_dropCallback, this),
               std::bind(&LuaMimeDropManager::_canDropCallback, this),
               Qt::ActionMask);
}

bool LuaMimeDropManager::_canDropCallback(const QMimeData *data, Qt::DropAction action, QDragEnterEvent *event, int row, int col, const QModelIndex &parent) {
    auto* tkData = qobject_cast<const TkMimeData*>(data);
    auto it = _luaHandlers.find(tkData->getVariant().getTypeString());
    if(it != _luaHandlers.end() && it->canDropCallback) {
        if(it->actions & action) {
            return it->canDropCallback(tkData->getVariant(), action, event, row, col, parent);
        }
    }
    return false;
}

bool LuaMimeDropManager::_dropCallback(const QMimeData *data, Qt::DropAction action, QDropEvent *event, int row, int col, const QModelIndex &parent) {
    auto* tkData = qobject_cast<const TkMimeData*>(data);
    auto it = _luaHandlers.find(tkData->getVariant().getTypeString());
    if(it != _luaHandlers.end()) {
        if(it->actions & action) {
            return it->dropCallback(tkData->getVariant(), action, event, row, col, parent);
        }
    }
    return false;
}
