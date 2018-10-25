#ifndef TK_DEBUG_H
#define TK_DEBUG_H

#include <QObject>
#include <QTreeWidget>
#include <ESGamma/utility/elysian_debug_base.hpp>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <libGyro/gyro_defines.h>
#include <QStack>
#include <QPoint>

#define DEBUG_FILTER_LIST_ENABLED
#define GY_FILE_QT_DEBUG_DIR            "logs"
#define DEBUG_QT_BUFF_SIZE              GYRO_PRINTF_BUFFER_SIZE
#define DEBUG_LOG_POLL_MSEC             500

class QString;
class QTreeWidgetItem;
class QVBoxLayout;
class QTextEdit;
class QMutex;
class QTimer;
class QMenu;

typedef elysian::DbgEvent DbgEvent;

class DebugLogTreeWidget: public QTreeWidget {
public:
    explicit        DebugLogTreeWidget(QWidget*parent = nullptr);
protected:
  //  virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual bool    eventFilter(QObject *object, QEvent* event) override;
};

class TkDebug:
        public QObject,
        public elysian::DebugBase
{
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
        EVMU,
        QESTKUTILS,
        SOURCE_MAX
    };

    struct QESTkUtilAdapter: public QESTkUtils::Debug::LogAdapter {
        virtual void log(QESTkUtils::Debug::LOG_SEVERITY severity, const char* fmt, ...) override {
            char buffer[GYRO_PRINTF_BUFFER_SIZE];
            va_list argptr;
            va_start(argptr, fmt);
            vsnprintf(buffer, sizeof(buffer), fmt, argptr);
            va_end(argptr);
            Logf((SEVERITY)severity, "%s", buffer);
        }
        virtual void    push(void) override {
            TkDebug::push();
        }
        virtual void    pop(unsigned depth=1) override {
            TkDebug::pop(depth);
        }

    } _qestkUtilsLogAdapter;

    static void Log(SEVERITY flag, QString msg, int src = TOOLKIT);
    static void Logf(SEVERITY flag, const char *string, ...);
    static void logQ(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static void push(void);
    static void pop(const unsigned count=1);
    static void resetStackDepth(void);

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

    static void setEVMUActive(bool active=true);
    static void setFilter(const TK_SOURCE source, const SEVERITY severity, const bool enabled);
    static bool isFilterEnabled(const TK_SOURCE source, const SEVERITY severity);
    static void clear(void);

    static QString getDebugLogPath(void);
    static QMenu*  getFilterMenu(void);

    void copyEvent(void);

private:
    struct LogEntry {
        enum TYPE { TEXT, PUSH, POP };
        QString     string;
        SEVERITY    level;
        int         source;
        TYPE        type;
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
    static bool                 _filterEnabled[SOURCE_MAX][DebugBase::LOG_OFF];
    static QAction*             _filterActions[SOURCE_MAX][DebugBase::LOG_OFF];
    static QVector<QString>     _commandHistory;
    static int                  _commandHistoryIndex;
    static bool                 _shiftDown;
    static QString              _lastString;
    static bool                 _prevMsgDiscarded;
    static QStack<bool>         _ignoredPushes;
    static bool                 _evmuActive;

    static QQueue<LogEntry>*    _pendingQueue;
    static QMutex*              _mutex;

    static QString*             _debugLogPath;


    constexpr static char    _srcEnumToStr[SOURCE_MAX][50] = {
        "LIBGYRO", "ENGINE", "LUA", "TOOLKIT", "QT", "COMMAND", "OPENGL", "EVMU"
    };

    constexpr static char _filterList[2][500] = {
        "libpng warning: iCCP: known incorrect sRGB profile ((null):0, (null))",
        "libpng warning: iCCP: known incorrect sRGB profile"
    };

};






//========= INLINEZ ==========

inline void TkDebug::writeLog(const int source, const SEVERITY level, const char* const str) {
    _lastString = str;
    Log(level, str, (TK_SOURCE)source);
    //elysian::DebugBase::writeLog((elysian::DebugBase::SOURCE)source, level, str);
}

inline QString TkDebug::getDebugLogPath(void) { return *_debugLogPath; }


inline QMenu* TkDebug::getFilterMenu(void) {
    return _filterMenu;
}

inline bool TkDebug::isFilterEnabled(const TK_SOURCE source, const SEVERITY severity) {
    Q_ASSERT(source < SOURCE_MAX && severity < LOG_OFF);
    return _filterEnabled[source][severity];
}

inline void TkDebug::setEVMUActive(bool active) { _evmuActive = active; }



#endif // DEBUG_H
