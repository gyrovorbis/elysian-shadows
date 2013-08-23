#ifndef TREENODE_H
#define TREENODE_H

#include <QTreeWidgetItem>
#include "selectable.h"

class TreeNode: public QTreeWidgetItem, public Selectable {
protected:
    QString _name;
    bool _hasChanged;
	virtual bool deactivate()=0;
public:
	enum ITEM_TYPE { ITEM_PROJECT, ITEM_LEVEL, ITEM_AREA, ITEM_ENTITY };

    TreeNode();
    QString getName() const;
    virtual void setName(const QString name);
	virtual bool activate()=0;
   // virtual bool Deactivate()=0;
    void setChanged(bool changed);
	virtual ITEM_TYPE getTreeWidgetItemType(void)=0;
};

#endif // TREENODE_H
