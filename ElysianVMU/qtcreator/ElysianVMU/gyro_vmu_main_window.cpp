#include <QPainter>
#include <QGLWidget>
#include <QStatusBar>
#include <QDebug>
#include <QLabel>
#include <QSettings>
#include <QMimeData>
#include <QTextCodec>
#include <QDesktopServices>

#define PIXEL_GHOST_FRAMES 10

#include "gyro_vmu_version.h"
#include "gyro_vmu_vms.h"
#include "gyro_vmu_vmi.h"
#include "gyro_vmu_main_window.hpp"
#include "gyro_vmu_tcp_dialog.hpp"
#include "gyro_vmu_com_dialog.hpp"
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

//#define VMS_IMAGE "/Users/falco/Desktop/VMU/Tetris.vms"
#ifdef VMU_DEBUG
//#define BIOS_IMAGE "/Users/falcogirgis/Documents/Dev/esdev/ElysianVMU/bin/vmu_bios.bin"
#endif
#define GYRO_VMU
#define VMU_STATUS_TEMP_TIMEOUT 2000
#define VMU_LOG_FILE "vmu_debug.txt"
#define SCREENSHOT_FILENAME "screenshot"
#define SCREENSHOT_FILENAME_EXT "png"
#define VMU_OUTPUT_DIR          "ElysianVMU"
#define SETTINGS_FILE           "UserSettings.ini"
#define SCREENSHOT_FILENAME_SIZE_MAX    100
//#define MARCUS_VMU
#define CYCLES      1000
#define AUDIO_SAMPLE_RATE   44100
#define STATE_FILE "elysianVMUSaveState.bin"
extern "C" void lcdrefresh(void);
extern "C" int loadbios(char *filename);
extern "C" void run_cpu();
extern "C" void resetcpu();
extern "C" void keypress(int i);
extern "C" void keyrelease(int i);
extern "C" int do_vmsgame(char *filename, char *biosname);
extern unsigned char sfr[0x100];
extern unsigned char xram[3][0x80];
extern unsigned char ram[2][0x100];
extern unsigned char wram[0x200];
extern unsigned char flash[0x20000];
extern int pc;
extern int lcd_updated;

extern "C" int gyloadflash(VMUDevice* dev, const char *filename);

static VMUMainWindow* _win = nullptr;


int compareCores(VMUDevice* dev) {
    //have to sync reset clock values


    //compare instruction
    if(dev->pc != pc) {
        Q_ASSERT(0);
        //return 0;
    }

    for(unsigned i = 0; i < RAM_SFR_SIZE; ++i)
        Q_ASSERT(dev->sfr[i] == sfr[i]);

    //compare memory
    for(unsigned j = 0; j < RAM_BANK_COUNT; ++j)
        for(unsigned i = 0; i < RAM_BANK_SIZE; ++i) {
            Q_ASSERT(dev->ram[j][i] == ram[j][i]);
        }

    for(unsigned j = 0; j < XRAM_BANK_COUNT; ++j)
        for(unsigned i = 0; i < XRAM_BANK_SIZE; ++i) {
            Q_ASSERT(dev->xram[j][i] == xram[j][i]);
        }

    for(unsigned i = 0; i < WRAM_SIZE; ++i) Q_ASSERT(dev->wram[i] == wram[i]);

    for(unsigned i = 0; i < FLASH_SIZE; ++i) Q_ASSERT(dev->flash[i] == flash[i]);

    //Q_ASSERT();
    //compare timers

    return 1;

}

FILE* VMUMainWindow::_logFile = nullptr;
int VMUMainWindow::_logDepth = 0;

