#ifndef PROPERTY_STACK_MODEL_H
#define PROPERTY_STACK_MODEL_H

#include <QStandardItemModel>
#include <QStack>

#define PROPERTY_STACK_MODEL_COLOR_DEPTH 5

class ESTkStandardItem;

class PropertyStackModel: public QStandardItemModel {
    Q_OBJECT
private:

	//Need to maintain a list of PropertyStandardItems, because these
	//may later be used in UndoCommands. If this is the case, the PropertyStackModel
	//cannot delete these and transfers ownership.
    QVector<ESTkStandardItem*>  _propertyList;
    QStack<QStandardItem*>      _curParentStack;
    QStandardItem*              _curParent;

    QVector<QPair<QColor, QColor>>            m_colors;
    QPair<QColor, QColor>                       m_dynamicColor;
    QPair<QColor, QColor>                   m_layoutColor;


	void _checkPropertyStandardItemAcquire(QStandardItem *const item);
	void _releasePropertyStandardItems(void);
    void    _updateBgColors(void);
    QColor _rowColor(QStandardItem* item) const;
public:

    PropertyStackModel(QObject *parent=nullptr);
	virtual ~PropertyStackModel(void);

    virtual QVariant                data(const QModelIndex &index, int role) const override;

	virtual void populateModel(void)=0;

	void clearStack(void);
	void insertHeaders(void);

    void pushLabel(const QString label, QStandardItem *const item=nullptr, QStandardItem *const item2=nullptr);
	void popLabel(void);
    void pushItem(QStandardItem* item);
    void popItem(void);

    void addRow(QStandardItem* label, QStandardItem* item, QStandardItem* item2=nullptr);
    void addProperty(const QString name, QStandardItem *const item, QStandardItem* item2=nullptr);

    void                            removeChildItems(QStandardItem* parent);
    static QStandardItem*           createReadOnlyItem(QString name=QString());

};


#endif // PROPERTY_STACK_MODEL_H
