#ifndef TK_FILE_BROWSER_PROPERTY_ITEM_DELEGATE_H
#define TK_FILE_BROWSER_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <functional>

#define FILE_BROWSER_PROPERTY_ITEM_BROWSE_ICON_PATH  ":/misc/images/browse_file_icon.ico"

class QPushButton;
class QHBoxLayout;

class FileBrowserPropertyItemDelegate: public DynamicContentPropertyItemDelegate {
public:
    //using ValidatorCallbackType             = std::function<bool(ESTkStandardItem*)>;
private:
    mutable QLineEdit*      _pathEdit       = nullptr;
    mutable QPushButton*    _browseButton   = nullptr;
    mutable QWidget*        _parentWidget   = nullptr;
    mutable QHBoxLayout*    _layout         = nullptr;
    bool                    _dir            = false;

protected:
    //virtual QWidget*        createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    //virtual void            setEditorData(QWidget *editor, const QModelIndex &index) const override;
    //virtual void            setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    //virtual void            updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;

public:

                            FileBrowserPropertyItemDelegate(void);
                            FileBrowserPropertyItemDelegate(TextCallbackType textCallback);

        void                setDir(const bool val);

};







inline void FileBrowserPropertyItemDelegate::setDir(const bool val) {
    _dir = val;
}



#endif // TK_FILE_BROWSER_PROPERTY_ITEM_DELEGATE_H
