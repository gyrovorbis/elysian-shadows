#include <QPainter>
#include <QGLWidget>
#include <QStatusBar>
#include <QDebug>
#include <QLabel>
#include <QSettings>
#include <QMimeData>
#include <QTextCodec>
#include <QDesktopServices>
#include <QTableView>
#include <qgifimage.h>

#include "gyro_vmu_version.h"
#include "gyro_vmu_vms.h"
#include "gyro_vmu_vmi.h"
#include "gyro_vmu_main_window.hpp"
#include "gyro_vmu_tcp_dialog.hpp"
#include "gyro_vmu_buzzer_dialog.hpp"
#include "gyro_vmu_address_space_model.hpp"
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
#   include "gyro_vmu_com_dialog.hpp"
#endif
#include "gyro_vmu_disassembler.h"
#include "ui_gyro_vmu_main_window.h"
#include <gyro_vmu_display.h>
#include <gyro_wav_api.h>
#include <gyro_vmu_sfr.h>
#include <gyro_vmu_device.h>
#include <gyro_audio_api.h>
#include <gyro_net_api.h>
#include <gyro_vmu_flash.h>
#include <gyro_vmu_battery.h>
#include <gyro_vmu_lcd.h>
#include <gyro_file_api.h>
#include <gyro_vmu_cpu.h>
#include <gyro_vmu_gamepad.h>
#include <gyro_input_api.h>
#include <gyro_timer_api.h>
#include <QWindowStateChangeEvent>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <cmath>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QESTkUtils/ui/widget/tk_documentation_browser.h>
#include <ElysianVMU/jet_set_radio_graffiti_tool.hpp>
#include <ElysianVMU/gyro_vmu_filesystem_model.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>

#define Q_CSTR(a) (a.toStdString().c_str())

//#define VMS_IMAGE "/Users/falco/Desktop/VMU/Tetris.vms"
#ifdef VMU_DEBUG
//#define BIOS_IMAGE "/Users/falcogirgis/Documents/Dev/esdev/ElysianVMU/bin/vmu_bios.bin"
#endif
#define VMU_STATUS_TEMP_TIMEOUT         2000
#define VMU_LOG_FILE                    "vmu_debug.txt"
#define SCREENSHOT_FILENAME             "screenshot"
#define SCREENSHOT_FILENAME_EXT         "png"
#define VMU_OUTPUT_DIR                  "ElysianVMU"
#define SETTINGS_FILE                   "UserSettings.ini"
#define SCREENSHOT_FILENAME_SIZE_MAX    100
#define CYCLES                          1000
#define AUDIO_SAMPLE_RATE               44100
#define STATE_FILE                      "elysianVMUSaveState.bin"

#define FRAME_ADVANCE_TICKS             ((1.0f/5.0f)*1000000000.0f)


