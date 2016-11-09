#ifndef TK_PROPERTY_ITEM_PROXY_DELEGATE_H
#define TK_PROPERTY_ITEM_PROXY_DELEGATE_H

#include <QStyledItemDelegate>

class PropertyItemDelegate;

/* Global delegate used for TkPropertyStackModel. It checks whether the
 * individual PropertyItems have their own delegates installed and defers
 * to them if so. This is required since you can only install delegates
 * on the model or row level, not the item level.
 */
class PropertyItemProxyDelegate: public QStyledItemDelegate {
    static PropertyItemDelegate*    _defaultTableDelegate;

    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual bool    editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    virtual void    updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual bool    eventFilter(QObject *object, QEvent *event) override;

};


#endif // TK_PROPERTY_ITEM_PROXY_DELEGATE_H
