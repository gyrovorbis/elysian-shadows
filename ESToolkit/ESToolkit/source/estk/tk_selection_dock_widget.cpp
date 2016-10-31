#include <QTreeView>
#include <QCloseEvent>
#include "tk_selection_dock_widget.h"
#include "ui_tk_selection_dock_widget.h"
#include <model/tk_property_item_delegate.h>

SelectionDockWidget::SelectionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _ui(new Ui::SelectionDockWidget)
{
    _ui->setupUi(this);
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

SelectionDockWidget::~SelectionDockWidget(void) {
    setSelectionModel(nullptr);
    delete _ui;
}

void SelectionDockWidget::setSelectionWidget(QWidget *widget) {
    setWidget(widget);
}

void SelectionDockWidget::setSelectionTreeWidget(void) {
    setWidget(_selectionTreeWidget);
}

void SelectionDockWidget::setSelectionModel(QAbstractItemModel *model) {
    _selectionTreeWidget->setModel(model);
    _sizeColumnsToDefault();
}

void SelectionDockWidget::expandSelectionTree(const int index) {
    _selectionTreeWidget->expandToDepth(index);
    _sizeColumnsToDefault();
}

void SelectionDockWidget::_sizeColumnsToDefault(void) {
    _selectionTreeWidget->resizeColumnToContents(0);
}

void SelectionDockWidget::closeEvent(QCloseEvent *event) {
    Q_EMIT uncheckMe(5);
    event->accept();
}

void SelectionDockWidget::restoreSelectionTreeWidgetDefaultItemDelegate(void) {
    _selectionTreeWidget->setItemDelegate(_defaultItemDelegate);
}