VMUMainWindow::VMUMainWindow(QWidget *parent, QString workingDir) :
    QMainWindow(parent),
    _fb(VMU_FB_DEFAULT_WIDTH, VMU_FB_DEFAULT_HEIGHT),
    _ui(new Ui::VMUMainWindow),
    _iconES(ICON_ES_PATH),
    _iconClock(ICON_CLOCK_PATH),
    _iconFile(ICON_FILE_PATH),
    _iconGame(ICON_GAME_PATH),
    _iconFlash(ICON_FLASH_PATH),
    _iconDisconnected(ICON_DISCONNECTED_PATH),
    _iconListening(ICON_LISTENING_PATH),
    _iconConnected(ICON_CONNECTED_PATH),
    _tcpDialog(new VmuTcpDialog),
    _comDialog(new VmuComDialog),
    _statusLabel(new QLabel("HALTED: No ROM Image")),
    _iconLabel(new QLabel()),
    _japCodec(QTextCodec::codecForName(VMU_TEXT_CODEC))
{
    _ui->setupUi(this);
    _fbLabel = new QLabel;


   // gridLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));

  //  gridLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));

    QVBoxLayout* vertLayout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    Q_ASSERT(vertLayout);
    //vertLayout->addLayout(gridLayout);
    vertLayout->addWidget(_fbLabel);
    vertLayout->setAlignment(_fbLabel, Qt::AlignHCenter);
    _fbLabel->setPixmap(_fb);
    _fbLabel->setMargin(0);
    _fbLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    vertLayout->setMargin(0);
    vertLayout->setSpacing(0);

    setAcceptDrops(true);
    _iconLabel->setMargin(0);
    _ui->statusbar->layout()->setSpacing(0);
    _ui->statusbar->insertPermanentWidget(0, _statusLabel);
    _ui->statusbar->insertPermanentWidget(1, _iconLabel);
    setWindowTitle(QString("ElysianVMU v ")+QString(GYRO_VMU_VERSION_STRING));
    _win = this;

    GYFileInitParams fileParams;
    strcpy(fileParams.workingDir, workingDir.isEmpty()?
               QDir::homePath().toStdString().c_str() : workingDir.toStdString().c_str());
    gyFileInit(&fileParams);

    if(workingDir.isEmpty()) {
        QDir outputDir(QDir::current());
        if(!outputDir.cd(VMU_OUTPUT_DIR) && !outputDir.mkdir(VMU_OUTPUT_DIR)) {
            qCritical() << QString("Could not set VMU working directory: [%s]")
                        << QDir().current().filePath(VMU_OUTPUT_DIR);
        } else {
            QDir::setCurrent(QDir::current().filePath(VMU_OUTPUT_DIR));
        }
    }

    _logFile = fopen(VMU_LOG_FILE, "w");


    GYDebugCallbacks dbgCallbacks = {
        &_log,
        &_push,
        &_pop
    };
    gySysDebugEnable(GY_DEBUG_VERBOSE, &dbgCallbacks);

    _gyLog(GY_DEBUG_VERBOSE, "Initializing ElysianVMU v %s", GYRO_VMU_VERSION_STRING);
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "Qt Library Versions");
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "Compile-Time: %s",    QT_VERSION_STR);
    _gyLog(GY_DEBUG_VERBOSE, "Run-Time: %s",        qVersion());
    _gyPop(1);

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
    GYNetInitParams netParams = {
        2 //maximum number of monitored sockets allowed
    };
    gyNetInit(&netParams);

    _dev = gyVmuDeviceCreate();
    _tcpDialog->setVmuDevice(_dev);
    _comDialog->setVmuDevice(_dev);

#ifdef BIOS_IMAGE
     gyloadbios(_dev, BIOS_IMAGE);
     resetCpu();
     _running = true;
#endif

#ifdef VMS_IMAGE
#ifdef GYRO_VMU
    gyVmuFlashLoadImage(_dev, VMS_IMAGE);
    resetCpu();
#endif
#ifdef MARCUS_VMU
    do_vmsgame(VMS_IMAGE, nullptr);
    resetcpu();
#endif
    _running = true;
#endif

    _fb.fill(Qt::black);
    QPainter painter(&_fb);
