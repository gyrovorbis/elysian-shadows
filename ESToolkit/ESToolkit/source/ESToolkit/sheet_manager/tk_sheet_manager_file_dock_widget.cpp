#include <QModelIndex>
#include <QFileSystemModel>
#include <QDebug>

#include <ESToolkit/sheet_manager/tk_sheet_manager_file_dock_widget.h>
#include "ui_tk_sheet_manager_file_dock_widget.h"

SheetManagerFileDockWidget::SheetManagerFileDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FileDockWidget)
{
    ui->setupUi(this);

    // configure m_fileModel to be displayed in m_fileTreeView
    QStringList filters;
	filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif";
    m_fileModel = new QFileSystemModel(this);
    QModelIndex idx = m_fileModel->setRootPath(QDir::homePath());
    m_fileModel->setNameFilters(filters);
    m_fileModel->setNameFilterDisables(false);

    // configure m_fileTreeView (FileDockWidget)
    QPalette p = ui->m_fileTreeView->palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    ui->m_fileTreeView->setPalette(p);
    ui->m_fileTreeView->setModel(m_fileModel);
    ui->m_fileTreeView->setRootIndex(idx);
    ui->m_fileTreeView->setColumnWidth(0, 10);
    ui->m_fileTreeView->setColumnHidden(1, true); // hide size (atleast for MAC)
    ui->m_fileTreeView->setColumnHidden(2, true); // hide type (atleast for MAC)
    ui->m_fileTreeView->setColumnHidden(3, true); // hide data modified (atleast for MAC)
    ui->m_fileTreeView->setColumnWidth(0, 500);

	//connect(ui->m_fileTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_m_fileTreeView_doubleClicked(QModelIndex)));
}

SheetManagerFileDockWidget::~SheetManagerFileDockWidget()
{
    delete ui;
}

void SheetManagerFileDockWidget::on_m_fileTreeView_doubleClicked(const QModelIndex &index) {
	if(m_fileModel->isDir(index)) return;
    //ADD: open file up in sheet manager

	Q_EMIT imageSelected(m_fileModel->filePath(index));
}

void SheetManagerFileDockWidget::on_m_fileTreeView_collapsed(const QModelIndex &/*index*/)
{
    //ui->m_fileTreeView->setColumnWidth(0, m_fileModel->);
}

void SheetManagerFileDockWidget::on_m_fileTreeView_expanded(const QModelIndex &/*index*/)
{
    //ui->m_fileTreeView->setHeaderMinimumSectionSize(30);
}
