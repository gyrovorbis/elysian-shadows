#ifndef TK_PROPERTY_ITEM_DELEGATE_H
#define TK_PROPERTY_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QVector>

class PropertyItemDelegate: public QStyledItemDelegate {

    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const = 0;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const = 0;
   // virtual bool    editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) = 0;

};

class PropertyItemProxyDelegate: public QStyledItemDelegate {

    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual bool    editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;


};

class EnumDropdownPropertyItemDelegate: public PropertyItemDelegate {
private:
    QVector<QString>    _stringNames;

protected:
    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;


public:

    void insertEnumValue(const QString name);



};






// ====== INLINEZ ======

void EnumDropdownPropertyItemDelegate::insertEnumValue(const QString name) {
    _stringNames.push_back(name);
}


#endif // TK_PROPERTY_ITEM_DELEGATE_H
