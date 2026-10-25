/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_DOCUMENTATION_BROWSER_H
#define TK_DOCUMENTATION_BROWSER_H

#include <QTextBrowser>
#include <QMainWindow>

#define ESTK_USER_MANUAL_DB_NAME            "ESToolkit"

#define ESTK_USER_MANUAL_HOME_PAGE          "qthelp://elysianshadows.toolkit.help/doc/index.html"
#define ESTK_USER_MANUAL_QHC_RESOURCE_PATH  ":/manual/docs/ESToolkit.qhc"
#define ESTK_USER_MANUAL_QCH_RESOURCE_PATH  ":/manual/docs/ESToolkit.qch"

#define ESTK_USER_MANUAL_QHC_FILE_NAME      "ESToolkit.qhc"
#define ESTK_USER_MANUAL_QCH_FILE_NAME      "ESToolkit.qch"
#define ESTK_USER_MANUAL_QHC_EXPANDED_PATH  "doc/" ESTK_USER_MANUAL_QHC_FILE_NAME
#define ESTK_USER_MANUAL_QCH_EXPANDED_PATH  "doc/" ESTK_USER_MANUAL_QCH_FILE_NAME


#define ESTK_USER_MANUAL_EXPANDED_DIR       "doc"

#define QESTKUTILS_HELP_DB_EXPANDED_DIR     "doc"

class QHelpEngine;
class QHelpSearchResultWidget;
class QPushButton;
class QLineEdit;

namespace QESTkUtils {

    class HelpBrowser : public QTextBrowser {
        Q_OBJECT
    public:
                                HelpBrowser(QHelpEngine* helpEngine, QWidget* parent=nullptr);
        virtual QVariant        loadResource (int type, const QUrl& name) override;
        QHelpEngine*            getHelpEngine(void) const;
    private:
        QHelpEngine* _helpEngine = nullptr;

    public Q_SLOTS:
        void onAnchorClicked(const QUrl& link);
    };

    class HelpDockWidget: public QMainWindow {
        Q_OBJECT
    private:
        //State Variables
        bool                        _dbLoaded           = false;
        bool                        _searchIndexed      = false;

        //UI Widgets
        HelpBrowser*                _helpBrowser        = nullptr;
        QHelpSearchResultWidget*    _searchResultWidget = nullptr;
        QLineEdit*                  _searchLineEdit     = nullptr;
        QPushButton*                _searchButton       = nullptr;

        //General Help Widget Info
        QString                     _dbName;
        QString                     _homePageUrl;

        //DB File Resources
        QString                     _dbResourcePath;
        QString                     _basePath;



    private Q_SLOTS:
        void            onSearchFinished(int resultCount);
        void            onIndexingStarted(void);
        void            onIndexingFinished(void);
        void            onSearchResultsRequestShowLink(const QUrl& link);
        void            onSearchButtonClicked(void);
    public:

                        HelpDockWidget(QWidget* parent,
                                       QString dbName,
                                       QString dbResourcePath,
                                       QString homepage,
                                       QString basePath=QString());

        void            openSearch(QString string);
        bool            openPage(QString page, QString section=QString());
        bool            isDbLoaded(void) const;
        bool            isSearchIndexed(void) const;



    };




//======== INLINEZ ========

    inline QHelpEngine* HelpBrowser::getHelpEngine(void) const { return _helpEngine; }
    inline bool HelpDockWidget::isDbLoaded(void) const { return _dbLoaded; }
    inline bool HelpDockWidget::isSearchIndexed(void) const { return _searchIndexed; }


}


#endif // TK_DOCUMENTATION_BROWSER_H

