#ifndef TOOLKIT_TREEWIDGET_H
#define TOOLKIT_TREEWIDGET_H

#include <QTreeWidget>
#include "assetManager.h"

class TreeNode;
class Project;
class TkLevel;
typedef TkLevel Level;
class TkArea;
typedef TkArea Area;

class AssetTreeWidget: public QTreeWidget, public AssetManager {
    Q_OBJECT
private:
    bool _ignoreItemChange;
	QTreeWidgetItem *_prev;
public:
    AssetTreeWidget(QWidget *parent=0);
private Q_SLOTS:
	void OnCurrentItemChanged(QTreeWidgetItem * current, int index);
	void onCustomContextMenuRequested(QPoint point);
public Q_SLOTS:
    void OnAddArea(Project *const project, Level * const level, Area * const area);
    void OnAddLevel(Project * const project, Level * const level);
    void OnAddProject(Project * const project);
    void OnAreaChange(Area * const area);
    void OnRemoveProject(Project *const project);
};


#endif // TOOLKIT_TREEVIEW_H
