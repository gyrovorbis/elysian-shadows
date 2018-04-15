#include <crtp/elysian_camera_system.hpp>
#include <gyro_video_api.h>
#include <ESToolkit/ui/widget/tk_engine_gl_scene_widget.h>
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"
#include <elysian_timer_system.hpp>
#include <system/elysian_light_system.hpp>
#include <ESToolkit/ui/widget/tk_map_view.h>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <ESToolkit/ui/tk_estoolkit.h>
#ifdef GYRO_VMU_APP
#include <gyro_vmu_main_window.hpp>
#endif

#include <QMouseEvent>
#include <spine/spine.h>
#include <QOpenGLContext>

EngineGLSceneWidget* EngineGLSceneWidget::_self = nullptr;

void EngineGLSceneWidget::makeCurrentContext(void) {
    if(_self && QGLContext::currentContext() != _self->context()) {

        _self->makeCurrent();
       //     glalVidOglInit(_self->_width, _self->_height, _self->_width, _self->_height);
    }
}

void EngineGLSceneWidget::doneCurrentContext(void) {
    if(_self && QGLContext::currentContext() == _self->context()) _self->doneCurrent();
}

EngineGLSceneWidget::EngineGLSceneWidget(QWidget *parent):
    QGLWidget(parent),
    _engineTask("ESGamma")
{
    assert(!_self);
            _self = this;
            setAttribute(Qt::WA_NativeWindow);
            //setAutoBufferSwap(true);

    connect(this, SIGNAL(engineUpdated()), getAssets(), SIGNAL(engineUpdated()));
    connect(&_engineUpdateTimer, SIGNAL(timeout()), this, SLOT(_updateEngine()));

    if(format().swapInterval() == -1) {
        qWarning() << "Swap buffers at V_blank not available: trying to force approx 60fps!";
        _engineUpdateTimer.setInterval(17);
    } else {
        _engineUpdateTimer.setInterval(0);
    }
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
    DbgEvent event("QGLWidget::initializeGL()");
   // initializeOpenGLFunctions();
 //   auto* con = context();
  //  assert(con->isValid());
  //  makeCurrent();
   // initializeGLFunctions();
   // doneCurrent();

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
    _engineUpdateTimer.start();
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

    //QTimer::singleShot(TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME, this, SLOT(_updateEngine()));

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

   // auto delta = gyTimerTicks();

    _wasGLError = false;

    getAssets()->getEngine()->getTimerSys()->pushTaskMonitor(&_engineTask);

    _monitor.start();



    //doneCurrent();
    //swapBuffers();
    getAssets()->getEngine()->processInput();
    getAssets()->getEngine()->update();
    getAssets()->getEngine()->render();
#ifdef GYRO_VMU_APP
    if(ESToolkit::getInstance()->getVMUMainWindow()) {
        Debug::setEVMUActive(true);
        ESToolkit::getInstance()->getVMUMainWindow()->update();
        Debug::setEVMUActive(false);
    }
#endif

    _monitor.stop();

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

   // auto result = gyTimerTicks() - delta;

   // auto sec = (double)result/1000000000.0;
   //qDebug() << "nsec - " << result << "sec - " << sec << "hz - " << 1.0/sec;


    Q_EMIT engineUpdated();
}

void EngineGLSceneWidget::dumpPerfMonitor(void) {
    static bool once = false;
    if(!once) {
        _monitor.addChild(&getAssets()->getEngine()->updateMonitor);
        _monitor.addChild(&getAssets()->getEngine()->renderMonitor);
        _monitor.addChild(&getAssets()->getEngine()->processInputMonitor);
        once = true;
    }
    elysian::PerformanceMonitor::print(&_monitor);

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
