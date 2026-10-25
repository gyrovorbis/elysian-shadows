/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SHEET_MANAGER_FILE_DOCK_WIDGET_H
#define SHEET_MANAGER_FILE_DOCK_WIDGET_H

#include <QDockWidget>

namespace Ui {
    class FileDockWidget;
}

class QFileSystemModel;
class QModelIndex;

class SheetManagerFileDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SheetManagerFileDockWidget(QWidget *parent = 0);
	~SheetManagerFileDockWidget();

Q_SIGNALS:
	void imageSelected(const QString img);

private Q_SLOTS:
    void on_m_fileTreeView_doubleClicked(const QModelIndex &index);

    void on_m_fileTreeView_collapsed(const QModelIndex &index);

    void on_m_fileTreeView_expanded(const QModelIndex &index);

private:
    Ui::FileDockWidget *ui;
    QFileSystemModel *m_fileModel;
};

#endif // FILEDOCKWIDGET_H
