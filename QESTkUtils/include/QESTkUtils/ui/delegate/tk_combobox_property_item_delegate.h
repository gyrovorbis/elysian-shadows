#ifndef TK_COMBOBOX_PROPERTY_ITEM_DELEGATE_H
#define TK_COMBOBOX_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <functional>
#include <QStringList>

class ESTkStandardItem;


class ComboBoxPropertyItemDelegate: public DynamicContentPropertyItemDelegate {
    using PopulatorType = std::function<QStringList(ESTkStandardItem*)>;

private:
    PopulatorType   _populator;
    bool            _editable   = false;

public:
                    ComboBoxPropertyItemDelegate(PopulatorType callback, bool editable=false);
                    ComboBoxPropertyItemDelegate(void) = default;

    void            setPopulator(PopulatorType callback);
    void            setEditable(bool editable);

    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

};



inline ComboBoxPropertyItemDelegate::ComboBoxPropertyItemDelegate(PopulatorType callback, bool editable):
    _populator(std::move(callback)),
    _editable(editable) {}


inline void ComboBoxPropertyItemDelegate::setPopulator(PopulatorType callback) {
    _populator = std::move(callback);
}

inline void ComboBoxPropertyItemDelegate::setEditable(bool editable) {
    _editable = editable;
}


#endif // TK_COMBOBOX_PROPERTY_ITEM_DELEGATE_H
