#ifndef TK_DOCUMENTATION_BROWSER_H
#define TK_DOCUMENTATION_BROWSER_H

#include <QTextBrowser>
#include <QHelpEngine>
#include <QDockWidget>

#define ESTK_USER_MANUAL_DB_NAME            "ESToolkit"

#define ESTK_USER_MANUAL_HOME_PAGE          "qthelp://elysianshadows.toolkit.help/doc/index.html"
#define ESTK_USER_MANUAL_QHC_RESOURCE_PATH  ":/manual/docs/ESToolkit.qhc"
#define ESTK_USER_MANUAL_QCH_RESOURCE_PATH  ":/manual/docs/ESToolkit.qch"

#define ESTK_USER_MANUAL_QHC_FILE_NAME      "ESToolkit.qhc"
#define ESTK_USER_MANUAL_QCH_FILE_NAME      "ESToolkit.qch"
#define ESTK_USER_MANUAL_QHC_EXPANDED_PATH  "doc/" ESTK_USER_MANUAL_QHC_FILE_NAME
#define ESTK_USER_MANUAL_QCH_EXPANDED_PATH  "doc/" ESTK_USER_MANUAL_QCH_FILE_NAME
#define ESTK_USER_MANUAL_EXPANDED_DIR       "doc"

class HelpBrowser : public QTextBrowser
{
public:
                            HelpBrowser(QHelpEngine* helpEngine, QWidget* parent=nullptr);
    virtual QVariant        loadResource (int type, const QUrl& name) override;
private:
    QHelpEngine* _helpEngine = nullptr;
};

class HelpDockWidget: public QDockWidget {
private:
    //HelpBrowser*    _helpBrowser = nullptr;
public:
    HelpDockWidget(QWidget*parent=nullptr);

};


#endif // TK_DOCUMENTATION_BROWSER_H

