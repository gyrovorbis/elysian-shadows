#ifndef GYRO_VMU_MAIN_WINDOW_HPP
#define GYRO_VMU_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <stdint.h>
#include <libGyro/gyro_system_api.h>
#include <ElysianVMU/qevmu_device.hpp>
#include <functional>

#define VMU_FB_DEFAULT_WIDTH            48
#define VMU_FB_DEFAULT_HEIGHT           40
#define VMU_ROM_HISTORY_SIZE            5
#define VMU_TRIGGER_SPEED_FACTOR        10.0f
#define VMU_FB_DEFAULT_ZOOM_LEVEL       8
#define FILTER_DUPLICATE_LOG_STRINGS    1
#define VMU_MAIN_WINDOW_SETTINGS_PREFIX "mainWindow"

#define VMU_STATUS_BAR_FORCE_UPDATE_INTERVAL    300

#define ICON_ES_PATH                    ":/icons/images/vmu_es.png"
#define ICON_CLOCK_PATH                 ":/icons/images/icon_clock.png"
#define ICON_FILE_PATH                  ":/icons/images/icon_file.png"
#define ICON_FLASH_PATH                 ":/icons/images/icon_flash.png"
#define ICON_GAME_PATH                  ":/icons/images/icon_game.png"
#define ICON_DISCONNECTED_PATH          ":/icons/images/task_bar_red.png"
#define ICON_LISTENING_PATH             ":/icons/images/task_bar_yellow.png"
#define ICON_CONNECTED_PATH             ":/icons/images/task_bar_green.png"
#define ICON_RECORDING_PATH             ":/icons/images/icon_recording.png"
#define ICON_TCP_LISTENING_PATH         ":/icons/images/task_bar_listening.png"
#define ICON_TCP_DISABLED_PATH          ":/icons/images/task_bar_disconnected.png"
#define ICON_TCP_CONNECTED_PATH         ":/icons/images/task_bar_connected.png"

class   QDir;
class   QGifImage;
class   QSettings;
class   QLabel;
class   QTextCodec;
class   QTableView;
struct  GYAudioSource;
struct  GYAudBuffer;
struct  VMUDevice;
struct  VMSFileInfo;
class   VmuTcpDialog;
class   VmuBuzzerDialog;
class   VmuAddressSpaceExplorerWidget;
class   QGraphicsPixmapItem;
class   JetSetRadioGraffitiTool;
class   VmuFileSystemTableModel;
class   VmuFileSystemTableView;
class   VmuFileSystemUtilizationBar;
class   VmuDeviceTableView;
class   VmuDevicePropertiesDialog;
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
class   VmuComDialog;
#endif

namespace QESTkUtils {
    class HelpDockWidget;
}

namespace Ui {
    class VMUMainWindow;
}

class VMUMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum REMOTE_STATE {
        REMOTE_NONE,
        REMOTE_DISABLED,
        REMOTE_LISTENING,
        REMOTE_CONNECTED
    };

    explicit            VMUMainWindow(QWidget *parent = 0, QString workingDir=QString());
    virtual             ~VMUMainWindow(void) override;

    bool                loadRom(QString filepath);
    bool                loadBios(QString filepath);
    void                resetCpu(void);
    bool                saveSettings(QSettings* settings=nullptr);
    bool                loadSettings(QSettings* settings=nullptr);
    auto                getUi(void) const -> Ui::VMUMainWindow*;
    bool                startTcpServer(unsigned port);

    //virtual void        paintEvent(QPaintEvent* event) override;
    virtual bool        event(QEvent* event) override;
    virtual void        closeEvent(QCloseEvent* event) override;
    virtual void        dragEnterEvent(QDragEnterEvent* event) override;
    virtual void        dropEvent(QDropEvent* event) override;
    virtual bool        eventFilter(QObject *watched, QEvent *event) override;

public Q_SLOTS:
        void                update(void);

