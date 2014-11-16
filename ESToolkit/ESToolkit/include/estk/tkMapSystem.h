#ifndef TK_MAP_SYSTEM_H
#define TK_MAP_SYSTEM_H

#include "elysian_map_system.hpp"
#include <QGLWidget>

class TkMapSystem:
        public QGLWidget,
        public elysian::MapSystemBase
{
    Q_OBJECT
public:

    elysian::Engine* engine;

    using elysian::MapSystemBase::render;
    using elysian::MapSystemBase::update;

protected:
    //reimplemented from QGLWidget
    virtual void initializeGL(void) override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL(void) override;
    virtual void paintEvent(QPaintEvent* event) override;

};


#endif // TK_MAP_SYSTEM_H
