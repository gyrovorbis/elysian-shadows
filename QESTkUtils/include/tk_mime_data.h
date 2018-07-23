#ifndef TK_MIME_DATA_H
#define TK_MIME_DATA_H

#include <QMimeData>
#include <crtp/elysian_lua_variant.hpp>
#include <utility/elysian_bit_mask.hpp>

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
        TYPE_MIME_DATA_ARRAY,
        TYPE_LUA_VARIANT
    };

    enum LUA_ATTRIB_BIT {
        LUA_ATTRIB_BIT_PREFAB
    };

                                TkMimeData(void) = default;
                                TkMimeData(int type);
                                TkMimeData(LuaVariant v);
                                TkMimeData(int type, LuaVariant v);
    virtual                     ~TkMimeData(void);

    const LuaVariant&           getVariant(void) const;
    void                        setVariant(LuaVariant v);

    int                         getType(void) const;
    void                        setType(const TYPE type);
    bool                        isVariant(void) const;
    bool                        isPrefab(void) const;
    bool                        isArray(void) const;

    virtual bool                hasFormat(const QString& mimetype) const override;
    virtual QStringList         formats(void) const override;

protected:

    void                        _updateLuaAttribMask(void);
    virtual QVariant            retrieveData(const QString& mimetype, QVariant::Type preferredType) const override;


private:

    const static QStringList    _formatsList;

    LuaVariant                  _variant;
    int                         _type       = TYPE_LUA_VARIANT;
    elysian::BitMask<uint32_t>  _luaAttribMask;
};







// ======== INLINEZ =========

inline TkMimeData::TkMimeData(int type): _type(type) {}
inline TkMimeData::TkMimeData(LuaVariant v): _variant(std::move(v)), _type(TYPE_LUA_VARIANT) { _updateLuaAttribMask(); }
inline TkMimeData::TkMimeData(int type, LuaVariant v): _variant(std::move(v)), _type(type) {}
inline const LuaVariant& TkMimeData::getVariant(void) const { return _variant; }
inline void TkMimeData::setVariant(LuaVariant v) { _variant = std::move(v); _type = TYPE_LUA_VARIANT; _updateLuaAttribMask(); }
inline int TkMimeData::getType(void) const { return _type; }
inline void TkMimeData::setType(const TYPE type) { _type = type; }
inline bool TkMimeData::isVariant(void) const { return _type == TYPE_LUA_VARIANT; }
inline bool TkMimeData::isArray(void) const { return _type == TYPE_MIME_DATA_ARRAY; }
inline bool TkMimeData::isPrefab(void) const { return _luaAttribMask.getBit(LUA_ATTRIB_BIT_PREFAB); }


#endif // TK_MIME_DATA_H