protected Q_SLOTS:
    void                _updateScene(void);
    void                _onLoadGameAboutToShow(void);
    void                _onLoadGameAboutToHide(void);
    void                _onSaveGameAboutToShow(void);
    void                _onSaveGameAboutToHide(void);
    void                _showAddressSpaceModel(VMUDevice* dev, int type);

    void on_actionLoad_ROM_triggered(void);
    void on_actionSave_State_triggered(void);
    void on_actionLoad_State_triggered(void);
    void on_actionReset_triggered(void);
    void on_actionScreenshot_triggered(void);
    void on_actionEmulate_Audio_toggled(bool arg1);
    void on_actionTCP_Connection_Manager_triggered(void);
    void on_actionQuick_Connect_triggered(void);
    void on_actionEnable_Low_Battery_toggled(bool arg1);
    void on_actionQuit_triggered(void);
    void on_actionView_Log_triggered(void);
    void on_actionLoad_BIOS_Image_triggered(void);
    void on_actionDisassemble_ROM_triggered(void);
    void on_actionEnable_Pixel_Ghosting_toggled(bool arg1);
    void on_actionPreserve_Aspect_Ratio_toggled(bool arg1);
    void on_action1x_triggered(void);
    void on_action2x_triggered(void);
    void on_action4x_triggered(void);
    void on_action8x_triggered(void);
    void on_action16x_triggered(void);
    void on_action32x_triggered(void);
    void on_action64x_triggered(void);
    void on_actionHalt_toggled(bool arg1);
    void on_actionAdvance_Frame_triggered(void);
    void on_actionRecordAnim_toggled(bool arg1);
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    void on_actionSerial_COM_Connection_Manager_triggered(void);
#endif
    void on_actionBIOS_Runtime_Disassembly_triggered(void);
    void on_actionBuzzer_Tool_triggered(void);
    void on_actionMemory_Browser_triggered(void);
    void on_actionDocumentation_triggered(bool checked);
    void on_actionJet_Set_Radio_Graffiti_Tool_triggered(void);
    void on_actionFilesystem_triggered(void);
    void on_actionExport_Flash_Image_triggered(void);
    void on_actionNew_Flash_Image_triggered(void);
    void on_actionFlash_Address_Space_triggered(void);
    void on_actionRAM_Address_Space_triggered(void);
    void on_actionDevice_Manager_triggered(void);
    void on_actionDevice_Properties_triggered(void);

private:

    static void         _push(void);
    static void         _pop(const unsigned depth);
    static void         _log(GY_DEBUG_SEVERITY level, const char* msg);
    static bool         _flushLog(void);

    void                _updateStatusbar(void);
    void                _updatePConRegister(void);
    void                _updateLcdAnimStatusMessage(void);
    void                _resizeWindowFromScreenZoom(int level);

    void                _updateIcon(void);
    void                _addAnimFrame(const QPixmap& pixmap);

    void                _setupAboutMsgBox(void);
    void                _saveSettings(void);
    void                _loadSettings(void);
    void                _pushRecentRom(QString path);

    Ui::VMUMainWindow*  _ui                 = nullptr;
    QPixmap             _fb;
    QPixmap             _iconES;
    QPixmap             _iconClock;
    QPixmap             _iconFile;
    QPixmap             _iconGame;
    QPixmap             _iconFlash;
    QPixmap             _iconDisconnected;
    QPixmap             _iconListening;
    QPixmap             _iconConnected;
    QPixmap             _iconRecording;
    QPixmap             _pendingAnimFrame;
    QTimer              _updateTimer;
    VmuFileSystemUtilizationBar*
                        _fileSystemBar      = nullptr;
    QESTkUtils::HelpDockWidget*
                        _helpDockWidget     = nullptr;
    VmuFileSystemTableModel*
                        _fileSysTableModel  = nullptr;
    VmuFileSystemTableView* _fileSysTable   = nullptr;
    VmuDeviceTableView* _deviceTable        = nullptr;
    VmuTcpDialog*       _tcpDialog          = nullptr;
    VmuBuzzerDialog*    _buzzerDialog       = nullptr;
    VmuAddressSpaceExplorerWidget*
                        _memBrowser   = nullptr;
    JetSetRadioGraffitiTool*
                        _jsrGraffitiTool    = nullptr;
    VmuDevicePropertiesDialog*
                        _propertiesDialog   = nullptr;
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    VmuComDialog*       _comDialog          = nullptr;
#endif
    QLabel*             _fbLabel            = nullptr;
    QLabel*             _statusLabel        = nullptr;
    QLabel*             _iconLabel          = nullptr;
    QLabel*             _recordingLabel     = nullptr;
    QLabel*             _remoteLabel        = nullptr;
    QMenu*              _loadGameMenu       = nullptr;
    QMenu*              _saveGameMenu       = nullptr;
    QGraphicsPixmapItem* _pixmapItem        = nullptr;
    QtVmuDevice          _dev;
    QGifImage*          _animGif            = nullptr;
    int64_t             _prevAnimFrameTicks = 0;
    int64_t             _prevTicks          = 0;
    int                 _iconAnimCounter    = 0;
    int                 _iconCurIndex       = -1;
    int                 _pConRegister       = 0;
    int                 _prevFbWidth        = 0;
    int                 _prevFbHeight       = 0;
    bool                _running            = false;
    bool                _halt               = false;
    bool                _wasHalted          = false;
    bool                _advanceFrame       = false;
    bool                _contAttached       = false;
    bool                _recording          = false;
    bool                _ghostingWasEnabled = false;
    bool                _displayWasEnabled  = true;
    REMOTE_STATE        _remoteState        = REMOTE_NONE;
    bool                _statusBarChanged   = true;

    static QDir*         _workingDir;
    static FILE*        _logFile;
    static int          _logDepth;

};

#endif // GYRO_VMU_MAIN_WINDOW_HPP
