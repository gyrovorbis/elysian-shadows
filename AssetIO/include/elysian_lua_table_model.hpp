#ifndef ELYSIAN_LUA_TABLE_MODEL_H
#define ELYSIAN_LUA_TABLE_MODEL_H

#include "luatables.h"
#include "elysian_lua_manager.hpp"

namespace elysian {

    class AbstractModel {


        virtual bool saveModel(FILE*const file) = 0;
        virtual bool loadModel(FILE*const file) = 0;

    };

    class LuaTableModel: public AbstractModel, public LuaTable {
    private:

    public:

        //LuaTableModel()

        LuaTableModel getParentModel(void) const;
        LuaTableModel getChildModel(const char* key) const;

        LuaTableModel pushModel(const char* key) const;
        LuaTableModel popModel(void) const;

        virtual bool saveModel(FILE*const file)  override;
        virtual bool loadModel(FILE*const file) override;

        virtual void updateModel(void) const {}
        virtual void doneLoadingModel(void) const {}

        const char* getKey(void) const;

    };

}

#endif // ELYSIAN_LUA_TABLE_MODEL_H
