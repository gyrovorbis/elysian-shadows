
#include "elysian_camera_system.hpp"
#include <gyro_video_api.h>
#include "engineGLSceneWidget.h"
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"
#include "elysian_light_system.hpp"

#include <QMouseEvent>
#include <spine/spine.h>
//#undef QT_NO_KEYWORDS


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

spAtlas* atlas;
spSkeletonData* skeletonData;
spSkeleton* skeleton;
spAnimationState* state;
spSkeletonJson* json;
spAnimationStateData* stateData;


static void _initSpineShit(void) {


    atlas = spAtlas_createFromFile("/assets/raptor.atlas", 0);
    json = spSkeletonJson_create(atlas);
    json->scale = 0.5f;
    skeletonData = spSkeletonJson_readSkeletonDataFile(json, "/assets/raptor.json");
    if (!skeletonData) {
       printf("Error: %s\n", json->error);
       exit(0);
    }
   spSkeletonJson_dispose(json);

   spBone_setYDown(true);

//         SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
        skeleton = spSkeleton_create(skeletonData);

        stateData = spAnimationStateData_create(skeletonData);
        state = spAnimationState_create(stateData);

       // drawable->timeScale = 1;

        skeleton->x = 320;
        skeleton->y = 500;
        spSkeleton_updateWorldTransform(skeleton);

        spAnimationState_setAnimationByName(state, 0, "walk", true);
        spAnimationState_setAnimationByName(state, 1, "empty", false);
        spAnimationState_addAnimationByName(state, 1, "gungrab", false, 2);
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

static void _renderSpineShit(void) {
      //  gyVidSkeleton(skeleton);

}

void EngineGLSceneWidget::paintGL(void) {
	if(!getCurrentArea()) return;

    static bool once = false;
    if(!once) {
        _initSpineShit();
        once = true;
    }

    float deltaTime = getAssets()->getEngine()->getTimerSys()->getDeltaTime();
    spSkeleton_update(skeleton, deltaTime);
    spAnimationState_update(state, deltaTime * 1.0f);
    spAnimationState_apply(state, skeleton);
    spSkeleton_updateWorldTransform(skeleton);


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

    getAssets()->getEngine()->render(&_renderSpineShit);

    Q_EMIT engineUpdated();
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
