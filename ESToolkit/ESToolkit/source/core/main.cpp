#define QT_NO_OPENGL_ES_2 //GLEW NEEDS THIS OR IT'LL SHIT ITS DRAWS
#include <QApplication>

#include <QSplashScreen>
#include "estoolkit.h"
#include <QPainter>
#include <QTimer>
#include "elysian_debug.hpp"
#include <QDebug>
#include <QRectF>
#include <initializer_list>
#include <iostream>

#include "elysian_engine.hpp"
#include "elysian_debug.hpp"

GYAppCbRet Init(int argc, char **argv) {

    //QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    //QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    //QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, false);
    //QApplication::setAttribute(Qt::AA_Use96Dpi, true);

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
