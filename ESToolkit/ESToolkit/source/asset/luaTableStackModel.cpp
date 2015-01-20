#include "luaTableStackModel.h"

LuaTableItem::LuaTableItem(const int ref, const int type, const QStandardItem* label):
    _luaRef(ref), _luaType(type), _labelItem(label)
{
    ParentType::_latentInit(this, &LuaTableItem::getValue, nullptr, UndoItem::LUA_TABLE_FIELD_ITEM, nullptr);
}

LuaTableItem::~LuaTableItem(void) { /*unref*/ }

QString LuaTableItem::getValue(void) const {

    if(!_populated) {
        auto* tableModel = static_cast<LuaTableStackModel*>(model());

        lua_rawgeti(*LuaTableStackModel::_mainState, LUA_REGISTRYINDEX, _luaRef);

        tableModel->pushItem(const_cast<QStandardItem*>(_labelItem)); //Fuckin' gross casting
        tableModel->_addTableFields(*LuaTableStackModel::_mainState, _luaRef);
        tableModel->popItem();

        lua_pop(*LuaTableStackModel::_mainState, 1);
        _populated = true;
    }

    return (_luaType == LUA_TTABLE)? "TABLE" : "USERDATA";
}


void LuaTableItem::_onFieldChanged(QString field) {
    model()->itemChanged(_fieldItems[field]);
}
