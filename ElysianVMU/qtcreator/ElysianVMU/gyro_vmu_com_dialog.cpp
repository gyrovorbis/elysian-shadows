#include "gyro_vmu_com_dialog.hpp"
#include "ui_gyro_vmu_com_dialog.h"
#include <gyro_vmu_port1.h>
#include <gyro_vmu_port7.h>
#include <QSettings>
#include <QSerialPort>
#include <gyro_system_api.h>
#include <inttypes.h>
#include <QTimer>

VmuComDialog::VmuComDialog(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::VmuComDialog)
{
    _ui->setupUi(this);
    _vmuPort1Ext = new VMUPort1Ext {
        this,
        &VmuComDialog::_vmuReadyRead,
        &VmuComDialog::_vmuRead,
        &VmuComDialog::_vmuWrite
    };

    _writeTimeout = new QTimer();
    _writeTimeout->setSingleShot(true);
    _writeTimeout->setInterval(GYRO_VMU_COM_DIALOG_WRITE_TIMEOUT_DEFAULT);
    connect(_writeTimeout, SIGNAL(timeout()), this, SLOT(onWriteTimeout()));
}

VmuComDialog::~VmuComDialog(void) {
    delete _writeTimeout;
    delete _vmuPort1Ext;
    delete _ui;
}


void VmuComDialog::show(void) {
    Q_ASSERT(_dev); //Uhm, you might wanna set a VMU device, dumbass...

    _updatePortInfoList();
    QDialog::show();
}

void VmuComDialog::saveSettings(QSettings& settings) {
    settings.beginGroup(GYRO_VMU_COM_DIALOG_SETTINGS_PREFIX);

    QSerialPortInfo* info = (_portIndex != -1)? &_portInfoList[_portIndex] : nullptr;

    settings.setValue("saved",              true);
    settings.setValue("geometry",           saveGeometry());
    if(info) {
        settings.setValue("portName",       info->portName());
        settings.setValue("baud",           _ui->baudComboBox->currentData().toInt());
        settings.setValue("parity",         _ui->parityComboBox->currentIndex());
        settings.setValue("dataBits",       _ui->dataBitsSpinBox->value());
        settings.setValue("stopBits",       _ui->stopBitsComboBox->currentIndex());
        settings.setValue("flowControl",    _ui->flowControlComboBox->currentIndex());

    }

    settings.endGroup();
}

void VmuComDialog::loadSettings(QSettings& settings) {
    _updatePortInfoList();
    settings.beginGroup(GYRO_VMU_COM_DIALOG_SETTINGS_PREFIX);

    bool saved = settings.value("saved", false).toBool();

    if(saved) {
        restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
        QString portName = settings.value("portName", QString()).toString();
        if(!portName.isEmpty()) {
            _ui->portNameComboBox->setCurrentText(portName);
            int baud = settings.value("baud", -1).toInt();
            if(baud != -1) _ui->baudComboBox->setCurrentText(QString::number(baud));
            _ui->parityComboBox->setCurrentIndex(settings.value("parity", 0).toInt());
            int dataBits = settings.value("dataBits", -1).toInt();
            if(dataBits != -1) _ui->dataBitsSpinBox->setValue(dataBits);
            _ui->stopBitsComboBox->setCurrentIndex(settings.value("stopBits", 0).toInt());
            _ui->flowControlComboBox->setCurrentIndex(settings.value("flowControl", 0).toInt());
            _updateCurrentPortInfo();

        }
    }
    settings.endGroup();

}

void VmuComDialog::updateConnectionStatus(void) {

}

