#include <QApplication>

#include <QSplashScreen>
#include <QSurfaceFormat>
#include <tk_estoolkit.h>
#include <QPainter>
#include <QTimer>
#include <QOpenGLContext>
#include "elysian_debug.hpp"
#include <QDebug>
#include <QRectF>
#include <QGLFormat>
#include <initializer_list>
#include <iostream>
#include <QOpenGLContext>
#include <QCommandLineParser>
#include "elysian_engine.hpp"
#include "elysian_debug.hpp"
#include <gyro_file_api.h>
#include <QDir>

#ifdef ESTK_UNIT_TESTS
#include <test_estk.h>
#endif


static GYAppCbRet Init(int argc, char **argv) {
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings); //WITHOUT THIS OSX SHITS ITSELF WHEN FUCKING WITH WIDGET SIZING/LOCATIONS
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);


//#else
    //QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

//#endif
   // QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    //QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    //QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    //QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, false);
    //QApplication::setAttribute(Qt::AA_Use96Dpi, true);
    QSurfaceFormat fmt;
   // fmt.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
   // fmt.setSamples(0);
    //fmt.setStereo(false);
   // fmt.setAlphaBufferSize(0);
   // fmt.setStencilBufferSize(0);
  /*  fmt.setSamples(1);
    fmt.setRedBufferSize(8); Uhh... Qt does a better job than I can... fuck.
    fmt.setBlueBufferSize(8);
    fmt.setGreenBufferSize(8);*/
    //fmt.setDepthBufferSize(32);
    //fmt.setAlphaBufferSize(8);
    fmt.setVersion( 3, 3 );
    fmt.setProfile( QSurfaceFormat::CoreProfile ); // Requires >=Qt-4.8.0
    fmt.setOption(QSurfaceFormat::DebugContext);
    // Set the default GL format to OpenGL 3.2 Core
    QSurfaceFormat::setDefaultFormat(fmt);

    QGLFormat glfmt;
            glfmt.setDepthBufferSize(32);
            //fmt.setAlphaBufferSize(8);
            glfmt.setVersion( 3, 3 );
            glfmt.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
            // Set the default GL format to OpenGL 3.2 Core
            QGLFormat::setDefaultFormat(glfmt);


    QRectF rect;
    rect.setSize(QSizeF(2.0f, 5.0f));


    QApplication a(argc, argv);

    qDebug() << "MAIN" << QOpenGLContext::currentContext();
    a.setOrganizationName("Elysian Shadows");
    a.setOrganizationDomain("elysianshadows.com");
    a.setApplicationName("ESTk");
    a.setApplicationVersion(QString("ESTkv")+QString(ESTOOLKIT_VERSION)+QString(" ")+QString(ESTOOLKIT_VERSION_NAME));
    a.setWindowIcon(QIcon(":/estk/images/estk.ico"));

    QPixmap splashScreen(":/splash/images/splashScreen.png");
    QPainter painter(&splashScreen);
    painter.setPen(QPen(QColor(225, 202, 251)));
    QFont splashFont("Arial", 10);
    splashFont.setBold(true);
    painter.setFont(splashFont);
    painter.drawText(25,82,QString("ESTkv")+QString(ESTOOLKIT_VERSION)+QString(" ")+QString(ESTOOLKIT_VERSION_NAME));
    painter.drawText(25,190,"http://elysianshadows.com/tkreleasenotes"); /* This is an invalid URL!!!! */

    QSplashScreen splash(splashScreen, Qt::WindowStaysOnTopHint);


#ifdef ESTK_UNIT_TESTS
    if(argc != 1) {
        return (GYAppCbRet)tktest::main(argc, argv);
    }
#endif
    GYFileInitParams fileParams;
    strcpy(fileParams.workingDir, Q_CSTR(QDir::homePath()));
    gyFileInit(&fileParams);
    QDir outputDir(QDir::current());
    if(!outputDir.cd(ESTOOLKIT_OUTPUT_DIR) && !outputDir.mkdir(ESTOOLKIT_OUTPUT_DIR)) {
        qCritical() << QString("Could not set ESTk working directory: [%s]") << QDir().current().filePath(ESTOOLKIT_OUTPUT_DIR);
    } else QDir::setCurrent(QDir::current().filePath(ESTOOLKIT_OUTPUT_DIR));

    ESToolkit w;
    //QApplication::setWindowIcon(QIcon(":/estk/images/estk.ico"));

// We don't want to wait for the splash screen whilst debugging. --Sara.
#ifdef QT_NO_DEBUG
    splash.show();
    QTimer::singleShot(2500, &splash, SLOT(close()));
    QTimer::singleShot(2500, &w, SLOT(showMaximized()));
#else
   //QTimer::singleShot(1000, &splash, SLOT(close()));
    QTimer::singleShot(0, &w, SLOT(showMaximized()));
#endif

    a.exec();


    return GY_RET_CONTINUE;
}




int main(int argc, char* argv[]) {
    return Init(argc, argv);
    //gySysRegisterAppInit(&Init);
}
