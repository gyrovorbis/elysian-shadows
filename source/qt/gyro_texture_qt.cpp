#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <QGLWidget>
//#include <QOpenGLWidget>
#include <gyro_input_api.h>
#include <QHash>
#include <QImage>
#include <QDebug>
#include <cassert>


extern GYInputInitParams    _initParams;
//static QHash<GLuint, QOpenGLTexture*> _qTextures;

int glalDefaultFbo(void) {
    //return reinterpret_cast<QOpenGLWidget*>(_initParams.glWidget)->defaultFramebufferObject();
return 0;
}

int _gyVidTexAlloc(GYTexture *const tex) {
    auto checkPowerOfTwo = [](const unsigned dimension) {
        return (dimension == 2 || dimension == 4 || dimension == 8 || dimension == 16 ||
                dimension == 32 || dimension == 64 || dimension == 128 || dimension == 256 ||
                dimension == 512 || dimension == 1024);
    };


     //static_cast<QOpenGLWidget*>(_initParams.glWidget)->makeCurrent();

    QString fileName = tex->filename;

    QStringList list = fileName.split("/");
    if(list.size() >= 2) {
        if(list[1] == "assets") {
            //qDebug() << fileName;
            fileName = fileName.remove("/assets/");
            //qDebug() << fileName;
        }
    }

    QImage pixmap(fileName);
    tex->width = pixmap.width();
    tex->height = pixmap.height();

    if(!checkPowerOfTwo(tex->width) || !checkPowerOfTwo(tex->height)) {
        _gyLog(GY_DEBUG_ERROR, "%s - <%d, %d> dimensions are not powers of two, dipshit!", tex->filename, tex->width, tex->height);
    }

    if(pixmap.isNull()) {
        tex->_data = 0;
//         static_cast<QOpenGLWidget*>(_initParams.glWidget)->doneCurrent();
        return 0; //fuck.
    }

static_cast<QGLWidget*>(_initParams.glWidget)->makeCurrent();


     tex->_data = (void*)(uintptr_t) static_cast<QGLWidget*>(_initParams.glWidget)->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::NoBindOption);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //  tex->_data = (void*)(uintptr_t)glTexture->textureId();
      //  glTexImage2D(GL_TEXTURE_2D, 0, img.format, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, pixels);
 //   _qTextures[(GLuint)(uintptr_t)tex->_data] = glTexture;

   //  static_cast<QOpenGLWidget*>(_initParams.glWidget)->doneCurrent();
    return (int)(uintptr_t)tex->_data;
}

int _gyVidTexFree(GYTexture *const tex) {
    (void)tex;
    if(!tex) return 0;
   // QOpenGLTexture* qgl = _qTextures[(GLuint)(uintptr_t)tex->_data];
   // qgl->destroy();
   // _qTextures.remove((GLuint)(uintptr_t)tex->_data);
    //delete qgl;
   // long int id = reinterpret_cast<long int>(tex->_data);
    //((QOpenGLContext*)QOpenGLContext::currentContext())->deleteTexture(id);
    return 1;
}

