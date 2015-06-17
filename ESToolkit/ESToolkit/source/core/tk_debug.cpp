//QT
#include <QCoreApplication>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <iostream>
#include <QTextEdit>
#include <QKeyEvent>
#include <gyro_system_api.h>
#include <QMutex>
#include <QQueue>
#include <QTimer>
#include <QFontDatabase>
#include <tk_menu_stack.h>
#include <QMenu>
//ESTK
#include "elysian_debug.hpp"
#include "elysian_lua_manager.hpp"
#include <QDir>

#ifdef ESTK_UNIT_TESTS
#include <test_estk.h>
#endif

TkDebug* TkDebug::_window = nullptr;
QTreeWidgetItem *TkDebug::_curParent;
QTreeWidgetItem *TkDebug::_prevItem;
QTreeWidget *TkDebug::_treeWidget;
QVBoxLayout *TkDebug::_vertLayout;
QTextEdit* TkDebug::_prompt;
QWidget* TkDebug::_centralWidget;
bool TkDebug::_pendingError = false;
unsigned TkDebug::_stackDepth = 0;
constexpr char TkDebug::_srcEnumToStr[TkDebug::SOURCE_MAX][50];
bool TkDebug::_filterEnabled[TkDebug::SOURCE_MAX][TkDebug::DBG_OFF] = {{ true }};
QVector<QString> TkDebug::_commandHistory;
int TkDebug::_commandHistoryIndex;
bool TkDebug::_shiftDown = false;
QMutex* TkDebug::_mutex = nullptr;
QQueue<TkDebug::LogEntry>* TkDebug::_pendingQueue = nullptr;
QString* TkDebug::_debugLogPath = nullptr;
QString TkDebug::_lastString;
bool TkDebug::_prevMsgDiscarded = false;
QStack<bool> TkDebug::_ignoredPushes;
QMenu* TkDebug::_filterMenu;

