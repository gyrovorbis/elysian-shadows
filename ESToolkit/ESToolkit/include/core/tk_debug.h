#ifndef TK_DEBUG_H
#define TK_DEBUG_H

#include <elysian_debug_base.hpp>
#include <QObject>
#include <fstream>

#define DEBUG_LOG_PATH                  "toolkit_debug.txt"
#define DEBUG_QT_BUFF_SIZE              50000
#define DEBUG_LOG_POLL_MSEC             500

#define Q_CSTR(a) (a.toStdString().c_str())

class QString;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;
class QTextEdit;
class QMutex;
class QTimer;

class TkDebug: public QObject, public elysian::DebugBase {
    Q_OBJECT
    friend class DebugBase;
public:

    enum TK_SOURCE {
        TOOLKIT = LUA+1,
        SOURCE_QT,
        COMMAND,
        OPENGL,
        SOURCE_MAX
    };

    //static void ESDebugWrapper(elysian::Asset::DBG_TYPE, const char msg[]);
    static void Log( LEVEL flag, QString msg, int src = TOOLKIT);
    //Fuck you, Marcel. Fuck QString. C-Style!
	static void Logf( LEVEL flag, const char *string, ...);
	//Event filter to catch QDebug/QFatal/QCritical messages
    static void logQ(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static void push(void);
    static void pop(void);

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

    static void setFilter(const int src, const bool enabled);
    static void clear(void);

    static QString getDebugLogPath(void);

private:
    struct LogEntry {
        enum TYPE { TEXT, PUSH, POP };
        QString string;
        LEVEL   level;
        int     source;
        TYPE    type;
    };

public Q_SLOTS:

    void onCommandSubmit(void);

protected Q_SLOTS:
    static void _servicePendingQueue(void);
    static void _postToQueue(const int src, const LEVEL level, const QString text);
    static void _processLogEntry(const LogEntry& entry);

private:

    static void                 _resizePrompt(void);
    virtual bool                eventFilter(QObject* object, QEvent* event) override;
    static void                 writeLog(const int source, const LEVEL level, const char* const str);

                                TkDebug(void);
    virtual                     ~TkDebug(void) override;

    static TkDebug*             _window;
    static QTreeWidgetItem*     _curParent;
    static QTreeWidgetItem*     _prevItem;
    static QTreeWidget*         _treeWidget;
    static QVBoxLayout*         _vertLayout;
    static QWidget*             _centralWidget;
    static QTextEdit*           _prompt;
    static bool                 _pendingError;
    static unsigned             _stackDepth;
    static std::ofstream*       _outfile;
    static bool                 _filterEnabled[SOURCE_MAX];
    static QVector<QString>     _commandHistory;
    static int                  _commandHistoryIndex;
    static bool                 _shiftDown;

    static QQueue<LogEntry>*    _pendingQueue;
    static QMutex*              _mutex;

    static QString*             _debugLogPath;

    constexpr static char    _srcEnumToStr[SOURCE_MAX][50] = {
        "LIBGYRO", "ENGINE", "ASSETIO", "LUA", "TOOLKIT", "QT", "COMMAND", "OPENGL"
    };

};








//========= INLINEZ ==========

inline void TkDebug::writeLog(const int source, const LEVEL level, const char* const str) {
    Log(level, str, (source == DebugBase::ASSETIO)? ENGINE : (TK_SOURCE)source);
    elysian::DebugBase::writeLog((elysian::DebugBase::SOURCE)source, level, str);
}

inline QString TkDebug::getDebugLogPath(void) { return *_debugLogPath; }


#endif // DEBUG_H
