#include "tk_documentation_browser.h"
#include <QTabWidget>
#include <QSplitter>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <elysian_debug.hpp>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>

/****************************************************************************
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
** http://doc.qt.digia.com/qq/qq28-qthelp.html
****************************************************************************/

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine,
                         QWidget* parent):QTextBrowser(parent),
                         _helpEngine(helpEngine)
{
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name){
    if (name.scheme() == "qthelp")
        return QVariant(_helpEngine->fileData(name));
    else
        return QTextBrowser::loadResource(type, name);
}


HelpDockWidget::HelpDockWidget(QWidget */*parent*/) {

#ifndef TOOLKIT_DOCUMENTATION_RESOURCES
    warnprintf("This build was compiled without documentation resource files!");

#else
    dbgprintf("Expanding ESToolkit User Manual files.");

    dbgpush();

    QDir dir(ESTK_USER_MANUAL_EXPANDED_DIR);
    if(!(dir.exists() || QDir::current().mkdir(ESTK_USER_MANUAL_EXPANDED_DIR))) {
        Debug::printf(Debug::LOG_WARNING, "Could not access docs directory! [%s]", dir.absolutePath().toStdString().c_str());

    } else {

        QDir oldCurrent = QDir::current();
        QDir::setCurrent(dir.absolutePath());

        //Stupid fucking Windows doesn't let us have write permission by default...
        QFile qchFile(ESTK_USER_MANUAL_QCH_FILE_NAME);
        qchFile.setPermissions(QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner);

        QFile qhcFile(ESTK_USER_MANUAL_QHC_FILE_NAME);
        qhcFile.setPermissions(QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner);


        if(QFile::exists(ESTK_USER_MANUAL_QCH_FILE_NAME) && !QFile::remove(ESTK_USER_MANUAL_QCH_FILE_NAME)) {
            Debug::printf(Debug::LOG_WARNING, "Could not overwrite existing QCH file!");
        } else {
            if(!QFile::copy(ESTK_USER_MANUAL_QCH_RESOURCE_PATH, ESTK_USER_MANUAL_QCH_FILE_NAME)) {
                Debug::printf(Debug::LOG_WARNING, "Could not expand QCH file to: [%s]", ESTK_USER_MANUAL_QCH_EXPANDED_PATH);
            }
        }

        if(QFile::exists(ESTK_USER_MANUAL_QHC_FILE_NAME) && !QFile::remove(ESTK_USER_MANUAL_QHC_FILE_NAME)) {
            Debug::printf(Debug::LOG_WARNING, "Could not overwrite existing QCH file!");
        } else {
            if(!QFile::copy(ESTK_USER_MANUAL_QHC_RESOURCE_PATH, ESTK_USER_MANUAL_QHC_FILE_NAME)) {
                Debug::printf(Debug::LOG_WARNING, "Could not expand QHC file to: [%s]", ESTK_USER_MANUAL_QHC_EXPANDED_PATH);
            }
        }

        QDir::setCurrent(oldCurrent.absolutePath());

    }

    QString dbFileName = dir.absolutePath()+QString("/")+ESTK_USER_MANUAL_DB_NAME+QString(".qhc");
    qDebug() << dbFileName;

        QHelpEngine* helpEngine = new QHelpEngine(dbFileName);
        if(!helpEngine->setupData()) {
            Debug::printf(Debug::LOG_WARNING, "QHelpEngine::setupData() failed!");
        }

        QTabWidget* tWidget = new QTabWidget;
        //tWidget->setWid(200);
        tWidget->addTab(helpEngine->contentWidget(), "Contents");
        tWidget->addTab(helpEngine->indexWidget(), "Index");

        HelpBrowser *textViewer = new HelpBrowser(helpEngine);
        textViewer->setSource(
               QUrl(ESTK_USER_MANUAL_HOME_PAGE));
        connect(helpEngine->contentWidget(),
           SIGNAL(linkActivated(QUrl)),
           textViewer, SLOT(setSource(QUrl)));

        connect(helpEngine->indexWidget(),
           SIGNAL(linkActivated(QUrl, QString)),
           textViewer, SLOT(setSource(QUrl)));

        QSplitter *horizSplitter = new QSplitter(Qt::Horizontal);
        horizSplitter->insertWidget(0, tWidget);
        horizSplitter->insertWidget(1, textViewer);
        horizSplitter->setSizes(QList<int>{300, 800});
       // horizSplitter->hide();

        setCentralWidget(horizSplitter);

        auto geo = geometry();
        geo.setWidth(700);
        geo.setHeight(500);
        setGeometry(geo);


       //addDockWidget(Qt::BottomDockWidgetArea, helpWindow);

        dbgpop();
    #endif
}