TkDebug::TkDebug(void) {
    _commandHistory = QVector<QString>();
    _centralWidget = new QWidget;
	_vertLayout = new QVBoxLayout;
    _centralWidget->setLayout(_vertLayout);

	_treeWidget = new QTreeWidget;
    _treeWidget->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    _treeWidget->setAutoScroll(false);
	_treeWidget->setColumnCount(2);
	QStringList labels;
	labels.append(QString("String"));
	labels.append(QString("Source"));
	_treeWidget->setHeaderLabels(labels);
	_treeWidget->setSortingEnabled(false);
	_curParent = _treeWidget->invisibleRootItem();
    //_treeWidget->header()->resizeSection(1, 100);
	_prevItem = _curParent;
    _treeWidget->setColumnWidth(0, 1000);
	_vertLayout->addWidget(_treeWidget);

    _prompt = new QTextEdit;
    _prompt->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    _prompt->installEventFilter(this);
    _vertLayout->addWidget(_prompt);
    _resizePrompt();

    _vertLayout->setContentsMargins(0, 0, 0, 0);
    _vertLayout->setSpacing(0);

    _mutex = new QMutex(QMutex::Recursive);
    _pendingQueue = new QQueue<LogEntry>;

/*
    QFont font;
    font.setPointSize(11);
    _treeWidget->setFont(font);
*/

    //connect(_prompt, SIGNAL(returnPressed()), this, SLOT(onCommandSubmit()));

	//Register QDebug callback
    qInstallMessageHandler(&logQ);
	//Register AssetIO debug interface!

#if 0
    if(QDir(GY_FILE_QT_DEBUG_DIR).exists() || QDir().mkdir(GY_FILE_QT_DEBUG_DIR)) {
        QDir debugDir(GY_FILE_QT_DEBUG_DIR);
        if(debugDir.exists() && QDir::setCurrent(debugDir.path())) {
            dbgprintf("Creating log files at %s", debugDir.absolutePath().toStdString().c_str());
        } else {
            errprintf("Could not cd into debug log file directory: %s", debugDir.path().toStdString().c_str());
        }

    } else {
        errprintf("Could not access debug log file directory: %s/%s", Q_CSTR(QDir::currentPath()), GY_FILE_QT_DEBUG_DIR);
    }
#endif
    dbgprintf("Creating log files at %s", Q_CSTR(QDir::currentPath()));
    _debugLogPath = new QString(QDir::currentPath()); //QString(QDir().current().absoluteFilePath(GY_FILE_QT_DEBUG_DIR));


    MenuStack sourcesMenu("Filters");
    auto addFilterSource = [&](const QString name, const Debug::TK_SOURCE source) {
        MenuStack severityMenu("Severity");
        severityMenu.insertCheckableAction("Verbose", [=](const bool checked) {
            Debug::setFilter(source, Debug::DBG_VERBOSE, checked);
        });
        severityMenu.insertCheckableAction("Warning", [=](const bool checked) {
            Debug::setFilter(source, Debug::DBG_WARNING, checked);
        });
        severityMenu.insertCheckableAction("Error", [=](const bool checked) {
            Debug::setFilter(source, Debug::DBG_ERROR, checked);
        });
        sourcesMenu.insertCheckableAction(name, [](const bool /*checked*/) {
            //for(auto it : action->menu()->actions()) {
             //   it->setChecked(checked);
            //}
        })->setMenu(severityMenu.first());
    };
    addFilterSource("LibGyro",  Debug::LIBGYRO);
    addFilterSource("Engine",   Debug::ENGINE);
    addFilterSource("Toolkit",  Debug::TOOLKIT);
    addFilterSource("Qt",       Debug::SOURCE_QT);
    addFilterSource("OpenGL",   Debug::OPENGL);
    addFilterSource("Lua",      Debug::LUA);
    addFilterSource("Commands", Debug::COMMAND);
    _filterMenu = sourcesMenu.first();
    _treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_onLogWidgetContextMenuRequested(QPoint)));

    QTimer::singleShot(DEBUG_LOG_POLL_MSEC, &TkDebug::_servicePendingQueue);

}

TkDebug::~TkDebug(void) {
    delete _mutex;
    delete _pendingQueue;
    delete _debugLogPath;
}

/*
void Debug::ESDebugWrapper(elysian::Asset::DBG_TYPE flag, const char msg[] ) {
    QString str(msg);
    Debug::Log(ASSETIO_DBG_WARNING, str);
}*/

/* Fuck you and your QStrings, Marcel. I'm doing this C-Style. */
void TkDebug::Logf(Debug::SEVERITY flag, const char *string, ...) {
    va_list argptr;
    char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_start(argptr, string);
    vsprintf(buffer, string, argptr);
    va_end(argptr);

    //WHEEEEEEEEEEEEEEEEEEEEEELL!!!!!11
    Log(flag, QString(buffer));
}

