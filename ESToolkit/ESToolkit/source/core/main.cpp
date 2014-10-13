#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QSplashScreen>
#include "estoolkit.h"
#include "debug.h"
#include <QDebug>
#include <QRectF>
#include <initializer_list>
#include <iostream>
#include "luaTableStandardItemModel.h"

#include "elysian_engine.hpp"
#include "elysian_debug.hpp"

elysian::Engine *engine;
elysian::Debug *debug;

void GammaInit(void) {
    /* The File subsystem needs to be initialized before anything else, ideally */
    gyFileInit("ESTeam", "ESGamma");

    debug = &elysian::Debug::getInstance();
    char verString[50];

    debug->parseArguments();
    strcpy(verString, "ESGamma v");
    strcat(verString, GYRO_LIB_VERSION);

    GYVidInitParams vidParams =
#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_Qt
    { (GY_VID_RESOLUTION)debug->getCmdLineArgs().resolution, GY_WIN_RESIZABLE, 480, 270, 1280, 720, verString, "icon.bmp"};
#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
    { GY_VID_1280x720, GY_WIN_FULLSCREEN, 1280, 720, 1280, 720 };
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
    { 0 };
#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
    { 0 };
#else
    { 0 };

#endif

    gySysInit();
    //gyVidInit(&vidParams);
    gyAudInit();
    //gyInputInit();
    //gyTimerInit();

    engine = new elysian::Engine;

    engine->loadAssets();
    debug->setEngine(*engine);
    //debug->sandboxInit();
    debug->initializeEngine();

}

GYAppCbRet Init(int argc, char **argv) {
    QRectF rect;
    rect.setSize(QSizeF(2.0f, 5.0f));

    QApplication a(argc, argv);
    a.setOrganizationName("Elysian Shadows");
    a.setOrganizationDomain("elysianshadows.com");
    a.setApplicationName("ESTk");
    a.setWindowIcon(QIcon(":/estk/images/estk.ico"));
    a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QPixmap splashScreen(":/splash/images/splashScreen.png");
    QPainter painter(&splashScreen);
    painter.setPen(QPen(QColor(225, 202, 251)));
    QFont splashFont("Arial", 10);
    splashFont.setBold(true);
    painter.setFont(splashFont);
    painter.drawText(25,82,QString("ESTkv")+QString(ESTOOLKIT_VERSION)+QString(" \"Sex and Candy\" Release"));
    painter.drawText(25,190,"http://elysianshadows.com/tkreleasenotes"); /* This is an invalid URL!!!! */

    QSplashScreen splash(splashScreen, Qt::WindowStaysOnTopHint);
    splash.show();

    ESToolkit w;

    GammaInit();

    //QApplication::setWindowIcon(QIcon(":/estk/images/estk.ico"));

// We don't want to wait for the splash screen whilst debugging. --Sara.
#ifdef QT_NO_DEBUG
    QTimer::singleShot(2500, &splash, SLOT(close()));
    QTimer::singleShot(2500, &w, SLOT(showMaximized()));
#else
    QTimer::singleShot(1000, &splash, SLOT(close()));
    QTimer::singleShot(0, &w, SLOT(showMaximized()));
#endif

    int execVal = a.exec();

    return GY_RET_CONTINUE;
}




void gyro_main(void) {
    gySysRegisterAppInit(&Init);
}