void VmuComDialog::_updatePortInfoList(void) {
    bool wasEmpty = _portInfoList.isEmpty();
    _portInfoList = QSerialPortInfo::availablePorts();
    if(_portInfoList.isEmpty()) {
        _ui->portGroupBox->setEnabled(false);
        _ui->portNameComboBox->setCurrentText("NO DEVICES FOUND!");
        _ui->baudComboBox->clear();
    } else {
        if(wasEmpty) {
            int index = 0;
            for(auto& it: _portInfoList) {
                _ui->portNameComboBox->addItem(it.portName(), index++);
            }

            _baudRates = QSerialPortInfo::standardBaudRates();
            _ui->baudComboBox->clear();
            for(auto& rate : _baudRates) {
                _ui->baudComboBox->addItem(QString::number(rate), rate);
            }

            _ui->portGroupBox->setEnabled(true);
            _ui->portNameComboBox->setCurrentIndex(0);
        }
    }
}

void VmuComDialog::_updateCurrentPortInfo(void) {
    if(_portIndex == -1) return;
    Q_ASSERT(_portIndex >= 0 && _portIndex < _portInfoList.size());

    QSerialPortInfo& info = _portInfoList[_portIndex];
    _ui->systemPathValueLabel->setText(info.systemLocation());
    _ui->manufacturerValueLabel->setText(!info.manufacturer().isEmpty()? info.manufacturer() : "Not Available");
    _ui->vendorIDValueLabel->setText(info.hasVendorIdentifier()? QString::number(info.vendorIdentifier()) : "Not Available");
    _ui->productIDValueLabel->setText(info.hasProductIdentifier()? QString::number(info.productIdentifier()) : "Not Available");
    _ui->serialNumberValueLabel->setText(!info.serialNumber().isEmpty()? info.serialNumber() : "Not Available");
    _ui->descValueLabel->setText(!info.description().isEmpty()? info.description() : "Not Available");
    _ui->statusValueLabel->setText(info.isBusy()? "BUSY" : "IDLE");

}

void VmuComDialog::on_portNameComboBox_currentIndexChanged(int index)
{
    _portIndex = index;
    _updateCurrentPortInfo();
}

void VmuComDialog::on_mainButtonBox_accepted()
{
    close();
}

void VmuComDialog::on_openPortButton_clicked()
{
    if(_activePort) {
        delete _activePort;
        _activePort = nullptr;
        _readBuffer.clear();
        _state = STATE_INACTIVE;
        _ui->infoGroupBox->setEnabled(true);
        _ui->configGroupBox->setEnabled(true);
        _ui->portNameLabel->setEnabled(true);
        _ui->portNameComboBox->setEnabled(true);
        _setConnectionStatusText("Not Connected");
        _ui->openPortButton->setText("Open Port");
        _setRemoteConnectionType(VMU_PORT7_CONNECTION_NONE);
        _updateCurrentPortInfo();

    } else {
        _activePort = new QSerialPort(_portInfoList[_portIndex]);
        _activePort->setBaudRate(_ui->baudComboBox->currentData().toInt());
        _activePort->setParity((QSerialPort::Parity)_ui->parityComboBox->currentIndex());
        _activePort->setDataBits((QSerialPort::DataBits)(_ui->dataBitsSpinBox->value()+(int)QSerialPort::Data5));
        _activePort->setStopBits((QSerialPort::StopBits)_ui->stopBitsComboBox->currentIndex());
        _activePort->setFlowControl((QSerialPort::FlowControl)_ui->flowControlComboBox->currentIndex());

        if(_activePort->open(QIODevice::ReadWrite)) {
            _state = STATE_WAIT_TYPE;
            _ui->infoGroupBox->setEnabled(false);
            _ui->configGroupBox->setEnabled(false);
            _ui->portNameLabel->setEnabled(false);
            _ui->portNameComboBox->setEnabled(false);


            _ui->openPortButton->setText("Close Port");

            _ui->connStatusValueLabel->setText("Connected");
            _updateCurrentPortInfo();
            _setConnectionStatusText("Connected", Qt::darkGreen);
            _setRemoteConnectionTypeText("Waiting...", Qt::darkYellow);

            _gyLog(GY_DEBUG_VERBOSE, "SERIAL MGR: Successfully opened COM port [%s] for Read/Write.", _portInfoList[_portIndex].portName().toStdString().c_str());

            uint8_t hshkByte = VMU_PORT7_CONNECTION_VMU;
            if(_writeSerial((const char*)&hshkByte, sizeof(hshkByte))) {
                _gyLog(GY_DEBUG_VERBOSE, "SERIAL MGR: Wrote handshake type byte: %x", hshkByte);
                _gyLog(GY_DEBUG_VERBOSE, "SERIAL MGR: Waiting for remote type handshake byte...");
            }

#if 0
            _setRemoteConnectionType(VMU_PORT7_CONNECTION_VMU);
      _state = STATE_ACTIVE;
#endif


        } else {
            _gyLog(GY_DEBUG_ERROR, "COM PORT: Could not open COM port! [Error: %s]", _errorToString(_activePort->error()).toStdString().c_str());
            //_ui->connStatusValueLabel->setText(QString("Could not open port! [Error: ")+_errorToString(_activePort->error())+QString("]"));
            _setConnectionStatusText(QString("Could not open port! [Error: ")+_errorToString(_activePort->error())+QString("]"), Qt::darkRed);
        }


    }
}

