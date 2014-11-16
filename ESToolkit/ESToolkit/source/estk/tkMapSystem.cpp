#include "tkMapSystem.h"
#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"

void TkMapSystem::initializeGL(void) {
    makeCurrent();
        qDebug() << "VERSION " << (char*)glGetString(GL_VERSION) <<"SCIZOR" << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    gyVidOglInit(size().width(), size().height(), size().width(), size().height());
    //getCurArea()->populateVbo();
    qDebug() << "C++ CONTEXT - " << context();
    //updateGL();


}

void TkMapSystem::resizeGL(int w, int h) {
    //makeCurrent();
    //gyVidOglInit(w, h, w, h);
    updateGL();
    qDebug() << isValid();
}

void TkMapSystem::paintGL(void) {
        qDebug() << "VERSION " << (char*)glGetString(GL_VERSION);
   // makeCurrent();
    qDebug() << isValid();
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
engine->update();
    engine->render();
}

void TkMapSystem::paintEvent(QPaintEvent* event) {

}
