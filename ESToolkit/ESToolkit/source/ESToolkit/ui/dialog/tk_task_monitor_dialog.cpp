/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ui/dialog/tk_task_monitor_dialog.h>
#include <QESTkUtils/ui/widget/tk_pie_chart_widget.h>
#include <QtWidgets>
#include <QTableView>
#include <QTreeView>

TaskMonitorDialog::TaskMonitorDialog(void)
{

    _setupViews();

    setWindowTitle(tr("Engine Performance Monitoring"));
    resize(870, 550);
}

void TaskMonitorDialog::setModel(QAbstractItemModel*const model)
{
    //model = new QStandardItemModel(8, 2, this);
    _model = model;
    _model->setHeaderData(0, Qt::Horizontal, tr("Task"));
    _model->setHeaderData(1, Qt::Horizontal, tr("Execution Time (ns)"));

    _table->setModel(_model);
    _pieChart->setModel(_model);

    _selectionModel = new QItemSelectionModel(_model);
    _table->setSelectionModel(_selectionModel);
    _pieChart->setSelectionModel(_selectionModel);
}

void TaskMonitorDialog::_setupViews(void)
{
    auto layout = new QHBoxLayout;

    QSplitter *splitter = new QSplitter;
    _table = new QTreeView;
   // _pieChart = new TkPieChartView;
    splitter->addWidget(_table);
    splitter->addWidget(_pieChart);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

//    QHeaderView *headerView = _table->horizontalHeader();
//    headerView->setStretchLastSection(true);

    layout->addWidget(splitter);
    setLayout(layout);
}
