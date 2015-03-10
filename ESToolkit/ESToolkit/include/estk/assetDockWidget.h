#ifndef ASSETDOCKWIDGET_H
#define ASSETDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class AssetDockWidget;
}

class AssetDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit AssetDockWidget(QWidget *parent = 0);
    ~AssetDockWidget();
    void clearTreeView(void);
Q_SIGNALS:
    void UncheckMe(int);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::AssetDockWidget *ui;
};

#endif // ASSETDOCKWIDGET_H
