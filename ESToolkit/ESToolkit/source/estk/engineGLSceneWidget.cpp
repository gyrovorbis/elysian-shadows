#include "elysian_camera_system.hpp"
#include <gyro_video_api.h>
#include "engineGLSceneWidget.h"
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"

#include <QMouseEvent>


EngineGLSceneWidget::EngineGLSceneWidget(const QGLFormat &format, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags f):
    QGLWidget(format, parent, shareWidget, f) {
    makeCurrent();
    glalVidOglInit(1280, 720, 1280, 720);
}

void EngineGLSceneWidget::stop(void) {
    getAssets()->getEngine()->setEngineState(elysian::Engine::ENGINE_ESTK_STOP);
}

void EngineGLSceneWidget::play(void) {
    getAssets()->getEngine()->setEngineState(elysian::Engine::ENGINE_NORMAL);
}

void EngineGLSceneWidget::advanceFrame(void) {
    _advanceFrame = true;
    getAssets()->getEngine()->setEngineState(elysian::Engine::ENGINE_NORMAL);
}

bool EngineGLSceneWidget::isPlaying(void) const { return getAssets()->getEngine()->getEngineState() != elysian::System::ENGINE_ESTK_STOP; }

void EngineGLSceneWidget::initializeGL(void) {
    makeCurrent();
    //sinitializeGLFunctions(context());

    unsigned parentWidth    = parentWidget()->size().width();
    unsigned parentHeight   = parentWidget()->size().height();
   // initializeOpenGLFunctions();
   // initializeGLFunctions();
        qDebug() << "VERSION " << (char*)glGetString(GL_VERSION) <<"SCIZOR" << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    //getCurArea()->populateVbo();
    qDebug() << "C++ CONTEXT - " << context();
    //updateGL();

    glalVidOglInit(parentWidth, parentHeight, parentWidth, parentHeight);
     if(getAssets()->getEngine()) getAssets()->getEngine()->getCameraSys()->initializeProjMatrix();

    //initTextures();
     //   geometries.init();

    // Use QBasicTimer because its faster than QTimer
  //
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

   // if(!once) {
           // setlocale(LC_NUMERIC, "C");
     glalVidOglInit(w, h, w, h);
     _width = w;
     _height = h;

     // Restore system locale
    // setlocale(LC_ALL, "");
     //   once = true;
   // }
 if(getAssets()->getEngine()) getAssets()->getEngine()->getCameraSys()->initializeProjMatrix();
    //gyVidCamOrthoProj(&cam, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 1000.0f);
    //gyVidCamUpdate(&cam);

    //makeCurrent();
    //gyVidOglInit(w, h, w, h);
    //updateGL();
   // qDebug() << isValid();
}

void EngineGLSceneWidget::paintGL(void) {
	if(!getCurrentArea()) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     //   qDebug() << "VERSION " << (char*)glGetString(GL_VERSION);
    //makeCurrent();
    //qDebug() << isValid();





    //gyVidSetDefaultShaderProgram();
    //getAssets()->getEngine()->getCameraSys()->update();

    //auto worldPos = GYVector3 { 0.0, 0.0, 1.0 };

    //gyVidSetCamWorldPos(&worldPos);

    //gyVidSceneBegin();

    gyMatLoadIdentity();


#if 0

        getCameraSys()->pushViewProjMatrix();
    auto color = GYColor4{ 255, 0, 255, 255 };
    auto vecColor = GYVector4 { 0.2f, 0.2f, 0.2f, 0.2f };


//for(unsigned i = 0; i < 1000; ++i)
   // getLightSys()->renderCube(i, i, i, i*10, i*10, i*10, 0);


    //gyVidSetGlobalAmbience(&vecColor);
    gyMatLoadIdentity();

    gyVidAlphaLevel(GY_ALPHA_FULL);

    gyVidQuadListBegin(&color);
    for(unsigned i = 0; i < 1000; ++i) {
        gyMatPush();
        gyMatTranslateScale(i*10, i*10, i, 10, 10, 1);

        gyVidRect(0);
        gyMatPop();
    }
    gyVidQuadListEnd();


    getCameraSys()->popViewProjMatrix();
/*
    gyVidAlphaLevel(GY_ALPHA_FULL);

    gyVidQuadListBegin(&color);
    for(unsigned i = 0; i < 1000; ++i) {
        gyMatPush();
        gyMatTranslateScale(i, i, i, i*10, i*10, i);

        gyVidRect(0);
        gyMatPop();
    }
    gyVidQuadListEnd();
*/


  //  for(unsigned i = 0; i < 1000; ++i)
    //    getLightSys()->renderCube(i, i, i, i*10, i*10, i*10, 0);

#else
    getAssets()->getEngine()->update();
    getAssets()->getEngine()->processInput();
    if(_advanceFrame) {
        _advanceFrame = false;
        getAssets()->getEngine()->setEngineState(elysian::System::ENGINE_ESTK_STOP);
    }

	getAssets()->getEngine()->getCameraSys()->setFollowEntity(nullptr);
	getAssets()->getEngine()->getCameraSys()->setDynamic(false);
	getAssets()->getEngine()->getCameraSys()->setBoundByMap(false);
	getAssets()->getEngine()->render();

#endif


    //gyVidSceneEnd();

//engine->update();
  //  engine->render();

    emit engineUpdated();
}




EngineGLSceneWidget::~EngineGLSceneWidget(void)
{
}

QSize EngineGLSceneWidget::sizeHint(void) const {
    if(getAssets()->getCurArea()) {
        return QSize(getAssets()->getCurArea()->getMapWidth()*32.0f, getAssets()->getCurArea()->getMapHeight()*32.0f);
    }
    return QSize(10,10);
}
