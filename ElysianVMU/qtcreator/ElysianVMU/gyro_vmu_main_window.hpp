#ifndef GYRO_VMU_MAIN_WINDOW_HPP
#define GYRO_VMU_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <stdint.h>
#include <gyro_system_api.h>

#define VMU_ROM_HISTORY_SIZE     5

struct VMUDevice;
struct VMSFileInfo;
struct GYAudioSource;
struct GYAudBuffer;
class QLabel;

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



        QPixmap             _fb;

        GYAudioSource*      _audioSrc       = nullptr;
        GYAudBuffer*        _audioBuff      = nullptr;
            VMUDevice*          _dev            = nullptr;
                Ui::VMUMainWindow*  _ui             = nullptr;
private:

    virtual void        paintEvent(QPaintEvent* event) override;
    virtual bool        event(QEvent* event) override;
    virtual void        closeEvent(QCloseEvent* event) override;
    virtual void        dragEnterEvent(QDragEnterEvent* event) override;
    virtual void        dropEvent(QDropEvent* event) override;

protected Q_SLOTS:
    void                _update(void);



private Q_SLOTS:
    void on_actionLoad_ROM_triggered();
    void on_actionSave_State_triggered();
    void on_actionLoad_State_triggered();
    void on_actionReset_triggered();
    void on_actionScreenshot_triggered();

    void on_actionEmulate_Audio_toggled(bool arg1);

    void on_actionDump_Icons_triggered();

private:

    static void         _push(void);
    static void         _pop(const unsigned depth);
    static void         _log(GY_DEBUG_SEVERITY level, const char* msg);

    bool                _loadIcons(void);
    void                _updateIcon(void);
    void                _freeIcons(void);

    void                _saveSettings(void);
    void                _loadSettings(void);
    void                _pushRecentRom(QString path);

    QTimer              _updateTimer;
    QVector<QImage>     _iconImages;
    QVector<QPixmap>    _iconPixmaps;
    QString             _romDir;
    QLabel*             _statusLabel        = nullptr;
    QLabel*             _iconLabel          = nullptr;
    VMSFileInfo*        _vmsHeader          = nullptr;
    uint16_t**          _iconsData          = nullptr;
    int64_t             _prevTicks          = 0;
    bool                _running            = false;
    bool                _contAttached       = false;
    float               _timeScalar         = 1.0f;
    int                 _iconAnimCounter    = 0;
    int                 _iconCurIndex       = 0;
    static FILE*        _logFile;
    static int          _logDepth;

};

#endif // GYRO_VMU_MAIN_WINDOW_HPP
