#include <QTreeWidgetItem>
#include <QCloseEvent>
#include "layerDockWidget.h"
#include "ui_layerdockwidget.h"

LayerDockWidget::LayerDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LayerDockWidget)
{
    ui->setupUi(this);
	//ui->m_layerTreeWidget->expandAll();
	ui->m_layerTreeWidget->setCurrentItem(ui->m_layerTreeWidget->topLevelItem(3));
	emit ActiveLayerChanged(4);
}

LayerDockWidget::~LayerDockWidget()
{
    delete ui;
}

void LayerDockWidget::ToggleDefaultItem(bool selected)
{
    if(selected) {
        ui->m_layerTreeWidget->clearSelection();
		//ui->m_layerTreeWidget->setCurrentItem(ui->m_layerTreeWidget->itemBelow(ui->m_layerTreeWidget->topLevelItem(0)));
		ui->m_layerTreeWidget->setCurrentItem(ui->m_layerTreeWidget->topLevelItem(3));
		emit ActiveLayerChanged(4);
    }

    else {
        ui->m_layerTreeWidget->clearSelection();
    }
}

void LayerDockWidget::on_m_layerTreeWidget_itemChanged(QTreeWidgetItem *item, int /*column*/)
{
#if 0 //Ain't got no parent layers anymore!
    if(item->whatsThis(0).toInt() == 0 || item->whatsThis(0).toInt() == 3) {

        if(item->checkState(0) == Qt::Unchecked) {
            QTreeWidgetItem *subLayer = item->child(0);
            subLayer->setCheckState(0, Qt::Unchecked);
            subLayer = item->child(1);
            subLayer->setCheckState(0, Qt::Unchecked);
        }

        else {
            QTreeWidgetItem *subLayer = item->child(0);
            subLayer->setCheckState(0, Qt::Checked);
            subLayer = item->child(1);
            subLayer->setCheckState(0, Qt::Checked);
        }
    }
#endif

    if(item->checkState(0) == Qt::Unchecked) {
		emit LayerVisibilityChanged(item->whatsThis(0).toInt(), false);
    }

    else {
		emit LayerVisibilityChanged(item->whatsThis(0).toInt(), true);

		//handle mutually-exclusive decorations
		switch(item->whatsThis(0).toInt()) {
		case 7: //elevation
			//indeces
			ui->m_layerTreeWidget->topLevelItem(4)->child(3)->setCheckState(0, Qt::Unchecked);
			emit LayerVisibilityChanged(8, false);
			//flip/rot
			ui->m_layerTreeWidget->topLevelItem(4)->child(5)->setCheckState(0, Qt::Unchecked);
			emit LayerVisibilityChanged(10, false);
			break;
		case 8: //indeces
			//elevation
			ui->m_layerTreeWidget->topLevelItem(4)->child(2)->setCheckState(0, Qt::Unchecked);
			emit LayerVisibilityChanged(7, false);
			//flip/rot
			ui->m_layerTreeWidget->topLevelItem(4)->child(5)->setCheckState(0, Qt::Unchecked);
			emit LayerVisibilityChanged(10, false);
			break;
		case 10: //flip/rot
			//elevation
			ui->m_layerTreeWidget->topLevelItem(4)->child(2)->setCheckState(0, Qt::Unchecked);
			emit LayerVisibilityChanged(7, false);
			//indeces
			ui->m_layerTreeWidget->topLevelItem(4)->child(3)->setCheckState(0, Qt::Unchecked);
			emit LayerVisibilityChanged(8, false);
			break;
		}
	}
	//if(item->whatsThis(0).toInt() == 7) ui->m_layerTreeWidget->setCurrentItem(_lastItem);
}

void LayerDockWidget::on_m_layerTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{
	if(!current) return;
#if 0 //Don't think we need any of this shit
	if(previous) previous->setSelected(false);
    if(current->whatsThis(0).toInt() == 0 || current->whatsThis(0).toInt() == 3) {
        ui->m_layerTreeWidget->setCurrentItem(current->child(0));
	} else if (current->whatsThis(0).toInt() == 7) {
		//ui->m_layerTreeWidget->clearSelection();
		//current->setSelected(false);
		//ui->m_layerTreeWidget->setCurrentItem(ui->m_layerTreeWidget->topLevelItem(0)->child(1));
	}
#endif
	emit ActiveLayerChanged(current->whatsThis(0).toInt());

}

void LayerDockWidget::closeEvent(QCloseEvent *event)
{
    emit UncheckMe(2);
    event->accept();
}

int LayerDockWidget::GetCurrentLayerIndex()
{
    return ui->m_layerTreeWidget->currentItem()->whatsThis(0).toInt();
}

void LayerDockWidget::setCurrentLayer(const Assets::LAYER_TYPE layer) {
	int index;
	switch(layer) {
	case Assets::LAYER_TILE1:
		index = 3;
		break;
	case Assets::LAYER_TILE2:
		index = 2;
		//ui->m_layerTreeWidget->clearSelection();
		//ui->m_layerTreeWidget->topLevelItem(0)->child(1)->setSelected(true);
		break;
	case Assets::LAYER_TILE3:
		index = 1;
		break;
	case Assets::LAYER_LIGHT:
		index = 0;
		break;
	default:
		Q_ASSERT(false);
	}
	ui->m_layerTreeWidget->setCurrentItem(NULL);
	ui->m_layerTreeWidget->setCurrentItem(ui->m_layerTreeWidget->topLevelItem(index));
/*
	for(int i = 0; i < ui->m_layerTreeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parent = ui->m_layerTreeWidget->topLevelItem(i);
		if(parent->whatsThis(0).toInt() == index) {
			ui->m_layerTreeWidget->clearSelection();
			ui->m_layerTreeWidget->setCurrentItem(parent);
			return;
		}
		for(int j = 0; j < parent->childCount(); ++j) {
			QTreeWidgetItem *child = parent->child(j);
			if(child->whatsThis(0).toInt() == index) {
				ui->m_layerTreeWidget->clearSelection();
				ui->m_layerTreeWidget->setCurrentItem(child);
				return;
			}
		}
	}*/

}

void LayerDockWidget::setTopLevelItemCheckState(const unsigned index, const bool value) {
	ui->m_layerTreeWidget->topLevelItem(index)->setCheckState(0, (value)? Qt::Checked : Qt::Unchecked);
}
