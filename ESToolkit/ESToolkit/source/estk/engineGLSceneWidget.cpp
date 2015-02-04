#include "elysian_camera_system.hpp"
#include <gyro_video_api.h>
#include "engineGLSceneWidget.h"
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"
#include "elysian_light_system.hpp"

#include <QMouseEvent>


EngineGLSceneWidget::EngineGLSceneWidget(const QGLFormat &format, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags f):
    QGLWidget(format, parent, shareWidget, f) {
    makeCurrent();
    glalVidOglInit(1280, 720, 1280, 720);
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
    makeCurrent();

    unsigned parentWidth    = parentWidget()->size().width();
    unsigned parentHeight   = parentWidget()->size().height();

    glalVidOglInit(parentWidth, parentHeight, parentWidth, parentHeight);
     if(getAssets()->getEngine()) getAssets()->getEngine()->getCameraSys()->initializeProjMatrix();
}

void EngineGLSceneWidget::startRenderTimer(void) {
    _timer.start(TK_ENGINE_GL_SCENE_WIDGET_UPDATE_TIME, this);
}


void EngineGLSceneWidget::timerEvent(QTimerEvent *)
{
        updateGL();

        unsigned parentWidth    = parentWidget()->size().width();
        unsigned parentHeight   = parentWidget()->size().height();
        if(_width != parentWidth || _height != parentHeight) {
            resizeGL(parentWidth, parentHeight);
        }

}

void EngineGLSceneWidget::resizeGL(int w, int h) {
     glalVidOglInit(w, h, w, h);
     _width = w;
     _height = h;

 if(getAssets()->getEngine()) getAssets()->getEngine()->getCameraSys()->initializeProjMatrix();

}

void EngineGLSceneWidget::paintGL(void) {
	if(!getCurrentArea()) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gyMatLoadIdentity();

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

	getAssets()->getEngine()->render();

    emit engineUpdated();
}

EngineGLSceneWidget::~EngineGLSceneWidget(void){}

QSize EngineGLSceneWidget::sizeHint(void) const {
    if(getAssets()->getCurArea()) {
        return QSize(getAssets()->getCurArea()->getMapWidth()*32.0f, getAssets()->getCurArea()->getMapHeight()*32.0f);
    }
    return QSize(10,10);
}

void EngineGLSceneWidget::setCameraManualOnPlay(const bool val) {
    _cameraManualOnPlay = val;

    if(getAssets()->getEngine()->getEngineState() != elysian::Engine::ENGINE_ESTK_STOP) {
        _cameraManual = val;
    }

}
