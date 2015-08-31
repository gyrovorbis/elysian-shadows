#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QEvent>
#include <QPushButton>
#include <QFileDialog>
#include <QMouseEvent>
#include <core/tk_file_utilities.h>
#include <QMenu>
#include <QInputDialog>
#include <QAction>
//#include <utility/elysian_lua_table_ref.hpp>

#include <model/tk_property_item_delegate.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_lua_table_stack_model.h>
#include <core/tk_menu_stack.h>

PropertyItemDelegate* PropertyItemProxyDelegate::_defaultTableDelegate = nullptr;


QWidget* PropertyItemProxyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->createEditor(parent, option, index);
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}


void PropertyItemProxyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));


    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setEditorData(editor, index);
    } else {
        return QStyledItemDelegate::setEditorData(editor, index);
    }

}


void PropertyItemProxyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));


    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setModelData(editor, model, index);
    } else {
        return QStyledItemDelegate::setModelData(editor, model, index);
    }
}

bool PropertyItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    auto* propModel =  qobject_cast<const PropertyStackModel*>(model);
    auto* item = propModel? propModel->itemFromIndex(index) : nullptr;
    auto* propItem = (item && item->type() >= QStandardItem::UserType)? static_cast<ESTkStandardItem*>(item) : nullptr;

    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::RightButton) {
            auto* menu = getContextMenu(propItem);
            if(menu) menu->exec(mouseEvent->globalPos());
            return true;
        }

    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}


bool PropertyItemProxyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

    auto* propModel =  qobject_cast<const PropertyStackModel*>(model);
    auto* item = propModel? propModel->itemFromIndex(index) : nullptr;
    auto* propItem = (item && item->type() >= QStandardItem::UserType)? static_cast<ESTkStandardItem*>(item) : nullptr;

    if(propItem) {
        if(propItem->getStyledItemDelegate()) {
            return static_cast<PropertyItemDelegate*>(propItem->getStyledItemDelegate())->editorEvent(event, model, option, index);
        }

        auto* luaItem = (propItem->type() == (int)ESTkStandardItemType::LuaTable)? static_cast<LuaTableFieldItem*>(propItem) : nullptr;
        if (luaItem && (luaItem->getValueLuaType() == LUA_TTABLE || luaItem->getParentLuaType() == LUA_TTABLE)) {
            if(!_defaultTableDelegate) {
                _defaultTableDelegate = new FileBrowserPropertyItemDelegate;
            }
            return _defaultTableDelegate->editorEvent(event, model, option, index);

        }

    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);



}



QWidget* EnumDropdownPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* editor = new QComboBox(parent);
    editor->setFocusPolicy(Qt::WheelFocus);

    for(unsigned i = 0; i < _stringNames.size(); ++i) {
        editor->addItem(_stringNames[i], i);
    }
    return editor;
}


void EnumDropdownPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(combo) combo->setCurrentIndex(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void EnumDropdownPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    if(combo) {
        auto* propModel = static_cast<const PropertyStackModel*>(model);
        auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

        item->setData(_stringNames[combo->currentIndex()], Qt::DisplayRole);
        item->setData(combo->currentIndex(), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}




QWidget* SpinboxPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* editor = new QSpinBox(parent);
    editor->setMaximum(_max);
    editor->setMinimum(_min);
    return editor;
}


void SpinboxPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QSpinBox* spinbox = qobject_cast<QSpinBox*>(editor);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(spinbox) spinbox->setValue(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void SpinboxPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QSpinBox* spinbox = qobject_cast<QSpinBox*>(editor);

    if(spinbox) {
        auto* propModel = static_cast<const PropertyStackModel*>(model);
        auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

        item->setData(spinbox->value(), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}


FileBrowserPropertyItemDelegate::FileBrowserPropertyItemDelegate(void) {

}

QWidget* FileBrowserPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    _parentWidget   = new QWidget(parent);
    _layout         = new QHBoxLayout;
    _browseButton   = new QPushButton("..");
    _pathEdit       = new QLineEdit;

    //_layout->setStretch(-1, -1);
    //_layout->setSpacing(-1);
    _layout->setMargin(0);
    _layout->addWidget(_pathEdit);
    _layout->addWidget(_browseButton);
    _pathEdit->setFont(QFont(_pathEdit->font().family(), 10));
    _pathEdit->setContentsMargins(0, 0, 0, 0);
    _pathEdit->setFixedHeight(15);
    _browseButton->setFixedSize(15, 15);

    _parentWidget->setLayout(_layout);
    _parentWidget->setFocusPolicy(Qt::StrongFocus);
    _parentWidget->setAutoFillBackground(true);

    QObject::connect(_browseButton, &QPushButton::pressed, [=](void) {
       QString str = _dir ? QFileDialog::getExistingDirectory(nullptr, tr("Locate Project Directory"), "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) :
                            QFileDialog::getOpenFileName(nullptr, tr("Locate File"), "./");
       if(!str.isNull()) {
            QString relFile = FileUtilities::getAssetRelativePath(str);
            _pathEdit->setText(relFile);
            item->setData(relFile, Qt::EditRole);
        }
    });


    return _parentWidget;
}

void FileBrowserPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    _pathEdit->setText(item? item->data(Qt::EditRole).toString() : "");
}

void FileBrowserPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

    item->setData(_pathEdit->text(), Qt::EditRole);

}

QMenu* PropertyItemDelegate::getContextMenu(ESTkStandardItem*const item) {
    MenuStack menu("PropertyItem");
    LuaTableFieldItem* fieldItem = (item->type() == (int)ESTkStandardItemType::LuaTable)? static_cast<LuaTableFieldItem*>(item) : nullptr;

    if(!fieldItem) return nullptr;

    menu.push("type");

    for(int i = 0; i < _supportedTypes.size(); ++i) {
        switch(_supportedTypes[i]) {
        case LUA_TNIL:
            menu.insertCheckableAction("nil", [=](const bool val) {
                fieldItem->setData("nil", Qt::EditRole);

            }, fieldItem->getValueLuaType() == LUA_TNIL);
            break;
        case LUA_TBOOLEAN:
            menu.insertCheckableAction("bool", [=](const bool val) {
                fieldItem->setData(false, Qt::EditRole);
            }, fieldItem->getValueLuaType() == LUA_TBOOLEAN);
            break;
        case LUA_TNUMBER:
            menu.insertCheckableAction("number", [=](const bool val) {
                fieldItem->setData(0, Qt::EditRole);

            }, fieldItem->getValueLuaType() == LUA_TNUMBER);
            break;
        case LUA_TSTRING:
            menu.insertCheckableAction("string", [=](const bool val) {
                fieldItem->setData("", Qt::EditRole);
            }, fieldItem->getValueLuaType() == LUA_TSTRING);
            break;
        case LUA_TTABLE:
            menu.insertCheckableAction("table", [=](const bool val) {
                //fieldItem->setData("nil", Qt::EditRole);

            }, fieldItem->getValueLuaType() == LUA_TTABLE);
            break;
        case LUA_TUSERDATA:
            break;
        }
    }

    menu.pop();

    if(fieldItem->getValueLuaType() == LUA_TTABLE) {
        menu.insertAction("Add Key", [=](bool) {
            QString key = QInputDialog::getText(nullptr, "Add New Field", "Key Name");
            if(key.isNull()) return;
#if 0
            elysian::LuaTableRef ref(fieldItem->getTableData()->getLuaRef());
            ref.insert(Q_CSTR(key), elysian::LuaVariant(12.0f));
           // fieldItem->getTableData()->insertTableFieldItem(key);
#endif

        });

    }

    _contextMenu = menu.first();

    _initialized = true;

    return _contextMenu;
}


