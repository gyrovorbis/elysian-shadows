#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
#include <QGLContext>
#include <QDebug>
#include <cassert>

int _gyVidTexAlloc(GYTexture *const tex) {
	const QGLContext* current = QGLContext::currentContext();

	QString fileName = tex->filename;

	QStringList list = fileName.split("/");
	if(list[1] == "assets") {
        //qDebug() << fileName;
		fileName = fileName.remove("/assets/");
        //qDebug() << fileName;
	}

	QPixmap pixmap(fileName);
    tex->width = pixmap.width();
    tex->height = pixmap.height();

	pixmap = pixmap.transformed(QTransform().scale(1, -1));

 //   tex->_pixmap = QPixmap(QString(tex->filename));
    *(int*)&tex->_data = ((QGLContext*)current)->bindTexture(pixmap, GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      //  glTexImage2D(GL_TEXTURE_2D, 0, img.format, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, pixels);

    return *(int*)&tex->_data;
}

int _gyVidTexFree(GYTexture *const tex) {
    (void)tex;
    long int id = reinterpret_cast<long int>(tex->_data);
    ((QGLContext*)QGLContext::currentContext())->deleteTexture(id);
    return 1;
}

