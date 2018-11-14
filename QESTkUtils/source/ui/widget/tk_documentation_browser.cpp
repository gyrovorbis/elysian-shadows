#include <QESTkUtils/ui/widget/tk_documentation_browser.h>
#include <QTabWidget>
#include <QSplitter>
#include <QFile>
#include <QDesktopServices>
#include <QDir>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
#   include <QHelpSearchEngine>
#   include <QHelpSearchResultWidget>
#   include <QVBoxLayout>
#   include <QPushButton>
#   include <QLineEdit>
#endif

namespace QESTkUtils {

/****************************************************************************
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
** http://doc.qt.digia.com/qq/qq28-qthelp.html
****************************************************************************/

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine,
                         QWidget* parent):
                            QTextBrowser(parent),
                            _helpEngine(helpEngine)
{
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(onAnchorClicked(const QUrl&)));
}

#if 0
if(ESToolkit::getInstance()->getAssets().getCurrentProject()) {
    QString localFile = link.toLocalFile();
    QString absPath = FileUtilities::getAbsoluteAssetPath(localFile);
    auto* asset = ESToolkit::getInstance()->getAssetDockWidget()->getAssetFileSystemModel()->getCustomAssetFileType(absPath);
    if(asset) asset->doubleClick(QModelIndex());
    else
} else {
   QMessageBox::critical(this, "Cannot open link!", "A valid project must be loaded before you can follow this link!");
}
#endif

void HelpBrowser::onAnchorClicked(const QUrl &link) {
    if(link.scheme() == "qthelp") {
        setSource(link);
    } else {
        QDesktopServices::openUrl(link);
    }
}

QVariant HelpBrowser::loadResource(int type, const QUrl& name){
    if (name.scheme() == "qthelp")
        return QVariant(_helpEngine->fileData(name));
    else
        return QTextBrowser::loadResource(type, name);
}


HelpDockWidget::HelpDockWidget(QWidget* parent,
               QString dbName,
               QString dbResourcePath,
               QString homepage,
               QString basePath):
    QMainWindow(parent),
    _dbName(std::move(dbName)),
    _homePageUrl(std::move(homepage)),
    _dbResourcePath(std::move(dbResourcePath)),
    _basePath(std::move(basePath))
{
    move(qApp->desktop()->availableGeometry(this).center()-geometry().center());
#ifndef QESTKUTILS_DOCUMENTATION_RESOURCES
    warnprintf("This build was compiled without documentation resource files!");

#else
    Debug::log(Debug::LOG_VERBOSE, "Expanding Documentation Database: [%s]", Q_CSTR(_dbName));

    Debug::push();

    QDir oldCurrent = QDir::current();
    QString dbFilePath;

    //Change into basePath if provided, otherwise assume current working directory as root!
    if(_basePath.isEmpty() || (!_basePath.isEmpty() && QDir::setCurrent(QDir(_basePath).absolutePath()))) {
        QDir dir(QESTKUTILS_HELP_DB_EXPANDED_DIR);

        if(!(dir.exists() || QDir::current().mkdir(QESTKUTILS_HELP_DB_EXPANDED_DIR))) {
            Debug::log(Debug::LOG_WARNING, "Could not access docs directory! [%s]", Q_CSTR(dir.absolutePath()));

        } else {
            QDir::setCurrent(dir.absolutePath());

            const QString qchFileName = _dbName + QString(".qch");
            const QString qhcFileName = _dbName + QString(".qhc");
            const QString resourcePath = _dbResourcePath + QString("/") + _dbName;
            const QString qchResourcePath = resourcePath + QString(".qch");
            const QString qhcResourcePath = resourcePath + QString(".qhc");

            //Stupid fucking Windows doesn't let us have write permission by default...
            QFile qchFile(qchFileName);
            qchFile.setPermissions(QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner);

            QFile qhcFile(qhcFileName);
            qhcFile.setPermissions(QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner);

            if(QFile::exists(qchFileName) && !QFile::remove(qchFileName)) {
                Debug::log(Debug::LOG_WARNING, "Could not overwrite existing QCH file!");
            } else {
                if(!QFile::copy(qchResourcePath, qchFileName)) {
                    Debug::log(Debug::LOG_WARNING, "Could not expand QCH file from [%s] to [%s]", Q_CSTR(qchResourcePath), Q_CSTR(QFileInfo(qchFile).absolutePath()));
                }
            }

            if(QFile::exists(qhcFileName) && !QFile::remove(qhcFileName)) {
                Debug::log(Debug::LOG_WARNING, "Could not overwrite existing QCH file!");
            } else {
                if(!QFile::copy(qhcResourcePath, qhcFileName)) {
                    Debug::log(Debug::LOG_WARNING, "Could not expand QHC file from [%s] to [%s]", Q_CSTR(qhcResourcePath), Q_CSTR(QFileInfo(qhcFile).absolutePath()));
                }
            }
            dbFilePath = QFileInfo(qhcFile).absolutePath();

            QDir::setCurrent(oldCurrent.absolutePath());

        }

    } else {
        Debug::log(Debug::LOG_ERROR, "Base path does not exist! [%s]", Q_CSTR(_basePath));
    }

    if(!dbFilePath.isEmpty()) {
        dbFilePath += QString("/") + _dbName + QString(".qhc");
        Debug::log(Debug::LOG_VERBOSE, "Attempting to load expanded DB file: [%s]", Q_CSTR(dbFilePath));
    }
    QHelpEngine* helpEngine = new QHelpEngine(dbFilePath);
    connect(helpEngine->searchEngine(), SIGNAL(indexingStarted()), this, SLOT(onIndexingStarted()));
    connect(helpEngine->searchEngine(), SIGNAL(indexingFinished()), this, SLOT(onIndexingFinished()));
    if(!helpEngine->setupData()) {
        Debug::log(Debug::LOG_ERROR, "QHelpEngine::setupData() failed!");
        _dbLoaded = false;
    } else _dbLoaded = true;

    QTabWidget* tWidget = new QTabWidget;
    tWidget->setContentsMargins(0, 0, 0, 0);
    tWidget->addTab(helpEngine->contentWidget(), "Contents");
    tWidget->addTab(helpEngine->indexWidget(), "Index");

    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->setMargin(0);
    searchLayout->setSpacing(0);
    QWidget* searchWidget = new QWidget;
    _searchLineEdit = new QLineEdit;
    _searchButton = new QPushButton("Search");
    searchLayout->addWidget(_searchLineEdit);
    searchLayout->addWidget(_searchButton);
    connect(_searchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));
    connect(_searchLineEdit, SIGNAL(returnPressed()), this, SLOT(onSearchButtonClicked()));
    searchWidget->setLayout(searchLayout);

    QVBoxLayout* naviLayout = new QVBoxLayout;
    naviLayout->setMargin(0);
    naviLayout->setSpacing(0);
    naviLayout->addWidget(tWidget);
    naviLayout->addWidget(searchWidget);
    QWidget* naviWidget = new QWidget;
    naviWidget->setLayout(naviLayout);
    naviWidget->setContentsMargins(0, 0, 0, 0);

    HelpBrowser *textViewer = new HelpBrowser(helpEngine);
    _helpBrowser = textViewer;
    textViewer->setSource(
           QUrl(_homePageUrl));
    connect(helpEngine->contentWidget(),
       SIGNAL(linkActivated(QUrl)),
       textViewer, SLOT(setSource(QUrl)));

    connect(helpEngine->indexWidget(),
       SIGNAL(linkActivated(QUrl, QString)),
       textViewer, SLOT(setSource(QUrl)));

    _searchResultWidget = helpEngine->searchEngine()->resultWidget(); //force the bitch to be constructed, so it's ready to show results.
    connect(_searchResultWidget, SIGNAL(requestShowLink(const QUrl&)), this, SLOT(onSearchResultsRequestShowLink(const QUrl&)));
    connect(helpEngine->searchEngine(), SIGNAL(searchingFinished(int)), this, SLOT(onSearchFinished(int)));

    QSplitter *horizSplitter = new QSplitter(Qt::Horizontal);
    horizSplitter->setContentsMargins(0, 0, 0, 0);
    horizSplitter->setLineWidth(0);
    horizSplitter->insertWidget(0, naviWidget);
    horizSplitter->insertWidget(1, textViewer);
    horizSplitter->setSizes(QList<int>{300, 800});
   // horizSplitter->hide();
    setCentralWidget(horizSplitter);

    auto geo = geometry();
    geo.setWidth(700);
    geo.setHeight(500);
    setGeometry(geo);

   //addDockWidget(Qt::BottomDockWidgetArea, helpWindow);

    Debug::pop();
