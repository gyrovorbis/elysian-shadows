#include "treenode.h"

void TreeNode::setNodeName(const QString name) {
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

QString TreeNode::getNodeName() const {
    return _name;
}
