#ifndef DEBUG_H
#define DEBUG_H

#include <QDialog>
#include <fstream>
#include "elysian_asset.hpp"

#define DEBUG_LOG_PATH "toolkit_debug.txt"

class QDialog;
class QString;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QVBoxLayout;
class QGridLayout;
class QCheckBox;

class Debug: public QDialog {
    Q_OBJECT
public:
	enum LEVEL {VERBOSE=1000, CRITICAL, FATAL};
	enum SOURCE { TOOLKIT, ASSETIO, SOURCE_QT };

    //static void ESDebugWrapper(elysian::Asset::DBG_TYPE, const char msg[]);
	static void Log( LEVEL flag, QString msg, SOURCE src = TOOLKIT);
    //Fuck you, Marcel. Fuck QString. C-Style!
	static void Logf( LEVEL flag, const char *string, ...);
	//Event filter to catch QDebug/QFatal/QCritical messages
    static void logQ(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	static void Push(void);
	static void Pop(void);

    static Debug &getInstance(void) {
		if(!_window) _window = new Debug;
		return *_window;
    }

	static void deleteInstance(void) { //clean you singleton shit up
		delete _window;
		_window = NULL;
	}

public slots:
    void OnClickToolkitCheckBox(bool value);
    void OnClickAssetioCheckBox(bool value);
    void OnClickClear(void);

private:
	static Debug* _window;

	class AssetIODebug: public elysian::Asset::DebugStack {
		void push(void) { Debug::Push(); }

		void pop(void) { Debug::Pop(); }

		inline void printf(const elysian::Asset::LOG_LEVEL level, const char* const str) {
			Debug::LEVEL code = Debug::VERBOSE;
			//translate from AssetIO level to TK level
            switch(level) {
            case elysian::Asset::LOG_VERBOSE:
				code = Debug::VERBOSE;
                break;
            case elysian::Asset::LOG_CRITICAL:
				code = Debug::CRITICAL;
                break;
            case elysian::Asset::LOG_FATAL:
				code = Debug::FATAL;
				break;
			}

			Debug::Log(code, str, Debug::ASSETIO);
		}

	} _assetioDebug;

    Debug();
	~Debug();
	static QTreeWidgetItem *_curParent;
	static QTreeWidgetItem *_prevItem;
    static QPushButton *_closeButton, *_clearButton;
	static QTreeWidget *_treeWidget;
    static QVBoxLayout *_vertLayout;
    static QGridLayout *_gridLayout;
    static QCheckBox *_toolkitCheckBox, *_assetioCheckBox;
	static bool _pendingError;
	static unsigned _stackDepth;
    static std::ofstream *_outfile;
};

#endif // DEBUG_H