#endif
}

void HelpDockWidget::openSearch(QString string) {
    if(!_helpBrowser) {
        QMessageBox::critical(this, "Cannot Search", "The requested search could not be completed. No documentation data found.");
        return;
    } else if(! _searchIndexed) {
        QMessageBox::critical(this, "Cannot Search", "Search engine is still indexing documentation contents. Cannot search yet.");
        return;
    }
#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
    _helpBrowser->getHelpEngine()->searchEngine()->search(string);
#else
    (void)string;
#endif

    //IF THERE'S ONLY 1 RESULT, GO AHEAD AND OPEN IT!!!
}

bool HelpDockWidget::openPage(QString page, QString section) {
    if(!_helpBrowser) {
        QMessageBox::critical(this, "Cannot Search", "The requested search could not be completed. No documentation data found.");
        return false;
    }

#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
    auto makeIdentifier = [](QString page, QString section) {
        if(!section.isNull()) {
            return page + QString("::") + section;
        } else return page;
    };

    //Try to open the page and section
    auto links = _helpBrowser->getHelpEngine()->linksForIdentifier(makeIdentifier(page, section));
    if(links.size() == 1) {
        _helpBrowser->setSource(links.constBegin().value());
        return true;
    } else {
        openSearch(section.isNull()? page : section); //section takes priority, more specific, fall-back to page if not provided
        return false;
    }

#else
    (void)page;
    (void)section;
#endif
}

void HelpDockWidget::onSearchFinished(int resultCount) {
#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
    if(resultCount) {
        _searchResultWidget->show();
    } else {
        QMessageBox::critical(this, "No Results Found!", "Search query yielded no results.");

    }
#endif
}

void HelpDockWidget::onIndexingStarted(void) {
    _searchIndexed = false;
    Debug::log(Debug::LOG_VERBOSE, "[HelpDockWidget] Indexing content for search engine...");
}

void HelpDockWidget::onIndexingFinished(void) {
    _searchIndexed = true;
    Debug::log(Debug::LOG_VERBOSE, "[HelpDockWidget] Search engine completed content indexing.");
}

void HelpDockWidget::onSearchResultsRequestShowLink(const QUrl &link) {
    _helpBrowser->onAnchorClicked(link);
    auto index = _helpBrowser->getHelpEngine()->contentWidget()->indexOf(link);
    _helpBrowser->getHelpEngine()->contentWidget()->setCurrentIndex(index);
    show();
}

void HelpDockWidget::onSearchButtonClicked(void) {
#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
    if(_searchLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Search failed!", "Search for what? Search field is empty. Enter a string!");
        return;
    }
    openSearch(_searchLineEdit->text());
#endif
}

}

