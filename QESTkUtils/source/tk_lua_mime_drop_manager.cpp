#include <QESTkUtils/tk_lua_mime_drop_manager.h>
#include <QESTkUtils/tk_mime_data.h>


LuaMimeDropManager::LuaMimeDropManager(QMap<QString, LuaHandler>&& handlers):
    _luaHandlers(std::move(handlers))
{
    using namespace std::placeholders;

    addHandler(TkMimeData::TYPE_LUA_VARIANT,
               std::bind(&LuaMimeDropManager::_dropCallback, this, _1, _2, _3, _4, _5, _6),
               std::bind(&LuaMimeDropManager::_canDropCallback, this, _1, _2, _3, _4, _5, _6),
               Qt::ActionMask);
}

bool LuaMimeDropManager::_canDropCallback(const QMimeData *data, Qt::DropAction action, QDragEnterEvent *event, int row, int col, const QModelIndex &parent) {
    auto* tkData = qobject_cast<const TkMimeData*>(data);
    auto it = _luaHandlers.find(tkData->getVariant().getTypeString());
    if(it != _luaHandlers.end()) {
        //if(it->actions & action) {
            return it->canDropCallback?
                        it->canDropCallback(tkData->getVariant(), action, event, row, col, parent) :
                        true;
       // }
    }
    return false;
}

bool LuaMimeDropManager::_dropCallback(const QMimeData *data, Qt::DropAction action, QDropEvent *event, int row, int col, const QModelIndex &parent) {
    auto* tkData = qobject_cast<const TkMimeData*>(data);
    auto it = _luaHandlers.find(tkData->getVariant().getTypeString());
    if(it != _luaHandlers.end()) {
       // if(it->actions & action) {
            return it->dropCallback(tkData->getVariant(), action, event, row, col, parent);
        }
   // }
    return false;
}
