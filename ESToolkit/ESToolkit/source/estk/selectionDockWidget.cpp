#include <QTreeView>
#include <QCloseEvent>
#include "selectionDockWidget.h"
#include "ui_selectiondockwidget.h"

SelectionDockWidget::SelectionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SelectionDockWidget)
{
    ui->setupUi(this);
    _selectionTreeWidget = new QTreeView();

    QPalette p = _selectionTreeWidget->palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    _selectionTreeWidget->setPalette(p);
	_selectionTreeWidget->setIndentation(9);

	_selectionTreeWidget->setAlternatingRowColors(true);

	_defaultItemDelegate = _selectionTreeWidget->itemDelegate();
}

SelectionDockWidget::~SelectionDockWidget()
{
    delete ui;
}

void SelectionDockWidget::SetSelectionWidget(QWidget *widget)
{
    setWidget(widget);
}

void SelectionDockWidget::SetSelectionTreeWidget()
{
    setWidget(_selectionTreeWidget);
}

void SelectionDockWidget::SetSelectionModel(QAbstractItemModel *model)
{
    _selectionTreeWidget->setModel(model);
    SizeColumnsToDefault();
}

void SelectionDockWidget::ExpandSelectionTree()
{
    _selectionTreeWidget->expandToDepth(0);
    SizeColumnsToDefault();
}

void SelectionDockWidget::SizeColumnsToDefault()
{
    _selectionTreeWidget->resizeColumnToContents(0);
}

void SelectionDockWidget::closeEvent(QCloseEvent *event)
{
    Q_EMIT UncheckMe(5);
    event->accept();
}

void SelectionDockWidget::restoreSelectionTreeWidgetDefaultItemDelegate(void) {
	_selectionTreeWidget->setItemDelegate(_defaultItemDelegate);
}
