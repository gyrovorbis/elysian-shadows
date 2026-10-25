/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SELECTION_DOCK_WIDGET_H
#define SELECTION_DOCK_WIDGET_H

#include <QDockWidget>

class QTreeView;
class QAbstractItemModel;
class QAbstractItemDelegate;
class SelectionTreeView;

namespace Ui {
    class SelectionDockWidget;
}

class SelectionDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit                    SelectionDockWidget(QWidget *parent = nullptr);
    virtual                     ~SelectionDockWidget(void);

    void                        setSelectionWidget(QWidget *widget);
    void                        setSelectionTreeView(void);
    void                        restoreSelectionTreeViewDefaultItemDelegate(void);
    void                        setSelectionModel(QAbstractItemModel *model);
    void                        expandSelectionTree(const int index=0);
    QTreeView*                  getSelectionTreeView(void);

Q_SIGNALS:
    void                        uncheckMe(int);
protected:

    virtual void                closeEvent(QCloseEvent *event) override;
    void                        _sizeColumnsToDefault(void);
private:

    Ui::SelectionDockWidget*    _ui;
    SelectionTreeView*          _selectionTreeView;
    QAbstractItemDelegate*      _defaultItemDelegate;
};






#endif // SELECTIONDOCKWIDGET_H