QString VmuComDialog::_errorToString(int errorCode) {
    switch(errorCode) {
    case QSerialPort::NoError:
        return "NoError";
    case QSerialPort::DeviceNotFoundError:
        return "DeviceNotFoundError";
    case QSerialPort::PermissionError:
        return "PermissionError";
    case QSerialPort::OpenError:
        return "OpenError";
    case QSerialPort::NotOpenError:
        return "NotOpenError";
    case QSerialPort::ParityError:
        return "ParityError";
    case QSerialPort::FramingError:
        return "FramingError";
    case QSerialPort::BreakConditionError:
        return "BreakConditionError";
    case QSerialPort::WriteError:
        return "WriteError";
    case QSerialPort::ReadError:
        return "ReadError";
    case QSerialPort::ResourceError:
        return "ResourceError";
    case QSerialPort::UnsupportedOperationError:
        return "UnsupportedOperationError";
    case QSerialPort::TimeoutError:
        return "TimeoutError";
    case QSerialPort::UnknownError:
        return "UnknownError";
    default:
        return "Invalid Error Type!";
    }
}

void VmuComDialog::_setConnectionStatusText(QString text, QColor color) {
    QPalette palette = _ui->connStatusValueLabel->palette();
    palette.setColor(QPalette::WindowText, color);
    _ui->connStatusValueLabel->setPalette(palette);
    _ui->connStatusValueLabel->setText(text);
}

void VmuComDialog::_setRemoteConnectionTypeText(QString text, QColor color) {
    QPalette palette = _ui->connTypeValueLabel->palette();
    palette.setColor(QPalette::WindowText, color);
    _ui->connTypeValueLabel->setPalette(palette);
    _ui->connTypeValueLabel->setText(text);
}

int VmuComDialog::_vmuReadyRead(void* context) {
    VmuComDialog* self = static_cast<VmuComDialog*>(context);
    return self->_readBuffer.size()-self->_readBufPos;
}

int VmuComDialog::_vmuRead(void* context, uint8_t *buff, size_t bytes) {
    VmuComDialog* self = static_cast<VmuComDialog*>(context);

    const size_t bytesLeft = self->_readBuffer.size()-self->_readBufPos;
    const size_t toRead = (bytes <= bytesLeft)? bytes : bytesLeft;

    if(toRead) {
        memcpy(buff, self->_readBuffer.constData()+self->_readBufPos, toRead);
        self->_readBufPos += toRead;
        if(self->_readBufPos == self->_readBuffer.size()) {
            self->_readBuffer.clear();
            self->_readBufPos = 0;
        } else if(self->_readBufPos > self->_readBuffer.size()) {
            Q_ASSERT(false);
        }
    } else {
        _gyLog(GY_DEBUG_ERROR, "Tried to read while no bytes were present!");
    }

    if(toRead < bytes) {
        _gyLog(GY_DEBUG_ERROR, "COM READ: Requested %d bytes and was only able to read %d bytes!", bytes, toRead);
    }
    return toRead;
}

