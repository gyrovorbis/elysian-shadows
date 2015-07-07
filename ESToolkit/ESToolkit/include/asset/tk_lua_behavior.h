#ifndef TK_LUA_BEHAVIOR_H
#define TK_LUA_BEHAVIOR_H

#include <elysian_lua_behavior_base.hpp>

class TkLuaBehavior: public elysian::LuaBehaviorBase {
public:

                    TkLuaBehavior(TkEntity& parent);
                    TkLuaBehavior(void) = default;
                    TkLuaBehavior(Entity& entity, const char *script, const OOLUA::Lua_table_ref paramTable);

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;

    QString         getScriptName(void) const;

};


inline TkLuaBehavior::TkLuaBehavior(TkEntity &parent): elysian::LuaBehaviorBase(parent) {}

inline TkLuaBehavior::TkLuaBehavior(Entity &entity, const char *script, const OOLUA::Lua_table_ref paramTable):
    elysian::LuaBehaviorBase(entity, script, paramTable) {}


#endif // TK_LUA_BEHAVIOR_H

