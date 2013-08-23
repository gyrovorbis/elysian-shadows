#ifndef PROPERTY_STACK_MODEL_H
#define PROPERTY_STACK_MODEL_H

#include <QStandardItemModel>

#define PROPERTY_STACK_MODEL_COLOR_DEPTH 5

class ESTkStandardItem;

class PropertyStackModel: public QStandardItemModel {
private:
	static bool _staticInit;
	static void _staticConstructor(void);

	static QColor _bgRowColor[PROPERTY_STACK_MODEL_COLOR_DEPTH][2];

	//Need to maintain a list of PropertyStandardItems, because these
	//may later be used in UndoCommands. If this is the case, the PropertyStackModel
	//cannot delete these and transfers ownership.
	QVector<ESTkStandardItem *> _propertyList;
	QStandardItem *_curParent;

	void _checkPropertyStandardItemAcquire(QStandardItem *const item);
	void _releasePropertyStandardItems(void);
public:

	PropertyStackModel(QObject *parent=0);
	virtual ~PropertyStackModel(void);

	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual void populateModel(void)=0;

	void clearStack(void);
	void insertHeaders(void);
	void pushLabel(const QString label, QStandardItem *const item=NULL);
	void popLabel(void);
	void addProperty(const QString name, QStandardItem *const item);

};


#endif // PROPERTY_STACK_MODEL_H
