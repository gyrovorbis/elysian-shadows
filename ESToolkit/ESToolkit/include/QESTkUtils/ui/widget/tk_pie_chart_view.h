#ifndef TK_PIE_CHART_VIEW_H
#define TK_PIE_CHART_VIEW_H


#include <QAbstractItemView>

//! [0]
class TkPieChartView : public QAbstractItemView
{
    Q_OBJECT

public:
    TkPieChartView(QWidget *parent = 0);

    QRect visualRect(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) Q_DECL_OVERRIDE;
    QModelIndex indexAt(const QPoint &point) const Q_DECL_OVERRIDE;

protected Q_SLOTS:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                     const QVector<int> &roles = QVector<int>()) Q_DECL_OVERRIDE;
    void rowsInserted(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;

protected:
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) Q_DECL_OVERRIDE;
    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;

    int horizontalOffset() const Q_DECL_OVERRIDE;
    int verticalOffset() const Q_DECL_OVERRIDE;

    bool isIndexHidden(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setSelection(const QRect&, QItemSelectionModel::SelectionFlags command) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;

    QRegion visualRegionForSelection(const QItemSelection &selection) const Q_DECL_OVERRIDE;

private:
    QRect itemRect(const QModelIndex &item) const;
    QRegion itemRegion(const QModelIndex &index) const;
    int rows(const QModelIndex &index = QModelIndex()) const;
    void updateGeometries() Q_DECL_OVERRIDE;

    int margin;
    int totalSize;
    int pieSize;
    int validItems;
    double totalValue;
    QPoint origin;
    QRubberBand *rubberBand;
};
//! [0]




#endif // TK_DRILLDOWN_CHART_H

