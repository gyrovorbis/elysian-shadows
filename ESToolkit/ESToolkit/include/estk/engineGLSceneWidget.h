#ifndef ENGINE_GL_SCENE_WIDGET_H
#define ENGINE_GL_SCENE_WIDGET_H

#include "assetManager.h"
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
//#include <QGLFunctions>

class MapView;

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
        void        setMapView(MapView* view);

        explicit    EngineGLSceneWidget(QWidget* parent = nullptr);
        virtual     ~EngineGLSceneWidget(void);


protected:
    QBasicTimer     _timer;
    unsigned        _width              = 0;
    unsigned        _height             = 0;
    bool            _advanceFrame       = false;
    bool            _cameraManual       = true;
    bool            _cameraManualOnPlay = false;
    MapView*        _mapView            = nullptr;


    //reimplemented from QGLWidget
    virtual void    initializeGL(void) override;
    virtual void    resizeGL(int w, int h) override;
    virtual void    paintGL(void) override;
    virtual void    timerEvent(QTimerEvent* e) override;
 //   virtual QSize   sizeHint(void) const override;
    virtual void    showEvent(QShowEvent *event) override;

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
