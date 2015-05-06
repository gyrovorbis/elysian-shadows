#ifndef SHEETDOCKWIDGET_H
#define SHEETDOCKWIDGET_H

#include <QDockWidget>

class QGraphicsScene;
class SourceView;

namespace Ui {
	class SheetDockWidget;
}

class SheetDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit SheetDockWidget(QWidget *parent = nullptr);
    void UpdateSheetView();
    void CenterSheetView(qreal x, qreal y);
    void SetSheetScene(QGraphicsScene *scene);
	void setViewEnabled(const bool val);
    Ui::SheetDockWidget* getUi(void) const;

    ~SheetDockWidget();

	SourceView *getSourceView(void) const;
Q_SIGNALS:
    void UncheckMe(int);
    void ReleaseFocus();
    void ExitPanMode();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::SheetDockWidget *ui;
};


//======= INLINEZ ========
inline Ui::SheetDockWidget* SheetDockWidget::getUi(void) const { return ui; }

#endif // SHEETDOCKWIDGET_H
