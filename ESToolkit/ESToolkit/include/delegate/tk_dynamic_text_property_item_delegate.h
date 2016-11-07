#ifndef TK_DYNAMIC_TEXT_PROPERTY_ITEM_DELEGATE_H
#define TK_DYNAMIC_TEXT_PROPERTY_ITEM_DELEGATE_H

#include <delegate/tk_property_item_delegate.h>
#include <functional>

class LuaTableFieldItem;


class DynamicTextPropertyItemDelegate: public PropertyItemDelegate {
    using CallbackType = std::function<void(LuaTableFieldItem*, QStyleOptionViewItem&)>;

private:
    CallbackType   _callback;

public:
                    DynamicTextPropertyItemDelegate(CallbackType callback);
                    DynamicTextPropertyItemDelegate(void) = default;

    void            setCallback(CallbackType callback);

    virtual void    paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};



inline DynamicTextPropertyItemDelegate::DynamicTextPropertyItemDelegate(CallbackType callback):
    _callback(std::move(callback)) {}


inline void DynamicTextPropertyItemDelegate::setCallback(CallbackType callback) {
    _callback = std::move(callback);
}

#endif // TK_DYNAMIC_TEXT_PROPERTY_ITEM_DELEGATE_H

