#ifndef TK_DEBUG_H
#define TK_DEBUG_H

#include <QObject>
#include <QTreeWidget>
#include <elysian_debug_base.hpp>
#include <gyro_defines.h>
#include <QStack>
#include <QPoint>

#define GY_FILE_QT_DEBUG_DIR            "logs"
#define DEBUG_QT_BUFF_SIZE              GYRO_PRINTF_BUFFER_SIZE
#define DEBUG_LOG_POLL_MSEC             500

#define Q_CSTR(a) (a.toStdString().c_str())

class QString;
class QTreeWidgetItem;
class QVBoxLayout;
class QTextEdit;
class QMutex;
class QTimer;
class QMenu;

typedef elysian::DbgEvent DbgEvent;

class DebugLogTreeWidget: public QTreeWidget {
    void keyPressEvent(QKeyEvent *event);
};

class TkDebug: public QObject, public elysian::DebugBase {
    Q_OBJECT
    friend class DebugBase;
public:

    enum TK_SOURCE {
        LIBGYRO     = DebugBase::LIBGYRO,
        ENGINE      = DebugBase::ENGINE,
        LUA         = DebugBase::LUA,
        TOOLKIT     = DebugBase::LUA+1,
        SOURCE_QT,
        COMMAND,
        OPENGL,
        SOURCE_MAX
    };

    //static void ESDebugWrapper(elysian::Asset::DBG_TYPE, const char msg[]);
    static void Log(SEVERITY flag, QString msg, int src = TOOLKIT);
    //Fuck you, Marcel. Fuck QString. C-Style!
    static void Logf(SEVERITY flag, const char *string, ...);
	//Event filter to catch QDebug/QFatal/QCritical messages
    static void logQ(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static void push(void);
    static void pop(const unsigned count=1);

    static TkDebug& getInstance(void) {
        if(!_window) _window = new TkDebug;
		return *_window;
    }

    static QWidget* getCentralWidget(void) {
        return _centralWidget;
    }

	static void deleteInstance(void) { //clean you singleton shit up
		delete _window;
        _window = nullptr;
	}

    static void setFilter(const TK_SOURCE source, const SEVERITY severity, const bool enabled);
    static void clear(void);

    static QString getDebugLogPath(void);
    static QMenu*  getFilterMenu(void);

private:
    struct LogEntry {
        enum TYPE { TEXT, PUSH, POP };
        QString string;
        SEVERITY   level;
        int     source;
        TYPE    type;
    };

public Q_SLOTS:

    void onCommandSubmit(void);

protected Q_SLOTS:
    static void _servicePendingQueue(void);
    static void _postToQueue(const TK_SOURCE src, const SEVERITY level, const QString text);
    static void _processLogEntry(const LogEntry& entry);
    void        _onLogWidgetContextMenuRequested(const QPoint point);

private:

    static void                 _resizePrompt(void);
    virtual bool                eventFilter(QObject* object, QEvent* event) override;
    static void                 writeLog(const int source, const SEVERITY level, const char* const str);

                                TkDebug(void);
    virtual                     ~TkDebug(void) override;

    static TkDebug*             _window;
    static QTreeWidgetItem*     _curParent;
    static QTreeWidgetItem*     _prevItem;
    static QTreeWidget*         _treeWidget;
    static QVBoxLayout*         _vertLayout;
    static QWidget*             _centralWidget;
    static QTextEdit*           _prompt;
    static QMenu*               _filterMenu;
    static bool                 _pendingError;
    static unsigned             _stackDepth;
    static bool                 _filterEnabled[SOURCE_MAX][DebugBase::DBG_OFF];
    static QVector<QString>     _commandHistory;
    static int                  _commandHistoryIndex;
    static bool                 _shiftDown;
    static QString              _lastString;
    static bool                 _prevMsgDiscarded;
    static QStack<bool>         _ignoredPushes;

    static QQueue<LogEntry>*    _pendingQueue;
    static QMutex*              _mutex;

    static QString*             _debugLogPath;


    constexpr static char    _srcEnumToStr[SOURCE_MAX][50] = {
        "LIBGYRO", "ENGINE", "LUA", "TOOLKIT", "QT", "COMMAND", "OPENGL"
    };

};








//========= INLINEZ ==========

inline void TkDebug::writeLog(const int source, const SEVERITY level, const char* const str) {
    _lastString = str;
    Log(level, str, (TK_SOURCE)source);
    //elysian::DebugBase::writeLog((elysian::DebugBase::SOURCE)source, level, str);
}

inline QString TkDebug::getDebugLogPath(void) { return *_debugLogPath; }


inline void TkDebug::setFilter(const TK_SOURCE src, const SEVERITY severity, const bool enabled) {
    _filterEnabled[src][severity] = enabled;
}

inline QMenu* TkDebug::getFilterMenu(void) {
    return _filterMenu;
}



#endif // DEBUG_H
