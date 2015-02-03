#include "luaTableStackModel.h"

LuaTableItem::LuaTableItem(const int ref, /*const int type,*/ const QStandardItem* label):
    _luaRef(ref), /*_luaType(type),*/ _labelItem(label)
{}


void LuaTableItem::insertingModelItem(void) {
    ParentType::_latentInit(this, &LuaTableItem::getValue, nullptr, UndoItem::LUA_TABLE_FIELD_ITEM, nullptr);
}

LuaTableItem::~LuaTableItem(void) { /*unref*/ }

QString LuaTableItem::getValue(void) const {

    if(!_populated) {
        const_cast<LuaTableItem*>(this)->_populateFieldItems(static_cast<PropertyStackModel*>(model()));
        _populated = true;
    }

    //RETURN ACTUAL LUA_TOLSTRING HERE, BECAUSE IT HAS HEXADECIMAL ADDRESSES AND SHIT
    return (_luaType == LUA_TTABLE)? "TABLE" : "USERDATA";
}


void LuaTableItem::onFieldChanged(QString field) {
    _fieldItems[field]->emitDataChanged();
    //model()->itemChanged(_fieldItems[field]);
}
