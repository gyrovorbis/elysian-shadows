#ifndef GYRO_VMU_MAIN_WINDOW_HPP
#define GYRO_VMU_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <stdint.h>
#include <gyro_system_api.h>

#define VMU_FB_DEFAULT_WIDTH            48
#define VMU_FB_DEFAULT_HEIGHT           40
#define VMU_TEXT_CODEC                  "Shift-JIS"
#define VMU_ROM_HISTORY_SIZE            5
#define VMU_TRIGGER_SPEED_FACTOR        10.0f
#define VMU_FB_DEFAULT_ZOOM_LEVEL       8
#define FILTER_DUPLICATE_LOG_STRINGS    1

#define ICON_ES_PATH                    ":/icons/images/vmu_es.png"
#define ICON_CLOCK_PATH                 ":/icons/images/icon_clock.png"
#define ICON_FILE_PATH                  ":/icons/images/icon_file.png"
#define ICON_FLASH_PATH                 ":/icons/images/icon_flash.png"
#define ICON_GAME_PATH                  ":/icons/images/icon_game.png"
#define ICON_DISCONNECTED_PATH          ":/icons/images/task_bar_red.png"
#define ICON_LISTENING_PATH             ":/icons/images/task_bar_yellow.png"
#define ICON_CONNECTED_PATH             ":/icons/images/task_bar_green.png"
#define ICON_RECORDING_PATH             ":/icons/images/icon_recording.png"

class   QGifImage;
class   QLabel;
class   QTextCodec;
struct  GYAudioSource;
struct  GYAudBuffer;
struct  VMUDevice;
struct  VMSFileInfo;
class   VmuTcpDialog;
class   QGraphicsPixmapItem;
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
class   VmuComDialog;
#endif

namespace Ui {
    class VMUMainWindow;
}

class VMUMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit            VMUMainWindow(QWidget *parent = 0, QString workingDir=QString());
    virtual             ~VMUMainWindow(void);

    bool                loadRom(QString filepath);
    void                resetCpu(void);
    auto                getUi(void) const -> Ui::VMUMainWindow*;

    //virtual void        paintEvent(QPaintEvent* event) override;
    virtual bool        event(QEvent* event) override;
    virtual void        closeEvent(QCloseEvent* event) override;
    virtual void        dragEnterEvent(QDragEnterEvent* event) override;
    virtual void        dropEvent(QDropEvent* event) override;
    virtual bool        eventFilter(QObject *watched, QEvent *event) override;

protected Q_SLOTS:
    void                _update(void);
    void                _updateScene(void);
    void                _onLoadGameAboutToShow(void);
    void                _onLoadGameAboutToHide(void);
    void                _onSaveGameAboutToShow(void);
    void                _onSaveGameAboutToHide(void);

    void on_actionLoad_ROM_triggered();
    void on_actionSave_State_triggered();
    void on_actionLoad_State_triggered();
    void on_actionReset_triggered();
    void on_actionScreenshot_triggered();
    void on_actionEmulate_Audio_toggled(bool arg1);
    void on_actionDump_Icons_triggered(void);
    void on_actionDump_Eyecatch_triggered(void);
    void on_actionTCP_Connection_Manager_triggered(void);
    void on_actionQuick_Connect_triggered();
    void on_actionEnable_Low_Battery_toggled(bool arg1);
    void on_actionQuit_triggered();
    void on_actionView_Log_triggered();
    void on_actionLoad_BIOS_Image_triggered();
    void on_actionDisassemble_ROM_triggered();
    void on_actionEnable_Pixel_Ghosting_toggled(bool arg1);
    void on_actionPreserve_Aspect_Ratio_toggled(bool arg1);
    void on_action1x_triggered();
    void on_action2x_triggered();
    void on_action4x_triggered();
    void on_action8x_triggered();
    void on_action16x_triggered();
    void on_action32x_triggered();
    void on_action64x_triggered();
    void on_actionHalt_toggled(bool arg1);
    void on_actionAdvance_Frame_triggered();
    void on_actionRecordAnim_toggled(bool arg1);
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    void on_actionSerial_COM_Connection_Manager_triggered();
#endif

private:

    static void         _push(void);
    static void         _pop(const unsigned depth);
    static void         _log(GY_DEBUG_SEVERITY level, const char* msg);

    void                _updatePConRegister(void);
    void                _updateLcdAnimStatusMessage(void);
    void                _resizeWindowFromScreenZoom(int level);

    bool                _loadIcons(void);
    void                _updateIcon(void);
    void                _freeIcons(void);
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
    QVector<QImage>     _iconImages;
    QVector<QPixmap>    _iconPixmaps;
    QImage              _eyecatchImage;
    QString             _romDir;
    VmuTcpDialog*       _tcpDialog          = nullptr;
#if GYRO_PLATFORM != GYRO_PLATFORM_RASPI
    VmuComDialog*       _comDialog          = nullptr;
#endif
    QLabel*             _fbLabel            = nullptr;
    QLabel*             _statusLabel        = nullptr;
    QLabel*             _iconLabel          = nullptr;
    QLabel*             _recordingLabel     = nullptr;
    QMenu*              _loadGameMenu       = nullptr;
    QMenu*              _saveGameMenu       = nullptr;
    QGraphicsPixmapItem*    _pixmapItem     = nullptr;
    VMSFileInfo*        _vmsHeader          = nullptr;
    uint16_t**          _iconsData          = nullptr;
    uint16_t*           _eyecatchData       = nullptr;
    QTextCodec*         _japCodec           = nullptr;
    VMUDevice*          _dev                = nullptr;
    QGifImage*          _animGif            = nullptr;
    int64_t             _prevAnimFrameTicks = 0;
    int64_t             _prevTicks          = 0;
    int                 _iconAnimCounter    = 0;
    int                 _iconCurIndex       = 0;
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

    static FILE*        _logFile;
    static int          _logDepth;

};

#endif // GYRO_VMU_MAIN_WINDOW_HPP
