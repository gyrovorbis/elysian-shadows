/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GL_UTILITIES_H
#define GL_UTILITIES_H

#include <QSurfaceFormat>
#include <QGLFormat>
#include <ESGamma/crtp/elysian_debug.hpp>

struct glUtilities {

    static void printGLSupport(void) {

        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL Version Check");
        Debug::push();
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_1_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_1));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_1_2 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_1_3 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_3));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_1_4 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_4));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_1_5 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_2_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_2_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_1));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_3_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_0));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_3_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_1));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_3_2 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_2));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_3_3 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_3));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_4_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_0));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_4_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_1));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_4_2 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_2));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_Version_4_3 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_4_3));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_ES_CommonLite_Version_1_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_CommonLite_Version_1_0));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_ES_CommonLite_Version_1_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_CommonLite_Version_1_1));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_ES_Common_Version_1_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Common_Version_1_0));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_ES_Common_Version_1_1 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Common_Version_1_1));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL_ES_Version_2_0 - %d", (bool)(int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0));
        Debug::Logf(Debug::LOG_VERBOSE, "OpenGL Support in ESTk - %d", glSupported());
        Debug::pop();
    }

    static bool glSupported(void) {
        return true;
           /* ((int)(QSurfaceFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) ||
            (int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_0) ||
            (int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Common_Version_1_1) ||
            (int)(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0));*/
    }

};



#endif // GLUTILITIES_H
