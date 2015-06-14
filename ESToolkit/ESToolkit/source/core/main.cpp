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
#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>
#include <QDebug>
#include <QPushButton>

#ifdef ESTK_UNIT_TESTS
#include <test_estk.h>
#endif

enum TK_CRASH_STATE {
    TK_CRASH_BUG_REPORT,
    TK_CRASH_RESUME,
    TK_CRASH_ABORT
};


static TK_CRASH_STATE _tkCrashReport(QStringList& /*args*/) {
    QDir::setCurrent(QDir::homePath());
    QFile file("ESTk/gamma_debug.txt");
    QString logText = "Debug log file could not be found!";

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << logText;
    } else {
        QTextStream in(&file);
        logText = in.readAll();
        file.close();
    }

    QMessageBox msgBox;
    msgBox.setText("Shit has Reested!");
    msgBox.setInformativeText("ESToolkit just shit its pants and was forced to close after encountering a fatal exception.");

    auto* bugReport = msgBox.addButton("Submit Bug Report", QMessageBox::HelpRole);
    msgBox.setStandardButtons(QMessageBox::Abort);
    auto* resume    = msgBox.addButton("Resume", QMessageBox::AcceptRole);
    msgBox.setDefaultButton(bugReport);
    msgBox.setIcon(QMessageBox::Critical);

    msgBox.setDetailedText(logText);

    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();

    TK_CRASH_STATE crashState;
    auto* clicked = msgBox.clickedButton();

    if(clicked == resume) {
        crashState = TK_CRASH_RESUME;
    } else if(clicked == bugReport) {
        crashState = TK_CRASH_BUG_REPORT;
    } else {
        crashState = TK_CRASH_ABORT;
    }

    return crashState;
}


static GYAppCbRet Init(int argc, char **argv) {
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings); //WITHOUT THIS OSX SHITS ITSELF WHEN FUCKING WITH WIDGET SIZING/LOCATIONS
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QSurfaceFormat fmt;
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


    QApplication a(argc, argv);
    auto args = QApplication::arguments();

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

    QString project;
    QString level;
    QString area;

    bool relaunching = false;
    for(auto it = args.begin(); it != args.end(); ++it) {
        if(*it == ESTOOLKIT_CRASH_CMD_LINE_ARG) {
            relaunching = true;
        } else if(*it == "--project") {
            project = *(it+1);
        } else if(*it == "--level") {
            level = *(it+1);
        } else if(*it == "--area") {
            area = *(it+1);
        }
    }


#ifdef ESTK_UNIT_TESTS
    if(argc != 1 && !relaunching) {
        return (GYAppCbRet)tktest::main(args);
    }
#endif
    GYFileInitParams fileParams;
    strcpy(fileParams.workingDir, Q_CSTR(QDir::homePath()));
    gyFileInit(&fileParams);
    QDir outputDir(QDir::current());
    if(!outputDir.cd(ESTOOLKIT_OUTPUT_DIR) && !outputDir.mkdir(ESTOOLKIT_OUTPUT_DIR)) {
        qCritical() << QString("Could not set ESTk working directory: [%s]") << QDir().current().filePath(ESTOOLKIT_OUTPUT_DIR);
    } else {
        QDir::setCurrent(QDir::current().filePath(ESTOOLKIT_OUTPUT_DIR));
    }

    ESToolkit w;
    w.setOutputDir(outputDir);
    //QApplication::setWindowIcon(QIcon(":/estk/images/estk.ico"));

    if(!relaunching) {
#ifdef QT_NO_DEBUG
        splash.show();
        QTimer::singleShot(2500, &splash, SLOT(close()));
        QTimer::singleShot(2500, &w, SLOT(showMaximized()));
#else
        QTimer::singleShot(0, &w, SLOT(showMaximized()));
#endif
    } else {

        //No splash screen if we're recovering from a crash
        switch(_tkCrashReport(args)) {
        case TK_CRASH_ABORT:
            return (GYAppCbRet)0;
        case TK_CRASH_RESUME:
            w.showMaximized();
            break;
        case TK_CRASH_BUG_REPORT:
            w.showMaximized();
            w.showGitlabBugReportTab();
        }

        //Recover previous state
        if(!project.isNull())       w.openProjectDir(project);
        if(!level.isNull())         w.getAssets().setCurrentLevel(level);
        if(!area.isNull())          w.getAssets().setCurrentArea(area);
    }

    return (GYAppCbRet)a.exec();
}




int main(int argc, char* argv[]) {
    return Init(argc, argv);
    //gySysRegisterAppInit(&Init);
}
