#ifndef TK_MAP_SYSTEM_H
#define TK_MAP_SYSTEM_H

#include "elysian_map_system.hpp"
#include <QGLWidget>

#include "geometryengine.h"
#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QGLShaderProgram>

class TkMapSystem:
        public QGLWidget,
		public elysian::MapSystemBase,
		public QGLFunctions
{
    Q_OBJECT
public:

    elysian::Engine* engine;

    using elysian::MapSystemBase::render;
    using elysian::MapSystemBase::update;

#if 0

protected:
    //reimplemented from QGLWidget
    virtual void initializeGL(void) override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL(void) override;
    virtual void paintEvent(QPaintEvent* event) override;
#else
TkMapSystem(QWidget *parent=nullptr);
~TkMapSystem();

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void timerEvent(QTimerEvent *e);

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void initShaders();
	void initTextures();

private:
	QBasicTimer timer;
	QGLShaderProgram program;
	GeometryEngine geometries;

	GLuint texture;

	QMatrix4x4 projection;

	QVector2D mousePressPosition;
	QVector3D rotationAxis;
	qreal angularSpeed;
	QQuaternion rotation;

#endif
};


#endif // TK_MAP_SYSTEM_H
