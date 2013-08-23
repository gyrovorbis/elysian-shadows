#ifndef LAYERDOCKWIDGET_H
#define LAYERDOCKWIDGET_H

#include <QDockWidget>
#include "assets.h"

class QTreeWidgetItem;

namespace Ui {
    class LayerDockWidget;
}

class LayerDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit LayerDockWidget(QWidget *parent = 0);
    ~LayerDockWidget();
    void ToggleDefaultItem(bool selected);
    int GetCurrentLayerIndex();
	void setCurrentLayer(const Assets::LAYER_TYPE layer);
	void setTopLevelItemCheckState(const unsigned index, const bool value);
signals:
    void ActiveLayerChanged(int index);
	void LayerVisibilityChanged(int index, bool visible);
    void UncheckMe(int);
private slots:
    void on_m_layerTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void on_m_layerTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::LayerDockWidget *ui;
};

#endif // LAYERDOCKWIDGET_H
