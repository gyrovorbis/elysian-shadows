#ifndef TK_DEBUG_H
#define TK_DEBUG_H

#include <elysian_debug_base.hpp>
#include <QObject>
#include <fstream>

#define DEBUG_LOG_PATH                  "toolkit_debug.txt"
#define DEBUG_QT_BUFF_SIZE              50000

class QString;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;
class QTextEdit;

class TkDebug: public QObject, public elysian::DebugBase {
    Q_OBJECT
    friend class DebugBase;
public:

    enum TK_SOURCE {
        TOOLKIT = LUA+1,
        SOURCE_QT,
        COMMAND,
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

public Q_SLOTS:

    void onCommandSubmit();

private:
    static void _resizePrompt(void);
    virtual bool eventFilter(QObject* object, QEvent* event) override;
    static void writeLog(const int source, const LEVEL level, const char* const str);

                             TkDebug(void);
    virtual                  ~TkDebug(void);
    static char              _qtDebugBuff[DEBUG_QT_BUFF_SIZE];
    static TkDebug*          _window;
    static QTreeWidgetItem*  _curParent;
    static QTreeWidgetItem*  _prevItem;
    static QTreeWidget*      _treeWidget;
    static QVBoxLayout*      _vertLayout;
    static QWidget*          _centralWidget;
    static QTextEdit*        _prompt;
    static bool              _pendingError;
    static unsigned          _stackDepth;
    static std::ofstream*    _outfile;
    static bool              _filterEnabled[SOURCE_MAX];
    static QVector<QString>  _commandHistory;
    static int               _commandHistoryIndex;
    static bool              _shiftDown;

    constexpr static char    _srcEnumToStr[SOURCE_MAX][50] = {
        "LIBGYRO", "ENGINE", "ASSETIO", "LUA", "TOOLKIT", "QT", "COMMAND"
    };

};


//========= INLINEZ ==========

inline void TkDebug::writeLog(const int source, const LEVEL level, const char* const str) {
    Log(level, str, (source == DebugBase::ASSETIO)? ENGINE : (TK_SOURCE)source);
    elysian::DebugBase::writeLog((elysian::DebugBase::SOURCE)source, level, str);
}


#endif // DEBUG_H
