#include "tk_property_stack_model.h"
#include "tk_property_standard_item.h"
#include <functional>

QStandardItem* PropertyStackModel::createReadOnlyItem(QString name) {
    auto* dummyItem = new QStandardItem(name);
    dummyItem->setFlags(Qt::ItemIsEnabled);
    return dummyItem;
}

PropertyStackModel::PropertyStackModel(QObject *parent): QStandardItemModel(parent) {
	_curParent = invisibleRootItem();
  //  insertHeaders();

    QVector<QColor> colors;
     colors.reserve(6);
     colors.push_back(QColor(255, 230, 191));
     colors.push_back(QColor(255, 255, 191));
     colors.push_back(QColor(191, 255, 191));
     colors.push_back(QColor(199, 255, 255));
     colors.push_back(QColor(234, 191, 255));
     colors.push_back(QColor(255, 191, 239));
     m_colors.reserve(colors.count());
     const int darknessFactor = 250;
     for (int i = 0; i < colors.count(); i++) {
         QColor c = colors.at(i);
         m_colors.push_back(qMakePair(c, c.darker(darknessFactor)));
     }
     QColor dynamicColor(191, 207, 255);
     QColor layoutColor(255, 191, 191);
     m_dynamicColor = qMakePair(dynamicColor, dynamicColor.darker(darknessFactor));
     m_layoutColor = qMakePair(layoutColor, layoutColor.darker(darknessFactor));

     //QObject::connect(this, &QAbstractItemModel::rowsInserted, [&](const QModelIndex & parent, int first, int last) { _updateBgColors();});
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
        _updateBgColors();
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




void PropertyStackModel::_updateBgColors(void)
{

    std::function<void(QStandardItem*)> updateItem = [&](QStandardItem* it) {
        if(!it) return;
        if(it != invisibleRootItem()) it->setBackground(QBrush(_rowColor(it)));
        for(unsigned y = 0; y < it->rowCount(); ++y) {
            for(unsigned x = 0; x < it->columnCount(); ++x) {
                updateItem(it->child(y, x));
            }
        }

    };

    updateItem(invisibleRootItem());
    /*

    if (m_sorting) {
        QMapIterator<QString, QtVariantProperty *> itProperty(m_nameToProperty);
        while (itProperty.hasNext()) {
            QtVariantProperty *property = itProperty.next().value();
            m_currentBrowser->addProperty(property);
        }
    } else {
        QListIterator<QtProperty *> itGroup(m_groups);
        while (itGroup.hasNext()) {
            QtProperty *group = itGroup.next();
            QtBrowserItem *item = m_currentBrowser->addProperty(group);
            if (m_currentBrowser == m_treeBrowser)
                m_treeBrowser->setBackgroundColor(item, propertyColor(group));
            group->setModified(m_currentBrowser == m_treeBrowser);
        }
    }*/
}


QColor PropertyStackModel::_rowColor(QStandardItem *item) const {
    /*   QtProperty *groupProperty = property;

       QMap<QtProperty *, QString>::ConstIterator itProp = m_propertyToGroup.constFind(property);
       if (itProp != m_propertyToGroup.constEnd())
           groupProperty = m_nameToGroup.value(itProp.value());*/

      // const int groupIdx;// = m_groups.indexOf(groupProperty);
if(!item) return QColor();
    auto*it = item;
       for(; it->parent() && it->parent() != invisibleRootItem(); it = it->parent());

       const int groupIdx = it->row();


       QPair<QColor, QColor> pair;
   //    if (groupIdx != -1) {
     //      if (groupProperty == m_dynamicGroup)
       //        pair = m_dynamicColor;
        //   else if (isLayoutGroup(groupProperty))
          //     pair = m_layoutColor;
          // else
               pair = m_colors[groupIdx % m_colors.count()];
      // }
      // if (!m_brightness)
        //   return pair.first;
       return pair.second;
}
