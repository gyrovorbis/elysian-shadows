#ifndef ENTITYDOCKWIDGET_H
#define ENTITYDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
    class EntityDockWidget;
}

class EntityTreeWidget;

class EntityDockWidget : public QDockWidget {
    Q_OBJECT
    
public:
    explicit EntityDockWidget(QWidget *parent = nullptr);
    virtual ~EntityDockWidget(void) override;

    EntityTreeWidget* getTreeWidget(void);
signals:
    void UncheckMe(int);
protected:
    virtual void closeEvent(QCloseEvent *event) override;
private:
    Ui::EntityDockWidget *ui;
};

#endif // ENTITYDOCKWIDGET_H
