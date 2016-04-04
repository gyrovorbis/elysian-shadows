#ifndef TK_MIME_DATA_H
#define TK_MIME_DATA_H

#include <QMimeData>
#include <crtp/elysian_lua_variant.hpp>

/*
 * Future work:
 * - Lua table      -> HTML
 * - Lua table      -> CSV
 * - Ud GYTexture   -> imageData
 * - Ud GYColor     -> colorData
 * - File paths     -> QUrlList
 */


class TkMimeData: public QMimeData {
    Q_OBJECT
public:

    enum TYPE {
        TYPE_LUA_VARIANT,
        TYPE_MIME_DATA_ARRAY,
        TYPE_LUA_PREFAB,
        TYPE_LUA_ENTITY_PREFAB
    };

                                TkMimeData(void) = default;
                                TkMimeData(const TYPE type);
                                TkMimeData(LuaVariant v);
                                TkMimeData(const TYPE type, LuaVariant v);
    virtual                     ~TkMimeData(void);

    const LuaVariant&           getVariant(void) const;
    void                        setVariant(LuaVariant v);

    TYPE                        getType(void) const;
    void                        setType(const TYPE type);

    virtual bool                hasFormat(const QString& mimetype) const override;
    virtual QStringList         formats(void) const override;

protected:

    virtual QVariant            retrieveData(const QString& mimetype, QVariant::Type preferredType) const override;

private:

    const static QStringList    _formatsList;

    LuaVariant                  _variant;
    TYPE                        _type       = TYPE_LUA_VARIANT;
};







// ======== INLINEZ =========

inline TkMimeData::TkMimeData(const TYPE type): _type(type) {}
inline TkMimeData::TkMimeData(LuaVariant v): _variant(std::move(v)) {}
inline TkMimeData::TkMimeData(const TYPE type, LuaVariant v): _variant(std::move(v)), _type(type) {}
inline const LuaVariant& TkMimeData::getVariant(void) const { return _variant; }
inline void TkMimeData::setVariant(LuaVariant v) { _variant = std::move(v); }
inline auto TkMimeData::getType(void) const -> TYPE { return _type; }
inline void TkMimeData::setType(const TYPE type) { _type = type; }



#endif // TK_MIME_DATA_H
