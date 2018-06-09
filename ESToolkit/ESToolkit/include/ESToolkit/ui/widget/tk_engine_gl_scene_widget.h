#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include <ESToolkit/core/tk_asset_manager.h>
#include <QGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QTimer>
#include <utility/elysian_performance_monitor.hpp>
#include <ESGamma/tk_task_monitor.h>
//#include <QOpenGLFunctions>
//#include <QGLFunctions>

class QOpenGLDebugLogger;
class QOpenGLDebugMessage;

class MapView;

//#define TK_ENGINE_GL_SCENE_DYNAMIC_UDPATE_TIME
#define TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME   (1000.0f/60.0f)
//#define TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME   0.0f

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
        void        dumpPerfMonitor(void);
        bool        isEngineValid(void) const;
        void        setEngineValid(bool val);

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
    bool                    _wasGLError         = false;
    bool                    _engineValid        = true;
    QString                 _prevGLError;
    TaskMonitor             _engineTask;
    elysian::PerformanceMonitor _monitor = elysian::PerformanceMonitor("Engine Frame");
    QTimer                  _engineUpdateTimer;

    static EngineGLSceneWidget* _self;


    //reimplemented from QGLWidget
    virtual void    initializeGL(void) override;
    virtual void    resizeGL(int w, int h) override;
    virtual void    paintGL(void) override;

 //   virtual QSize   sizeHint(void) const override;
  //  virtual void    showEvent(QShowEvent *event) override;

protected Q_SLOTS:

    void    _updateEngine(void);
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

inline bool EngineGLSceneWidget::isEngineValid(void) const { return _engineValid; }
inline void EngineGLSceneWidget::setEngineValid(bool val) {
    if(_engineValid != val) {
        setEnabled(val);
        _engineValid = val;
    }
}


#endif // TK_MAP_SYSTEM_H
