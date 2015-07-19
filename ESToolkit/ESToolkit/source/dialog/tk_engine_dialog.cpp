#include <QProcess>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QRadioButton>
#include <fstream>

#include "tk_engine_dialog.h"
#include "tk_assets.h"
#include <tk_project.h>
#include <asset/tk_level.h>
#include <asset/tk_area.h>
#include <elysian_debug.hpp>
#include <core/tk_file_utilities.h>

EngineDialog::EngineDialog(QWidget *parent): QMainWindow(parent), _currentType(ENGINE_PC) {
	QWidget *centralWidget = new QWidget;
	_mainLayout = new QVBoxLayout;
	centralWidget->setLayout(_mainLayout);
	setCentralWidget(centralWidget);

	_log = new QListWidget;
    _mainLayout->addWidget(_log);

	_startPosLabel = new QLabel("Start Pos:");
	_startPosXLabel = new QLabel("x");
	_startPosYLabel = new QLabel("y");
	_startPosXEdit = new QLineEdit;
	_startPosYEdit = new QLineEdit;
	_startPosXEdit->setText(ENGINE_DIALOG_START_X);
	_startPosYEdit->setText(ENGINE_DIALOG_START_Y);

	_clearButton = new QPushButton;
    _clearButton->setText("Clear");
	_restartButton = new QPushButton;
    _restartButton->setText("Restart");
	_closeButton = new QPushButton;
    _closeButton->setText("Close");

	_startPosLayout = new QHBoxLayout;
    _startPosLayout->addWidget(_startPosLabel);
    _startPosLayout->addWidget(_startPosXLabel);
    _startPosLayout->addWidget(_startPosXEdit);
    _startPosLayout->addWidget(_startPosYLabel);
    _startPosLayout->addWidget(_startPosYEdit);
    _mainLayout->addLayout(_startPosLayout);

    _resolutionLayout = new QHBoxLayout;
    _resButton1 = new QRadioButton;
    _resButton1->setText("1024x720");
    connect(_resButton1, SIGNAL(clicked(bool)), this, SLOT(onResolutionRadioButtonsClicked(bool)));
    _resolutionLayout->addWidget(_resButton1);

    _resButton2 = new QRadioButton;
    _resButton2->setText("640x480");
    connect(_resButton2, SIGNAL(clicked(bool)), this, SLOT(onResolutionRadioButtonsClicked(bool)));
    _resolutionLayout->addWidget(_resButton2);

    _resButton3 = new QRadioButton;
    _resButton3->setText("320x240");
    _resButton3->setChecked(true);
    connect(_resButton3, SIGNAL(clicked(bool)), this, SLOT(onResolutionRadioButtonsClicked(bool)));
    _resolutionLayout->addWidget(_resButton3);
    _mainLayout->addLayout(_resolutionLayout);

	_buttonLayout = new QHBoxLayout;
    _buttonLayout->addWidget(_clearButton);
    _buttonLayout->addWidget(_restartButton);
    _buttonLayout->addWidget(_closeButton);
    _mainLayout->addLayout(_buttonLayout);

    setWindowTitle("Invoke Engine");

    _engine = new QProcess(this);

	//connect(this, SIGNAL(destroyed()), this, SLOT(OnFinish()));
    connect(_closeButton, SIGNAL(clicked()), this, SLOT(OnClickClose()));
    connect(_restartButton, SIGNAL(clicked()), this, SLOT(OnClickRestart()));
    connect(_clearButton, SIGNAL(clicked()), this, SLOT(OnClickClear()));
    connect(_engine, SIGNAL(started()), this, SLOT(OnStart()));
    connect(_engine, SIGNAL(readyReadStandardOutput()), this, SLOT(OnReadyReadStandardOutput()));
    connect(_engine, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStandardError()));
    connect(_engine, SIGNAL(finished(int)), this, SLOT(OnFinish(int)));
    connect(_engine, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChange(QProcess::ProcessState)));
	connect(_engine, SIGNAL(error(QProcess::ProcessError)), this, SLOT(OnError(QProcess::ProcessError)));

    //Load from file through settings dialog
    //_enginePathx86 = ENGINE_DIALOG_DEFAULT_PATH;

}

EngineDialog::~EngineDialog(void) {
    kill();
}

void EngineDialog::onStateChange(QProcess::ProcessState state) {
    switch(state) {
    case QProcess::NotRunning:
        qDebug() << "NOT RUNNING STATE!!!!";
        break;
    case QProcess::Starting:
        qDebug() << "STARTING STATE!!!!";
        break;
    case QProcess::Running:
        qDebug() << "RUNNING STATE1!!!!";
        break;

    }
}

