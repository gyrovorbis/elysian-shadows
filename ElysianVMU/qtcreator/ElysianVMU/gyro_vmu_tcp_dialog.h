#ifndef GYRO_VMU_TCP_DIALOG_H
#define GYRO_VMU_TCP_DIALOG_H

#include <QDialog>

#define GYRO_VMU_TCP_DIALOG_ALLOW_LOOPBACK
#define GYRO_VMU_TCP_DIALOG_SETTINGS_PREFIX         "tcp"

class   QSettings;
struct  VMUDevice;

namespace Ui {
    class VmuTcpDialog;
}

class VmuTcpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit    VmuTcpDialog(QWidget* parent = nullptr);
    virtual     ~VmuTcpDialog(void);

    void        show(void);
    void        setConnectionStatus(QColor color, QString text);
    bool        quickConnect(void);
    void        saveSettings(QSettings& settings);
    void        loadSettings(QSettings& settings);
    void        updateConnectionStatus(void);

    bool        startTcpServer(void);
    bool        stopTcpServer(void);
    bool        connectToServer(void);
    bool        disconnectFromServer(void);

    void        setVmuDevice(VMUDevice* dev);
    VMUDevice*  getVmuDevice(void) const;

    bool        getClientEnabled(void) const;
    bool        getHostEnabled(void) const;
    bool        getAllowVmuConnections(void) const;
    bool        getAllowDcConnections(void) const;
    bool        getUseLatestConnection(void) const;
    bool        getLaunchOnStart(void) const;
    int         getHostPort(void) const;
    QString     getRemoteHost(void) const;
    int         getRemotePort(void) const;

    void        setClientEnabled(bool val);
    void        setHostEnabled(bool val);
    void        setAllowVmuConnections(bool val);
    void        setAllowDcConnections(bool val);
    void        setUseLatestConnection(bool val);
    void        setLaunchOnStart(bool val);
    void        setHostPort(int port);
    void        setRemoteHost(QString hostname);
    void        setRemotePort(int port);

private Q_SLOTS:
    void        on_connectButton_clicked(void);
    void        on_startServerButton_clicked(void);
    void        on_clientGroupBox_toggled(bool arg1);
    void        on_hostGroupBox_toggled(bool arg1);
    void        on_mostRecentCheckBox_stateChanged(int arg1);

private:
    VMUDevice*          _dev            = nullptr;
    Ui::VmuTcpDialog*   _ui;
    bool                _serverActive   = false;
    bool                _clientActive   = false;
};







// ====== INLINEZ ======

inline void VmuTcpDialog::setVmuDevice(VMUDevice* dev) {
    _dev = dev;
}

inline VMUDevice* VmuTcpDialog::getVmuDevice(void) const { return _dev; }

#endif // GYRO_VMU_TCP_DIALOG_H