bool VMUMainWindow::eventFilter(QObject *watched, QEvent *event) {
    if(watched == _ui->graphicsView->scene()) {
    switch(event->type()) {
        case QEvent::GraphicsSceneDragEnter: {
            QGraphicsSceneDragDropEvent* sceneEvent = static_cast<QGraphicsSceneDragDropEvent*>(event);
            auto* mime = sceneEvent->mimeData();
            if(mime->hasUrls()) {
                for(auto&& url : mime->urls()) {
                    if(url.isLocalFile()) {
                        QFileInfo info(url.toLocalFile());
                        QString suff = info.suffix().toLower();
                        if(suff == "vms" || suff == "lcd" || suff == "vmi" || suff == "vmu" ||
                           suff == "bin" || suff == "dci" || suff == "dcm") {
                            sceneEvent->setAccepted(true);
                            return true;
                        }
                    }
                }
            }
            break;
        }
        case QEvent::GraphicsSceneDragMove: {
                event->setAccepted(true);
                return true;
        }
        case QEvent::GraphicsSceneDrop: {
            QGraphicsSceneDragDropEvent* sceneEvent = static_cast<QGraphicsSceneDragDropEvent*>(event);
            auto* mime = sceneEvent->mimeData();
            if(mime->hasUrls()) {
                for(auto&& url : mime->urls()) {
                    if(url.isLocalFile()) {
                        QFileInfo info(url.toLocalFile());
                        QString suff = info.suffix().toLower();
                        if(suff == "vms" || suff == "lcd" || suff == "vmi" || suff == "dci" ||
                           suff == "vmu" || suff == "bin" || suff == "dcm")
                        {
                            sceneEvent->acceptProposedAction();
                            loadRom(info.absoluteFilePath());
                            return true;
                        }
                    }
                }
            }
            break;
        }
        case QEvent::DragEnter:
            break;
        case QEvent::Drop:
            break;
        default: break;
        }
    } else if(watched == _ui->graphicsView) {
        switch(event->type()) {
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
                event->ignore();
                return true;
        default: break;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

FILE* VMUMainWindow::_logFile = nullptr;
int VMUMainWindow::_logDepth = 0;

VMUMainWindow::VMUMainWindow(QWidget *parent, QString workingDir) :
    QMainWindow(parent),
    _ui(new Ui::VMUMainWindow),
    _fb(VMU_FB_DEFAULT_WIDTH, VMU_FB_DEFAULT_HEIGHT),
    _iconES(ICON_ES_PATH),
    _iconClock(ICON_CLOCK_PATH),
    _iconFile(ICON_FILE_PATH),
    _iconGame(ICON_GAME_PATH),
    _iconFlash(ICON_FLASH_PATH),
    _iconDisconnected(ICON_TCP_DISABLED_PATH),
    _iconListening(ICON_TCP_LISTENING_PATH),
    _iconConnected(ICON_TCP_CONNECTED_PATH),
    _iconRecording(ICON_RECORDING_PATH),
    _fileSysTableModel(new VmuFileSystemTableModel),
    _fileSysTable(new VmuFileSystemTableView(this)),
    _tcpDialog(new VmuTcpDialog),
    _buzzerDialog(new VmuBuzzerDialog),
    _statusLabel(new QLabel("Drag & Drop VMU Files")),
    _iconLabel(new QLabel()),
    _recordingLabel(new QLabel()),
    _remoteLabel(new QLabel()),
    _loadGameMenu(new QMenu()),
    _saveGameMenu(new QMenu())
{
    //UI Initialization/Setup
    _ui->setupUi(this);
    setAcceptDrops(true);
    VmuQtUtils::initialize();
    _iconLabel->setMargin(0);
    _iconLabel->setFixedSize(21, 21);
    _recordingLabel->setMargin(0);
    _remoteLabel->setMargin(0);
    _remoteLabel->setFixedSize(20, 20);
    _ui->statusbar->layout()->setSpacing(0);
    _ui->statusbar->insertPermanentWidget(0, _remoteLabel);
    _ui->statusbar->insertPermanentWidget(1, _recordingLabel);
    _ui->statusbar->insertPermanentWidget(2, _statusLabel);
    _ui->statusbar->insertPermanentWidget(3, _iconLabel);
    setWindowTitle(QString("ElysianVMU v ")+QString(GYRO_VMU_VERSION_STRING));

#ifdef VMU_CHILD_APP
    _ui->actionQuit->setEnabled(false);
#endif

    //Create framebuffer label and layouts for centering and managing its size

    //_fbLabel = new QLabel;
    QVBoxLayout* vertLayout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    Q_ASSERT(vertLayout);
    //vertLayout->addWidget(_fbLabel);
    //vertLayout->setAlignment(_fbLabel, Qt::AlignHCenter);
    //_fbLabel->setPixmap(_fb);
    //_fbLabel->setMargin(0);
    //_fbLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    vertLayout->setMargin(0);
    vertLayout->setSpacing(0);

    QGraphicsScene* scene = new QGraphicsScene();
    scene->installEventFilter(this);
    _pixmapItem = new QGraphicsPixmapItem(_iconES);
    scene->addItem(_pixmapItem);
    _ui->graphicsView->setAcceptDrops(true);
    _ui->graphicsView->setScene(scene);
    _ui->graphicsView->installEventFilter(this);



    //Initialize libGyro File API, working directory, and log file
#ifndef VMU_CHILD_APP
    GYFileInitParams fileParams;
    strcpy(fileParams.workingDir, workingDir.isEmpty()?
               QDir::homePath().toStdString().c_str() : workingDir.toStdString().c_str());
    gyFileInit(&fileParams);
#endif


    _workingDir = new QDir(workingDir.isEmpty()? QDir::homePath() : workingDir);
    if(!_workingDir->cd(VMU_OUTPUT_DIR) && !_workingDir->mkdir(VMU_OUTPUT_DIR)) {
        qCritical() << QString("Could not set VMU working directory: [%s]")
                    << _workingDir->filePath(VMU_OUTPUT_DIR);
    } else {
        // _workingDir->setPath(_workingDir->filePath(VMU_OUTPUT_DIR));
    }

    _logFile = fopen(_workingDir->absoluteFilePath(VMU_LOG_FILE).toStdString().c_str(), "w");

    //Initialize libGyro Debug API with logging
#ifndef VMU_CHILD_APP
    GYDebugCallbacks dbgCallbacks = {
        &_log,
        &_push,
        &_pop
    };
    gySysDebugEnable(GY_DEBUG_VERBOSE, &dbgCallbacks);
#endif
    _gyLog(GY_DEBUG_VERBOSE, "Initializing ElysianVMU v %s", GYRO_VMU_VERSION_STRING);
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "Qt Library Versions");
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "Compile-Time: %s",    QT_VERSION_STR);
    _gyLog(GY_DEBUG_VERBOSE, "Run-Time: %s",        qVersion());
    _gyPop(1);

    //Initialize libGyro System, Timer, Audio, Input, and Network APIs
#ifndef VMU_CHILD_APP
    GYSysInitParams sysParams = {
        (void*)this,
        [](void* /*context*/) {
            Q_ASSERT(0); //crashed!!!
        }
    };
    gySysInit(&sysParams);
    gyTimerInit();
    gyAudInit();

    GYInputInitParams inputParams = {
        static_cast<QObject*>(this),
        nullptr,
        4
    };
    gyInputInit(&inputParams);
#endif
    GYNetInitParams netParams = {
#ifdef VMU_CHILD_APP
        2 + 4//maximum number of monitored sockets allowed (2 base + 4 from ESGamma)
#else
        2
#endif

    };
    gyNetInit(&netParams);

    //Create VMU instance
    _dev = gyVmuDeviceCreate();

    //Initialize external file menu dialogs
#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
    _helpDockWidget = new QESTkUtils::HelpDockWidget(nullptr,
                                                     "ElysianVMU",
                                                     ":/manual/docs",
                                                     "qthelp://elysianvmu.help/doc/index.html",
                                                     VMU_OUTPUT_DIR);
    _ui->actionDocumentation->setEnabled(true);
#else
    _ui->actionDocumentation->setEnabled(false);
#endif
    _tcpDialog->setVmuDevice(_dev);
    _buzzerDialog->setVmuDevice(_dev);
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    QAction* comConfig = new QAction("Serial/COM Connection Manager");
    connect(comConfig, SIGNAL(triggered()), this, SLOT(on_actionSerial_COM_Connection_Manager_triggered()));
    _ui->menuSerial_Communications->addAction(comConfig);
    _comDialog = new VmuComDialog;
    _comDialog->setVmuDevice(_dev);
#endif

    //Connect Load/Save File Menu Logic
    _ui->actionLoad_Game->setMenu(_loadGameMenu);
    _ui->actionSave_Game->setMenu(_saveGameMenu);
    connect(_loadGameMenu, SIGNAL(aboutToShow()), this, SLOT(_onLoadGameAboutToShow()));
    connect(_loadGameMenu, SIGNAL(aboutToHide()), this, SLOT(_onLoadGameAboutToHide()));
    connect(_saveGameMenu, SIGNAL(aboutToShow()), this, SLOT(_onSaveGameAboutToShow()));
    connect(_saveGameMenu, SIGNAL(aboutToHide()), this, SLOT(_onSaveGameAboutToHide()));

    //Begin running (debugging)
#ifdef BIOS_IMAGE
     gyloadbios(_dev, BIOS_IMAGE);
     resetCpu();
     _running = true;
#endif

#ifdef VMS_IMAGE
    gyVmuFlashLoadImage(_dev, VMS_IMAGE);
    resetCpu();
    _running = true;
#endif

    //Clear framebuffer
    _fb.fill(Qt::black);
    QPainter painter(&_fb);
    painter.drawPixmap(QPoint(0,0), _iconES);

    //Set ESC to exit fullscreen
    auto* exitFs = new QAction(this);
    exitFs->setShortcut(Qt::Key_Escape);
    connect(exitFs, &QAction::triggered, this, [&](void) {
        if(windowState()&Qt::WindowFullScreen)
            showNormal();

    });
    addAction(exitFs);

    _setupAboutMsgBox();

    //Set update timer
#ifndef VMU_CHILD_APP
    _updateTimer.setInterval(17);
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    _updateTimer.start();
#endif

    _gyPop(1);
    _resizeWindowFromScreenZoom(VMU_FB_DEFAULT_ZOOM_LEVEL);
    _loadSettings();
}



VMUMainWindow::~VMUMainWindow(void)
{
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing ElysianVMU");
    _gyPush();
    _freeIcons();
    gyVmuDeviceDestroy(_dev);
    gyNetUninit();
#ifndef VMU_CHILD_APP
    gyInputUninit();
    gyAudUninit();
    gyTimerUninit();
    gySysUninit();
    _gyPop(1);
    gyDebugUninit();
#else
    _gyPop(1);
#endif
    fclose(_logFile);
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    delete _comDialog;
#endif
    delete _tcpDialog;
    delete _ui;
}

void VMUMainWindow::_setupAboutMsgBox(void) {
    connect(_ui->actionAbout, &QAction::triggered, [this]{
        QMessageBox::about(this, "About ElysianVMU",
        QString("ElysianVMU is a Softare emulator for the Sega Dreamcast Visual Memory Unit.<br><br>")+
        QString("Version ")+QString(GYRO_VMU_VERSION_STRING)+QString("<br>") +
        QString("<a href=\"http://www.elysianshadows.com\">elysianshadows.com</a><br>") +
        QString("</table><tr><td>Software: </td><td> Falco Girgis</td></tr>") +
        QString("<tr><td>Hardware: </td><td>Tulio Goncalves</td></tr>") +
        QString("<tr><td>Graphics: </td><td>Patrick Kowalik</td></tr></table><br><br>") +
        QString("Special Thanks:<br> Deunan Knute, Marcus Comstedt, Inolen, RinMaru, and all of the Elysian Shadows Kickstarter backers and supporters  :)</br</br>") +
        QString("</table><br><br>") +
        QString("Powered by LibGyro (version ")+QString(GYRO_LIB_VERSION)+QString(")<br><br>") +
        QString("<i>ElysianVMU Copyright 2017-2018 Elysian Shadows</i><br>") +
        QString("<i>Dreamcast and VMU Copyright Sega</i><br><br>"));
    });
}

auto VMUMainWindow::getUi(void) const -> Ui::VMUMainWindow* {
    return _ui;
}

void VMUMainWindow::_onLoadGameAboutToShow(void) {
    _gyLog(GY_DEBUG_VERBOSE, "LOAD SHOW");
}

void VMUMainWindow::_onLoadGameAboutToHide(void) {
    _gyLog(GY_DEBUG_VERBOSE, "LOAD HIDE");
}

void VMUMainWindow::_onSaveGameAboutToShow(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Save SHOW");
}

void VMUMainWindow::_onSaveGameAboutToHide(void) {
    _gyLog(GY_DEBUG_VERBOSE, "SAVE HIDE");
}


bool VMUMainWindow::event(QEvent* event) {
    switch(event->type()) {
    case QEvent::WindowStateChange: {
        QWindowStateChangeEvent* w = static_cast<QWindowStateChangeEvent*>(event);
        int wasFullScreen = w->oldState()&Qt::WindowFullScreen;
        if(wasFullScreen != (windowState()&Qt::WindowFullScreen)) {
            if(wasFullScreen) _ui->statusbar->show();
            else _ui->statusbar->hide();
        }
    }
    default:
    break;
    }

    return QMainWindow::event(event);
}


void VMUMainWindow::closeEvent(QCloseEvent* /*event*/) {
    _saveSettings();
}


void VMUMainWindow::update(void) {
    //Render scene no matter what in case resolution changed, if nothing changed, it'll exit early.
    _updateScene();

    bool ranThisFrame = false;
    int64_t deltaTicks;

    auto* gameEntry = gyVmuFlashDirEntryGame(_dev);
    if(!_running) {
        if(gameEntry || _dev->biosLoaded) {
            _gyLog(GY_DEBUG_VERBOSE, "New game detected, resetting CPU!");
            resetCpu();
            _running = true;
        }

    } else {
        if(!gameEntry && !_dev->biosLoaded) {
            _gyLog(GY_DEBUG_VERBOSE, "Game was removed, halting CPU!");
            resetCpu();
            _fb.fill(Qt::black);
            _updateScene();
            _running = false;
        }
    }

    static int statusBarCount = 0;
    if(++statusBarCount >= VMU_STATUS_BAR_FORCE_UPDATE_INTERVAL) {
        _statusBarChanged = true;
        _updateStatusbar();
    }

    if(!_running && !_dev->lcdFile) return;

    if(_halt) {
        if(!_advanceFrame) deltaTicks = 0;
        else deltaTicks = FRAME_ADVANCE_TICKS;
    } else {
        if(_wasHalted) {

            deltaTicks = FRAME_ADVANCE_TICKS;
        }
        else {
            auto ticks = gyTimerTicks();
            deltaTicks = ticks - _prevTicks;
        }
    }
    _wasHalted = _halt;
    _advanceFrame = false;

    //store socket status before update

      for(int i = 0; i < 1; ++i) {

#ifndef VMU_DEBUG
        #if 1
          ranThisFrame = gyVmuDeviceUpdate(_dev, (float)deltaTicks/1000000000.0f);
#else
                    ranThisFrame = gyVmuDeviceUpdate(_dev, (float)deltaTicks/10000000.0f);
#endif
#else
        gyVmuCpuTick(_dev, 1.0f/30.0f);
#endif
      }

   _prevTicks = gyTimerTicks();

    if(ranThisFrame) {

        if(_contAttached && !_dev->gamepad.contAttached) {
            _ui->statusbar->showMessage("Controller Disconnected!", VMU_STATUS_TEMP_TIMEOUT);
        } else if(!_contAttached && _dev->gamepad.contAttached) {
            _ui->statusbar->showMessage("Controller Connected!", VMU_STATUS_TEMP_TIMEOUT);
        }
        _contAttached = (bool)_dev->gamepad.contAttached;

        _tcpDialog->updateConnectionStatus();
        _updateStatusbar();

        if(!_dev->lcdFile) _updatePConRegister();
        else _updateLcdAnimStatusMessage();
        _updateIcon();

    }
}

void VMUMainWindow::_updateStatusbar(void) {
    REMOTE_STATE next;

    switch(gyVmuSerialConnectionType(_dev)) {
        case GY_VMU_TCP_CONNECTION_VMU:
        next = REMOTE_CONNECTED;
        break;
    case GY_VMU_TCP_CONNECTION_DC:
        next = REMOTE_CONNECTED;
        break;
    default:
        if(_tcpDialog->isTcpServerActive()) {
            next = REMOTE_LISTENING;

        } else {
            next = REMOTE_DISABLED;
        }
    }

    bool remoteDCStatus = false;

    if(_remoteState != next) {
        _statusBarChanged = true;
        switch(next) {
        case REMOTE_CONNECTED: {
            remoteDCStatus = true;
            _remoteLabel->setToolTip("Connected to Remote Device");
            _remoteLabel->setPixmap(_iconConnected);
            break;
        } case REMOTE_LISTENING:
            _remoteLabel->setToolTip("Listening for Remote Devices");
            _remoteLabel->setPixmap(_iconListening);
            break;
        case REMOTE_DISABLED:
            _remoteLabel->setToolTip("Remote Server Disabled");
            _remoteLabel->setPixmap(_iconDisconnected);
        default:
            break;
        }

        _remoteState = next;
    }

    if(_statusBarChanged) {
        QPalette palette = _statusLabel->palette();

        if(remoteDCStatus) {
            palette.setColor(QPalette::WindowText, QColor(Qt::darkGreen));
            _statusLabel->setText("Connected to Remote DC");

        } else {
            palette.setColor(QPalette::WindowText, QColor(Qt::black));
            if(_dev->lcdFile) {
                _statusLabel->setText(QString("LCD Animation"));

            } else {

                auto* gameEntry = gyVmuFlashDirEntryGame(_dev);
                if(gameEntry) {
                    auto* vmsHeader = gyVmuFlashDirEntryVmsHeader(_dev, gameEntry);
                    char string[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
                    _vmsHeader = vmsHeader;
                    gyVmuVmsHeaderVmuDescGet(vmsHeader, string);
                    _statusLabel->setText(VmuQtUtils::japCodec->toUnicode(string));
                } else {
                    int fileCount = gyVmuFlashFileCount(_dev);
                    if(fileCount) {
                        VMUFlashMemUsage mem = gyVmuFlashMemUsage(_dev);
                        QString label = QString("File(s): %1  Free Blocks: %2/%3").arg(fileCount).arg(mem.blocksFree).arg(gyVmuFlashUserDataBlocks(_dev));
                        _statusLabel->setText(label);
                    } else if(_dev->biosLoaded) {
                        palette.setColor(QPalette::WindowText, QColor(Qt::gray));
                        _statusLabel->setText("System BIOS Image");
                    } else {
                        palette.setColor(QPalette::WindowText, QColor(Qt::darkRed));
                        _statusLabel->setText("Drag & Drop VMU File.");
                    }
                }

            }
        }

        _statusLabel->setPalette(palette);
        _statusBarChanged = false;

    }
}


void VMUMainWindow::_updateScene(void) {
    bool updatedFb = false;

    //Update framebuffer texture based on VMU LCD screen state
    {
        QPainter painter(&_fb);
        if((!gyVmuDisplayEnabled(_dev) && _displayWasEnabled) || !_running) {
            painter.drawPixmap(QPoint(0,0), _iconES);
            _displayWasEnabled = false;
            updatedFb = true;
        } else if(gyVmuDisplayUpdateEnabled(_dev)) {
            _displayWasEnabled = true;
            if(_dev->display.screenChanged) {
                for(int y = 0; y < VMU_DISP_PIXEL_HEIGHT; ++y) {
                    for(int x = 0; x < VMU_DISP_PIXEL_WIDTH; ++x) {
                        int col = gyVmuDisplayPixelGhostValue(_dev, x, y);
                        painter.fillRect(x, y, 1, 1, QBrush(QColor(col, col, col, 255)));
                    }
                }
                updatedFb = true;
                if(_recording) _addAnimFrame(_fb);
                _dev->display.screenChanged = 0;
            }
        }
    }

    //Resize framebuffer to the viewport size to produce final image
    int width = _ui->graphicsView->viewport()->width();//centralWidget()->width();
    int height = _ui->graphicsView->viewport()->height();//centralWidget()->height();

    if(!updatedFb && width == _prevFbWidth && height == _prevFbHeight) return;

    QPixmap scaledFb = _fb.scaled(width, height,
                                  _ui->actionPreserve_Aspect_Ratio->isChecked()?
                                      Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);

    //Scaled dimensions of final image
    int frameHeight = scaledFb.height();
    int frameWidth = scaledFb.width();

    //Apply icon pixmaps to SCALED framebuffer, so they can stay higher resolution
    {
        QPainter painter2(&scaledFb);
        float yMargin       = 0.20f;
        float leftOverH     = 1.0f - ((float)VMU_DISP_PIXEL_HEIGHT/(float)VMU_FB_DEFAULT_HEIGHT);
        float iconHeight    = leftOverH*(1.0f-yMargin)*(float)frameHeight;
        float iconY         = frameHeight-iconHeight-((yMargin)*leftOverH*(float)frameHeight)/2.0f;
        float xMargin       = 1.0f/20.0f;
        float leftOverW     = 1.0f - xMargin;
        float iconWidth     = (2.0f/11.0f)*(float)frameWidth*leftOverW;
        float iconXStart    = xMargin*0.5*(float)frameWidth;
        float iconXStride   = iconWidth+(1.0f/11.0f)*leftOverW*(float)frameWidth;

        if(gyVmuDisplayIconGet(_dev, VMU_DISP_ICN_FILE))
            painter2.drawPixmap(iconXStart, iconY, iconWidth, iconHeight, _iconFile);
        if(gyVmuDisplayIconGet(_dev, VMU_DISP_ICN_GAME))
            painter2.drawPixmap(iconXStart+iconXStride, iconY, iconWidth, iconHeight, _iconGame);
        if(gyVmuDisplayIconGet(_dev, VMU_DISP_ICN_CLOCK))
            painter2.drawPixmap(iconXStart+iconXStride*2.0f, iconY, iconWidth, iconHeight, _iconClock);
        if(gyVmuDisplayIconGet(_dev, VMU_DISP_ICN_FLASH))
            painter2.drawPixmap(iconXStart+iconXStride*3.0f, iconY, iconWidth, iconHeight, _iconFlash);
    }

    //Update the pixmap within the central widget
    //_fbLabel->setPixmap(scaledFb);
    _pixmapItem->setPixmap(scaledFb);
    _ui->graphicsView->setSceneRect(0, 0, frameWidth, frameHeight);
    //QPainter painter(this);
    //painter.drawPixmap(0, 0, scaledFb);

    _prevFbWidth = width;
    _prevFbHeight = height;
}

bool VMUMainWindow::loadRom(QString filepath) {
#if 1
    int prevFileCount = gyVmuFlashFileCount(_dev);
    VMUFlashDirEntry* dirEntry = nullptr;
    bool status = VmuQtUtils::loadVMUImage(_dev, filepath, &dirEntry, true);

    if(status) {

        if(gyVmuFlashDirEntryGame(_dev)) resetCpu();

        _pushRecentRom(filepath);

        /* Grab first file if we loaded entire VMU image (instead of one ROM),
         * and that image only had one file in it. */
        if(!dirEntry) {
            if(gyVmuFlashFileCount(_dev)) {
                dirEntry = gyVmuFlashFileAtIndex(_dev, 0);
            }
        }

        if(dirEntry) {
            QString typeStr;

            //Loaded a data file or multiple files
            if(dirEntry->fileType == VMU_FLASH_FILE_TYPE_DATA) {
                typeStr = "DATA";
                if(!_fileSysTable->isVisible()) on_actionFilesystem_triggered();
            } else typeStr = "GAME";

            char buff[100];
            gyVmuFlashDirEntryName(dirEntry, buff);
            QString msg = QString("Loaded: %1 [%2]").arg(buff).arg(typeStr);
            _ui->statusbar->showMessage(msg, VMU_STATUS_TEMP_TIMEOUT);
            _statusBarChanged = true;
            _updateStatusbar();

        //We loaded an image with more than one file
        } else if(gyVmuFlashFileCount(_dev) > 1) {
            char buff[100];
            int fileCount = gyVmuFlashFileCount(_dev);
            if(!_fileSysTable->isVisible()) on_actionFilesystem_triggered();
            QString msg = QString("Loaded: %1 Files").arg(fileCount);
            _ui->statusbar->showMessage(msg, VMU_STATUS_TEMP_TIMEOUT);
            _statusBarChanged = true;
            _updateStatusbar();
        }


    }
#else
    if(filepath.isEmpty()) return false;

    QFileInfo info(filepath);
    _romDir = info.absoluteDir().absolutePath();

    //If loading a .VMI, we need to get the .VMS file too.
    if(info.exists()) {
        if(info.suffix() == "VMI" || info.suffix() == "vmi") {
            VMIFileInfo vmiInfo;
            if(gyVmuFlashLoadVMI(&vmiInfo, filepath.toStdString().c_str())) {
                char buff[33];
                QString fileName;
                gyVmuVmiFileInfoResourceNameGet(&vmiInfo, buff);
                fileName = QString(buff) + QString(".vms");
                if(!info.absoluteDir().exists(fileName)) {
                    fileName = QString(buff) + QString(".VMS");
                    if(!info.absoluteDir().exists(fileName)) {
                        fileName = "";
                    }
                }

                if(!fileName.isEmpty()) {
                    loadRom(info.absoluteDir().absoluteFilePath(fileName));
                } else {
                    QString newVMSPath = filepath.split(".")[0] + QString(".vms");
                    if(QMessageBox::question(nullptr, "VMS Not found!",
                                        QString("The specified VMI file references ")+QString(buff)+QString(".vms")+
                                        QString(" which does not exist. Try loading ")+newVMSPath+QString(" instead?"))
                            == QMessageBox::Yes) {
                        return loadRom(newVMSPath);
                    }
                }

            } else {

            }
        }
    }

    VMU_LOAD_IMAGE_STATUS status;
    if(info.exists() && gyVmuFlashLoadImage(_dev, filepath.toStdString().c_str(), &status)) {
        if(_dev->lcdFile) {
            _statusBarChanged = true;
            _pushRecentRom(filepath);
            _gyLog(GY_DEBUG_VERBOSE, "Playing LCD animation.");
            resetCpu();
            return true;
        } else {
            _vmsHeader = (VMSFileInfo*)&_dev->flash[VMU_FLASH_GAME_VMS_HEADER_OFFSET];

            resetCpu();

            _statusBarChanged = true;
            _gyLog(GY_DEBUG_VERBOSE, "Running VMU CPU.");
            _running = true;
            _pushRecentRom(filepath);
            return true;
        }
    }
    _gyLog(GY_DEBUG_ERROR, "Error Loading File! [%s]", info.absoluteFilePath().toStdString().c_str());
    return false;
#endif
}

void VMUMainWindow::resetCpu(void) {

    gyVmuDeviceReset(_dev);

    gyVmuBuzzerEnabledSet(_dev, _ui->actionEmulate_Audio->isChecked());

     //Have to apply non-default settings after CPU reset, since that clears everything back to default state.
    if(_ui->actionEnable_Low_Battery->isChecked()) {
        gyVmuBatteryLowSet(_dev, 1);
        gyVmuBatteryMonitorEnabledSet(_dev, 1);
    }

    gyVmuDisplayGhostingEnabledSet(_dev, _ui->actionEnable_Pixel_Ghosting->isChecked());
}

void VMUMainWindow::_updateLcdAnimStatusMessage(void) {
    static const char* stateNames[] = {
        "STOPPED",
        "PLAYING",
        "COMPLETE"
    };
    char buff[100];

    sprintf(buff, "frame: [%d/%d]\t state: %s", _dev->lcdFile->currentFrame, _dev->lcdFile->header->frameCount-1,
            stateNames[_dev->lcdFile->state]);
    _ui->statusbar->showMessage(buff, VMU_STATUS_TEMP_TIMEOUT);
}

void VMUMainWindow::on_actionLoad_ROM_triggered(void)
{
    loadRom(QFileDialog::getOpenFileName(nullptr, "Browse VMS ROM to Open", VmuQtUtils::getRomDir(),
                                         "VMS Files (*.vms *.VMS);;Nexus Files (*.dci);;Web Browser Files (*.vmi *.VMI);;Dream Animator Files (*.lcd *.LCD);;Raw Flash Image Files (*.vmu *.VMU *.bin *.BIN);;Nexus Flash Image Files(*.dcm *.DCM)"));
}

void VMUMainWindow::on_actionSave_State_triggered(void)
{
    if(!_running) return;
    _gyLog(GY_DEBUG_VERBOSE, "Saving state [%s].", _workingDir->absoluteFilePath(STATE_FILE).toStdString().c_str());
    _gyPush();

    FILE* file = fopen(Q_CSTR(_workingDir->absoluteFilePath(STATE_FILE)), "wb");
    if(!file) {
        _gyLog(GY_DEBUG_ERROR, "Could not open file for writing!");
    } else {
        fwrite(_dev, sizeof(VMUDevice), 1, file);
        fclose(file);
        _ui->statusbar->showMessage("State Saved.", VMU_STATUS_TEMP_TIMEOUT);
    }

    _gyPop(1);
}

void VMUMainWindow::on_actionLoad_State_triggered(void)
{
    if(!_running) return;

    _gyLog(GY_DEBUG_VERBOSE, "Loading state [%s].", _workingDir->absoluteFilePath(STATE_FILE).toStdString().c_str());
    _gyPush();

    FILE* file = fopen(Q_CSTR(_workingDir->absoluteFilePath(STATE_FILE)), "rb");
    if(!file) {
        _gyLog(GY_DEBUG_ERROR, "Could not open file for reading!");
    } else {
        fread(_dev, sizeof(VMUDevice), 1, file);
        fclose(file);

        //adjust pointer values
        _dev->imem = _dev->flash;

        int xramBk = gyVmuMemRead(_dev, SFR_ADDR_XBNK);
        int ramBk = (gyVmuMemRead(_dev, SFR_ADDR_PSW)&SFR_PSW_RAMBK0_MASK)>>SFR_PSW_RAMBKO_POS;
        _dev->memMap[VMU_MEM_SEG_XRAM]   = _dev->xram[xramBk];
        _dev->memMap[VMU_MEM_SEG_SFR]    = _dev->sfr;
        _dev->memMap[VMU_MEM_SEG_GP1]    = _dev->ram[ramBk];
        _dev->memMap[VMU_MEM_SEG_GP2]    = &_dev->ram[ramBk][VMU_MEM_SEG_SIZE];

        _ui->statusbar->showMessage("State Loaded.", VMU_STATUS_TEMP_TIMEOUT);

    }

    _gyPop(1);
}


void VMUMainWindow::_push(void) {
    ++_logDepth;
}

void VMUMainWindow::_pop(const unsigned depth) {
    _logDepth -= depth;
}

void VMUMainWindow::_log(GY_DEBUG_SEVERITY level, const char *msg) {
    char tabBuff[100] = { 0 };

    for(int i = 0; i < _logDepth && i < sizeof(tabBuff)-1; ++i) {
        strcat(tabBuff, "\t");
    }

    static const char* prefix[3] = {
        "",
        "! - ",
        "X - "
    };
#if 0
    char buffer[10000] = {0};
    strcat(buffer, tabBuff);
    strcat(buffer, prefix[level]);
    strcat(buffer, msg);
    strcat(buffer, "\n");
    printf(buffer);
#endif
#ifdef VMU_DEBUG
   // return;
#endif
#if FILTER_DUPLICATE_LOG_STRINGS
    static QString previous;
    if(previous == msg) return;
    previous = msg;
#endif
    qDebug() << tabBuff << prefix[level] << msg;
    fprintf(_logFile, "%s%s%s\n", tabBuff, prefix[level], msg);
    fflush(_logFile);
}

void VMUMainWindow::on_actionReset_triggered(void)
{
    _ui->statusbar->showMessage("CPU has been reset.", VMU_STATUS_TEMP_TIMEOUT);
    resetCpu();
}

void VMUMainWindow::on_actionScreenshot_triggered(void)
{
    char buffer[SCREENSHOT_FILENAME_SIZE_MAX];
    unsigned int number = 0;

    do {
        sprintf(buffer, "%s_%d.%s", Q_CSTR(_workingDir->absoluteFilePath(SCREENSHOT_FILENAME)), ++number, SCREENSHOT_FILENAME_EXT);
    } while(QFileInfo(buffer).exists());

    QFile file(buffer);
    file.open(QIODevice::WriteOnly);
    _gyLog(GY_DEBUG_VERBOSE, "Saving screenshot[%s]", _workingDir->absoluteFilePath(buffer).toStdString().c_str());
    _ui->statusbar->showMessage("Screenshot saved.", VMU_STATUS_TEMP_TIMEOUT);
    _fb.save(&file, SCREENSHOT_FILENAME_EXT);

}

void VMUMainWindow::_saveSettings(void) {
    QSettings settings(_workingDir->absoluteFilePath(SETTINGS_FILE), QSettings::IniFormat);
    char buffer[256];

    _gyLog(GY_DEBUG_VERBOSE, "Saving User Settings [%s].", settings.fileName().toStdString().c_str());

    settings.setValue("settings",   true);
    settings.setValue("geometry",   saveGeometry());
    settings.setValue("state",      saveState());
    settings.setValue("romDir",     VmuQtUtils::getRomDir());
    settings.setValue("audio",      _ui->actionEmulate_Audio->isChecked());
    settings.setValue("ghosting",   _ui->actionEnable_Pixel_Ghosting->isChecked());
    settings.setValue("lowBattery", _ui->actionEnable_Low_Battery->isChecked());
    settings.setValue("aspectRatio", _ui->actionPreserve_Aspect_Ratio->isChecked());

    for(int i = 0; i < _ui->menuLoad_Recent->actions().size(); ++i) {
        sprintf(buffer, "recentRoms%d", i);
        settings.setValue(buffer, _ui->menuLoad_Recent->actions()[i]->text());
    }

    _tcpDialog->saveSettings(settings);
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    _comDialog->saveSettings(settings);
#endif
}

void VMUMainWindow::_loadSettings(void) {
    QSettings settings(_workingDir->absoluteFilePath(SETTINGS_FILE), QSettings::IniFormat);
    char buffer[256];

    _gyLog(GY_DEBUG_VERBOSE, "Loading User Settings [%s].", settings.fileName().toStdString().c_str());
    _gyPush();
    bool saved = settings.value("settings", false).toBool();

    if(saved) {
        restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
        restoreState(settings.value("state", QByteArray()).toByteArray());
        VmuQtUtils::setRomDir(settings.value("romDir", "").toString());
        _ui->actionEmulate_Audio->setChecked(settings.value("audio", true).toBool());
        _ui->actionEnable_Low_Battery->setChecked(settings.value("lowBattery", false).toBool());
        _ui->actionEnable_Pixel_Ghosting->setChecked(settings.value("ghosting", true).toBool());
        _ui->actionPreserve_Aspect_Ratio->setChecked(settings.value("aspectRatio", true).toBool());

        for(int i = VMU_ROM_HISTORY_SIZE-1; i >= 0; --i) {
            sprintf(buffer, "recentRoms%d", i);
            auto recentRom = settings.value(buffer, "").toString();
            if(!recentRom.isEmpty()) _pushRecentRom(recentRom);
        }
    } else {
        _gyLog(GY_DEBUG_WARNING, "No settings found! Using defaults!");
    }

    _tcpDialog->loadSettings(settings);
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    _comDialog->loadSettings(settings);
#endif

    _gyPop(1);
}

void VMUMainWindow::_pushRecentRom(QString path) {
    QAction* action = nullptr;

    //Find out whether the action is already in the menu
    for(auto&& it : _ui->menuLoad_Recent->actions()) {
        if(it->text() == path) {
            action = it;
            break;
        }
    }

    if(action) {
        //Remove it if so
        _ui->menuLoad_Recent->removeAction(action);
    } else {
        //Create a new action if not
        action = new QAction(path, nullptr);
        connect(action, &QAction::triggered, [this, path](void) {
            loadRom(path);
        });
    }

    //Insert action at beginning
    if(_ui->menuLoad_Recent->actions().size() == 0) {
        _ui->menuLoad_Recent->addAction(action);
    } else {
        _ui->menuLoad_Recent->insertAction(_ui->menuLoad_Recent->actions().first(), action);

        //Determine if we have too many actions
        auto actions = _ui->menuLoad_Recent->actions();
        int overflow = actions.size() - VMU_ROM_HISTORY_SIZE;

        //Trim the last actions
        if(overflow > 0) {
            for(int i = 0; i < overflow; ++i) {
                _ui->menuLoad_Recent->removeAction(actions[actions.size()-1-i]);
            }
        }
    }
}

void VMUMainWindow::dragEnterEvent(QDragEnterEvent *event) {
#if 1
    auto* mime = event->mimeData();
    if(mime->hasUrls()) {
        for(auto&& url : mime->urls()) {
            if(url.isLocalFile()) {
                QFileInfo info(url.toLocalFile());
                QString suff = info.suffix().toLower();
                if(suff == "vms" || suff == "lcd" || suff == "vmi" ||
                   suff == "vmu" || suff == "bin" || suff == "dci" ||
                   suff == "dcm")
                {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
#endif
    QMainWindow::dragEnterEvent(event);
}

void VMUMainWindow::dropEvent(QDropEvent* event) {
#if 1
    auto* mime = event->mimeData();
    if(mime->hasUrls()) {
        for(auto&& url : mime->urls()) {
            if(url.isLocalFile()) {
                QFileInfo info(url.toLocalFile());
                QString suff = info.suffix().toLower();
                if(suff == "vms" || suff == "lcd" || suff == "vmi" ||
                   suff == "vmu" || suff == "bin" || suff == "dci" ||
                   suff == "dcm")
                {
                    event->acceptProposedAction();
                    loadRom(info.absoluteFilePath());
                    return;
                }
            }
        }
    }
#endif
    QMainWindow::dropEvent(event);
}

void VMUMainWindow::_freeIcons(void) {
    int iconCount = _iconImages.size();

    //Clean up icons if we're about to load more!
    if(iconCount) {
        _gyLog(GY_DEBUG_VERBOSE, "Freeing icon data.");

        _iconImages.clear();
        _iconPixmaps.clear();
        for(int i = 0; i < iconCount; ++i)
            free(_iconsData[i]);
        free(_iconsData);

        if(_eyecatchData) {
            _gyLog(GY_DEBUG_VERBOSE, "Freeing eyecatch data.");
            free(_eyecatchData);
        }
    }
}

bool VMUMainWindow::_loadIcons(void) {
    bool hasIcons = false;
    //Clear out old shit
    _freeIcons();

    _gyLog(GY_DEBUG_VERBOSE, "Loading icon data.");
    _gyPush();

    //_ui->statusbar->removeWidget(_iconLabel);
    _iconAnimCounter    = 0;
    _iconCurIndex       = 0;

    auto* dirEntry = gyVmuFlashDirEntryGame(_dev);
    _iconsData    = gyVmuVMSFileInfoCreateIconsARGB444(_dev, dirEntry);

    if(_iconsData) {
        for(unsigned i = 0; i < _vmsHeader->iconCount; ++i) {
            _gyLog(GY_DEBUG_VERBOSE, "Creating texture for icon %d.", i);
            _iconImages.push_back(QImage((uchar*)_iconsData[i], VMU_VMS_ICON_BITMAP_WIDTH, VMU_VMS_ICON_BITMAP_HEIGHT, QImage::Format_ARGB4444_Premultiplied));
            _iconPixmaps.push_back(QPixmap::fromImage(_iconImages[i]).scaled(21, 21, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        _iconLabel->setPixmap(_iconPixmaps[0]);
        hasIcons = true;
    }

    _eyecatchData = gyVmuVMSFileInfoCreateEyeCatchARGB444(_dev, dirEntry);

    if(_eyecatchData) {
        _gyLog(GY_DEBUG_VERBOSE, "Creating texture for eyecatch.");
        _eyecatchImage = QImage((uchar*)_eyecatchData, VMU_VMS_EYECATCH_BITMAP_WIDTH, VMU_VMS_EYECATCH_BITMAP_HEIGHT,  QImage::Format_ARGB4444_Premultiplied);
    }

    _gyPop(1);
    return hasIcons;
}

void VMUMainWindow::_updateIcon(void) {
    if(!_iconPixmaps.size()) return;

    if(++_iconAnimCounter >= _vmsHeader->animSpeed*2) { //x2, because I believe units are frames, and I think the DC ROM is @ 30fps.
        _iconAnimCounter = 0;

        if(++_iconCurIndex >= _vmsHeader->iconCount) {
            _iconCurIndex = 0;
        }

        _iconLabel->setPixmap(_iconPixmaps[_iconCurIndex]);
    }

}


void VMUMainWindow::on_actionEmulate_Audio_toggled(bool arg1)
{
    if(!arg1) {
        _ui->statusbar->showMessage("Audio Emulation Disabled.", VMU_STATUS_TEMP_TIMEOUT);
        _gyLog(GY_DEBUG_VERBOSE, "Audio Emulation Disabled.");
    } else {
        _ui->statusbar->showMessage("Audio Emulation Enabled.", VMU_STATUS_TEMP_TIMEOUT);
        _gyLog(GY_DEBUG_VERBOSE, "Audio Emulation Enabled.");
    }
}

void VMUMainWindow::on_actionDump_Icons_triggered(void)
{
    //+2 digits + 4 ext + 1 NULL byte
    char filename[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+7];
    char vmsDesc[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];

    //Ain't nothing to dump!
    if(!_iconImages.size()) {
        QMessageBox::critical(nullptr, "No Icons", "The current VMS image does not have any icons!");
        return;
    }

    _gyLog(GY_DEBUG_VERBOSE, "Dumping VMS Icons.");
    _gyPush();

    gyVmuVmsHeaderVmuDescGet(_vmsHeader, vmsDesc);

    for(int i = 0; i < _iconImages.size(); ++i) {
        sprintf(filename, "%s_%d.png", vmsDesc, i);
        if(!_iconImages[i].save(VmuQtUtils::japCodec->toUnicode(_workingDir->absoluteFilePath(filename).toStdString().c_str())))
            _gyLog(GY_DEBUG_ERROR, "Could not dump icon to file [%s]!", _workingDir->absoluteFilePath(filename).toStdString().c_str());
        else
            _gyLog(GY_DEBUG_VERBOSE, "Dumped icon to file [%s].", _workingDir->absoluteFilePath(filename).toStdString().c_str());
    }

    _gyPop(1);
}

void VMUMainWindow::_updatePConRegister(void) {
    int pcon = gyVmuMemRead(_dev, SFR_ADDR_PCON) & (SFR_PCON_HALT_MASK|SFR_PCON_HOLD_MASK);

    if(pcon != _pConRegister) {
        switch(pcon) {
        case 0:
            _ui->statusbar->showMessage("CPU State: RUNNING", VMU_STATUS_TEMP_TIMEOUT);
            break;
        case 1:
            _ui->statusbar->showMessage("CPU State: HOLD", VMU_STATUS_TEMP_TIMEOUT);
            break;
        case 2:
            _ui->statusbar->showMessage("CPU State: HALT", VMU_STATUS_TEMP_TIMEOUT);
            break;
        case 3:
            _ui->statusbar->showMessage("CPU State: UNKNOWN", VMU_STATUS_TEMP_TIMEOUT);
            break;
        }

        _pConRegister = pcon;
    }

}

void VMUMainWindow::on_actionDump_Eyecatch_triggered(void)
{
    //+2 digits + 4 ext + 1 NULL byte
    char filename[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+7];
    char vmsDesc[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];

    //Ain't nothing to dump!
    if(!_eyecatchData) {
        QMessageBox::critical(nullptr, "No Eyecatch", "The current VMS image does not have an eyecatch graphic!");
        return;
    }

    _gyLog(GY_DEBUG_VERBOSE, "Dumping VMS Eyecatch.");
    _gyPush();

    gyVmuVmsHeaderVmuDescGet(_vmsHeader, vmsDesc);

    sprintf(filename, "%s_eyecatch.png", vmsDesc);
    if(!_eyecatchImage.save(VmuQtUtils::japCodec->toUnicode(_workingDir->absoluteFilePath(filename).toStdString().c_str())))
        _gyLog(GY_DEBUG_ERROR, "Could not dump eyecatch to file [%s]!", _workingDir->absoluteFilePath(filename).toStdString().c_str());
    else
        _gyLog(GY_DEBUG_VERBOSE, "Dumped eyecatch to file [%s].", _workingDir->absoluteFilePath(filename).toStdString().c_str());

    _gyPop(1);

}

void VMUMainWindow::on_actionTCP_Connection_Manager_triggered(void)
{
    _tcpDialog->show();
}

void VMUMainWindow::on_actionQuick_Connect_triggered(void)
{
    _tcpDialog->quickConnect();
}

void VMUMainWindow::on_actionEnable_Low_Battery_toggled(bool arg1)
{
    _gyLog(GY_DEBUG_VERBOSE, "Low Battery Emulation: %s", arg1? "ON" : "OFF");
    gyVmuBatteryLowSet(_dev, arg1);
    gyVmuBatteryMonitorEnabledSet(_dev, arg1);
}

void VMUMainWindow::on_actionQuit_triggered(void)
{
    _saveSettings();
    QApplication::quit();
}

void VMUMainWindow::on_actionView_Log_triggered(void)
{
    gyVmuFlashPrintFilesystem(_dev);
    QDesktopServices::openUrl(QUrl::fromLocalFile(_workingDir->absoluteFilePath(VMU_LOG_FILE)));
}

bool VMUMainWindow::loadBios(QString filepath) {
    QFileInfo info(filepath);
    filepath = info.absoluteFilePath();

    if(gyloadbios(_dev, filepath.toStdString().c_str())) {
        resetCpu();
        _running = true;

        _statusBarChanged = true;

        return true;
    }

    return false;
}

void VMUMainWindow::on_actionLoad_BIOS_Image_triggered(void)
{
    QString file = QFileDialog::getOpenFileName(nullptr, "Browse BIOS file to open",
                                                VmuQtUtils::getRomDir(),
                                                "Binary Files(*.bin *.BIN)");

    if(file.isNull()) return;

    loadBios(file);
}

void VMUMainWindow::on_actionDisassemble_ROM_triggered(void)
{
    if(!_running || !_vmsHeader) {
        QMessageBox::critical(nullptr, "Failed to disassemble ROM", "You don't even have a ROM loaded to disassemble!");
        return;
    }

    char vmsDesc[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
    char fileName[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+5];
    gyVmuVmsHeaderVmuDescGet(_vmsHeader, vmsDesc);
    sprintf(fileName, "%s.ASM", vmsDesc);
    auto unicodeFile = VmuQtUtils::japCodec->toUnicode(fileName);
    bool lineNumbers = (QMessageBox::question(nullptr, "Disassemble ROM", "Include memory addresses in disassembly file ["+
                                              unicodeFile+"]?")
            == QMessageBox::Yes);
    gyVmuDisassemble((const unsigned char*)_dev->flash, VMU_FLASH_BLOCK_SIZE*8, unicodeFile.toStdString().c_str(), lineNumbers);
    QDesktopServices::openUrl(QUrl::fromLocalFile(unicodeFile));
}

void VMUMainWindow::on_actionEnable_Pixel_Ghosting_toggled(bool arg1)
{
    gyVmuDisplayGhostingEnabledSet(_dev, arg1);
    if(!arg1) {
        _gyLog(GY_DEBUG_VERBOSE, "Pixel Ghosting Disabled.");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "Pixel Ghosting Enabled.");
    }
}

void VMUMainWindow::on_actionPreserve_Aspect_Ratio_toggled(bool arg1)
{
    if(!arg1) {
        _gyLog(GY_DEBUG_VERBOSE, "LCD aspect ratio enabled.");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "LCD aspect ratio disabled.");
    }
    repaint();
}

void VMUMainWindow::_resizeWindowFromScreenZoom(int level) {
    _ui->graphicsView->setSceneRect(0, 0, VMU_FB_DEFAULT_WIDTH*level, VMU_FB_DEFAULT_HEIGHT*level);
    _updateScene();
    _ui->graphicsView->resize(VMU_FB_DEFAULT_WIDTH*level, VMU_FB_DEFAULT_HEIGHT*level);
    _ui->graphicsView->setMinimumSize(VMU_FB_DEFAULT_WIDTH*level, VMU_FB_DEFAULT_HEIGHT*level);
    _ui->graphicsView->adjustSize();

    centralWidget()->resize(_ui->graphicsView->size());
    //resize(_ui->graphicsView->size());
    centralWidget()->setMinimumSize(_ui->graphicsView->size());
    //setMinimumSize(sizeHint());
    centralWidget()->adjustSize();

    //centralWidget()->resize(centralWidget()->sizeHint());
    setMinimumSize(0, 0);
    //resize(frameSize());
    adjustSize();
    _ui->graphicsView->setMinimumSize(0, 0);
    centralWidget()->setMinimumSize(0, 0);
    setMinimumSize(0, 0);
}

void VMUMainWindow::on_action1x_triggered()
{
    _resizeWindowFromScreenZoom(1);
}

void VMUMainWindow::on_action2x_triggered()
{
    _resizeWindowFromScreenZoom(2);
}

void VMUMainWindow::on_action4x_triggered()
{
    _resizeWindowFromScreenZoom(4);
}

void VMUMainWindow::on_action8x_triggered()
{
    _resizeWindowFromScreenZoom(8);
}

void VMUMainWindow::on_action16x_triggered()
{
    _resizeWindowFromScreenZoom(16);
}

void VMUMainWindow::on_action32x_triggered()
{
    _resizeWindowFromScreenZoom(32);
}

void VMUMainWindow::on_action64x_triggered()
{
    _resizeWindowFromScreenZoom(64);
}

#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
void VMUMainWindow::on_actionSerial_COM_Connection_Manager_triggered()
{
    _comDialog->show();
}
#endif

void VMUMainWindow::on_actionHalt_toggled(bool arg1) {
    _halt = arg1;
    _ui->statusbar->showMessage(arg1? "Halting Execution." : "Resuming Execution.", VMU_STATUS_TEMP_TIMEOUT);
}

void VMUMainWindow::on_actionAdvance_Frame_triggered() {
    _advanceFrame = true;
    _ui->statusbar->showMessage("Advancing Frame.", VMU_STATUS_TEMP_TIMEOUT/4);
}

void VMUMainWindow::_addAnimFrame(const QPixmap &pixmap) {
    uint64_t deltaTicks;
    if(_pendingAnimFrame.isNull()) {
        _prevAnimFrameTicks = gyTimerTicks();
    } else {
        uint64_t curTicks = gyTimerTicks();
        deltaTicks = curTicks - _prevAnimFrameTicks;
        _prevAnimFrameTicks = curTicks;

        float deltaTicksF = (float)deltaTicks/1000000.0f;
        //_gyLog(GY_DEBUG_VERBOSE, "TICKS - %f", deltaTicksF);
        _animGif->addFrame(_pendingAnimFrame.toImage(), deltaTicksF);
    }

    _pendingAnimFrame = pixmap;
}

void VMUMainWindow::on_actionRecordAnim_toggled(bool arg1) {
    _recording = arg1;

    if(arg1) { //Begin recording
        delete _animGif;

        _gyLog(GY_DEBUG_VERBOSE, "Video Recording: CAPTURING.");
        _gyPush();

        _ui->statusbar->showMessage("Recording.", VMU_STATUS_TEMP_TIMEOUT);
        _recordingLabel->setPixmap(_iconRecording);

        _animGif = new QGifImage(QSize(VMU_DISP_PIXEL_WIDTH, VMU_DISP_PIXEL_HEIGHT));
        _animGif->setGlobalColorTable({
              qRgb(0, 0, 0),
              qRgb(255, 255, 255)
          }, Qt::white);
        //_animGif->setDefaultTransparentColor(Qt::white);
        _animGif->setDefaultDelay(100);
        /* Immediately add current frame to kick-off the animation.
         * (otherwise we're skipping the first frame). */
        _addAnimFrame(_fb);

        _ghostingWasEnabled = gyVmuDisplayGhostingEnabledGet(_dev);
        on_actionEnable_Pixel_Ghosting_toggled(false);

        _gyPop(1);
    } else { //Stop recording and render to GIF.


        _gyLog(GY_DEBUG_VERBOSE, "Video Recording: COMPLETE.");
        _gyPush();

        _ui->statusbar->showMessage("Recording Stopped.", VMU_STATUS_TEMP_TIMEOUT);
        _recordingLabel->setPixmap(QPixmap());

        //Flush animation queue
        _addAnimFrame(QPixmap());
#if 1
        QString filter = "Animated GIF (*.gif)";
        QString path = QFileDialog::getSaveFileName(nullptr,
                                                    "Specify Output Destination",
                                                    VmuQtUtils::getRomDir(),
                                                    filter,
                                                    &filter);

        if(path.isEmpty()) return;
#else
        //+10 digits + 4 ext + 1 NULL byte
        char filename[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+15];
        char vmsDesc[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
        int animNum = 0;

        gyVmuVmsHeaderVmuDescGet(_vmsHeader, vmsDesc);
        do {
            sprintf(filename, "%s_%d.gif", vmsDesc, ++animNum);
        } while(QFile::exists(filename));
#endif
        _gyLog(GY_DEBUG_VERBOSE, "Saving to Animated GIF: [%s]", Q_CSTR(path));
        _gyPush();

        if(!_animGif->save(path)) {
            _gyLog(GY_DEBUG_ERROR, "File could not be saved!");
            return;
        }
        else QDesktopServices::openUrl(QUrl::fromLocalFile(path));

        on_actionEnable_Pixel_Ghosting_toggled(_ghostingWasEnabled);
        _gyPop(2);
    }
}

bool VMUMainWindow::startTcpServer(unsigned port) {
    _tcpDialog->setHostPort(port);
    _tcpDialog->setAllowDcConnections(true);
    _tcpDialog->setAllowVmuConnections(true);
    _tcpDialog->stopTcpServer();
    return _tcpDialog->startTcpServer();
}
#include <gyro_wav_api.h>


void VMUMainWindow::on_actionBIOS_Runtime_Disassembly_triggered() {
#if 0
    GYWav* wav = gyAudWavCreate("/Users/falcogirgis/square.wav");
    GYAudioSource* src = gyAudSourceCreate();
    gyAudSourcePlayWav(src, wav);

#else

    char* buff = (char*)malloc((ROM_SIZE+1)*400);

        gyVmuBiosDisassemblyPrints(buff);

        QFile file(_workingDir->absoluteFilePath("bios_disassembly.txt"));
        if(file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << buff << endl;
        }

        free(buff);

#endif
}

void VMUMainWindow::on_actionBuzzer_Tool_triggered() {
    _buzzerDialog->show();
}

void VMUMainWindow::on_actionMemory_Browser_triggered(void)
{
    if(!_memBrowser) {
        _memBrowser = new VmuMemoryBrowserWidget(_dev, nullptr);
    }
    _memBrowser->show();
}

void VMUMainWindow::on_actionDocumentation_triggered(bool checked)
{
#ifdef QESTKUTILS_DOCUMENTATION_RESOURCES
    _helpDockWidget->show();
#endif
}

void VMUMainWindow::on_actionJet_Set_Radio_Graffiti_Tool_triggered() {
    if(!_jsrGraffitiTool) {
        _jsrGraffitiTool = new JetSetRadioGraffitiTool(this);
    }
    _jsrGraffitiTool->show();
}

void VMUMainWindow::on_actionFilesystem_triggered(void)
{
    delete _fileSysTableModel;
    _fileSysTableModel = new VmuFileSystemTableModel;
    _fileSysTableModel->setDevice(_dev);
    _fileSysTable->setModel(_fileSysTableModel);
    _fileSysTable->show();

}

void VMUMainWindow::on_actionExport_Flash_Image_triggered(void)
{
    QString filter = "Flash Image Files (*.bin)";
    QString path = QFileDialog::getSaveFileName(nullptr,
                                                "Choose Flash Image Export Location",
                                                VmuQtUtils::getRomDir(),
                                                filter,
                                                &filter);

    if(path.isEmpty()) return;

    gyVmuFlashExportImage(_dev, Q_CSTR(path));

}

void VMUMainWindow::on_actionNew_Flash_Image_triggered()
{
    int fileCount = gyVmuFlashFileCount(_dev);
    if(gyVmuFlashDirEntryGame(_dev)) --fileCount;

    if(fileCount) {
        auto resp = QMessageBox::question(nullptr, "Save Changes",
                            "Would you like to save changes made to the current flash image?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if(resp == QMessageBox::Cancel) return;

        if(resp == QMessageBox::Yes) {
            QString filter = "Flash Image Files (*bin)";
            QString path = QFileDialog::getSaveFileName(nullptr, "Choose Flash Image Export Location",
                                                        VmuQtUtils::getRomDir(),
                                                        filter,
                                                        &filter);

            if(path.isEmpty()) return;

            gyVmuFlashExportImage(_dev, Q_CSTR(path));
        }

        gyVmuFlashFormatDefault(_dev);
    }
}
