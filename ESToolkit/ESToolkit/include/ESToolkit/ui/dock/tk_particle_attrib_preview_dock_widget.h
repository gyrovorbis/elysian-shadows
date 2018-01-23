#ifndef TK_PARTICLE_ATTRIB_PREVIEW_DOCK_WIDGET_H
#define TK_PARTICLE_ATTRIB_PREVIEW_DOCK_WIDGET_H

#include <QDockWidget>

class QAbstractItemModel;
class QTreeView;
class Vector2DWidget;

class ParticleAttribPreviewDockWidget : public QDockWidget {
    Q_OBJECT
public:
    explicit    ParticleAttribPreviewDockWidget(QWidget* parent = nullptr);

    void        setModel(QAbstractItemModel* model) const;
    Vector2DWidget* getVector2DWidget(void) const;

private:
    QTreeView*                              _treeView;
    Vector2DWidget*                         _vectorWidget;
};



//========== INLINEZ ============
inline Vector2DWidget* ParticleAttribPreviewDockWidget::getVector2DWidget(void) const { return _vectorWidget; }


#endif // HISTORYDOCKWIDGET_H


