#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include "assetManager.h"
#include <QGLWidget>

#include "geometryengine.h"
#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QGLShaderProgram>
#include <QGLFunctions>

class EngineGLSceneWidget:
        public QGLWidget,
        public AssetManager,
        public QGLFunctions
{
    Q_OBJECT
public:

        void startRenderTimer(void);

        QBasicTimer timer;

        EngineGLSceneWidget(const QGLFormat & format, QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
        ~EngineGLSceneWidget(void);

#if 1
        QGLShaderProgram program;
        GeometryEngine geometries;

        GLuint texture;

        QMatrix4x4 projection;

        QVector2D mousePressPosition;
        QVector3D rotationAxis;
        qreal angularSpeed;
        QQuaternion rotation;


protected:
    unsigned _width     = 0;
    unsigned _height    = 0;

    //reimplemented from QGLWidget
    virtual void initializeGL(void) override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL(void) override;
    virtual void timerEvent(QTimerEvent *e) override;



#else

protected:
	void timerEvent(QTimerEvent *e);

	void initializeGL();
	void resizeGL(int w, int h);

	void initShaders();


private:


#endif
};


#endif // TK_MAP_SYSTEM_H
