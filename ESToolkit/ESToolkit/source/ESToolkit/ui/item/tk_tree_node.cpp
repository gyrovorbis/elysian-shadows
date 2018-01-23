#include <ESToolkit/ui/item/tk_tree_node.h>

TreeNode::TreeNode(void) {
    QBrush brush = QBrush(Qt::gray);
    setForeground(0, brush);
}

void TreeNode::setNodeName(const QString name) {
  //  _name = name;

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
        setText(0, text(0) + "*");
    }

    else {
        QString name = text(0);
        _hasChanged = false;
        name.chop(1);
        setText(0, name);
    }
}

QString TreeNode::getNodeName() const {
    return text(0);
}

void TreeNode::onDoubleClick(void) {


}
