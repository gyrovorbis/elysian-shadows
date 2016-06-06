#include <QPainter>
#include <QGLWidget>
#include <QStatusBar>
#include <QDebug>
#include <QLabel>
#include <QSettings>
#include <QMimeData>

#include "gyro_vmu_vms.h"
#include "gyro_vmu_main_window.hpp"
#include "ui_gyro_vmu_main_window.h"
#include <gyro_vmu_display.h>
#include <gyro_wav_api.h>
#include <gyro_vmu_sfr.h>
#include <gyro_vmu_device.h>
#include <gyro_audio_api.h>
#include <gyro_vmu_flash.h>
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

static unsigned char icon[4][7] = {
  { 0x2a, 0x23, 0x47, 0xfb, 0x23, 0x23, 0x3f },
  { 0xff, 0x83, 0x93, 0xbb, 0x93, 0xbb, 0xff },
  { 0x7c, 0x82, 0x92, 0x9b, 0x82, 0x82, 0x7c },
  { 0x10, 0x28, 0xef, 0x6e, 0x6c, 0x7e, 0xc3 }
};

extern "C" void playSoundAL(int period, int pulseWidth, int db);

//#define VMS_IMAGE "/Users/falco/Desktop/VMU/Tetris.vms"
#define BIOS_IMAGE "/Users/falco/Documents/Dev/esdev/ElysianVMU/bin/DCVMSBIOS.bin"
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
    _fb(48, 41),
    _ui(new Ui::VMUMainWindow),
    _statusLabel(new QLabel("HALTED: No ROM Image")),
    _iconLabel(new QLabel())
{


    _ui->setupUi(this);
    setAcceptDrops(true);
    _iconLabel->setMargin(0);
    _ui->statusbar->layout()->setSpacing(0);
    _ui->statusbar->insertPermanentWidget(0, _statusLabel);
    _ui->statusbar->insertPermanentWidget(1, _iconLabel);
    _win = this;

    GYFileInitParams fileParams;
    strcpy(fileParams.workingDir, workingDir.isEmpty()? QDir::homePath().toStdString().c_str() : workingDir.toStdString().c_str());
    gyFileInit(&fileParams);

    if(workingDir.isEmpty()) {
        QDir outputDir(QDir::current());
        if(!outputDir.cd(VMU_OUTPUT_DIR) && !outputDir.mkdir(VMU_OUTPUT_DIR)) {
            qCritical() << QString("Could not set VMU working directory: [%s]") << QDir().current().filePath(VMU_OUTPUT_DIR);
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

    _gyLog(GY_DEBUG_VERBOSE, "Initializing ElysianVMU v 0.0.0.1a");
    _gyPush();

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

    _dev = gyVmuDeviceCreate();

#ifdef BIOS_IMAGE
     gyloadbios(_dev, BIOS_IMAGE);
     gyVmuCpuReset(_dev);
     _running = true;
#endif

#ifdef VMS_IMAGE
#ifdef GYRO_VMU
    gyVmuFlashLoadImage(_dev, VMS_IMAGE);
    gyVmuCpuReset(_dev);
#endif
#ifdef MARCUS_VMU
    do_vmsgame(VMS_IMAGE, nullptr);
    resetcpu();
#endif
    _running = true;
#endif

    _audioSrc = gyAudSourceCreate();
    _audioBuff = gyAudBufferCreate();
    GYVector3 pos = { 0.0f, 0.0f, 0.0f };
    gyAudListenerSetPos(&pos);
    gyAudSetReferenceDistance(0);
    gyAudSourceSetfv(_audioSrc, GY_SOURCE_POS, &pos);
    gyAudSourceSetfv(_audioSrc, GY_SOURCE_VEL, &pos);
    gyAudSourceSetf(_audioSrc, GY_SOURCE_ROLLOFF_FACTOR, 0.0f);
    gyAudSourceSetf(_audioSrc, GY_SOURCE_GAIN, 1.0f);
    gyAudSourceSetf(_audioSrc, GY_SOURCE_PITCH, 1.0f);
    gyAudSourceSeti(_audioSrc, GY_SOURCE_LOOPING, 1);

    _fb.fill(Qt::black);
    QPainter painter(&_fb);
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
    painter.drawImage(QPoint(0,0), QImage(":/icons/images/vmu_es.png"));


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
        QString("Version 0.0.1 Alpha<br>") +
        QString("<a href=\"http://www.elysianshadows.com\">elysianshadows.com</a><br>") +
        QString("</table><tr><td>Programming: </td><td> Falco Girgis</td></tr>") +
        QString("<tr><td>Graphics: </td><td>Patrick Kowalik</td></tr></table><br><br>") +
        QString("Special Thanks:<br> Marcus Comstedt, Lauren Scheeler, Marcel Girgis, Marcus Girgis, Andrew Girgis, Andrew Hill, and all of our awesome Kickstarter backers. :)</br</br>") +
        QString("</table><br><br>") +
        QString("Powered by LibGyro<br><br>") +
        QString("<i>ElysianVMU Copyright 2017 Elysian Shadows</i><br>") +
        QString("<i>Dreamcast and VMU Copyright Sega</i><br><br>"));
    });


    _loadSettings();

}