void EngineDialog::closeEvent(QCloseEvent *event) {
    kill();
    Q_EMIT stopped();

}

bool EngineDialog::Invoke(const ENGINE_TYPE type) {
    _currentType = type;
    if(!getAssets()->getCurrentProject() || !getAssets()->getCurrentLevel() || !getAssets()->getCurrentArea()) return true;

	std::ofstream argFile;
    QStringList args;

    //Save current project here!

    _engine->setWorkingDirectory(getAssets()->getCurrentProject()->getLocation());
	//relative level and area paths are hacked to FUCK!
    args << "-l" << FileUtilities::getAssetRelativePath(getAssets()->getCurrentLevel()->getNodePath());
    args << "-a" << FileUtilities::getAssetRelativePath(getAssets()->getCurrentArea()->getNodePath());
	args << "-x" << _startPosXEdit->text() << "-y" << _startPosYEdit->text();
    args << "-r" << QString::number(_resolution);

	//Write argument text file
    argFile.open(ELYSIAN_DEBUG_ARG_FILE_NAME);
	for(int i = 0; i < args.size(); ++i) argFile << args.at(i).toStdString() << " ";
	argFile.close();

    switch(type) {
    case ENGINE_PC:
#ifdef Q_OS_WIN
        _engine->start(_enginePathx86);
#elif defined(Q_OS_LINUX)
        _engine->start(_enginePathx86);
#elif defined(Q_OS_MAC)
        _engine->start(_enginePathx86);
#endif
        break;
    case ENGINE_DC:
        {
        //Check if we have all of the required Dreamcast settings
        if(_dcIpAddr.isNull() || _dcMacAddr.isNull()) return false;
#ifdef Q_OS_WIN
        _engine->start(QString(ENGINE_DIALOG_DCTOOL_PATH)+QString(" -t ")+_dcIpAddr+QString(" -m ")+_dcMacAddr+QString(" -x ")+QString(ENGINE_DIALOG_DC_PATH)+QString(" -c ")+getCurrentProject()->getLocation());
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
        if(_sudoPswd.isNull()) return false; //required for OSX/Linux

        static QProcess *echo = NULL;
        static QProcess *arp = NULL;

		delete arp;
        delete echo;
        arp = new QProcess(this);
        echo = new QProcess(this);
        echo->setStandardOutputProcess(arp);
        echo->start(QString("echo ")+_sudoPswd);
		arp->start(QString("sudo -S arp -s ")+_dcIpAddr+QString(" ")+_dcMacAddr);
        arp->waitForFinished();
        delete echo;
        echo = new QProcess(this);
        echo->setStandardOutputProcess(_engine);
        echo->start(QString("echo ")+_sudoPswd);
        _engine->start(QString("sudo -S ") + QString(ENGINE_DIALOG_DCTOOL_PATH)+QString(" -t ")+_dcIpAddr+QString(" -x ")+QString(ENGINE_DIALOG_DC_PATH)+QString(" -c ")+getCurrentProject()->getLocation());
#endif
        break;
    }
    case ENGINE_iOS:
        _engine->setWorkingDirectory(getAssets()->getCurrentProject()->getNodePath() + QString("/bin/"));
        _engine->start("/bin/sh", QStringList { getAssets()->getCurrentProject()->getNodePath() + "/bin/launchESTkiOSDevice.sh"});
        break;
    case ENGINE_iOS_Sim:
        _engine->start("/bin/sh", QStringList {getAssets()->getCurrentProject()->getNodePath() + "/bin/launchESGammaiOS.sh" });
    default:
        break;
    }

    _engine->setProcessChannelMode(QProcess::MergedChannels);
    _engine->waitForStarted();
    _engine->closeWriteChannel();

    return true;
}

void EngineDialog::OnStart(void) {
    QListWidgetItem *item = new QListWidgetItem;
    QBrush b(Qt::blue);
    item->setForeground(b);
    item->setText("ENGINE STARTED!");
    _log->addItem(item);
    _log->scrollToBottom();
}

