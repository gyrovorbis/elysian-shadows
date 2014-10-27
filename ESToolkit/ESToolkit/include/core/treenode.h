#ifndef TREENODE_H
#define TREENODE_H

#include <QTreeWidgetItem>
#include "selectable.h"

class TreeNode: public QTreeWidgetItem, public Selectable {
protected:
    QString         _name;
    bool            _hasChanged         = false;
    virtual bool    deactivate(void)    = 0;
public:
	enum ITEM_TYPE { ITEM_PROJECT, ITEM_LEVEL, ITEM_AREA, ITEM_ENTITY };

    QString             getNodeName(void) const;
    virtual void        setNodeName(const QString name);
    virtual bool        activate(void) = 0;
    void                setChanged(bool changed);
    virtual ITEM_TYPE   getTreeWidgetItemType(void) = 0;
};

#endif // TREENODE_H