VMUMainWindow::~VMUMainWindow(void)
{
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing ElysianVMU");
    _gyPush();
    gyAudSourceDestroy(_audioSrc);
    gyAudBufferDestroy(_audioBuff);
    _freeIcons();
    gyVmuDeviceDestroy(_dev);
    gyInputUninit();
    gyAudUninit();
    gyTimerUninit();
    gySysUninit();
    _gyPop(1);
    gyDebugUninit();
    fclose(_logFile);
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

void VMUMainWindow::paintEvent(QPaintEvent *) {
    QPainter painter2(this);
    int y = 0;
#ifndef Q_OS_MAC //Menubar is not part of the window widget on a Mac.
    if(_ui->menubar->isVisible()) y = _ui->menubar->height();
#endif
    int height = geometry().height();
    if(_ui->statusbar->isVisible()) height -= _ui->statusbar->height();
    height -= y;
    painter2.drawPixmap(0, y, geometry().width(), height, _fb);
}

void VMUMainWindow::_update(void) {

    GYKeyboard *kbd;
    GYController* cont;

    gyKeyboardPoll(&kbd);
    bool hasCont = gyControllerPoll(0, &cont);

    if(hasCont && !_contAttached) {
        _ui->statusbar->showMessage("Controller detected!", VMU_STATUS_TEMP_TIMEOUT);
    } else if(!hasCont && _contAttached) {
        _ui->statusbar->showMessage("Controller removed!", VMU_STATUS_TEMP_TIMEOUT);
    }
    _contAttached = hasCont;

    if(!_running) return;

    auto ticks = gyTimerTicks();
    auto deltaTicks = ticks-_prevTicks;
    _prevTicks = ticks;

    //Keyboard input
    int u   = kbd->keys[GY_KEY_DPAD_U];
    int d   = kbd->keys[GY_KEY_DPAD_D];
    int l   = kbd->keys[GY_KEY_DPAD_L];
    int r   = kbd->keys[GY_KEY_DPAD_R];
    int a   = kbd->keys[GY_KEY_A];
    int b   = kbd->keys[GY_KEY_B];
    int m   = kbd->keys[GY_KEY_M];
    int s   = kbd->keys[GY_KEY_S];
    int lt  = kbd->keys[GY_KEY_L];
    int rt  = kbd->keys[GY_KEY_R];


    //Combine with Controller input
    if(hasCont && cont) {
        //Combine Dpad input with Analog input
        u   |= cont->buttons[GY_CONTROLLER_BUT_DPAD_UP]       | cont->buttons[GY_CONTROLLER_BUT_ANALOG1_UP];
        d   |= cont->buttons[GY_CONTROLLER_BUT_DPAD_DOWN]     | cont->buttons[GY_CONTROLLER_BUT_ANALOG1_DOWN];
        l   |= cont->buttons[GY_CONTROLLER_BUT_DPAD_LEFT]     | cont->buttons[GY_CONTROLLER_BUT_ANALOG1_LEFT];
        r   |= cont->buttons[GY_CONTROLLER_BUT_DPAD_RIGHT]    | cont->buttons[GY_CONTROLLER_BUT_ANALOG1_RIGHT];
        a   |= cont->buttons[GY_CONTROLLER_BUT_CIRCLE];
        b   |= cont->buttons[GY_CONTROLLER_BUT_X];
        m   |= cont->buttons[GY_CONTROLLER_BUT_SELECT];
        s   |= cont->buttons[GY_CONTROLLER_BUT_START];
        lt  |= cont->buttons[GY_CONTROLLER_BUT_LSHOULDER];
        rt  |= cont->buttons[GY_CONTROLLER_BUT_RSHOULDER];
    }

    _timeScalar = 1.0f;
    if(lt&GY_BUT_FLAGS_IS_PRESSED) _timeScalar /= 10.0f;
    if(rt&GY_BUT_FLAGS_IS_PRESSED) _timeScalar *= 10.0f;

#ifdef GYRO_VMU

    if(u == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_UP, 1);
    else if(u == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_UP, 0);
    if(d == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_DOWN, 1);
    else if(d == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_DOWN, 0);
    if(l == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_LEFT, 1);
    else if(l == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_LEFT, 0);
    if(r == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_RIGHT, 1);
    else if(r == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_RIGHT, 0);

    if(a == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_A, 1);
    else if(a == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_A, 0);
    if(b == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_B, 1);
    else if(b == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_B, 0);
    if(m == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_MODE, 1);
    else if(m == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_MODE, 0);
    if(s == GY_BUT_STATE_TAPPED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_SLEEP, 1);
    else if(s == GY_BUT_STATE_RELEASED) gyVmuButtonStateSet(_dev, VMU_GAMEPAD_SLEEP, 0);

#endif

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

      for(int i = 0; i < 1; ++i) {
#ifdef GYRO_VMU
#ifndef VMU_DEBUG
            gyVmuCpuTick(_dev, _timeScalar*(float)deltaTicks/1000000000.0f);
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


     _updateIcon();
     lcdrefresh();
}



extern "C" void lcdrefresh(void) {
    QPainter painter(&_win->_fb);


#ifdef GYRO_VMU
    unsigned char *sfr;
    unsigned char (*xram)[0x80];
    sfr = _win->_dev->sfr;
    xram = _win->_dev->xram;

#endif

      int y, x, i, a, b=0, p=0;

    p = sfr[0x22];
    if(p>=0x83)
        p -= 0x83;
    b = (p>>6);
    p = (p&0x3f)*2;
    for(y=0; y<32; y++) {
        for(x=0; x<48; ) {
            unsigned value = xram[b][p++];
            //if(!lcdon)
             //   value = 0;

            for(int i = 7; i >= 0; --i) {
                if((value>>(unsigned)i)&0x1) {
                    painter.fillRect(x, y, 1, 1, QBrush(Qt::black));
                } else {
                    painter.fillRect(x, y, 1, 1, QBrush(Qt::white));
                }
                x++;
            }

            if((p&0xf)>=12)
                p+=4;
            if(p>=128) {
                b++;
                p-=128;
            }
            if(b==2 && p>=6) {
                b = 0;
                p -= 6;
            }
        }
    }


    p++;
    if((p&0xf)>=12)
        p+=4;
    if(p>=128) {
        b++;
        p-=128;
    }
    if(b==2 && p>=6) {
        b = 0;
        p -= 6;
    }
    for(i=0; i<4; i++) {
        a = xram[b][p++]>>(2*(3-i));
        //if(!lcdon)
          //  a = 0;
        for(y=0; y<7; y++) {
            int value = icon[i][y];
            for(x=0; x<7; x++) {
                if((value>>(7-x))&a) {
                    painter.fillRect((2+i*12+x)*1, (y+33)*1, 1, 1, QBrush(Qt::black));
                } else {
                    painter.fillRect((2+i*12+x)*1, (y+33)*1, 1, 1, QBrush(Qt::white));
                }
            }
        }
        if((p&0xf)>=12)
            p+=4;
        if(p>=128) {
            b++;
            p-=128;
        }
        if(b==2 && p>=6) {
            b = 0;
            p -= 6;
        }


    }

    lcd_updated = 0;

    _win->repaint();
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
        gyVmuCpuReset(_dev);
        //_statusLabel->setText(QString("RUNNING: ")+filepath.split("/").last());
        char string[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
        gyVmuVmsHeaderVmuDescGet(_vmsHeader, string);
        _statusLabel->setText(QString("RUNNING: ")+QString(string));
        _loadIcons();
        _gyLog(GY_DEBUG_VERBOSE, "Running VMU CPU.");
        _running = true;
        _pushRecentRom(filepath);
        return true;
    }
    _gyLog(GY_DEBUG_ERROR, "Error Loading File! [%s]", info.absoluteFilePath().toStdString().c_str());
    return false;

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

        lcdrefresh();

    }

    _gyPop(1);
}

extern "C" void playSoundAL(int period, int pulseWidth, int /*db*/) {
    if(!_win->_ui->actionEmulate_Audio->isChecked()) return;

    if(period > 0) {

        if(pulseWidth > 0 && pulseWidth <= period) {
            float   periodSample  = 0.000183f;
            float   freqSample    = roundf(1.0f/periodSample);
            int     freqSamplei   = freqSample;
            //float   freqSound     = 1.0f/(0.000183f*(float)period);
            float   periodSecs    = period*periodSample;
            float   samples       = freqSample*periodSecs;
            int     sampleSize    = roundf(samples);
            float   invDutyCycle  = (float)pulseWidth/(float)period;
            int     activeCycle   = roundf(invDutyCycle*(float)sampleSize);

            unsigned char buff[sampleSize];


            memset(buff, 0x7f, activeCycle);
            memset(&buff[activeCycle], 0xff, sampleSize-activeCycle);

            gyAudSourceStop(_win->_audioSrc);
            alSourcei(*(ALuint*)_win->_audioSrc, AL_BUFFER, 0);
            gyAudBufferData(_win->_audioBuff, buff, AL_FORMAT_MONO8, sampleSize, freqSamplei);
            gyAudSourcePlayBuffer(_win->_audioSrc, _win->_audioBuff);
        } else {
            //SHIT HAS REESTED
        }


    } else {
        gyAudSourceStop(_win->_audioSrc);

    }
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
#ifdef VMU_DEBUG
    return;
#endif
    fprintf(_logFile, "%s%s%s\n", tabBuff, prefix[level], msg);
    fflush(_logFile);
}

void VMUMainWindow::on_actionReset_triggered(void)
{
    _ui->statusbar->showMessage("CPU has been reset.", VMU_STATUS_TEMP_TIMEOUT);
    gyVmuCpuReset(_dev);
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

    for(int i = 0; i < _ui->menuLoad_Recent->actions().size(); ++i) {
        sprintf(buffer, "recentRoms%d", i);
        settings.setValue(buffer, _ui->menuLoad_Recent->actions()[i]->text());
    }

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

        for(int i = VMU_ROM_HISTORY_SIZE-1; i >= 0; --i) {
            sprintf(buffer, "recentRoms%d", i);
            auto recentRom = settings.value(buffer, "").toString();
            if(!recentRom.isEmpty()) _pushRecentRom(recentRom);
        }
    } else {
        _gyLog(GY_DEBUG_WARNING, "No settings found! Using defaults!");
    }

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
    if(!_iconImages.size()) return;

    _gyLog(GY_DEBUG_VERBOSE, "Dumping VMS Icons.");
    _gyPush();

    gyVmuVmsHeaderVmuDescGet(_vmsHeader, vmsDesc);

    for(int i = 0; i < _iconImages.size(); ++i) {
        sprintf(filename, "%s%d.png", vmsDesc, i);
        if(!_iconImages[i].save(filename))
            _gyLog(GY_DEBUG_ERROR, "Could not dump icon to file [%s]!", QDir::current().absoluteFilePath(filename).toStdString().c_str());
        else
            _gyLog(GY_DEBUG_VERBOSE, "Dumped icon to file [%s].", QDir::current().absoluteFilePath(filename).toStdString().c_str());
    }

    _gyPop(1);
}
