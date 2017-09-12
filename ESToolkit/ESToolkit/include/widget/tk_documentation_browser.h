#ifndef TK_DOCUMENTATION_BROWSER_H
#define TK_DOCUMENTATION_BROWSER_H

#include <QTextBrowser>
#include <QHelpEngine>
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

#define TOOLKIT_DOCUMENTATION_RESOURCES

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
                            HelpBrowser(QHelpEngine* helpEngine, QWidget* parent=nullptr);
    virtual QVariant        loadResource (int type, const QUrl& name) override;
    QHelpEngine*            getHelpEngine(void) const;
private:
    QHelpEngine* _helpEngine = nullptr;

private Q_SLOTS:
    void onAnchorClicked(const QUrl& link);
};

class HelpDockWidget: public QMainWindow {
private:
    HelpBrowser*    _helpBrowser = nullptr;
public:
                    HelpDockWidget(QWidget*parent=nullptr);

    void            openSearch(QString string);
    bool            openPage(QString page, QString section=QString());

};




//======== INLINEZ ========
inline QHelpEngine* HelpBrowser::getHelpEngine(void) const { return _helpEngine; }


#endif // TK_DOCUMENTATION_BROWSER_H

