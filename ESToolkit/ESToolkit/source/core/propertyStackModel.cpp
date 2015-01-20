#include "propertyStackModel.h"
#include "propertyStandardItem.h"

bool PropertyStackModel::_staticInit = false;
QColor PropertyStackModel::_bgRowColor[PROPERTY_STACK_MODEL_COLOR_DEPTH][2];

void PropertyStackModel::_staticConstructor(void) {
	_bgRowColor[0][0].setRgb(255, 255, 0);
	_bgRowColor[0][1].setRgb(100, 255, 0);

	_bgRowColor[1][0].setRgb(0, 255, 255);
	_bgRowColor[1][1].setRgb(0, 255, 100);

	_bgRowColor[2][0].setRgb(255, 0, 255);
	_bgRowColor[2][1].setRgb(100, 0, 100);

	_bgRowColor[3][0].setRgb(255, 100, 0);
	_bgRowColor[3][1].setRgb(100, 50, 0);

	_bgRowColor[4][0].setRgb(255, 255, 100);
	_bgRowColor[4][1].setRgb(100, 255, 50);
}


PropertyStackModel::PropertyStackModel(QObject *parent): QStandardItemModel(parent) {
	_curParent = invisibleRootItem();
	if(!_staticInit) {
		_staticConstructor();
		_staticInit = true;
	}
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
    _curParent = item;
    _curParentStack.push_back(item);
}

void PropertyStackModel::pushLabel(const QString label, QStandardItem *const item) {
		QList<QStandardItem *> rowItems;
		QStandardItem *labelItem = new QStandardItem(label);
		QStandardItem *dummyItem;
        //set label-specific properties
		labelItem->setFlags(Qt::ItemIsEnabled);
		rowItems << labelItem;

		if(item) rowItems << item;
		else { //Insert an invisible dummy item.
			dummyItem = new QStandardItem;
			dummyItem->setEnabled(false);
			dummyItem->setSelectable(false);
			rowItems << dummyItem;
		}

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


void PropertyStackModel::addRow(QStandardItem *label, QStandardItem *item) {
        QList<QStandardItem*> rowItems;

        //name
        label->setFlags(Qt::ItemIsEnabled);
        rowItems << label;
\
        //value
        rowItems << item;
        _curParent->appendRow(rowItems);
        _checkPropertyStandardItemAcquire(item);
}


void PropertyStackModel::addProperty(const QString name, QStandardItem *const item) {
        QList<QStandardItem*> rowItems;

        //name
        QStandardItem *nameItem = new QStandardItem(name);
        nameItem->setFlags(Qt::ItemIsEnabled);
        rowItems << nameItem;
\
        //value
        rowItems << item;
        _curParent->appendRow(rowItems);

		_checkPropertyStandardItemAcquire(item);
}

void PropertyStackModel::_checkPropertyStandardItemAcquire(QStandardItem *const item) {
	if(item && item->type() == QStandardItem::UserType) {
		//Save item to list, to check ownership later, so we don't delete it when it's still used
		ESTkStandardItem *property = static_cast<ESTkStandardItem *>(item);
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
