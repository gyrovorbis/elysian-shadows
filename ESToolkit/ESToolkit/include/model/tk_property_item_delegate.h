#ifndef TK_PROPERTY_ITEM_DELEGATE_H
#define TK_PROPERTY_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

class PropertyItemDelegate: public QStyledItemDelegate {

    virtual QWidget createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const = 0;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const = 0;
    virtual void    editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) = 0;

};


#endif // TK_PROPERTY_ITEM_DELEGATE_H
