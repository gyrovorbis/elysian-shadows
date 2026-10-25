/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ui/widget/tk_selection_tree_view.h>
#include <QCloseEvent>
#include <ESToolkit/ui/dock/tk_selection_dock_widget.h>
#include "ui_tk_selection_dock_widget.h"
#include <QESTkUtils/ui/delegate/tk_property_item_proxy_delegate.h>

SelectionDockWidget::SelectionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _ui(new Ui::SelectionDockWidget)
{
    _ui->setupUi(this);
    //setMouseTracking(true);
    _selectionTreeView = new SelectionTreeView;

    _selectionTreeView->setItemDelegate(new PropertyItemProxyDelegate);
    //_selectionTreeView->viewport()->setMouseTracking(true);
    _defaultItemDelegate = _selectionTreeView->itemDelegate();
}

SelectionDockWidget::~SelectionDockWidget(void) {
    setSelectionModel(nullptr);
    delete _ui;
}

void SelectionDockWidget::setSelectionWidget(QWidget *widget) {
    setWidget(widget);
}

void SelectionDockWidget::setSelectionTreeView(void) {
    setWidget(_selectionTreeView);
}

void SelectionDockWidget::setSelectionModel(QAbstractItemModel *model) {
    _selectionTreeView->setModel(model);
    _sizeColumnsToDefault();
}

void SelectionDockWidget::expandSelectionTree(const int index) {
    _selectionTreeView->expandToDepth(index);
    _sizeColumnsToDefault();
}

void SelectionDockWidget::_sizeColumnsToDefault(void) {
    _selectionTreeView->resizeColumnToContents(0);
}

void SelectionDockWidget::closeEvent(QCloseEvent *event) {
    Q_EMIT uncheckMe(5);
    event->accept();
}

void SelectionDockWidget::restoreSelectionTreeViewDefaultItemDelegate(void) {
    _selectionTreeView->setItemDelegate(_defaultItemDelegate);
}

QTreeView* SelectionDockWidget::getSelectionTreeView(void) {
    return _selectionTreeView;
}
