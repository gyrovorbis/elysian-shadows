#ifndef TREENODE_H
#define TREENODE_H

#include <QTreeWidgetItem>
#include "tk_selectable.h"


//This entire class is reested to all fuck
class TreeNode: public QTreeWidgetItem, public Selectable {
protected:
    QString         _nodeName;
    QString         _nodePath;
    bool            _hasChanged         = false;
    virtual bool    deactivate(void);
public:
    enum ITEM_TYPE { ITEM_SUBDIR, ITEM_PROJECT, ITEM_LEVEL, ITEM_AREA, ITEM_ENTITY };

                        TreeNode(void);

    QString             getNodeName(void) const;
    QString             getNodePath(void) const;
    void                setNodePath(const QString path);
    void                setNodeName(const QString name);
    virtual bool        activate(void);
    void                setChanged(bool changed);
    virtual ITEM_TYPE   getTreeWidgetItemType(void);
    virtual QAbstractItemModel* getModel(void) const;
    virtual TYPE getType(void) const;
};




// ======= INLINEZ ==========

inline QString TreeNode::getNodePath(void) const { return _nodePath; }

inline void TreeNode::setNodePath(const QString path) { _nodePath = path; }
inline bool TreeNode::activate(void) { return true; }
inline bool TreeNode::deactivate(void) { return true; }
inline auto TreeNode::getTreeWidgetItemType(void) -> ITEM_TYPE { return ITEM_SUBDIR; }
inline QAbstractItemModel* TreeNode::getModel(void) const { return nullptr; }
inline auto TreeNode::getType(void) const -> TYPE { return EMPTY; }


#endif // TREENODE_H
