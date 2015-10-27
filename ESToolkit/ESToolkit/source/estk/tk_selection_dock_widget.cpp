#include <QTreeView>
#include <QCloseEvent>
#include "tk_selection_dock_widget.h"
#include "ui_tk_selection_dock_widget.h"
#include <model/tk_property_item_delegate.h>

SelectionDockWidget::SelectionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SelectionDockWidget)
{
    ui->setupUi(this);
    _selectionTreeWidget = new QTreeView();
    QFont font = _selectionTreeWidget->font();
    font.setPointSize(10);
    _selectionTreeWidget->setFont(font);

    QPalette p = _selectionTreeWidget->palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    _selectionTreeWidget->setPalette(p);
	_selectionTreeWidget->setIndentation(9);
	_selectionTreeWidget->setAlternatingRowColors(true);

    _selectionTreeWidget->setItemDelegate(new PropertyItemProxyDelegate);
	_defaultItemDelegate = _selectionTreeWidget->itemDelegate();
}

SelectionDockWidget::~SelectionDockWidget()
{
    SetSelectionModel(nullptr);
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

void SelectionDockWidget::ExpandSelectionTree(const int index)
{
    _selectionTreeWidget->expandToDepth(index);
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
    //_selectionTreeWidget->setItemDelegate(_defaultItemDelegate);
}
