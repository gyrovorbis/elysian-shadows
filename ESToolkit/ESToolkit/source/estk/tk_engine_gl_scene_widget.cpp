
#include "crtp/elysian_camera_system.hpp"
#include <gyro_video_api.h>
#include "tk_engine_gl_scene_widget.h"
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"
#include <elysian_timer_system.hpp>
#include "elysian_light_system.hpp"
#include "tk_map_view.h"
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>

#include <QMouseEvent>
#include <spine/spine.h>
#include <QOpenGLContext>
//#undef QT_NO_KEYWORDS

EngineGLSceneWidget* EngineGLSceneWidget::_self = nullptr;

void EngineGLSceneWidget::makeCurrentContext(void) {
    if(_self) {

        _self->makeCurrent();
       //     glalVidOglInit(_self->_width, _self->_height, _self->_width, _self->_height);
    }
}

void EngineGLSceneWidget::doneCurrentContext(void) {
    if(_self) _self->doneCurrent();
}

EngineGLSceneWidget::EngineGLSceneWidget(QWidget *parent):
    QGLWidget(parent),
    _engineTask("ESGamma")
{
    assert(!_self);
            _self = this;
            setAttribute(Qt::WA_NativeWindow);
            setAutoBufferSwap(true);

    connect(this, SIGNAL(engineUpdated()), getAssets(), SIGNAL(engineUpdated()));
}


void EngineGLSceneWidget::stop(void) {
    getAssets()->getEngine()->setEngineState(elysian::Engine::ENGINE_ESTK_STOP);
    _cameraManual = true;
}

void EngineGLSceneWidget::play(void) {
    getAssets()->getEngine()->setEngineState(elysian::Engine::ENGINE_NORMAL);
    _cameraManual = _cameraManualOnPlay;
}

void EngineGLSceneWidget::advanceFrame(void) {
    _advanceFrame = true;
    getAssets()->getEngine()->setEngineState(elysian::Engine::ENGINE_NORMAL);
}

bool EngineGLSceneWidget::isPlaying(void) const { return getAssets()->getEngine()->getEngineState() != elysian::System::ENGINE_ESTK_STOP; }




void EngineGLSceneWidget::initializeGL(void) {
    DbgEvent event("Initializing OpenGL");
   // initializeOpenGLFunctions();
 //   auto* con = context();
  //  assert(con->isValid());
  //  makeCurrent();
   // initializeGLFunctions();
   // doneCurrent();
    qDebug() << QOpenGLContext::currentContext();

    //unsigned parentWidth    = parentWidget()->size().width();
    //unsigned parentHeight   = parentWidget()->size().height();

  //  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(_onContextAboutToBeDestroyed()));
makeCurrent();
//glalVidGlewInit();
      // glalVidOglInit(1280, 720, 1280, 720);
  //  glalVidOglInit(parentWidth, parentHeight, parentWidth, parentHeight);
     if(getAssets()->getEngine()) getAssets()->getEngine()->getCameraSys()->initializeProjMatrix();

    // auto* ctx = QOpenGLContext::currentContext();
    // if(ctx && ctx->hasEx tension(QByteArrayLiteral("GL_KHR_debug"))) {
#if 0
         _glLogger = new QOpenGLDebugLogger(this);
         connect(_glLogger, &QOpenGLDebugLogger::messageLogged, this, &EngineGLSceneWidget::_onGLMessageLogged);
         if(_glLogger->initialize()) {
             Debug::Logf(Debug::LOG_VERBOSE, "OpenGL debug logger initialized successfullly.");
             _glLogger->startLogging();
         } else {
             Debug::Logf(Debug::LOG_WARNING, "OpenGL debug logger could not be initialized.");
         }
#endif

    // } else {
    //	 Debug::Logf(Debug::LOG_VERBOSE, "OpenGL debug logger extensions not detected!");
    // }

    // _mapView->setSceneRect(0, 0, parentWidth, parentHeight);

   //  doneCurrent();

}


void EngineGLSceneWidget::startRenderTimer(void) {
    makeCurrent();

        //_timer.start(TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME, this);

    //QTimer::singleShot(TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME, this, SLOT(_updateEngine()));
    //_timer.start(TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME, this);
   // doneCurrent();
    _updateEngine();
    _initialized = true;
    //timerEvent(nullptr); //go ahead and update the nigger.
}
/*
void EngineGLSceneWidget::showEvent(QShowEvent *event) {
    //startRenderTimer();
}*/


