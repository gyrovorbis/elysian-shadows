#ifndef GL_UTILITIES_H
#define GL_UTILITIES_H

#include <QGLFormat>
#include "debug.h"

struct glUtilities {

    static void printGLSupport(void) {
        Debug::Logf(Debug::VERBOSE, "OpenGL Version Check");
        Debug::Push();
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_1_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_1));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_1_2 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_1_3 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_3));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_1_4 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_4));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_1_5 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_2_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_2_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_1));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_3_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_0));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_3_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_1));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_3_2 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_2));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_3_3 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_3));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_4_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_0));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_4_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_1));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_4_2 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_2));
        Debug::Logf(Debug::VERBOSE, "OpenGL_Version_4_3 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_3));
        Debug::Logf(Debug::VERBOSE, "OpenGL_ES_CommonLite_Version_1_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_CommonLite_Version_1_0));
        Debug::Logf(Debug::VERBOSE, "OpenGL_ES_CommonLite_Version_1_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_CommonLite_Version_1_1));
        Debug::Logf(Debug::VERBOSE, "OpenGL_ES_Common_Version_1_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Common_Version_1_0));
        Debug::Logf(Debug::VERBOSE, "OpenGL_ES_Common_Version_1_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Common_Version_1_1));
        Debug::Logf(Debug::VERBOSE, "OpenGL_ES_Version_2_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0));
        Debug::Logf(Debug::VERBOSE, "OpenGL Support in ESTk - %d", glSupported());
        Debug::Pop();
    }

    static bool glSupported(void) {
        return
            ((int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) ||
            (int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_0) ||
            (int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Common_Version_1_1) ||
            (int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0));
    }

};



#endif // GLUTILITIES_H
