#ifndef ENGINE_DIALOG_H
#define ENGINE_DIALOG_H

#include <QMainWindow>
#include <QProcess>
#include "assetManager.h"

//Default command-line arg starting values
#define ENGINE_DIALOG_START_X			"200"
#define ENGINE_DIALOG_START_Y			"200"

//Binary paths
#define ENGINE_DIALOG_WIN_PATH			"ESGamma.exe"
#define ENGINE_DIALOG_OSX_PATH			"ESGamma.app"
#define ENGINE_DIALOG_LINUX_PATH		"ESGamma"
#define ENGINE_DIALOG_DC_PATH			"bin/ESGammaDC.elf"

//DC-Tool binary paths
#ifdef Q_OS_WIN
#	define ENGINE_DIALOG_DCTOOL_PATH	"bin/dc-tool-win.exe"
#elif defined(Q_OS_MAC)
#	define ENGINE_DIALOG_DCTOOL_PATH	"bin/dc-tool-osx"
#elif defined(Q_OS_LINUX)
#	define ENGINE_DIALOG_DCTOOL_PATH	"bin/dc-tool-linux"
#endif

class QListWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;

/*
Should invoke engine NOT save the current area and only
invoke with a copy?
*/
class EngineDialog: public QMainWindow, public AssetManager {
    Q_OBJECT
public:
    enum ENGINE_TYPE { ENGINE_PC, ENGINE_DC, ENGINE_PSP, ENGINE_iOS };
    EngineDialog(QWidget *parent=0);

    bool Invoke(const ENGINE_TYPE type);
private:
    ENGINE_TYPE _currentType;
    QProcess *_engine;

    QListWidget *_log;
    QPushButton *_clearButton, *_restartButton, *_closeButton;
    QLabel *_startPosLabel, *_startPosXLabel, *_startPosYLabel;
    QLineEdit *_startPosXEdit, *_startPosYEdit;
    QVBoxLayout *_mainLayout;
    QHBoxLayout *_startPosLayout, *_buttonLayout;
    QHBoxLayout *_resolutionLayout;
    QRadioButton *_resButton1, *_resButton2, *_resButton3;

    //Settings
    QString _sudoPswd;

    //Dreamcast-specific settings
    QString _dcMacAddr;
    QString _dcIpAddr;

    //Resolution
    unsigned _resolution = 0;


private slots:
	void OnReadyReadStandardOutput(void);
	void OnClickClose(void);
	void OnClickClear(void);
	void OnClickRestart(void);
    void OnStart(void);
	void OnError(QProcess::ProcessError error);
    void OnFinish(int result);
    void onResolutionRadioButtonsClicked(bool checked);

public slots:
    void OnDreamcastIpChanged(QString string);
    void OnDreamcastMacChanged(QString string);
    void OnSudoPswdChanged(QString string);


public:

    unsigned getResolution(void) const { return _resolution; }
    void setResolution(const unsigned res);

    void getStartPos(unsigned &x, unsigned &y) const;
    void setStartPos(const unsigned x, const unsigned y);

};


#endif // ENGINE_DIALOG_H
