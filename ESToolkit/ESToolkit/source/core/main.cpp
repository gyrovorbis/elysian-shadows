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

void consumeArgs() {
    qDebug() << "ARGS CONSUMED!";
}

template <class First, class... Rest>
void consumeArgs(First first, Rest... rest) {
    // Do something with first
    qDebug() << "ARG" << first << endl;

    consumeArgs(rest...); // Unpack the arguments for further treatment
}


template<typename RType, typename... PTypes>
RType _callFunction(const char* funcName, PTypes... parameters) {
    qDebug() << funcName;
    //consumeArgs(parameters...);

    /*std::function<void(PTypes... A)> Unpack =*/
    [&](PTypes... A) {
        auto l = {((printf("%d\n", &A)), 0)...};
    }(parameters...);
}

int main(int argc, char *argv[]) {
    QRectF rect;
    rect.setSize(QSizeF(2.0f, 5.0f));

    QApplication a(argc, argv);
    a.setOrganizationName("Elysian Shadows");
    a.setOrganizationDomain("ESDev");
	a.setApplicationName("ESTk");
	a.setWindowIcon(QIcon(":/estk/images/estk.ico"));
	a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

	QPixmap splashScreen(":/splash/images/splashScreen.png");
	QPainter painter(&splashScreen);
	painter.setPen(QPen(QColor(225, 202, 251)));
    QFont splashFont("Arial", 10);
    splashFont.setBold(true);
	painter.setFont(splashFont);
	painter.drawText(25,82,QString("ESTkv")+QString(ESTOOLKIT_VERSION)+QString(" \"Audacious Cuntwhisp\" Release"));
	painter.drawText(25,190,"http://elysianshadows.com/tkreleasenotes");

    QSplashScreen splash(splashScreen, Qt::WindowStaysOnTopHint);
	splash.show();

    ESToolkit w;

	//QApplication::setWindowIcon(QIcon(":/estk/images/estk.ico"));
	QTimer::singleShot(2500, &splash, SLOT(close()));
	QTimer::singleShot(2500, &w, SLOT(showMaximized()));

    return a.exec();
}
