/********************************************************************************
** Form generated from reading UI file 'gyro_vmu_main_window.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GYRO_VMU_MAIN_WINDOW_H
#define UI_GYRO_VMU_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VMUMainWindow
{
public:
    QAction *actionLoad_ROM;
    QAction *actionSave_State;
    QAction *actionLoad_State;
    QAction *actionAbout;
    QAction *actionReset;
    QAction *actionScreenshot;
    QAction *actionEmulate_Audio;
    QAction *actionDump_Icons;
    QAction *actionDump_Eyecatch;
    QAction *actionEnable_Low_Battery;
    QAction *actionQuick_Connect;
    QAction *actionTCP_Connection_Manager;
    QAction *actionConfigure_Input_Devices;
    QAction *actionQuit;
    QAction *actionView_Log;
    QAction *actionLoad_BIOS_Image;
    QAction *actionDisassemble_ROM;
    QAction *actionEnable_Pixel_Ghosting;
    QAction *action1x;
    QAction *action2x;
    QAction *action4x;
    QAction *action8x;
    QAction *action16x;
    QAction *action32x;
    QAction *action64x;
    QAction *actionPreserve_Aspect_Ratio;
    QAction *actionSerial_COM_Connection_Manager;
    QAction *actionHalt;
    QAction *actionAdvance_Frame;
    QAction *actionRecordAnim;
    QAction *actionSave_Game;
    QAction *actionLoad_Game;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuLoad_Recent;
    QMenu *menuEmulation;
    QMenu *menuSerial_Communications;
    QMenu *menuPlayback;
    QMenu *menuTools;
    QMenu *menuScreen_Capture;
    QMenu *menuHelp;
    QMenu *menuView;
    QMenu *menuPixel_Perfect_Aspect_Zoom;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *VMUMainWindow)
    {
        if (VMUMainWindow->objectName().isEmpty())
            VMUMainWindow->setObjectName(QStringLiteral("VMUMainWindow"));
        VMUMainWindow->resize(320, 240);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/images/vmu_icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        VMUMainWindow->setWindowIcon(icon);
        VMUMainWindow->setLayoutDirection(Qt::LeftToRight);
        actionLoad_ROM = new QAction(VMUMainWindow);
        actionLoad_ROM->setObjectName(QStringLiteral("actionLoad_ROM"));
        actionSave_State = new QAction(VMUMainWindow);
        actionSave_State->setObjectName(QStringLiteral("actionSave_State"));
        actionLoad_State = new QAction(VMUMainWindow);
        actionLoad_State->setObjectName(QStringLiteral("actionLoad_State"));
        actionAbout = new QAction(VMUMainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionReset = new QAction(VMUMainWindow);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        actionScreenshot = new QAction(VMUMainWindow);
        actionScreenshot->setObjectName(QStringLiteral("actionScreenshot"));
        actionEmulate_Audio = new QAction(VMUMainWindow);
        actionEmulate_Audio->setObjectName(QStringLiteral("actionEmulate_Audio"));
        actionEmulate_Audio->setCheckable(true);
        actionEmulate_Audio->setChecked(true);
        actionDump_Icons = new QAction(VMUMainWindow);
        actionDump_Icons->setObjectName(QStringLiteral("actionDump_Icons"));
        actionDump_Eyecatch = new QAction(VMUMainWindow);
        actionDump_Eyecatch->setObjectName(QStringLiteral("actionDump_Eyecatch"));
        actionEnable_Low_Battery = new QAction(VMUMainWindow);
        actionEnable_Low_Battery->setObjectName(QStringLiteral("actionEnable_Low_Battery"));
        actionEnable_Low_Battery->setCheckable(true);
        actionQuick_Connect = new QAction(VMUMainWindow);
        actionQuick_Connect->setObjectName(QStringLiteral("actionQuick_Connect"));
        actionTCP_Connection_Manager = new QAction(VMUMainWindow);
        actionTCP_Connection_Manager->setObjectName(QStringLiteral("actionTCP_Connection_Manager"));
        actionConfigure_Input_Devices = new QAction(VMUMainWindow);
        actionConfigure_Input_Devices->setObjectName(QStringLiteral("actionConfigure_Input_Devices"));
        actionConfigure_Input_Devices->setEnabled(false);
        actionQuit = new QAction(VMUMainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionView_Log = new QAction(VMUMainWindow);
        actionView_Log->setObjectName(QStringLiteral("actionView_Log"));
        actionLoad_BIOS_Image = new QAction(VMUMainWindow);
        actionLoad_BIOS_Image->setObjectName(QStringLiteral("actionLoad_BIOS_Image"));
        actionDisassemble_ROM = new QAction(VMUMainWindow);
        actionDisassemble_ROM->setObjectName(QStringLiteral("actionDisassemble_ROM"));
        actionEnable_Pixel_Ghosting = new QAction(VMUMainWindow);
        actionEnable_Pixel_Ghosting->setObjectName(QStringLiteral("actionEnable_Pixel_Ghosting"));
        actionEnable_Pixel_Ghosting->setCheckable(true);
        actionEnable_Pixel_Ghosting->setChecked(true);
        action1x = new QAction(VMUMainWindow);
        action1x->setObjectName(QStringLiteral("action1x"));
        action2x = new QAction(VMUMainWindow);
        action2x->setObjectName(QStringLiteral("action2x"));
        action4x = new QAction(VMUMainWindow);
        action4x->setObjectName(QStringLiteral("action4x"));
        action8x = new QAction(VMUMainWindow);
        action8x->setObjectName(QStringLiteral("action8x"));
        action16x = new QAction(VMUMainWindow);
        action16x->setObjectName(QStringLiteral("action16x"));
        action32x = new QAction(VMUMainWindow);
        action32x->setObjectName(QStringLiteral("action32x"));
        action64x = new QAction(VMUMainWindow);
        action64x->setObjectName(QStringLiteral("action64x"));
        actionPreserve_Aspect_Ratio = new QAction(VMUMainWindow);
        actionPreserve_Aspect_Ratio->setObjectName(QStringLiteral("actionPreserve_Aspect_Ratio"));
        actionPreserve_Aspect_Ratio->setCheckable(true);
        actionPreserve_Aspect_Ratio->setChecked(true);
        actionSerial_COM_Connection_Manager = new QAction(VMUMainWindow);
        actionSerial_COM_Connection_Manager->setObjectName(QStringLiteral("actionSerial_COM_Connection_Manager"));
        actionHalt = new QAction(VMUMainWindow);
        actionHalt->setObjectName(QStringLiteral("actionHalt"));
        actionHalt->setCheckable(true);
        actionAdvance_Frame = new QAction(VMUMainWindow);
        actionAdvance_Frame->setObjectName(QStringLiteral("actionAdvance_Frame"));
        actionRecordAnim = new QAction(VMUMainWindow);
        actionRecordAnim->setObjectName(QStringLiteral("actionRecordAnim"));
        actionRecordAnim->setCheckable(true);
        actionSave_Game = new QAction(VMUMainWindow);
        actionSave_Game->setObjectName(QStringLiteral("actionSave_Game"));
        actionLoad_Game = new QAction(VMUMainWindow);
        actionLoad_Game->setObjectName(QStringLiteral("actionLoad_Game"));
        centralwidget = new QWidget(VMUMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        VMUMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(VMUMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 320, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuLoad_Recent = new QMenu(menuFile);
        menuLoad_Recent->setObjectName(QStringLiteral("menuLoad_Recent"));
        menuEmulation = new QMenu(menubar);
        menuEmulation->setObjectName(QStringLiteral("menuEmulation"));
        menuSerial_Communications = new QMenu(menuEmulation);
        menuSerial_Communications->setObjectName(QStringLiteral("menuSerial_Communications"));
        menuPlayback = new QMenu(menuEmulation);
        menuPlayback->setObjectName(QStringLiteral("menuPlayback"));
        menuTools = new QMenu(menubar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuScreen_Capture = new QMenu(menuTools);
        menuScreen_Capture->setObjectName(QStringLiteral("menuScreen_Capture"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuPixel_Perfect_Aspect_Zoom = new QMenu(menuView);
        menuPixel_Perfect_Aspect_Zoom->setObjectName(QStringLiteral("menuPixel_Perfect_Aspect_Zoom"));
        VMUMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(VMUMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        VMUMainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuEmulation->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionLoad_ROM);
        menuFile->addAction(menuLoad_Recent->menuAction());
        menuFile->addAction(actionLoad_BIOS_Image);
        menuFile->addSeparator();
        menuFile->addAction(actionSave_Game);
        menuFile->addAction(actionLoad_Game);
        menuFile->addAction(actionSave_State);
        menuFile->addAction(actionLoad_State);
        menuFile->addSeparator();
        menuFile->addAction(actionReset);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuEmulation->addAction(menuPlayback->menuAction());
        menuEmulation->addSeparator();
        menuEmulation->addAction(actionEmulate_Audio);
        menuEmulation->addAction(actionEnable_Pixel_Ghosting);
        menuEmulation->addAction(actionEnable_Low_Battery);
        menuEmulation->addAction(actionConfigure_Input_Devices);
        menuEmulation->addAction(menuSerial_Communications->menuAction());
        menuSerial_Communications->addAction(actionQuick_Connect);
        menuSerial_Communications->addAction(actionTCP_Connection_Manager);
        menuPlayback->addAction(actionHalt);
        menuPlayback->addAction(actionAdvance_Frame);
        menuTools->addAction(actionScreenshot);
        menuTools->addAction(menuScreen_Capture->menuAction());
        menuTools->addAction(actionDump_Icons);
        menuTools->addAction(actionDump_Eyecatch);
        menuTools->addAction(actionDisassemble_ROM);
        menuTools->addAction(actionView_Log);
        menuScreen_Capture->addAction(actionRecordAnim);
        menuHelp->addAction(actionAbout);
        menuView->addAction(actionPreserve_Aspect_Ratio);
        menuView->addAction(menuPixel_Perfect_Aspect_Zoom->menuAction());
        menuPixel_Perfect_Aspect_Zoom->addAction(action1x);
        menuPixel_Perfect_Aspect_Zoom->addAction(action2x);
        menuPixel_Perfect_Aspect_Zoom->addAction(action4x);
        menuPixel_Perfect_Aspect_Zoom->addAction(action8x);
        menuPixel_Perfect_Aspect_Zoom->addAction(action16x);
        menuPixel_Perfect_Aspect_Zoom->addAction(action32x);
        menuPixel_Perfect_Aspect_Zoom->addAction(action64x);

        retranslateUi(VMUMainWindow);

        QMetaObject::connectSlotsByName(VMUMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VMUMainWindow)
    {
        VMUMainWindow->setWindowTitle(QApplication::translate("VMUMainWindow", "ElysianVMU v 0.0.1 Alpha", Q_NULLPTR));
        actionLoad_ROM->setText(QApplication::translate("VMUMainWindow", "Open ROM", Q_NULLPTR));
        actionLoad_ROM->setShortcut(QApplication::translate("VMUMainWindow", "Ctrl+O", Q_NULLPTR));
        actionSave_State->setText(QApplication::translate("VMUMainWindow", "Save State", Q_NULLPTR));
        actionSave_State->setShortcut(QApplication::translate("VMUMainWindow", "Shift+F1", Q_NULLPTR));
        actionLoad_State->setText(QApplication::translate("VMUMainWindow", "Load State", Q_NULLPTR));
        actionLoad_State->setShortcut(QApplication::translate("VMUMainWindow", "F1", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("VMUMainWindow", "About", Q_NULLPTR));
        actionReset->setText(QApplication::translate("VMUMainWindow", "Reset", Q_NULLPTR));
        actionScreenshot->setText(QApplication::translate("VMUMainWindow", "Screenshot", Q_NULLPTR));
        actionEmulate_Audio->setText(QApplication::translate("VMUMainWindow", "Enable Audio", Q_NULLPTR));
        actionDump_Icons->setText(QApplication::translate("VMUMainWindow", "Dump Icons", Q_NULLPTR));
        actionDump_Eyecatch->setText(QApplication::translate("VMUMainWindow", "Dump Eyecatch", Q_NULLPTR));
        actionEnable_Low_Battery->setText(QApplication::translate("VMUMainWindow", "Enable Low Battery", Q_NULLPTR));
        actionQuick_Connect->setText(QApplication::translate("VMUMainWindow", "Quick Connect", Q_NULLPTR));
        actionTCP_Connection_Manager->setText(QApplication::translate("VMUMainWindow", "TCP Connection Manager", Q_NULLPTR));
        actionConfigure_Input_Devices->setText(QApplication::translate("VMUMainWindow", "Configure Input Devices", Q_NULLPTR));
        actionQuit->setText(QApplication::translate("VMUMainWindow", "Quit", Q_NULLPTR));
        actionView_Log->setText(QApplication::translate("VMUMainWindow", "View Log", Q_NULLPTR));
        actionLoad_BIOS_Image->setText(QApplication::translate("VMUMainWindow", "Load BIOS Image", Q_NULLPTR));
        actionDisassemble_ROM->setText(QApplication::translate("VMUMainWindow", "Disassemble ROM", Q_NULLPTR));
        actionEnable_Pixel_Ghosting->setText(QApplication::translate("VMUMainWindow", "Enable Pixel Ghosting", Q_NULLPTR));
        action1x->setText(QApplication::translate("VMUMainWindow", "1x", Q_NULLPTR));
        action2x->setText(QApplication::translate("VMUMainWindow", "2x", Q_NULLPTR));
        action4x->setText(QApplication::translate("VMUMainWindow", "4x", Q_NULLPTR));
        action8x->setText(QApplication::translate("VMUMainWindow", "8x", Q_NULLPTR));
        action16x->setText(QApplication::translate("VMUMainWindow", "16x", Q_NULLPTR));
        action32x->setText(QApplication::translate("VMUMainWindow", "32x", Q_NULLPTR));
        action64x->setText(QApplication::translate("VMUMainWindow", "64x", Q_NULLPTR));
        actionPreserve_Aspect_Ratio->setText(QApplication::translate("VMUMainWindow", "Preserve Aspect Ratio", Q_NULLPTR));
        actionSerial_COM_Connection_Manager->setText(QApplication::translate("VMUMainWindow", "Serial/COM Connection Manager", Q_NULLPTR));
        actionHalt->setText(QApplication::translate("VMUMainWindow", "Halt", Q_NULLPTR));
        actionHalt->setShortcut(QApplication::translate("VMUMainWindow", "F2", Q_NULLPTR));
        actionAdvance_Frame->setText(QApplication::translate("VMUMainWindow", "Advance Frame", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionAdvance_Frame->setToolTip(QApplication::translate("VMUMainWindow", "Advance Execution by a Frame", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionAdvance_Frame->setShortcut(QApplication::translate("VMUMainWindow", "F3", Q_NULLPTR));
        actionRecordAnim->setText(QApplication::translate("VMUMainWindow", "Record", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionRecordAnim->setToolTip(QApplication::translate("VMUMainWindow", "Record to Animated GIF", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionRecordAnim->setShortcut(QApplication::translate("VMUMainWindow", "Ctrl+R", Q_NULLPTR));
        actionSave_Game->setText(QApplication::translate("VMUMainWindow", "Save Game", Q_NULLPTR));
        actionLoad_Game->setText(QApplication::translate("VMUMainWindow", "Load Game", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("VMUMainWindow", "File", Q_NULLPTR));
        menuLoad_Recent->setTitle(QApplication::translate("VMUMainWindow", "Open Recent", Q_NULLPTR));
        menuEmulation->setTitle(QApplication::translate("VMUMainWindow", "Emulation", Q_NULLPTR));
        menuSerial_Communications->setTitle(QApplication::translate("VMUMainWindow", "Serial Communications", Q_NULLPTR));
        menuPlayback->setTitle(QApplication::translate("VMUMainWindow", "Playback", Q_NULLPTR));
        menuTools->setTitle(QApplication::translate("VMUMainWindow", "Tools", Q_NULLPTR));
        menuScreen_Capture->setTitle(QApplication::translate("VMUMainWindow", "Capture Animation", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("VMUMainWindow", "Help", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("VMUMainWindow", "View", Q_NULLPTR));
        menuPixel_Perfect_Aspect_Zoom->setTitle(QApplication::translate("VMUMainWindow", "Pixel-Perfect Zoom", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class VMUMainWindow: public Ui_VMUMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GYRO_VMU_MAIN_WINDOW_H
