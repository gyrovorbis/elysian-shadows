#include <QMenu>
#include <QMessageBox>
#include <tk_asset_tree_widget.h>
#include "tk_tree_node.h"
#include "elysian_debug.hpp"
#include "elysian_area.hpp"
#include <tk_project.h>
#include "elysian_level.hpp"
#include "tk_assets.h"
#include "elysian_engine.hpp"
#include "elysian_level_system.hpp"
#include <tk_project.h>

AssetTreeWidget::AssetTreeWidget(QWidget *parent): QTreeWidget(parent), _ignoreItemChange(false), _prev(NULL) {
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(OnCurrentItemChanged(QTreeWidgetItem*,int)));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	connect(getAssets(), SIGNAL(areaAdded(Project *const, Level *const, Area *const)), this, SLOT(OnAddArea(Project* const, Level* const, Area* const)));
	connect(getAssets(), SIGNAL(levelAdded(Project *const, Level  *const)), this, SLOT(OnAddLevel(Project * const, Level * const)));
	connect(getAssets(), SIGNAL(projectAdded(Project *const)), this, SLOT(OnAddProject(Project *const)));
	connect(getAssets(), SIGNAL(areaChanged(Area *const)), this, SLOT(OnAreaChange(Area *const)));
	connect(getAssets(), SIGNAL(projectRemoved(Project*const)), this, SLOT(OnRemoveProject(Project*const)));

}

void AssetTreeWidget::OnCurrentItemChanged(QTreeWidgetItem * current, int /*index*/) {
	if(_ignoreItemChange) return;
    if(!current) return;
	if(current == _prev) {
		setCurrentSelection(static_cast<Selectable *>(static_cast<TreeNode *>(current)));
		return;
	}
	else _prev = current;

    current->setExpanded(true);

    _ignoreItemChange = true;
	((TreeNode *)current)->activate();

	//Double cast needed for adjusting memory address offsets. Soooooo fucking reested.
	setCurrentSelection(static_cast<Selectable *>(static_cast<TreeNode *>(current)));
    _ignoreItemChange = false;
}


void AssetTreeWidget::OnAddArea(Project * const /*project*/, Level *const level, Area *const area) {
    level->addChild(area);
}

void AssetTreeWidget::OnAddLevel(Project *const project, Level *const level) {
    project->addChild(level);
}

void AssetTreeWidget::OnAddProject(Project *const project) {
    addTopLevelItem(project);
}

void AssetTreeWidget::OnAreaChange(Area * const area) {
    if(area && currentItem() == area) {
		setCurrentSelection(static_cast<Selectable *>(static_cast<TreeNode *>(area)));
		return;
	}

    setCurrentItem(nullptr);
	if(area) setCurrentItem(area);
	else setCurrentItem(getCurrentLevel());
 //   area->setSelected(true);
	//Debug::Log(Debug::VERBOSE, "PostCurrItem - %s", ((Area*)currentItem())->getName().toStdString().c_str());
}

/* ON REMOVES */
void AssetTreeWidget::OnRemoveProject(Project * const project) {
    takeTopLevelItem(indexOfTopLevelItem(project));
}

void AssetTreeWidget::onCustomContextMenuRequested(QPoint point) {
	QModelIndex modelIndex = indexAt(point);
	TreeNode *treeNode = static_cast<TreeNode *>(itemFromIndex(modelIndex));
	QList<QAction*> actions;
	QMenu *menu;
	QAction *result;

	if(!treeNode) return;

	switch(treeNode->getTreeWidgetItemType()) {
	case TreeNode::ITEM_PROJECT:
        actions.push_back(new QAction("Add Level", nullptr));
        actions.push_back(new QAction("Reload", nullptr));
        menu = new QMenu("Project", nullptr);
		result = menu->exec(actions, mapToGlobal(point));
		if(result) {
			Project *project = static_cast<Project*>(treeNode);
			if(result->text() == QString("Add Level")) {
                auto* newLev = project->newLevel();
                //getAssets()->getEngine()->getLevelSys()->addLevel(newLev);
                getAssets()->setCurrentArea(nullptr);
                getAssets()->setCurrentLevel(newLev);
                getAssets()->setCurrentSelection(newLev);
            } else if(result->text() == QString("Reload")) {
                getAssets()->reloadProject();
            }
		}
		break;

	case TreeNode::ITEM_LEVEL:
        actions.push_back(new QAction("Add Area", nullptr));
        actions.push_back(new QAction("Duplicate", nullptr));
        actions.push_back(new QAction("Remove", nullptr));
        actions.push_back(new QAction("Reload", nullptr));
        menu = new QMenu("Level", nullptr);
		result = menu->exec(actions, mapToGlobal(point));
		if(result) {
			Level *level = static_cast<Level*>(treeNode);
			if(result->text() == QString("Add Area")) {
                auto* newArea = level->newArea();
                //getAssets()->getEngine()->getLevelSys()->addArea(newArea);
                getAssets()->setCurrentArea(newArea);
                getAssets()->setCurrentSelection(newArea);
			} else if(result->text() == QString("Remove")) {
                if(QMessageBox::critical(nullptr, tr("Warning!"), tr("Are you SURE that you want to permanently delete the selected level?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
					return;
				}

				Project *project = level->getProject();
                getAssets()->setCurrentLevel(nullptr);
                project->removeLevel(level->getNodeName());
                getAssets()->setCurrentLevel(project->getFirstLevel());
            } else if(result->text() == QString("Duplicate")) {
                level->duplicate();
            } else if(result->text() == QString("Reload")) {
               getAssets()->reloadLevel(level);

            }
		}
		break;

	case TreeNode::ITEM_AREA:
        actions.push_back(new QAction("Remove", nullptr));
        actions.push_back(new QAction("Duplicate", nullptr));
        actions.push_back(new QAction("Reload", nullptr));
        menu = new QMenu("Area", nullptr);
		result = menu->exec(actions, mapToGlobal(point));
		if(result) {
			Area *area = static_cast<Area*>(treeNode);
			if(result->text() == QString("Remove")) {
                if(QMessageBox::critical(nullptr, tr("Warning!"), tr("Are you SURE that you want to permanently delete the selected area?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
					return;
				}

                Level* level = area->getLevel();
                getAssets()->setCurrentArea(nullptr);
                level->removeArea(area->getNodeName());
                getAssets()->setCurrentArea(level->getFirstArea());


            } else if(result->text() == QString("Duplicate")) {
                area->duplicate();
            } else if(result->text() == QString("Reload")) {
                getAssets()->reloadArea(area);
            }
		}
		break;

	default:
		Q_ASSERT(false);
		return;
	}

	delete menu;

}

