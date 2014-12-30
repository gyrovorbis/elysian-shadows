#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include "assetManager.h"
#include <QGLWidget>

#include <QGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QGLShaderProgram>

#define TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME   (5)

class EngineGLSceneWidget:
        public QGLWidget,
        public AssetManager
{
    Q_OBJECT
public:

        void startRenderTimer(void);
		void advanceFrame(void);
		void play(void);
		void stop(void);
		bool isPlaying(void) const;

		explicit EngineGLSceneWidget(const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);
		virtual ~EngineGLSceneWidget(void);


protected:
	QBasicTimer _timer;
	unsigned	_width			= 0;
	unsigned	_height			= 0;
	bool		_advanceFrame	= false;


    //reimplemented from QGLWidget
    virtual void initializeGL(void) override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL(void) override;
	virtual void timerEvent(QTimerEvent* e) override;
};








// ===== INLINEZ =====




#endif // TK_MAP_SYSTEM_H
