/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef LAYERDOCKWIDGET_H
#define LAYERDOCKWIDGET_H

#include <QDockWidget>
#include <ESToolkit/core/tk_asset_manager.h>

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
    virtual ~LayerDockWidget();
    void ToggleDefaultItem(bool selected);
    int GetCurrentLayerIndex();
	void setTopLevelItemCheckState(const unsigned index, const bool value);
public Q_SLOTS:
        void setCurrentLayer(const LAYER_TYPE layer);
Q_SIGNALS:
    void ActiveLayerChanged(int index);
	void LayerVisibilityChanged(int index, bool visible);
    void UncheckMe(int);
private Q_SLOTS:
    void on_m_layerTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void on_m_layerTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
protected:
    void closeEvent(QCloseEvent *event);
private:
    void _Q_EMITActiveLayerChanged(int index);
    Ui::LayerDockWidget *ui;
    bool    _causedChange;
};





//======= INLINEZ ======

inline void LayerDockWidget::_Q_EMITActiveLayerChanged(int index) {
    _causedChange = true;
    Q_EMIT ActiveLayerChanged(index);
    _causedChange = false;
}


#endif // LAYERDOCKWIDGET_H
