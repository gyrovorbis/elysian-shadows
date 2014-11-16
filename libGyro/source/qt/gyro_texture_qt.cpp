#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
#include <QGLContext>
#include <QDebug>
#include <cassert>

int _gyVidTexAlloc(GYTexture *const tex, const unsigned width, const unsigned height) {
    (void)width;
    (void)height;
    const QGLContext* current = QGLContext::currentContext();

    qDebug() << "CONTEXT - " << current;

 //   tex->_pixmap = QPixmap(QString(tex->filename));
    tex->_data = (void*)((QGLContext*)current)->bindTexture(QPixmap(QString(tex->filename)), GL_TEXTURE_2D);

    qDebug() << "TEXTURE " << tex->_data;

    return *(int*)&tex->_data;
}

int _gyVidTexFree(GYTexture *const tex) {
   // int id = reinterpret_cast<int>(tex->_data);
   // ((QGLContext*)QGLContext::currentContext())->deleteTexture(id);
    return 1;
}

