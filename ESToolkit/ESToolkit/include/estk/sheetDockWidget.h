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
    explicit SheetDockWidget(QWidget *parent = 0);
    void UpdateSheetView();
    void CenterSheetView(qreal x, qreal y);
    void SetSheetScene(QGraphicsScene *scene);
	void setViewEnabled(const bool val);

    ~SheetDockWidget();

	SourceView *getSourceView(void) const;
signals:
    void UncheckMe(int);
    void ReleaseFocus();
    void ExitPanMode();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::SheetDockWidget *ui;
};

#endif // SHEETDOCKWIDGET_H