void EngineDialog::OnError(QProcess::ProcessError error) {
	QListWidgetItem *item = new QListWidgetItem;
	QBrush b(Qt::red);
	item->setForeground(b);

	switch(error) {
	case QProcess::FailedToStart:
        item->setText(QString("ENGINE DBG_ERROR: Failed to launch!"));
		break;
	case QProcess::Crashed:
        item->setText(QString("ENGINE DBG_ERROR: Engine has crashed!"));
		break;
	case QProcess::Timedout:
        item->setText(QString("ENGINE DBG_ERROR: Engine has timed out!"));
		break;
	case QProcess::WriteError:
        item->setText(QString("ENGINE DBG_ERROR: Engine write error!"));
		break;
	case QProcess::ReadError:
        item->setText(QString("ENGINE DBG_ERROR: Engine read error!"));
		break;
	case QProcess::UnknownError:
        item->setText(QString("ENGINE DBG_ERROR: Unknown Error!"));
		break;
    default:
        qCritical() << "WHAT IN FUCK's CUNT";
	}

	_log->addItem(item);
	_log->scrollToBottom();

    Q_EMIT stopped();
}

void EngineDialog::onReadyReadStandardError(void) {
    //Qt-based Method
    QString string;
    QTextStream stream(&string);

    stream << _engine->readAllStandardError();
    stream.flush();
    qWarning() << string;
    QStringList stringList = string.split("\n");

    for(int i = 0; i < stringList.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(stringList[i]);
        item->setForeground(QBrush(Qt::darkRed));
        item->setSelected(true);
        _log->addItem(item);
        //qWarning() << stringList[i];
     }
    _log->scrollToBottom();

}

void EngineDialog::OnReadyReadStandardOutput(void) {
#if 0
	//C-style black magic, bitches!
	//Cute, but it was fucked on Windows for some reason. :(

    char *pch;
    QString string = _engine->readAllStandardOutput();
    qDebug() << string;
	pch = strtok((char *)string.toStdString().c_str(), token);
    while (pch != NULL) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString(pch));
        item->setSelected(true);
        _log->addItem(item);

		pch = strtok (NULL, token);
     }
    _log->scrollToBottom();
#endif
	//Qt-based Method
    QString string;
    QTextStream stream(&string);

    stream << _engine->readAllStandardOutput();
    stream.flush();

    qDebug() << string;

	//qDebug() << string; This'll wrap back to Tk now... Fuck that.
	QStringList stringList = string.split("\n");

	for(int i = 0; i < stringList.size(); ++i) {
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(stringList[i]);
		item->setSelected(true);
		_log->addItem(item);
	 }
	_log->scrollToBottom();
}



void EngineDialog::OnClickClear(void) {
    _log->clear();
}

void EngineDialog::OnFinish(int result) {

    qDebug() << result;
    //OnClickClose();
    //Q_EMIT stopped();
}

void EngineDialog::OnClickClose(void) {
    kill();
    _log->clear();
    close();
    Q_EMIT stopped();
}

void EngineDialog::OnClickRestart(void) {
    _engine->close();
    kill();
    Invoke(_currentType);
}

void EngineDialog::OnDreamcastIpChanged(QString string) {
    _dcIpAddr = string;
}

void EngineDialog::OnDreamcastMacChanged(QString string) {
    _dcMacAddr = string;
}

void EngineDialog::OnSudoPswdChanged(QString string) {
    _sudoPswd = string;
}


void EngineDialog::onResolutionRadioButtonsClicked(bool /*checked*/) {
    if(_resButton1->isChecked()) _resolution = 2;
    if(_resButton2->isChecked()) _resolution = 1;
    if(_resButton3->isChecked()) _resolution = 0;
}


void EngineDialog::setResolution(const unsigned res) {
    _resolution = res;
    switch(res) {
    case 0:
        _resButton3->setChecked(true);
        break;
    case 1:
        _resButton2->setChecked(true);
        break;
    case 2:
        _resButton1->setChecked(true);
        break;
    }
}


void EngineDialog::getStartPos(unsigned &x, unsigned &y) const {
    x = _startPosXEdit->text().toUInt();
    y = _startPosYEdit->text().toUInt();
}

void EngineDialog::setStartPos(const unsigned x, const unsigned y) {
    _startPosXEdit->setText(QString::number(x));
    _startPosYEdit->setText(QString::number(y));
}

void EngineDialog::onEnginePathx86Changed(QString path) {
    _enginePathx86 = path;
}

/*
 * End external/background tasks and processes used to launch and run builds.
 */
void EngineDialog::kill(void) {
    if(_currentType == ENGINE_iOS_Sim) {
        QProcess::startDetached("killall", QStringList { "iOS Simulator" });
    } else if(_currentType == ENGINE_iOS) {
        QProcess::startDetached("killall", QStringList { "deviceconsole"});
        QProcess::startDetached("killall", QStringList { "instruments"});
        QProcess::startDetached("killall", QStringList { "ios-deploy"});
    }
    _engine->kill();
}
