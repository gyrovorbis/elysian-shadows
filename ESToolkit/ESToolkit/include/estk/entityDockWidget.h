#ifndef ENTITYDOCKWIDGET_H
#define ENTITYDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class EntityDockWidget;
}

class EntityDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit EntityDockWidget(QWidget *parent = 0);
    ~EntityDockWidget();
signals:
    void UncheckMe(int);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::EntityDockWidget *ui;
};

#endif // ENTITYDOCKWIDGET_H
