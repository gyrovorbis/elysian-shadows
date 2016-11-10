#ifndef TK_DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_H
#define TK_DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_H

#include <delegate/tk_property_item_delegate.h>
#include <model/tk_lua_table_stack_model.h>
#include <functional>
#include <QIcon>
#include <QVector>


#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING   2
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN    5
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH   ":/misc/images/plus_icon.ico"
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_MINUS_PATH  ":/misc/images/minus_icon.ico"

class DynamicContentPropertyItemDelegate: public PropertyItemDelegate {
public:
    using TextCallbackType      = std::function<void(ESTkStandardItem*, QStyleOptionViewItem&)>;
    using ButtonCallbackType    = std::function<void(ESTkStandardItem*)>;
    using DeleteCallbackType    = std::function<void(ESTkStandardItem*)>;

private:
    struct Button {
        QIcon               icon;
        ButtonCallbackType  callback;
        bool                persistent;
    };

    QVector<Button>         _buttons;
    TextCallbackType        _textCallback;
    DeleteCallbackType      _deleteCallback;


public:

    static void             luaArraySizeTextCallback(ESTkStandardItem* item, QStyleOptionViewItem& option);
    template<typename T>
    static void             luaAddItemButtonCallback(ESTkStandardItem* item);


                            DynamicContentPropertyItemDelegate(TextCallbackType callback);
                            DynamicContentPropertyItemDelegate(void) = default;

    void                    setTextCallback(TextCallbackType callback);
    void                    setDeleteCallback(DeleteCallbackType callback);
    void                    addButton(QIcon icon, ButtonCallbackType callback, bool persistent=false);

    virtual void            paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual bool            editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    virtual bool            eventFilter(QObject* editor, QEvent* event) override;

};







inline DynamicContentPropertyItemDelegate::DynamicContentPropertyItemDelegate(TextCallbackType callback):
    _textCallback(std::move(callback)) {}


inline void DynamicContentPropertyItemDelegate::setTextCallback(TextCallbackType callback) {
    _textCallback = std::move(callback);
}

inline void DynamicContentPropertyItemDelegate::setDeleteCallback(DeleteCallbackType callback) {
    _deleteCallback = std::move(callback);
}

inline void DynamicContentPropertyItemDelegate::addButton(QIcon icon, ButtonCallbackType callback, bool persistent) {
    _buttons.push_back(Button {
                            std::move(icon),
                            std::move(callback),
                            persistent
                       });

}

template<typename T>
inline void DynamicContentPropertyItemDelegate::luaAddItemButtonCallback(ESTkStandardItem* item) {
    LuaTableFieldItem* luaItem = static_cast<LuaTableFieldItem*>(item);
    const LuaVariant& table = luaItem->getTableData()->getTableRef();
    table[table.getLength()+1].setVariant(new T);
}

#endif // TK_DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_H

