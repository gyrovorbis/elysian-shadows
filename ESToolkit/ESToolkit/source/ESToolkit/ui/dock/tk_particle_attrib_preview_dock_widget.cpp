#include <ESToolkit/ui/dock/tk_particle_attrib_preview_dock_widget.h>
#include <QESTkUtils/ui/widget/tk_2d_vector_widget.h>
#include <QHBoxLayout>
#include <QTreeView>

ParticleAttribPreviewDockWidget::ParticleAttribPreviewDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _treeView(new QTreeView),
    _vectorWidget(new Vector2DWidget)
{
    setObjectName("particlePreviewDockWidget");
    setWindowTitle("Particle Preview");
    auto layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_vectorWidget);
    layout->addWidget(_treeView);
    auto widget = new QWidget;
    widget->setLayout(layout);
    setWidget(widget);

    QPalette p = _treeView->palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    _treeView->setPalette(p);



}

void ParticleAttribPreviewDockWidget::setModel(QAbstractItemModel* model) const {
    _treeView->setModel(model);

}
