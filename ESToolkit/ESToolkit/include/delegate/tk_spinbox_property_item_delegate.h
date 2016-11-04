#ifndef TK_SPINBOX_PROPERTY_ITEM_DELEGATE_H
#define TK_SPINBOX_PROPERTY_ITEM_DELEGATE_H

#include <delegate/tk_property_item_delegate.h>


class SpinboxPropertyItemDelegate: public PropertyItemDelegate {
private:
    int     _min = 0;
    int     _max = 100;

protected:
    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;


public:

                    SpinboxPropertyItemDelegate(void) = default;
                    SpinboxPropertyItemDelegate(const int max, const int min);

    int             getMinValue(void) const;
    int             getMaxValue(void) const;
    void            setMinValue(const int min);
    void            setMaxValue(const int max);



};







inline SpinboxPropertyItemDelegate::SpinboxPropertyItemDelegate(const int min, const int max): PropertyItemDelegate(), _min(min), _max(max) {}

inline void SpinboxPropertyItemDelegate::setMinValue(const int min) { _min = min; }
inline void SpinboxPropertyItemDelegate::setMaxValue(const int max) { _max = max; }

#endif // TK_SPINBOX_PROPERTY_ITEM_DELEGATE_H
