#include "tk_lua_explorer_dialog.h"
#include "ui_tk_lua_explorer.h"
//#include "luaTableStandardItemModel.h"
#include <QAbstractItemModel>
#include <QStyledItemDelegate.h>
#include <QLabel>

LuaExplorerDialog::LuaExplorerDialog(QWidget *parent): QDialog(parent), _ui(new Ui::LuaExplorerDialog) {
    _ui->setupUi(this);

    _ui->columnView->setItemDelegate(new QStyledItemDelegate);
    _ui->columnView->setPreviewWidget(new QLabel("LOLOOLPREVIEW"));
}


void LuaExplorerDialog::setModel(QAbstractItemModel* const model) {
    _ui->columnView->setModel(model);
    _ui->columnView->setPreviewWidget(new QLabel("LOLOOLPREVIEW"));
}
