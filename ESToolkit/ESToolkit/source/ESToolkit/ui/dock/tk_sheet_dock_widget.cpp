#include <QGraphicsScene>
#include <QCloseEvent>

#include <ESToolkit/ui/dock/tk_sheet_dock_widget.h>
#include "ui_tk_sheet_dock_widget.h"
#include <AssetIO/tk_level.h>
#include <QESTkUtils/tile_engine/tk_source_view.h>
#include <AssetIO/tk_map_entry.h>
#include <QSpinBox>

SheetDockWidget::SheetDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SheetDockWidget)
{
    ui->setupUi(this);

    ui->elevSpinBox->setMinimum(0);
    ui->elevSpinBox->setMaximum(ELYSIAN_TILE_ENTRY_ELEV_MAX);
    ui->elevSpinBox->setValue(0);

    ui->planeComboBox->addItem("Bottom");
    ui->planeComboBox->addItem("Front");
    ui->planeComboBox->addItem("Left");
    ui->planeComboBox->addItem("Right");
    ui->planeComboBox->addItem("Front 45 Degrees");
    ui->planeComboBox->addItem("Left 45 Degrees");
    ui->planeComboBox->addItem("Right 45 Degrees");
    ui->planeComboBox->setCurrentIndex(0);

    ui->orientComboBox->addItem("Normal");
    ui->orientComboBox->addItem("Horizontal Flip");
    ui->orientComboBox->addItem("Vertical Flip");
    ui->orientComboBox->addItem("Rotate 90 Degrees");
    ui->orientComboBox->addItem("Rotate 180 Degrees");
    ui->orientComboBox->addItem("Rotate 270 Degrees");
    ui->orientComboBox->addItem("Horiz 90 Degrees");
    ui->orientComboBox->addItem("Horiz 270 Degrees");
    ui->orientComboBox->setCurrentIndex(0);

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
    Q_EMIT UncheckMe(3);
    event->accept();
}

void SheetDockWidget::on_planeComboBox_currentIndexChanged(int index)
{
    ui->m_tileSelectionView->setPlane((GY_RECT_PLANE_TYPE)index);
}

void SheetDockWidget::on_elevSpinBox_editingFinished()
{
    ui->m_tileSelectionView->setElev(ui->elevSpinBox->value());
}

void SheetDockWidget::on_orientComboBox_currentIndexChanged(int index)
{
    ui->m_tileSelectionView->setOrient((GY_RECT_TEX_ORIENT)index);
}
