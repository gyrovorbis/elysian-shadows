#ifndef LAYERDOCKWIDGET_H
#define LAYERDOCKWIDGET_H

#include <QDockWidget>
#include "assetManager.h"

class QTreeWidgetItem;

namespace Ui {
    class LayerDockWidget;
}

class LayerDockWidget : public QDockWidget, public AssetManager
{
    Q_OBJECT
    typedef Assets::LAYER_TYPE LAYER_TYPE;
    
public:
    explicit LayerDockWidget(QWidget *parent = 0);
    ~LayerDockWidget();
    void ToggleDefaultItem(bool selected);
    int GetCurrentLayerIndex();
	void setTopLevelItemCheckState(const unsigned index, const bool value);
public slots:
        void setCurrentLayer(const LAYER_TYPE layer);
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
    void _emitActiveLayerChanged(int index);
    Ui::LayerDockWidget *ui;
    bool    _causedChange;
};





//======= INLINEZ ======

inline void LayerDockWidget::_emitActiveLayerChanged(int index) {
    _causedChange = true;
    emit ActiveLayerChanged(index);
    _causedChange = false;
}


#endif // LAYERDOCKWIDGET_H
