#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include "assetManager.h"
#include <QGLWidget>

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
	bool		_playing		= false;
	bool		_advanceFrame	= false;


    //reimplemented from QGLWidget
    virtual void initializeGL(void) override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL(void) override;
	virtual void timerEvent(QTimerEvent* e) override;
};








// ===== INLINEZ =====

inline void EngineGLSceneWidget::advanceFrame(void) { _advanceFrame = true; }
inline bool EngineGLSceneWidget::isPlaying(void) const { return _playing; }
inline void EngineGLSceneWidget::stop(void) { _playing = false; }
inline void EngineGLSceneWidget::play(void) { _playing = true; }

#endif // TK_MAP_SYSTEM_H
