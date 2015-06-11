#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include <tk_asset_manager.h>
#include <QGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <tk_task_monitor.h>
//#include <QOpenGLFunctions>
//#include <QGLFunctions>

class QOpenGLDebugLogger;
class QOpenGLDebugMessage;

class MapView;

#define TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME   (1000/60)

class EngineGLSceneWidget:
        public QGLWidget,
       // public QOpenGLFunctions,
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
        void        setMapView(MapView* view);

        explicit    EngineGLSceneWidget(QWidget* parent = nullptr);
        virtual     ~EngineGLSceneWidget(void);

        static void makeCurrentContext(void);
        static void doneCurrentContext(void);


protected:
    QBasicTimer             _timer;
    unsigned                _width              = 0;
    unsigned                _height             = 0;
    bool                    _advanceFrame       = false;
    bool                    _cameraManual       = true;
    bool                    _cameraManualOnPlay = false;
    MapView*                _mapView            = nullptr;
    bool                    _initialized        = false;
    QOpenGLDebugLogger*     _glLogger           = nullptr;
    TaskMonitor             _engineTask;

    static EngineGLSceneWidget* _self;


    //reimplemented from QGLWidget
    virtual void    initializeGL(void) override;
    virtual void    resizeGL(int w, int h) override;
    virtual void    paintGL(void) override;
    virtual void    timerEvent(QTimerEvent* e) override;
 //   virtual QSize   sizeHint(void) const override;
  //  virtual void    showEvent(QShowEvent *event) override;

protected Q_SLOTS:
    void    _onContextAboutToBeDestroyed(void);
    void    _onResized();

    void    _onGLMessageLogged(const QOpenGLDebugMessage& message);

Q_SIGNALS:

    void            engineUpdated(void);

private Q_SLOTS:


};








// ===== INLINEZ =====

inline bool EngineGLSceneWidget::isCameraManual(void) const { return _cameraManual; }

inline void EngineGLSceneWidget::setCameraManual(const bool val) { _cameraManual = val; }

inline bool EngineGLSceneWidget::isCameraManualOnPlay(void) const { return _cameraManualOnPlay; }

inline void EngineGLSceneWidget::setMapView(MapView *view) { _mapView = view; }


#endif // TK_MAP_SYSTEM_H