int VmuComDialog::_vmuWrite(void* context, const uint8_t *buff, size_t bytes) {
    VmuComDialog* self = static_cast<VmuComDialog*>(context);
    return self->_writeSerial((const char*)buff, bytes);
}

void VmuComDialog::onBytesWritten(qint64 bytes) {
    _pendingBytes -= bytes;
    if(_pendingBytes < 0) _pendingBytes = 0;
    _writeTimeout->start();
}

bool VmuComDialog::_setRemoteConnectionType(uint8_t type) {
    bool connected = false;

    switch(type) {
    case VMU_PORT7_CONNECTION_DC:
        _gyLog(GY_DEBUG_VERBOSE, "SERIAL: Recieved type 'DC' for remote connection type handshake!");
        _setRemoteConnectionTypeText(QString("DC"), Qt::darkGreen);
        connected = true;
        break;
    case VMU_PORT7_CONNECTION_VMU:
        _gyLog(GY_DEBUG_VERBOSE, "SERIAL: Recieved type 'VMU' for remote connection type handshake!");
        _setRemoteConnectionTypeText(QString("VMU"), Qt::darkGreen);
        connected = true;
        break;
    case VMU_PORT7_CONNECTION_NONE:
        _gyLog(GY_DEBUG_WARNING, "SERIAL: Recieved type 'NONE' [0x00] for remote connection type handshake!");
        _setRemoteConnectionTypeText("None", Qt::black);
        connected = false;
        break;
    default:
        _gyLog(GY_DEBUG_ERROR, "SERIAL: Recieved invalid byte [%x] for remote connection type handshake!", type);
        _setRemoteConnectionTypeText(QString("Invalid Response: ")+QString(type), Qt::darkRed);
        return 0;
    }

    gyVmuPort7Connect(_dev, (VMU_PORT7_CONNECTION_TYPE)type);
    gyVmuPort1ExtConnect(_dev, connected? _vmuPort1Ext : nullptr);

    return connected;
}

void VmuComDialog::onReadyRead(void) {
    if(_state == STATE_WAIT_TYPE) {
        auto data = _activePort->readAll();
        if(data.size() == 1) {
            if(_setRemoteConnectionType(data[0])) {
                _state = STATE_ACTIVE;
            }
        } else {
            _gyLog(GY_DEBUG_ERROR, "SERIAL: Receiving too many bytes [%d] for connection type handshake!", data.size());
        }
    } else {
        _readBuffer.append(_activePort->readAll());
    }
}

void VmuComDialog::onError(QSerialPort::SerialPortError error) {
    _gyLog(GY_DEBUG_ERROR, "SERIAL ERROR[%s]: %s", _errorToString(error).toStdString().c_str(), _activePort->errorString().toStdString().c_str());
}

qint64 VmuComDialog::_writeSerial(const char *data, qint64 bytes) {
    Q_ASSERT(_activePort);

    qint64 written = _activePort->write(data, bytes);

    if(written == -1) {
        _gyLog(GY_DEBUG_ERROR, "COULDNT WRITE!");
    } else if(written != bytes) {
        _gyLog(GY_DEBUG_ERROR, "Only wrote %lld of %lld bytes!!!", written, bytes);
    } else {
        _pendingBytes += written;
        _writeTimeout->start();
    }

    return written;
}


void VmuComDialog::onWriteTimeout(void) {
    if(_pendingBytes) {
        _gyLog(GY_DEBUG_ERROR, "Timed out writing after %d msec with %lld bytes left!", _writeTimeout->interval(), _pendingBytes);
        _pendingBytes = 0;
    }
}