#if 0
    for(int i=0; i<4; i++) {
        for(int y=0; y<7; y++) {
            int value = icon[i][y];
            for(int x=0; x<7; x++) {
                int a = i==0? 1 : 0;
                if((value>>(7-x))&a) {
                    painter.fillRect((2+i*12+x)*1, (y+33)*1, 1, 1, QBrush(Qt::black));
                } else {
                    painter.fillRect((2+i*12+x)*1, (y+33)*1, 1, 1, QBrush(Qt::white));
                }
            }
        }
    }
#endif
    painter.drawPixmap(QPoint(0,0), _iconES);

    _updateTimer.setInterval(17);
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(_update()));
    _updateTimer.start();

    auto* exitFs = new QAction(this);
    exitFs->setShortcut(Qt::Key_Escape);
    connect(exitFs, &QAction::triggered, this, [&](void) {
        if(windowState()&Qt::WindowFullScreen)
            showNormal();

    });
    addAction(exitFs);
    _gyPop(1);

    connect(_ui->actionAbout, &QAction::triggered, [this]{
        QMessageBox::about(this, "About ElysianVMU",
        QString("ElysianVMU is a Softare emulator for the Sega Dreamcast Visual Memory Unit.<br><br>")+
        QString("Version ")+QString(GYRO_VMU_VERSION_STRING)+QString("<br>") +
        QString("<a href=\"http://www.elysianshadows.com\">elysianshadows.com</a><br>") +
        QString("</table><tr><td>Software: </td><td> Falco Girgis</td></tr>") +
        QString("<tr><td>Hardware: </td><td>Tulio Goncalves</td></tr>") +
        QString("<tr><td>Graphics: </td><td>Patrick Kowalik</td></tr></table><br><br>") +
        QString("Special Thanks:<br> Deunan Knute, Marcus Comstedt, Lauren Girgis, Marcel Girgis, Marcus Girgis, Andrew Girgis, Andrew Hill, and all of our awesome Kickstarter backers. :)</br</br>") +
        QString("</table><br><br>") +
        QString("Powered by LibGyro (version ")+QString(GYRO_LIB_VERSION)+QString(")<br><br>") +
        QString("<i>ElysianVMU Copyright 2017 Elysian Shadows</i><br>") +
        QString("<i>Dreamcast and VMU Copyright Sega</i><br><br>"));
    });

    _resizeWindowFromScreenZoom(8);
    _loadSettings();

}

