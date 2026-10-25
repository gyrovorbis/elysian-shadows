#include <QESTkUtils/tk_mime_data.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <ESToolkit/ESGamma/tk_lua_manager.h>

const QStringList TkMimeData::_formatsList = {
    "text/plain"
};

TkMimeData::~TkMimeData(void) {
    //we need to delete each individual element of QMimeData for array types.
    if(_type == TYPE_MIME_DATA_ARRAY) {
        const int length = _variant.getLength();
        for(int i = 1; i <= length; ++i) {
            QMimeData* data = static_cast<QMimeData*>(_variant.getValue(i).getValue<void*>());
            delete data;
        }
    }
}

QVariant TkMimeData::retrieveData(const QString& mimeType, QVariant::Type /*preferredType*/) const {
    if(mimeType == "text/plain") {
        return QString(LuaManager::callGlobalFunction<const char*>("serialize", nullptr, _variant));
    }
    return QVariant();
}

QStringList TkMimeData::formats(void) const {
    QStringList formats = _formatsList;
    if(strcmp(_variant.getTypeString(), "Entity") == 0) {
        //Entity* entity = _variant.getValue<Entity*>();
        //if(!entity->getTexturePath().isNull()) formats << QString("");
    } else if(strcmp(_variant.getTypeString(), "Color4") == 0) {
        //formats << QString("");
    }

    return formats;
}

bool TkMimeData::hasFormat(const QString& mimeType) const {
    return _formatsList.contains(mimeType);
}

void TkMimeData::_updateLuaAttribMask(void) {
    if(isVariant() && LuaManager::getPrefabName(_variant) != nullptr) _luaAttribMask.setBit(LUA_ATTRIB_BIT_PREFAB, true);
}


