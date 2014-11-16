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
#include <gyro_system_api.h>
//ESTK
#include "debug.h"

Debug* Debug::_window = NULL;
QTreeWidgetItem *Debug::_curParent = NULL;
QTreeWidgetItem *Debug::_prevItem = NULL;
QPushButton *Debug::_closeButton = NULL;
QPushButton *Debug::_clearButton = NULL;
QTreeWidget *Debug::_treeWidget = NULL;
QVBoxLayout *Debug::_vertLayout = NULL;
QGridLayout *Debug::_gridLayout = NULL;
QCheckBox *Debug::_toolkitCheckBox = NULL;
QCheckBox *Debug::_assetioCheckBox = NULL;
bool Debug::_pendingError = false;
unsigned Debug::_stackDepth = 0;
std::ofstream *Debug::_outfile = NULL;

Debug::Debug(void) {
	_vertLayout = new QVBoxLayout;
    this->setLayout(_vertLayout);

	_treeWidget = new QTreeWidget;
	_treeWidget->setColumnCount(2);
	QStringList labels;
	labels.append(QString("String"));
	labels.append(QString("Source"));
	_treeWidget->setHeaderLabels(labels);
	_treeWidget->setSortingEnabled(false);
	_curParent = _treeWidget->invisibleRootItem();
	_prevItem = _curParent;

	_vertLayout->addWidget(_treeWidget);

	_gridLayout = new QGridLayout;
    _vertLayout->addLayout(_gridLayout);
	_toolkitCheckBox = new QCheckBox("Toolkit");
    _toolkitCheckBox->setChecked(true);
	_assetioCheckBox = new QCheckBox("AssetIO");
    _assetioCheckBox->setChecked(true);
	_clearButton = new QPushButton("Clear");
	_closeButton = new QPushButton("Close");
    _closeButton->setDefault(true);
    _gridLayout->addWidget(_clearButton, 0, 1);
    _gridLayout->addWidget(_closeButton, 1, 1);
    _gridLayout->addWidget(_toolkitCheckBox, 0, 0);
    _gridLayout->addWidget(_assetioCheckBox, 1, 0);

    _treeWidget->setColumnWidth(0,500);
    setWindowTitle("Toolkit Debug Log");

    setMinimumSize(600,400);

	connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(_clearButton, SIGNAL(clicked()), this, SLOT(OnClickClear()));
    connect(_toolkitCheckBox, SIGNAL(clicked(bool)), this, SLOT(OnClickToolkitCheckBox(bool)));
    connect(_assetioCheckBox, SIGNAL(clicked(bool)), this, SLOT(OnClickAssetioCheckBox(bool)));

	//Register QDebug callback
    //qInstallMessageHandler(&logQ);
	//Register AssetIO debug interface!
	elysian::Asset::setDebugStack(&_assetioDebug);

	_outfile = new std::ofstream(DEBUG_LOG_PATH);
	if(!_outfile->is_open()) qDebug() << "!!!!!- could not open Debug log file for writing!";
}

Debug::~Debug(void) {
    _outfile->close();
    delete _outfile;
}

/*
void Debug::ESDebugWrapper(elysian::Asset::DBG_TYPE flag, const char msg[] ) {
    QString str(msg);
    Debug::Log(ASSETIO_CRITICAL, str);
}*/

/* Fuck you and your QStrings, Marcel. I'm doing this C-Style. */
void Debug::Logf(Debug::LEVEL flag, const char *string, ...) {
    va_list argptr;
    char buffer[ELYSIAN_DEBUG_BUFFER_SIZE];
    va_start(argptr, string);
    vsprintf(buffer, string, argptr);
    va_end(argptr);

    //WHEEEEEEEEEEEEEEEEEEEEEELL!!!!!11
    Log(flag, QString(buffer));
}

void Debug::logQ(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	char debugBuff[500];
	LEVEL level;

	switch (type) {
	case QtDebugMsg:
		sprintf(debugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
		level = VERBOSE;
		break;
	case QtWarningMsg:
		sprintf(debugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
		level = VERBOSE;
		break;
	case QtCriticalMsg:
		sprintf(debugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
		level = CRITICAL;
		break;
	case QtFatalMsg:
		sprintf(debugBuff, "%s (%s:%u, %s)", msg.toStdString().c_str(), context.file, context.line, context.function);
		level = FATAL;
	}

    std::cerr << msg.toStdString() << std::endl;
	Log(level, debugBuff, SOURCE_QT);
}

void Debug::Push(void) {
	++_stackDepth;
    Q_ASSERT(_curParent);
	_curParent = _prevItem;
}

void Debug::Pop(void) {
	--_stackDepth;
	_curParent = _curParent->parent();
	if(!_curParent) _curParent = _treeWidget->invisibleRootItem();
}

void Debug::Log(Debug::LEVEL flag, QString msg, Debug::SOURCE src) {
	_prevItem = new QTreeWidgetItem;
	_prevItem->setText(0, msg);

	//Write to logfile
        for(unsigned i = 0; i < _stackDepth; ++i) *_outfile << "\t";
        *_outfile << msg.toStdString() << std::endl;

	QString srcLabel;

	switch(src) {
	case Debug::TOOLKIT:
		srcLabel = "Toolkit";
		break;
	case Debug::ASSETIO:
		srcLabel = "AssetIO";
		break;
	case Debug::SOURCE_QT:
		srcLabel = "Qt";
		break;
	}

	_prevItem->setText(1, srcLabel);

    Q_ASSERT(_curParent); //better never happen!
	_curParent->addChild(_prevItem);

	if(flag >= Debug::CRITICAL) {
		_pendingError = true;
		QTreeWidgetItem *it = _prevItem;
		do {
			QBrush b(Qt::red);
			it->setForeground(0, b);
			it->setForeground(1, b);
			it->setExpanded(true);
			it = it->parent();
		} while(it);

        //If we've returned to the top level with a pending error,
        //pop the log for the user to see.
        if(_curParent == _treeWidget->invisibleRootItem() && _pendingError) {
                getInstance().show();
        }
	}

	_treeWidget->scrollToBottom();
    //gyPrintf(msg.toStdString().c_str());

}

/*
  ADDITIONAL COMPLEXITY REQUIRED, OR THIS ISN'T GOING TO WORK!
  */
void Debug::OnClickToolkitCheckBox(bool value) {
	//Iterate through every Toolkit item and hide/unhide it
	QList<QTreeWidgetItem*> itemList = _treeWidget->findItems("Toolkit", Qt::MatchExactly, 1);
	for(QList<QTreeWidgetItem*>::iterator it = itemList.begin(); it != itemList.end(); ++it) {
		(*it)->setHidden(!value);
	}
}

void Debug::OnClickAssetioCheckBox(bool value) {
	//Iterate through every AssetIO item and hide/unhide it
	QList<QTreeWidgetItem*> itemList = _treeWidget->findItems("AssetIO", Qt::MatchExactly, 1);
	for(QList<QTreeWidgetItem*>::iterator it = itemList.begin(); it != itemList.end(); ++it) {
		(*it)->setHidden(!value);
	}
}

void Debug::OnClickClear(void) {
	_treeWidget->clear();
	//These need to be reset, because the TreeWidget deleted all TreeWidgetItems
	_curParent = _prevItem = _treeWidget->invisibleRootItem();
}