VMUMainWindow::~VMUMainWindow(void)
{
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing ElysianVMU");
    _gyPush();
    _freeIcons();
    gyVmuDeviceDestroy(_dev);
    gyNetUninit();
    gyInputUninit();
    gyAudUninit();
    gyTimerUninit();
    gySysUninit();
    _gyPop(1);
    gyDebugUninit();
    fclose(_logFile);
    delete _comDialog;
    delete _tcpDialog;
    delete _ui;
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


void VMUMainWindow::_update(void) {
    bool ranThisFrame = false;

    if(!_running) return;

    auto ticks = gyTimerTicks();
    auto deltaTicks = ticks-_prevTicks;


#ifdef MARCUS_VMU
    static int cycles = 0;


#if 1

    if(u == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_UP);
    else if(u == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_UP);
    if(d == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_DOWN);
    else if(d == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_DOWN);
    if(l == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_LEFT);
    else if(l == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_LEFT);
    if(r == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_RIGHT);
    else if(r == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_RIGHT);

    if(a == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_A);
    else if(a == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_A);
    if(b == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_B);
    else if(b == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_B);
    if(m == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_MODE);
    else if(m == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_MODE);
    if(s == GY_BUT_STATE_TAPPED) keypress(VMU_GAMEPAD_SLEEP);
    else if(s == GY_BUT_STATE_RELEASED) keyrelease(VMU_GAMEPAD_SLEEP);

#endif


#endif

    //store socket status before update
#if 0
    auto* prevSocket = _dev->serialPort._tcpSocket;
#endif

      for(int i = 0; i < 1; ++i) {
#ifdef GYRO_VMU
#ifndef VMU_DEBUG
        ranThisFrame = gyVmuDeviceUpdate(_dev, (float)deltaTicks/1000000000.0f);
#else
        gyVmuCpuTick(_dev, 1.0f/30.0f);
#endif
#endif
#ifdef MARCUS_VMU
          run_cpu();
#endif
#if defined(GYRO_VMU)&&defined(MARCUS_VMU)

      compareCores(_dev);

#endif
      }

    if(ranThisFrame) {
         _prevTicks = gyTimerTicks();

        if(_contAttached && !_dev->gamepad.contAttached) {
            _ui->statusbar->showMessage("Controller Disconnected!", VMU_STATUS_TEMP_TIMEOUT);
        } else if(!_contAttached && _dev->gamepad.contAttached) {
            _ui->statusbar->showMessage("Controller Connected!", VMU_STATUS_TEMP_TIMEOUT);
        }
        _contAttached = (bool)_dev->gamepad.contAttached;

#if 0
        //Check if the TCP connection was changed from outside (ie: disconnected)
        if(prevSocket != _dev->serialPort._tcpSocket) {
            _tcpDialog->updateConnectionStatus();
        }
#endif
        _updatePConRegister();
        _updateIcon();
//        lcdrefresh();
        repaint();
        //if(gyVmuDisplayUpdate(_dev, (float)deltaTicks/1000000000.0f))
       // if(_dev->display.xramChanged) {
       //     _dev->display.xramChanged = 0;
           // lcdrefresh();
        //}
    }
}


void VMUMainWindow::paintEvent(QPaintEvent*) {
    //Update framebuffer texture based on VMU LCD screen state
    {
        QPainter painter(&_fb);
        if(!gyVmuDisplayEnabled(_win->_dev)) {
            painter.drawPixmap(QPoint(0,0), _win->_iconES);
        } else if(gyVmuDisplayUpdateEnabled(_win->_dev)) {
            for(int y = 0; y < VMU_DISP_PIXEL_HEIGHT; ++y) {
                for(int x = 0; x < VMU_DISP_PIXEL_WIDTH; ++x) {
                    int col = gyVmuDisplayPixelGhostValue(_dev, x, y);
                    painter.fillRect(x, y, 1, 1, QBrush(QColor(col, col, col, 255)));
                }
            }
        }
    }

    //Resize framebuffer to the viewport size to produce final image
    int width = centralWidget()->width();
    int height = centralWidget()->height();
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
#ifndef Q_OS_MAC //Add menubar offset to icon's y
      //  if(_ui->menubar->isVisible()) iconY += _ui->menubar->height();
#endif
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
    _fbLabel->setPixmap(scaledFb);

}

bool VMUMainWindow::loadRom(QString filepath) {
    if(filepath.isEmpty()) return false;

    QFileInfo info(filepath);
    _romDir = info.absoluteDir().absolutePath();

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

    if(info.exists() && gyVmuFlashLoadImage(_dev, filepath.toStdString().c_str())) {
        _vmsHeader = (VMSFileInfo*)&_dev->flash[VMU_FLASH_GAME_VMS_HEADER_OFFSET];

        resetCpu();

        //_statusLabel->setText(QString("RUNNING: ")+filepath.split("/").last());
        char string[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
        gyVmuVmsHeaderVmuDescGet(_vmsHeader, string);
        _statusLabel->setText(QString("RUNNING: ")+_japCodec->toUnicode(string));
        _loadIcons();
        _gyLog(GY_DEBUG_VERBOSE, "Running VMU CPU.");
        _running = true;
        _pushRecentRom(filepath);
        return true;
    }
    _gyLog(GY_DEBUG_ERROR, "Error Loading File! [%s]", info.absoluteFilePath().toStdString().c_str());
    return false;
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

void VMUMainWindow::on_actionLoad_ROM_triggered(void)
{
    loadRom(QFileDialog::getOpenFileName(nullptr, "Browse VMS ROM to Open", _romDir,
                                         "VMS Files(*.vms *.VMS);;Web Browser Files(*.vmi *.VMI)"));
}

void VMUMainWindow::on_actionSave_State_triggered(void)
{
    if(!_running) return;
    _gyLog(GY_DEBUG_VERBOSE, "Saving state [%s].", QDir::current().absoluteFilePath(STATE_FILE).toStdString().c_str());
    _gyPush();

    FILE* file = fopen(STATE_FILE, "wb");
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

    _gyLog(GY_DEBUG_VERBOSE, "Loading state [%s].", QDir::current().absoluteFilePath(STATE_FILE).toStdString().c_str());
    _gyPush();

    FILE* file = fopen(STATE_FILE, "rb");
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

    for(int i = 0; i < _logDepth; ++i) {
        strcat(tabBuff, "\t");
    }

    static const char* prefix[3] = {
        "",
        "! - ",
        "X - "
    };
    qDebug() << tabBuff << prefix[level] << msg;
#if 0
    char buffer[10000] = {0};
    strcat(buffer, tabBuff);
    strcat(buffer, prefix[level]);
    strcat(buffer, msg);
    strcat(buffer, "\n");
    printf(buffer);
#endif
#ifdef VMU_DEBUG
    return;
#endif
#if FILTER_DUPLICATE_LOG_STRINGS
    static QString previous;
    if(previous == msg) return;
    previous = msg;
#endif
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
        sprintf(buffer, "%s%d.%s", SCREENSHOT_FILENAME, ++number, SCREENSHOT_FILENAME_EXT);
    } while(QFileInfo(buffer).exists());

    QFile file(buffer);
    file.open(QIODevice::WriteOnly);
    _gyLog(GY_DEBUG_VERBOSE, "Saving screenshot[%s]", QDir::current().absoluteFilePath(buffer).toStdString().c_str());
    _ui->statusbar->showMessage("Screenshot saved.", VMU_STATUS_TEMP_TIMEOUT);
    _fb.save(&file, SCREENSHOT_FILENAME_EXT);

}

void VMUMainWindow::_saveSettings(void) {
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    char buffer[256];

    _gyLog(GY_DEBUG_VERBOSE, "Saving User Settings [%s].", settings.fileName().toStdString().c_str());

    settings.setValue("settings",   true);
    settings.setValue("geometry",   saveGeometry());
    settings.setValue("state",      saveState());
    settings.setValue("romDir",     _romDir);
    settings.setValue("audio",      _ui->actionEmulate_Audio->isChecked());
    settings.setValue("ghosting",   _ui->actionEnable_Pixel_Ghosting->isChecked());
    settings.setValue("lowBattery", _ui->actionEnable_Low_Battery->isChecked());
    settings.setValue("aspectRatio", _ui->actionPreserve_Aspect_Ratio->isChecked());

    for(int i = 0; i < _ui->menuLoad_Recent->actions().size(); ++i) {
        sprintf(buffer, "recentRoms%d", i);
        settings.setValue(buffer, _ui->menuLoad_Recent->actions()[i]->text());
    }

    _tcpDialog->saveSettings(settings);
    _comDialog->saveSettings(settings);

}

void VMUMainWindow::_loadSettings(void) {
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    char buffer[256];

    _gyLog(GY_DEBUG_VERBOSE, "Loading User Settings [%s].", settings.fileName().toStdString().c_str());
    _gyPush();
    bool saved = settings.value("settings", false).toBool();

    if(saved) {
        restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
        restoreState(settings.value("state", QByteArray()).toByteArray());
        _romDir = settings.value("romDir", "").toString();
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
    _comDialog->loadSettings(settings);

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
    auto* mime = event->mimeData();
    if(mime->hasUrls()) {
        for(auto&& url : mime->urls()) {
            if(url.isLocalFile()) {
                QFileInfo info(url.toLocalFile());
                if(info.suffix() == "VMS" || info.suffix() == "vms") {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
    QMainWindow::dragEnterEvent(event);
}

void VMUMainWindow::dropEvent(QDropEvent* event) {
    auto* mime = event->mimeData();
    if(mime->hasUrls()) {
        for(auto&& url : mime->urls()) {
            if(url.isLocalFile()) {
                QFileInfo info(url.toLocalFile());
                if(info.suffix() == "VMS" || info.suffix() == "vms") {
                    event->acceptProposedAction();
                    loadRom(info.absoluteFilePath());
                    return;
                }
            }
        }
    }
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

    _iconsData    = gyVmuVMSFileInfoCreateIconsARGB444(_vmsHeader);

    if(_iconsData) {
        for(unsigned i = 0; i < _vmsHeader->iconCount; ++i) {
            _gyLog(GY_DEBUG_VERBOSE, "Creating texture for icon %d.", i);
            _iconImages.push_back(QImage((uchar*)_iconsData[i], VMU_VMS_ICON_BITMAP_WIDTH, VMU_VMS_ICON_BITMAP_HEIGHT, QImage::Format_ARGB4444_Premultiplied));
            _iconPixmaps.push_back(QPixmap::fromImage(_iconImages[i]).scaled(21, 21, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        _iconLabel->setPixmap(_iconPixmaps[0]);
        hasIcons = true;
    }

    _eyecatchData = gyVmuVMSFileInfoCreateEyeCatchARGB444(_vmsHeader);

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
        sprintf(filename, "%s%d.png", vmsDesc, i);
        if(!_iconImages[i].save(_japCodec->toUnicode(filename).toStdString().c_str()))
            _gyLog(GY_DEBUG_ERROR, "Could not dump icon to file [%s]!", QDir::current().absoluteFilePath(filename).toStdString().c_str());
        else
            _gyLog(GY_DEBUG_VERBOSE, "Dumped icon to file [%s].", QDir::current().absoluteFilePath(filename).toStdString().c_str());
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
    if(!_eyecatchImage.save(_japCodec->toUnicode(filename).toStdString().c_str()))
        _gyLog(GY_DEBUG_ERROR, "Could not dump eyecatch to file [%s]!", QDir::current().absoluteFilePath(filename).toStdString().c_str());
    else
        _gyLog(GY_DEBUG_VERBOSE, "Dumped eyecatch to file [%s].", QDir::current().absoluteFilePath(filename).toStdString().c_str());

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
    QApplication::quit();
}

void VMUMainWindow::on_actionView_Log_triggered(void)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::current().absoluteFilePath(VMU_LOG_FILE)));
}

void VMUMainWindow::on_actionLoad_BIOS_Image_triggered(void)
{
    QString file = QFileDialog::getOpenFileName(nullptr, "Browse BIOS file to open", _romDir,
                                             "Binary Files(*.bin *.BIN)");

    if(file.isNull()) return;

    QFileInfo info(file);
    file = info.absoluteFilePath();

    if(gyloadbios(_dev, file.toStdString().c_str())) {
        resetCpu();
        _running = true;
    }

}

void VMUMainWindow::on_actionDisassemble_ROM_triggered(void)
{
    if(!_running) {
        QMessageBox::critical(nullptr, "Failed to disassemble ROM", "You don't even have a ROM loaded to disassemble!");
        return;
    }

    char vmsDesc[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
    char fileName[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+5];
    gyVmuVmsHeaderVmuDescGet(_vmsHeader, vmsDesc);
    sprintf(fileName, "%s.ASM", vmsDesc);
    auto unicodeFile = _japCodec->toUnicode(fileName);
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
    _fbLabel->resize(VMU_FB_DEFAULT_WIDTH*level, VMU_FB_DEFAULT_HEIGHT*level);
    _fbLabel->setMinimumSize(VMU_FB_DEFAULT_WIDTH*level, VMU_FB_DEFAULT_HEIGHT*level);
    adjustSize();
    _fbLabel->setMinimumSize(0, 0);
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

void VMUMainWindow::on_actionSerial_COM_Connection_Manager_triggered()
{
    _comDialog->show();
}