void EngineGLSceneWidget::_updateEngine(void)
{
    makeCurrent();
   // update();
    updateGL();
        //       _mapView->scene()->invalidate();
    //_mapView->update();
   // _mapView->updateSceneRect(_mapView->sceneRect());
  //  paintGL();

 //  #ifndef __WIN32
    unsigned parentWidth    = parentWidget()->size().width();
        unsigned parentHeight   = parentWidget()->size().height();
        if(_width != parentWidth || _height != parentHeight) {
            resizeGL(parentWidth, parentHeight);
        }
//#endif

    QTimer::singleShot(TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME, this, SLOT(_updateEngine()));

}

void EngineGLSceneWidget::resizeGL(int w, int h) {
makeCurrent();
   glalVidOglInit(w, h, w, h);
     _width = w;
     _height = h;

    // _mapView->setSceneRect(0, 0, w, h);

 if(getAssets()->getEngine()) getAssets()->getEngine()->getCameraSys()->initializeProjMatrix();
// doneCurrent();

}

void EngineGLSceneWidget::paintGL(void) {
    if(!getCurrentArea() || !_initialized) return;
    makeCurrent();

    auto delta = gyTimerTicks();

    static bool once = false;
    if(!once) {
       // _initSpineShit();
        once = true;
    }

    _wasGLError = false;

    getAssets()->getEngine()->getTimerSys()->pushTaskMonitor(&_engineTask);

    getAssets()->getEngine()->render();

    //doneCurrent();
    //swapBuffers();

    getAssets()->getEngine()->update();
    getAssets()->getEngine()->processInput();
    if(_advanceFrame) {

        _advanceFrame = false;
        getAssets()->getEngine()->setEngineState(elysian::System::ENGINE_ESTK_STOP);
    }

    if(_cameraManual) {
        getAssets()->getEngine()->getCameraSys()->setFollowEnabled(false);
        getAssets()->getEngine()->getCameraSys()->setDynamic(false);
        getAssets()->getEngine()->getCameraSys()->setBoundByMap(false);
    } else {
        getAssets()->getEngine()->getCameraSys()->setFollowEnabled(true);
        getAssets()->getEngine()->getCameraSys()->setDynamic(true);
        getAssets()->getEngine()->getCameraSys()->setBoundByMap(true);
    }
    //  getAssets()->getEngine()->getLightSys()->setGlobalAmbientColor(GYVector4 { 1.0f, 1.0f, 1.0f, 1.0f});

    //getAssets()->getEngine()->render(&_renderSpineShit);

      //context()->swapBuffers(context()->surface()); // DO QOPENGL WIDGET'S FUCKING JOB FOR IT!
        //  update();

    getAssets()->getEngine()->getTimerSys()->popTaskMonitor(&_engineTask);

    //auto result = gyTimerTicks() - delta;

    //auto sec = (double)result/1000000000.0;
   // qDebug() << "nsec - " << result << "sec - " << sec << "hz - " << 1.0/sec;


    Q_EMIT engineUpdated();
}

EngineGLSceneWidget::~EngineGLSceneWidget(void){}
/*
QSize EngineGLSceneWidget::sizeHint(void) const {
    if(getAssets()->getCurArea()) {
        return QSize(getAssets()->getCurArea()->getMapWidth()*32.0f, getAssets()->getCurArea()->getMapHeight()*32.0f);
    }
    return QSize(10,10);
}
*/
void EngineGLSceneWidget::setCameraManualOnPlay(const bool val) {
    _cameraManualOnPlay = val;

    if(getAssets()->getEngine()->getEngineState() != elysian::Engine::ENGINE_ESTK_STOP) {
        _cameraManual = val;
    }

}

void EngineGLSceneWidget::_onContextAboutToBeDestroyed(void) {
    qDebug() << "DESTROYING CONTEXT!!!";
}

void EngineGLSceneWidget::_onResized(void) {
    qDebug() << "RESIZERY!!!!";
}

void EngineGLSceneWidget::_onGLMessageLogged(const QOpenGLDebugMessage &message) {
    if(!_wasGLError && _prevGLError != message.message()) {
        dbgprintf(message.message().toStdString().c_str());
        _prevGLError = message.message();
        _wasGLError = true;
    }
}
