#ifndef TK_FILE_BROWSER_PROPERTY_ITEM_DELEGATE_H
#define TK_FILE_BROWSER_PROPERTY_ITEM_DELEGATE_H

#include <delegate/tk_property_item_delegate.h>

class QPushButton;
class QHBoxLayout;

class FileBrowserPropertyItemDelegate: public PropertyItemDelegate {
private:
    mutable QLineEdit*      _pathEdit       = nullptr;
    mutable QPushButton*    _browseButton   = nullptr;
    mutable QWidget*        _parentWidget   = nullptr;
    mutable QHBoxLayout*    _layout         = nullptr;
    bool                    _dir            = false;

protected:
    virtual QWidget*        createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void            setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void            setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;

public:

                            FileBrowserPropertyItemDelegate(void);

        void                setDir(const bool val);

};




inline void FileBrowserPropertyItemDelegate::setDir(const bool val) {
    _dir = val;
}



#endif // TK_FILE_BROWSER_PROPERTY_ITEM_DELEGATE_H
