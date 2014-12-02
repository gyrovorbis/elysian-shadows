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

    QPixmap pixmap(tex->filename);
    pixmap = pixmap.transformed(QTransform().scale(1, -1));


    qDebug() << "CONTEXT - " << current;

 //   tex->_pixmap = QPixmap(QString(tex->filename));
    *(int*)&tex->_data = ((QGLContext*)current)->bindTexture(pixmap, GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      //  glTexImage2D(GL_TEXTURE_2D, 0, img.format, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, pixels);

    qDebug() << "TEXTURE " << tex->_data;

    return *(int*)&tex->_data;
}

int _gyVidTexFree(GYTexture *const tex) {
   // int id = reinterpret_cast<int>(tex->_data);
   // ((QGLContext*)QGLContext::currentContext())->deleteTexture(id);
    return 1;
}

