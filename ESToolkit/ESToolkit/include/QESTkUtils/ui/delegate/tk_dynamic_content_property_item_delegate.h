#ifndef TK_DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_H
#define TK_DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_property_item_delegate.h>
#include <QESTkUtils/model/tk_lua_table_stack_model.h>
#include <functional>
#include <QIcon>
#include <QVector>

#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING       2
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN        5
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_TOOLTIP_TIME_DEFAULT 2000
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH       ":/misc/images/plus_icon.ico"
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_MINUS_PATH      ":/misc/images/minus_icon.ico"
#define DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_POSITION_PATH   ":/misc/images/position_icon.png"

class DynamicContentPropertyItemDelegate: public PropertyItemDelegate {
public:
    using TextCallbackType      = std::function<void(ESTkStandardItem*, QStyleOptionViewItem&)>;
    using ButtonCallbackType    = std::function<void(ESTkStandardItem*)>;
    using UseButtonCallbackType = std::function<bool(ESTkStandardItem*)>;
    using DeleteCallbackType    = std::function<void(ESTkStandardItem*)>;

protected:
    struct Button {
        QIcon                   icon;
        ButtonCallbackType      callback;
        UseButtonCallbackType   useButtonCallback;
        bool                    persistent;
        QString                 toolTip;
    };

    QVector<Button>         _buttons;
    TextCallbackType        _textCallback;
    DeleteCallbackType      _deleteCallback;
    QString                 _toolTip;
    int                     _toolTipTime = DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_TOOLTIP_TIME_DEFAULT;

    static void             _luaArraySizeTextCallback(ESTkStandardItem* item, QStyleOptionViewItem& option, bool emptyInvalid=false);

    QVector<bool>           _getUseButtonData(ESTkStandardItem* item, unsigned& useButtonCount) const;

public:

    template <bool EmptyInvalid=false>
    static void             luaArraySizeTextCallback(ESTkStandardItem* item, QStyleOptionViewItem& option);
    template<typename T>
    static void             luaAddItemButtonCallback(ESTkStandardItem* item);


                            DynamicContentPropertyItemDelegate(TextCallbackType callback);
                            DynamicContentPropertyItemDelegate(void) = default;

    void                    setTextCallback(TextCallbackType callback);
    void                    setDeleteCallback(DeleteCallbackType callback);
    void                    addButton(QIcon icon, ButtonCallbackType callback, QString tooltip=QString(), bool persistent=false, UseButtonCallbackType useCallback=UseButtonCallbackType());
    DeleteCallbackType      getDeleteCallback(void) const;
    QString                 getToolTip(void) const;
    void                    setToolTip(QString str);

    virtual void            paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual bool            editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

};





//========== INLINEZ ===========


inline DynamicContentPropertyItemDelegate::DynamicContentPropertyItemDelegate(TextCallbackType callback):
    _textCallback(std::move(callback)) {}


inline void DynamicContentPropertyItemDelegate::setTextCallback(TextCallbackType callback) {
    _textCallback = std::move(callback);
}

inline void DynamicContentPropertyItemDelegate::setDeleteCallback(DeleteCallbackType callback) {
    _deleteCallback = std::move(callback);
}

inline auto DynamicContentPropertyItemDelegate::getDeleteCallback(void) const -> DeleteCallbackType {
    return _deleteCallback;
}

inline QString DynamicContentPropertyItemDelegate::getToolTip(void) const { return _toolTip; }
inline void DynamicContentPropertyItemDelegate::setToolTip(QString str) { _toolTip = str; }

inline void DynamicContentPropertyItemDelegate::addButton(QIcon icon, ButtonCallbackType callback, QString tooltip, bool persistent, UseButtonCallbackType useCallback) {
    _buttons.push_back(Button {
                            std::move(icon),
                            std::move(callback),
                            std::move(useCallback),
                            persistent,
                            tooltip
                       });

}

template<typename T>
inline void DynamicContentPropertyItemDelegate::luaAddItemButtonCallback(ESTkStandardItem* item) {
    LuaTableFieldItem* luaItem = static_cast<LuaTableFieldItem*>(item);
    const LuaVariant& table = luaItem->getTableData()->getTableRef();
    table[table.getLength()+1].setVariant(new T);
}

template<bool EmptyInvalid>
inline void DynamicContentPropertyItemDelegate::luaArraySizeTextCallback(ESTkStandardItem *item, QStyleOptionViewItem& option) {
    _luaArraySizeTextCallback(item, option, EmptyInvalid);
}



#endif // TK_DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_H

