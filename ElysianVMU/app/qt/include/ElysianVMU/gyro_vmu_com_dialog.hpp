/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VMU_COM_DIALOG_H
#define GYRO_VMU_COM_DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QList>
#include <QSerialPortInfo>


#define GYRO_VMU_COM_DIALOG_SETTINGS_PREFIX         "com"
#define GYRO_VMU_COM_DIALOG_WRITE_TIMEOUT_DEFAULT   5000

class   QSettings;
class   QSerialPort;
struct  VMUDevice;
struct  VMUPort1Ext;

namespace Ui {
    class VmuComDialog;
}

class VmuComDialog : public QDialog {
    Q_OBJECT

    enum CONNECTION_STATE {
        STATE_INACTIVE,
        STATE_WAIT_TYPE,
        STATE_ACTIVE
    };

public:

    explicit                VmuComDialog(QWidget* parent = nullptr);
    virtual                 ~VmuComDialog(void);

    void                    setVmuDevice(VMUDevice* dev);

    void                    show(void);

    void                    saveSettings(QSettings& settings);
    void                    loadSettings(QSettings& settings);
    void                    updateConnectionStatus(void);

protected:

    void                    _setConnectionStatusText(QString text, QColor color=QColor(Qt::black));
    void                    _setRemoteConnectionTypeText(QString text, QColor color=QColor(Qt::black));
    void                    _updatePortInfoList(void);
    void                    _updateCurrentPortInfo(void);
    static QString          _errorToString(int errorCode);

    qint64                  _writeSerial(const char* data, qint64 bytes);

    static int              _vmuReadyRead(void* context);
    static int              _vmuRead(void* context, uint8_t* buff, size_t bytes);
    static int              _vmuWrite(void* context, const uint8_t* buff, size_t bytes);

    bool                    _setRemoteConnectionType(uint8_t type);

private Q_SLOTS:

    void                    onError(QSerialPort::SerialPortError error);
    void                    onReadyRead(void);
    void                    onBytesWritten(qint64 bytes);
    void                    onWriteTimeout(void);

    void                    on_portNameComboBox_currentIndexChanged(int index);
    void                    on_mainButtonBox_accepted();
    void                    on_openPortButton_clicked();

private:



    QList<QSerialPortInfo>  _portInfoList;
    QList<qint32>           _baudRates;
    QByteArray              _readBuffer;
    int                     _portIndex      = -1;
    VMUPort1Ext*            _vmuPort1Ext    = nullptr;
    VMUDevice*              _dev            = nullptr;
    QSerialPort*            _activePort     = nullptr;
    QTimer*                 _writeTimeout   = nullptr;
    CONNECTION_STATE        _state          = STATE_INACTIVE;
    qint64                  _pendingBytes   = 0;
    qint64                  _readBufPos     = 0;
    Ui::VmuComDialog*       _ui;

};







//========= INLINEZ ========

inline void VmuComDialog::setVmuDevice(VMUDevice* dev) {
    _dev = dev;
}

#endif // GYRO_VMU_COM_DIALOG_H
