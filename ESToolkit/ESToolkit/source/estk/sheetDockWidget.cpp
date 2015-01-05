#include <QGraphicsScene>
#include <QCloseEvent>

#include "sheetDockWidget.h"
#include "ui_sheetdockwidget.h"
#include "elysian_level.hpp"
#include "sourceView.h"

SheetDockWidget::SheetDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SheetDockWidget)
{
    ui->setupUi(this);

	connect(ui->m_tileSelectionView, SIGNAL(releaseFocus()),this,SIGNAL(ReleaseFocus()));
	connect(ui->m_tileSelectionView, SIGNAL(exitPanMode()),this,SIGNAL(ExitPanMode()));
}

void SheetDockWidget::setViewEnabled(const bool val) {
	ui->m_tileSelectionView->setEnabled(val);
}

void SheetDockWidget::UpdateSheetView()
{
    ui->m_tileSelectionView->update();
}

void SheetDockWidget::CenterSheetView(qreal x, qreal y)
{
    ui->m_tileSelectionView->centerOn(x, y);
}

void SheetDockWidget::SetSheetScene(QGraphicsScene *scene)
{
    ui->m_tileSelectionView->setScene(scene);
}

SheetDockWidget::~SheetDockWidget()
{
    delete ui;
}

SourceView *SheetDockWidget::getSourceView(void) const {
	return ui->m_tileSelectionView;
}

void SheetDockWidget::closeEvent(QCloseEvent *event)
{
    emit UncheckMe(3);
    event->accept();
}
