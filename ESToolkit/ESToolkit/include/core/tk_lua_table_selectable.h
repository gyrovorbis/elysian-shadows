#ifndef TK_LUA_TABLE_SELECTABLE_H
#define TK_LUA_TABLE_SELECTABLE_H

#include <core/tk_selectable.h>
#include <model/tk_lua_table_stack_model.h>

class LuaTableSelectable: public LuaTableStackModel, public Selectable {
public:
                                LuaTableSelectable(LuaVariant ref);
    virtual Selectable::TYPE    getType(void) const override;
    virtual QAbstractItemModel* getModel(void) const override;
    virtual void                setCurrentSelection(const bool val, const Selectable*const other=nullptr) override;
    virtual bool                isTemporary(void) const override;

};



// ==== INLINEZ ====

inline LuaTableSelectable::LuaTableSelectable(LuaVariant ref): LuaTableStackModel(std::move(ref)) {}
inline Selectable::TYPE LuaTableSelectable::getType(void) const { return Selectable::LUA_TABLE; }
inline QAbstractItemModel* LuaTableSelectable::getModel(void) const { return const_cast<LuaTableSelectable*>(this); }
inline void LuaTableSelectable::setCurrentSelection(const bool val, const Selectable * const /*other*/) {
    if(val) populateModel();
}
inline bool LuaTableSelectable::isTemporary(void) const { return true; }


#endif // TK_LUA_TABLE_SELECTABLE_H
