#ifndef LUA_EXPLORER_DIALOG_H
#define LUA_EXPLORER_DIALOG_H

#include <QDialog>
#include "elysian_lua_manager.hpp"

namespace Ui {
    class LuaExplorerDialog;
}

class QAbstractItemModel;

class LuaExplorerDialog: public QDialog, public elysian::LuaManager {
private:
    Ui::LuaExplorerDialog*  _ui;

public:

    explicit    LuaExplorerDialog(QWidget* parent=nullptr);

    void        setModel(QAbstractItemModel*const model);




};


#endif // LUAEXPLORERDIALOG_H