void TkDebug::logQ(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    char qtDebugBuff[DEBUG_QT_BUFF_SIZE];
    SEVERITY level;

	switch (type) {
	case QtDebugMsg:
        sprintf(qtDebugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
        level = DBG_VERBOSE;
		break;
	case QtWarningMsg:
        sprintf(qtDebugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
        level = DBG_WARNING;
		break;
	case QtCriticalMsg:
        sprintf(qtDebugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
        level = DBG_WARNING;
		break;
	case QtFatalMsg:
        sprintf(qtDebugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
        level = DBG_ERROR;
	}

    Log(level, qtDebugBuff, SOURCE_QT);
}

void TkDebug::push(void) {
    //++_stackDepth;
    _mutex->lock();
    elysian::DebugBase::push();
    if(_prevMsgDiscarded) {
        _ignoredPushes.push(true);
    } else {
        _pendingQueue->enqueue(LogEntry { "PUSH", DBG_VERBOSE, TOOLKIT, LogEntry::PUSH });
        _ignoredPushes.push(false);
    }

    _mutex->unlock();
#ifdef ESTK_UNIT_TESTS
    tktest::ESTkTest::pushSubTest(_lastString);
#endif
}

void TkDebug::pop(const unsigned count) {
    _mutex->lock();
    elysian::DebugBase::pop(count);
    for(unsigned i = 0; i < count; ++i) {
        if(!_ignoredPushes.pop()) _pendingQueue->enqueue(LogEntry { "POP", DBG_VERBOSE, TOOLKIT, LogEntry::POP });
    }
    _mutex->unlock();
#ifdef ESTK_UNIT_TESTS
    tktest::ESTkTest::popSubTest(true);
#endif
}

void TkDebug::Log(SEVERITY flag, QString msg, int src) {
    _postToQueue((TK_SOURCE)src, flag, msg);
}

void TkDebug::clear(void) {
	_treeWidget->clear();
	//These need to be reset, because the TreeWidget deleted all TreeWidgetItems
	_curParent = _prevItem = _treeWidget->invisibleRootItem();
}

void TkDebug::onCommandSubmit(void) {
    QString str = _prompt->toPlainText();

    if(_shiftDown) {
        _resizePrompt();
        //str.append("\n");
        //_prompt->setText(str);
        return;
    }

    //str.chop(1);

    //Add to history
    if(_commandHistoryIndex >= _commandHistory.size()) {
        _commandHistory.push_back(str);
    } else {
        _commandHistory[_commandHistoryIndex] = str;
    }
    ++_commandHistoryIndex;

    //Add to console log

    QString dispStr = str;
    dispStr = dispStr.replace("\n", "\n> ");
    Debug::Log(DBG_VERBOSE, QString("> ")+dispStr, COMMAND);
    const unsigned depth = Debug::getStackDepth();
    auto* parent = _prevItem;
    Debug::push();
    elysian::LuaManager::runString(str.toStdString().c_str());

    //Update widget
    parent->setExpanded(true);
    _prompt->clear();
    _resizePrompt();
   // _treeWidget->scrollToBottom();
    Debug::pop(Debug::getStackDepth()-depth);
}


bool TkDebug::eventFilter(QObject* /*object*/, QEvent* event) {
    QKeyEvent*      keyEvent = static_cast<QKeyEvent*>(event);
    bool retVal = false;

    switch(event->type()) {
    case QEvent::Resize:
        _resizePrompt();
        break;
    case QEvent::KeyPress:
        switch(keyEvent->key()) {
        case Qt::Key_Up:
            if(_commandHistoryIndex != 0 && _shiftDown) {
                --_commandHistoryIndex;
                _prompt->setText(_commandHistory[_commandHistoryIndex]);
                _resizePrompt();
                retVal = true;
            }
            break;
        case Qt::Key_Down:
            if(_commandHistoryIndex != _commandHistory.size() && _shiftDown) {
                ++_commandHistoryIndex;
                if(_commandHistoryIndex == _commandHistory.size()) _prompt->clear();
                else _prompt->setText(_commandHistory[_commandHistoryIndex]);
                _resizePrompt();
                retVal = true;
            }
            break;
        case Qt::Key_Return:
            onCommandSubmit();
            if(_shiftDown) _prompt->insertPlainText("\n");
            retVal = true;
            _resizePrompt();
            break;
        case Qt::Key_Shift:
            _shiftDown = true;
            break;
        default: break;

        }
        break;

    case QEvent::KeyRelease:
        switch(keyEvent->key()) {
        case Qt::Key_Shift:
            _shiftDown = false;
            break;
        default: break;
        }
        break;
    default:
        break;

    }

    return retVal;

}

void TkDebug::_resizePrompt(void) {
    unsigned nRows = _prompt->toPlainText().split("\n").size();
    QFontMetrics m(_prompt->font());
    int RowHeight = m.lineSpacing();
    _prompt->setFixedHeight(nRows*RowHeight+4);
    _treeWidget->scrollToBottom();
}

void TkDebug::_postToQueue(const TK_SOURCE src, const SEVERITY level, const QString text) {
    _mutex->lock();
    elysian::DebugBase::writeLog((elysian::DebugBase::SOURCE)src, level, Q_CSTR(text));
    if(_filterEnabled[src][level]) {
        _pendingQueue->enqueue(LogEntry{ text, level, src, LogEntry::TEXT });
        _prevMsgDiscarded = false;
    } else {
        _prevMsgDiscarded = true;
    }
    _mutex->unlock();

#ifdef ESTK_UNIT_TESTS
    if(level == Debug::DBG_WARNING) tktest::ESTkTest::flagWarning(text);
    else if(level == Debug::DBG_ERROR) tktest::ESTkTest::flagError(text);
#endif
}

//could probably move critical section into the loop, but I kind of like doing it in batches better
void TkDebug::_servicePendingQueue(void) {
    _mutex->lock();

    const bool wasEmpty = _pendingQueue->isEmpty();

    while(!_pendingQueue->isEmpty()) {
        const LogEntry&& entry = _pendingQueue->dequeue();

        switch(entry.type) {
        case LogEntry::TEXT:
            _processLogEntry(entry);
            break;
        case LogEntry::PUSH:
            Q_ASSERT(_curParent);
            _curParent = _prevItem;
            break;
        case LogEntry::POP:
            _curParent = _curParent->parent();
            if(!_curParent) _curParent = _treeWidget->invisibleRootItem();
            break;
        }
    }

    _mutex->unlock();

    //Update GUI and widgets and shit
    if(!wasEmpty) {
        _treeWidget->scrollToBottom();
    }

    QTimer::singleShot(DEBUG_LOG_POLL_MSEC, &TkDebug::_servicePendingQueue);

}


void TkDebug::_processLogEntry(const LogEntry& entry) {
    int src             = entry.source;
    const QString msg   = entry.string;
    SEVERITY flag       = entry.level;

    QBrush brush;
    _prevItem = new QTreeWidgetItem;
    _prevItem->setText(0, msg);

    switch(src) {
    case LUA:
        brush = QBrush(Qt::darkCyan);
        break;
    case COMMAND:
        brush = QBrush(Qt::darkGreen);
        break;
    default:
        brush = QBrush(Qt::black);
        break;
    }

    _prevItem->setForeground(0, brush);
    _prevItem->setForeground(1, brush);
    _prevItem->setText(1, _srcEnumToStr[src]);

    Q_ASSERT(_curParent); //better never happen!

    _curParent->addChild(_prevItem);

    //if(flag == DBG_VERBOSE && !_filterSourceEnabled[src]) _prevItem->setHidden(true);

    //ths is why shit is so slow. Retarded time complexity
    if(flag >= Debug::DBG_WARNING) {
        _pendingError = true;

        QTreeWidgetItem *it = _prevItem;
        brush = QBrush(flag == Debug::DBG_ERROR? Qt::darkRed : Qt::darkYellow);
        it->setForeground(0, brush);
        it->setForeground(1, brush);
        do {
            it->setExpanded(true);
            it = (flag == Debug::DBG_ERROR)? it->parent() : nullptr;
        } while(it);

        if(flag == Debug::DBG_ERROR) _prevItem->setSelected(true);
        //_treeWidget->scrollToItem(_prevItem);

        //If we've returned to the top level with a pending error,
        //pop the log for the user to see.
        //if(_curParent == _treeWidget->invisibleRootItem() && _pendingError) {
        //        getInstance().show();
        //}
    } /*else if(src == LUA) {
        QTreeWidgetItem *it = _prevItem;
        do {
            it->setExpanded(true);
            it = it->parent();
        } while(it);
    }*/

}

void TkDebug::_onLogWidgetContextMenuRequested(const QPoint point) {
    _filterMenu->exec(_treeWidget->mapToGlobal(point));
}

