#include "propertyStackModel.h"
#include "propertyStandardItem.h"

QStandardItem* PropertyStackModel::createReadOnlyItem(QString name) {
    auto* dummyItem = new QStandardItem(name);
    dummyItem->setFlags(Qt::ItemIsEnabled);
    return dummyItem;
}

PropertyStackModel::PropertyStackModel(QObject *parent): QStandardItemModel(parent) {
	_curParent = invisibleRootItem();
  //  insertHeaders();
}

PropertyStackModel::~PropertyStackModel(void) {
	//clear();
	_releasePropertyStandardItems();
}

QVariant PropertyStackModel::data(const QModelIndex &index, int role) const {

	switch((Qt::ItemDataRole)role) {
		/*
		  This was for alternating row colors... it doesn't work the way I wanted it to, though.
		  Maybe some day I'll revisit the reest.
		*/
		//case Qt::BackgroundRole:
			//return _bgRowColor[0][index.row()%2];
		default:
			return QStandardItemModel::data(index, role);
	}
}

void PropertyStackModel::clearStack(void) {
	_releasePropertyStandardItems();
	clear();
	_curParent = invisibleRootItem();
}

void PropertyStackModel::insertHeaders(void) {
	setHorizontalHeaderItem(0, new QStandardItem("Name"));
	setHorizontalHeaderItem(1, new QStandardItem("Value"));
}

void PropertyStackModel::pushItem(QStandardItem* item) {
    if(!item) item = invisibleRootItem();
    _curParent = item;
    _curParentStack.push_back(item);
}

void PropertyStackModel::pushLabel(const QString label, QStandardItem *const item, QStandardItem *const item2) {
        QList<QStandardItem *> rowItems;
        QStandardItem *labelItem = createReadOnlyItem(label);

        rowItems << labelItem;

        if(item) rowItems << item;
        else { //Insert an invisible dummy item.

            rowItems << createReadOnlyItem();
        }

        if(columnCount() > 2) rowItems << ((item2)? item2 : createReadOnlyItem());

        _curParent->appendRow(rowItems);
        _curParent = labelItem;
        //_curParentStack.push_back(_curParent);

        _checkPropertyStandardItemAcquire(item);
}

void PropertyStackModel::popLabel(void) {
		_curParent = _curParent->parent();
		if(!_curParent) _curParent = invisibleRootItem();
}

void PropertyStackModel::popItem(void) {
    _curParentStack.pop();

    _curParent = (_curParentStack.size() > 0)? _curParentStack.back() : invisibleRootItem();
}


void PropertyStackModel::addRow(QStandardItem *label, QStandardItem *item, QStandardItem *item2) {
        QList<QStandardItem*> rowItems;

        //name
        label->setFlags(Qt::ItemIsEnabled);
        rowItems << label;
\
        //value
        rowItems << item;

        if(columnCount() > 2) rowItems << ((item2)? item2 : createReadOnlyItem());

        _curParent->appendRow(rowItems);
        _checkPropertyStandardItemAcquire(item);
}


void PropertyStackModel::addProperty(const QString name, QStandardItem *const item, QStandardItem*const item2) {
    addRow(createReadOnlyItem(name), item, item2);
}

void PropertyStackModel::_checkPropertyStandardItemAcquire(QStandardItem *const item) {
	if(item && item->type() == QStandardItem::UserType) {
		//Save item to list, to check ownership later, so we don't delete it when it's still used
		ESTkStandardItem *property = static_cast<ESTkStandardItem *>(item);
        property->activate();
		property->acquire();
		_propertyList.push_back(property);
	}
}

void PropertyStackModel::_releasePropertyStandardItems(void) {
	for(int i = 0; i < _propertyList.size(); ++i) {
		if(_propertyList[i]->getRefCount() > 1) { //Someone else (UndoItem) is using this StandardItem
			//takeItem(indexFromItem(_propertyList[i]).row(),
					 //indexFromItem(_propertyList[i]).column());
			//takeRow(_propertyList[i]->parent()->row()/*, _propertyList[i]->parent()->index()*/);
			_propertyList[i]->parent()->takeChild(_propertyList[i]->row(), _propertyList[i]->column());
			_propertyList[i]->release();
		}
	}
	_propertyList.clear();
}
