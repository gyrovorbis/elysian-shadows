/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_texture_api.h>
#include <libGyro/gyro_system_api.h>
#include <gl/gyro_gl.h>
#include <QGLWidget>
#include <QOpenGLContext>
//#include <QOpenGLWidget>
#include <libGyro/gyro_input_api.h>
#include <QHash>
#include <QImage>
#include <QDebug>
#include <cassert>


extern GYInputInitParams    _initParams;
//static QHash<GLuint, QOpenGLTexture*> _qTextures;
uint32_t glalDefaultFbo(void) {
    if(!_initParams.glWidget) return 0;
    return static_cast<QGLWidget*>(_initParams.glWidget)->context()->contextHandle()->defaultFramebufferObject();
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
    QImage qimg(fileName);
    tex->width  = qimg.width();
    tex->height = qimg.height();

    if(qimg.isNull()) {
        //_gyLog(GY_DEBUG_ERROR,"SHIT HAS REESTED TO ALL FUCK! <%d, %d>", tex->width, tex->height);
        tex->_data = nullptr;
//         static_cast<QOpenGLWidget*>(_initParams.glWidget)->doneCurrent();
        return 0; //fuck.
    }

        tex->bpp    = qimg.depth();
      //  _gyLog(GY_DEBUG_WARNING, "BIT DEPTH BITCH - %d", tex->bpp);
#if 1
        qimg = qimg.mirrored();

         qimg = QGLWidget::convertToGLFormat(qimg);
#endif


    tex->width  = qimg.width();
    tex->height = qimg.height();

    if(qimg.isNull()) {
      //  _gyLog(GY_DEBUG_ERROR,"SHIT HAS REESTED TO ALL FUCK!");
        tex->_data = nullptr;
//         static_cast<QOpenGLWidget*>(_initParams.glWidget)->doneCurrent();
        return 0; //fuck.
    }

    if(!checkPowerOfTwo(tex->width) || !checkPowerOfTwo(tex->height)) {
        _gyLog(GY_DEBUG_WARNING, "%s - <%d, %d> dimensions are not powers of two (suboptimal performance on some platforms).", tex->filename, tex->width, tex->height);
    }

    static_cast<QGLWidget*>(_initParams.glWidget)->makeCurrent();

#if 0
     tex->_data = (void*)(uintptr_t) static_cast<QGLWidget*>(_initParams.glWidget)->bindTexture(qimg, GL_TEXTURE_2D, GL_RGBA, QGLContext::NoBindOption);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#else
    glGenTextures(1, (GLuint*)&tex->_data);
    glBindTexture(GL_TEXTURE_2D, (GLuint)(uintptr_t)tex->_data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, qimg.width(), qimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, qimg.bits());
    glEnable(GL_TEXTURE_2D);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return (int)(uintptr_t)tex->_data;
}

int _gyVidTexFree(GYTexture *const tex) {
    if(!tex) return 0;
    static_cast<QGLWidget*>(_initParams.glWidget)->deleteTexture((GLuint)(uintptr_t)tex->_data);
   // QOpenGLTexture* qgl = _qTextures[(GLuint)(uintptr_t)tex->_data];
   // qgl->destroy();
   // _qTextures.remove((GLuint)(uintptr_t)tex->_data);
    //delete qgl;
   // long int id = reinterpret_cast<long int>(tex->_data);
    //((QOpenGLContext*)QOpenGLContext::currentContext())->deleteTexture(id);
    return 1;
}

