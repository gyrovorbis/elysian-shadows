/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_TASK_MONITOR_DIALOG_H
#define TK_TASK_MONITOR_DIALOG_H

#include <QDialog>

class QAbstractItemModel;
class QAbstractItemView;
class QItemSelectionModel;
class QTableView;
class QTreeView;

class TaskMonitorDialog : public QDialog
{
    Q_OBJECT
private:
    void                    _setupViews();

    QAbstractItemModel*     _model;
    QAbstractItemView*      _pieChart;
    QTreeView*              _table;
    QItemSelectionModel*    _selectionModel;


public:
                            TaskMonitorDialog(void);
    void                    setModel(QAbstractItemModel*const model);

};


#endif // TK_TASK_MONITOR_DIALOG_H

