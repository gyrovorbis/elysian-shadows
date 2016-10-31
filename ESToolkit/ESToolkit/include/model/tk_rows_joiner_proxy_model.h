/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * yshurik@lynxline.com wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Oleksandr Iakovliev
 * ----------------------------------------------------------------------------
 * Falco Girgis Modifications:
 * + reset() is deprecated, replaced with beginResetModel() and endResetModel()
 *
 * Thank you, my nigga. This is an elegant solution to a very fundamental
 * architectural problem. I will buy you drinks for the whole night should
 * we ever meet, friend.
 * ----------------------------------------------------------------------------
 */

#ifndef ROWS_JOINER_PROXY_MODEL_H
#define ROWS_JOINER_PROXY_MODEL_H

#include <QAbstractItemModel>

class RowsJoinerProxy : public QAbstractItemModel {
Q_OBJECT
public:
    RowsJoinerProxy(QObject *);
    virtual ~RowsJoinerProxy();

    int indexOf(QAbstractItemModel *) const;
    QList<QAbstractItemModel *> models() const;

    void insertSourceModel(QAbstractItemModel *, int idx = -1);
    void removeSourceModel(QAbstractItemModel *);

    QModelIndex mapToSource(const QModelIndex &) const;
    QModelIndex mapFromSource(const QModelIndex &) const;

    virtual QModelIndex parent(const QModelIndex &) const;
    virtual QModelIndex index(int, int, const QModelIndex & p = QModelIndex()) const;

    virtual int	rowCount(const QModelIndex & p = QModelIndex()) const;
    virtual int	columnCount(const QModelIndex & p = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual Qt::ItemFlags flags(const QModelIndex & i) const { return mapToSource(i).flags(); }
    virtual QVariant data(const QModelIndex & i, int r) const { return mapToSource(i).data(r); }
    virtual bool setData(const QModelIndex &i, const QVariant &v, int r) {
        if (!mapToSource(i).model()) return false;
        QAbstractItemModel * m = const_cast<QAbstractItemModel *>(mapToSource(i).model());
        return m->setData(mapToSource(i),v,r);
    }

private Q_SLOTS:
    void s_rowsAboutToBeInserted(QModelIndex, int, int);
    void s_rowsAboutToBeRemoved(QModelIndex, int, int);
    void s_rowsInserted(QModelIndex, int, int);
    void s_rowsRemoved(QModelIndex, int, int);
    void s_dataChanged(QModelIndex, QModelIndex);
    void s_modelReset();
    void s_destroyed(QObject *);

private:
    class Private;
    Private * d;
	bool	_ignoreReset = false;
};


#endif // ROWSJOINERPROXYMODEL_H
