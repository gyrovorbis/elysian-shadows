#ifndef TK_LUA_TABLE_SELECTABLE_H
#define TK_LUA_TABLE_SELECTABLE_H

#include <core/tk_selectable.h>
#include <model/tk_lua_table_stack_model.h>

class LuaTableSelectable: public LuaTableStackModel, public Selectable {
public:
                                LuaTableSelectable(const LuaVariant& ref);
    virtual Selectable::TYPE    getType(void) const override;
    virtual QAbstractItemModel* getModel(void) const override;
    virtual void                setCurrentSelection(const bool val, const Selectable*const other=nullptr) override;

};



// ==== INLINEZ ====

inline LuaTableSelectable::LuaTableSelectable(const LuaVariant& ref): LuaTableStackModel(ref) {}
inline Selectable::TYPE LuaTableSelectable::getType(void) const { return Selectable::LUA_TABLE; }
inline QAbstractItemModel* LuaTableSelectable::getModel(void) const { return const_cast<LuaTableSelectable*>(this); }
inline void LuaTableSelectable::setCurrentSelection(const bool val, const Selectable * const /*other*/) {
    if(val) populateModel();
}



#endif // TK_LUA_TABLE_SELECTABLE_H
