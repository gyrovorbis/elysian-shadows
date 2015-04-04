#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include "assetManager.h"
#define QT_NO_OPENGL_ES_2
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
//#include <QGLFunctions>

#define TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME   5

class EngineGLSceneWidget:
        public QOpenGLWidget,
        public AssetManager
{
    Q_OBJECT
public:

        void        startRenderTimer(void);
        void        advanceFrame(void);
        void        play(void);
        void        stop(void);
        bool        isPlaying(void) const;
        bool        isCameraManual(void) const;
        void        setCameraManual(const bool val);
        bool        isCameraManualOnPlay(void) const;
        void        setCameraManualOnPlay(const bool val);

        explicit    EngineGLSceneWidget(QWidget* parent = nullptr);
        virtual     ~EngineGLSceneWidget(void);


protected:
    QBasicTimer     _timer;
    unsigned        _width              = 0;
    unsigned        _height             = 0;
    bool            _advanceFrame       = false;
    bool            _cameraManual       = true;
    bool            _cameraManualOnPlay = false;

    //reimplemented from QGLWidget
    virtual void    initializeGL(void) override;
    virtual void    resizeGL(int w, int h) override;
    virtual void    paintGL(void) override;
    virtual void    timerEvent(QTimerEvent* e) override;
    virtual QSize   sizeHint(void) const;

Q_SIGNALS:

    void            engineUpdated(void);

private Q_SLOTS:


};








// ===== INLINEZ =====

inline bool EngineGLSceneWidget::isCameraManual(void) const { return _cameraManual; }

inline void EngineGLSceneWidget::setCameraManual(const bool val) { _cameraManual = val; }

inline bool EngineGLSceneWidget::isCameraManualOnPlay(void) const { return _cameraManualOnPlay; }




#endif // TK_MAP_SYSTEM_H
