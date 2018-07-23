#ifndef TK_PLAIN_TEXT_EDIT_PROPERTY_ITEM_DELEGATE_H
#define TK_PLAIN_TEXT_EDIT_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>

namespace QESTkUtils {


    class PlainTextEditPropertyItemDelegate: public DynamicContentPropertyItemDelegate {
    public:

        virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
        virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
        virtual void    updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        virtual void    paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        //virtual QSize   sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;



    };



}


#endif // TK_PLAIN_TEXT_EDIT_PROPERTY_ITEM_DELEGATE_H
