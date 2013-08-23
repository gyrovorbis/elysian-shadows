#include "treenode.h"

void TreeNode::setName(const QString name) {
    _name = name;

    if(_hasChanged == true) {
		//qDebug("add the star!");
        setText(0, QString(name + "*"));
    }

    else {
        setText(0, name);
    }
}

void TreeNode::setChanged(bool clean)
{
    if(!clean) {
        _hasChanged = true;
        setText(0, _name + "*");
    }

    else {
        _hasChanged = false;
        setText(0, _name);
    }
}

TreeNode::TreeNode() : _hasChanged(false) {}

QString TreeNode::getName() const {
    return _name;
}
