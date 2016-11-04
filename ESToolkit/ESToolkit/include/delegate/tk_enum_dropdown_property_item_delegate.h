#ifndef TK_ENUM_DROPDOWN_PROPERTY_ITEM_DELEGATE_H
#define TK_ENUM_DROPDOWN_PROPERTY_ITEM_DELEGATE_H

#include <delegate/tk_property_item_delegate.h>

class EnumDropdownPropertyItemDelegate: public PropertyItemDelegate {
private:
    QStringList    _stringNames;

protected:
    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;


public:

                    EnumDropdownPropertyItemDelegate(void) = default;
                    EnumDropdownPropertyItemDelegate(QStringList strings);

    void            insertEnumValue(const char* name);
};




inline EnumDropdownPropertyItemDelegate::EnumDropdownPropertyItemDelegate(QStringList strings): PropertyItemDelegate(), _stringNames(strings) {}

inline void EnumDropdownPropertyItemDelegate::insertEnumValue(const char* name) {
    _stringNames.push_back(name);
}


#endif // TK_ENUM_DROPDOWN_PROPERTY_ITEM_DELEGATE_H
