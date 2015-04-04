#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
#include <QOpenGLContext>
#include <QOpenGLTexture>
#include <QHash>
#include <QImage>
#include <QDebug>
#include <cassert>

static QHash<GLuint, QOpenGLTexture*> _qTextures;

int _gyVidTexAlloc(GYTexture *const tex) {
  //  const QOpenGLContext* current = QOpenGLContext::currentContext();

	QString fileName = tex->filename;

	QStringList list = fileName.split("/");
	if(list[1] == "assets") {
        //qDebug() << fileName;
		fileName = fileName.remove("/assets/");
        //qDebug() << fileName;
	}

    QImage pixmap(fileName);
    tex->width = pixmap.width();
    tex->height = pixmap.height();
    if(pixmap.isNull()) return 0; //fuck.

 //   tex->_pixmap = QPixmap(QString(tex->filename));
    QOpenGLTexture* glTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    //glTexture->setFormat(QOpenGLTexture::RGBA);
    glTexture->setData(pixmap, QOpenGLTexture::DontGenerateMipMaps);
    glTexture->bind();
    //((QOpenGLContext*)current)->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA,  QOpenGLContext::NoBindOption);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    tex->_data = (void*)(uintptr_t)glTexture->textureId();
      //  glTexImage2D(GL_TEXTURE_2D, 0, img.format, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, pixels);
    _qTextures[(GLuint)(uintptr_t)tex->_data] = glTexture;
    return (int)(uintptr_t)tex->_data;
}

int _gyVidTexFree(GYTexture *const tex) {
    (void)tex;
    if(!tex) return 0;
    QOpenGLTexture* qgl = _qTextures[(GLuint)(uintptr_t)tex->_data];
    qgl->destroy();
    _qTextures.remove((GLuint)(uintptr_t)tex->_data);
    delete qgl;
   // long int id = reinterpret_cast<long int>(tex->_data);
    //((QOpenGLContext*)QOpenGLContext::currentContext())->deleteTexture(id);
    return 1;
}

