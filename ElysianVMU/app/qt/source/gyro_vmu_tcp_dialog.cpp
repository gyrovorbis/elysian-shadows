/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include "gyro_vmu_tcp_dialog.hpp"
#include "ui_gyro_vmu_tcp_dialog.h"
#include "gyro_vmu_serial.h"
#include "gyro_system_api.h"
#include "gyro_vmu_device.h"
#include <gyro_net_api.h>
#include <QMessageBox>
#include <QSettings>

VmuTcpDialog::VmuTcpDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::VmuTcpDialog)
{
    _ui->setupUi(this);
}

VmuTcpDialog::~VmuTcpDialog(void)
{
    delete _ui;
}

bool VmuTcpDialog::getClientEnabled(void) const {
    return _ui->clientGroupBox->isChecked();
}
bool VmuTcpDialog::getHostEnabled(void) const {
    return _ui->hostGroupBox->isChecked();
}
bool VmuTcpDialog::getAllowVmuConnections(void) const {
    return _ui->vmuClientCheckBox->isChecked();
}
bool VmuTcpDialog::getAllowDcConnections(void) const {
    return _ui->dcClientCheckBox->isChecked();
}
bool VmuTcpDialog::getUseLatestConnection(void) const {
    return _ui->mostRecentCheckBox->isChecked();
}
bool VmuTcpDialog::getLaunchOnStart(void) const {
    return _ui->programStartCheckBox->isChecked();
}
int  VmuTcpDialog::getHostPort(void) const {
    return _ui->listenPortSpinBox->value();
}
QString VmuTcpDialog::getRemoteHost(void) const {
    return _ui->remoteIpText->text();
}
int VmuTcpDialog::getRemotePort(void) const {
    return _ui->portSpinbox->value();
}
void VmuTcpDialog::setClientEnabled(bool val) {
    _ui->clientGroupBox->setChecked(val);
}
void VmuTcpDialog::setHostEnabled(bool val) {
    _ui->hostGroupBox->setChecked(val);
}
void VmuTcpDialog::setAllowVmuConnections(bool val) {
    _ui->vmuClientCheckBox->setChecked(val);
}
void VmuTcpDialog::setAllowDcConnections(bool val) {
    _ui->dcClientCheckBox->setChecked(val);
}
void VmuTcpDialog::setUseLatestConnection(bool val) {
    _ui->mostRecentCheckBox->setChecked(val);
}
void VmuTcpDialog::setLaunchOnStart(bool val) {
    _ui->programStartCheckBox->setChecked(val);
}
void VmuTcpDialog::setHostPort(int port) {
    _ui->listenPortSpinBox->setValue(port);
}
void VmuTcpDialog::setRemoteHost(QString hostname) {
    _ui->ipEdit->setText(hostname);
}
void VmuTcpDialog::setRemotePort(int port) {
    _ui->portSpinbox->setValue(port);
}

void VmuTcpDialog::show(void) {
    updateConnectionStatus();
    QDialog::show();
}

void VmuTcpDialog::setConnectionStatus(QColor color, QString text) {
    QPalette palette = _ui->connectionStatusText->palette();
    palette.setColor(QPalette::WindowText, color);
    _ui->connectionStatusText->setPalette(palette);
    _ui->connectionStatusText->setText(text);
}

bool VmuTcpDialog::quickConnect(void) {
    if(!_clientActive) on_connectButton_clicked();
    return _clientActive;
}

void VmuTcpDialog::updateConnectionStatus(void) {
    #if 1
    bool connected = false;
    switch(gyVmuSerialConnectionType(_dev)) {
    case VMU_SERIAL_CONNECTION_NONE:
        _ui->remoteTypeText->setText("None");
        break;
    case VMU_SERIAL_CONNECTION_VMU:
        _ui->remoteTypeText->setText("VMU");
        connected = true;
        break;
    case VMU_SERIAL_CONNECTION_DC:
        _ui->remoteTypeText->setText("Dreamcast");
        connected = true;
        break;
    }

    //Update in the case of server disconnect
    if(_clientActive && !_dev->serialPort._tcpSocket) {
        disconnectFromServer();
    }

    if(connected) {
        GYIpAddress ipAddr;
        gyNetPeerIpAddress(&ipAddr, _dev->serialPort._tcpSocket);
        _ui->remoteIpText->setText(gyNetIpAddressToString(&ipAddr));
        _ui->remotePortText->setText(QString::number(ipAddr.port));
        setConnectionStatus(Qt::darkGreen, "Connected");
    } else {
        _ui->remoteIpText->setText("None");
        _ui->remotePortText->setText("None");
        if(_serverActive) setConnectionStatus(Qt::darkYellow, "Listening for Incoming Connections...");
        else setConnectionStatus(Qt::black, "Not Connected");
    }
#endif
}

void VmuTcpDialog::saveSettings(QSettings &settings) {
    settings.beginGroup(GYRO_VMU_TCP_DIALOG_SETTINGS_PREFIX);

    settings.setValue("saved",              true);
    settings.setValue("geometry",           saveGeometry());
    settings.setValue("clientEnabled",      getClientEnabled());
    settings.setValue("hostEnabled",        getHostEnabled());
    settings.setValue("vmuConnections",     getAllowVmuConnections());
    settings.setValue("dcConnections",      getAllowDcConnections());
    settings.setValue("latestConnection",   getUseLatestConnection());
    settings.setValue("launchOnStart",      getLaunchOnStart());
    settings.setValue("hostPort",           getHostPort());
    settings.setValue("remoteHost",         getRemoteHost());
    settings.setValue("remotePort",         getRemotePort());

    settings.endGroup();
}

void VmuTcpDialog::loadSettings(QSettings &settings) {
    settings.beginGroup(GYRO_VMU_TCP_DIALOG_SETTINGS_PREFIX);

    bool saved = settings.value("saved", false).toBool();

    if(saved) {
        restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
        setClientEnabled(settings.value("clientEnabled", true).toBool());
        setHostEnabled(settings.value("hostEnabled", false).toBool());
        setAllowVmuConnections(settings.value("vmuConnections", true).toBool());
        setAllowDcConnections(settings.value("dcConnections", true).toBool());
        setUseLatestConnection(settings.value("latestConnection", false).toBool());
        setLaunchOnStart(settings.value("launchOnStart", false).toBool());
        setHostPort(settings.value("hostPort", 1234).toInt());
        setRemoteHost(settings.value("remoteHost", QString("")).toString());
        setRemotePort(settings.value("remotePort", 1234).toInt());
    }
    settings.endGroup();

    if(getLaunchOnStart()) startTcpServer();
}

void VmuTcpDialog::on_hostGroupBox_toggled(bool arg1)
{
    //Shit's mutually exclusive, disable client shit.
    if(arg1 && _ui->clientGroupBox->isChecked()) {
        _ui->clientGroupBox->setChecked(false);
    }
}

void VmuTcpDialog::on_clientGroupBox_toggled(bool arg1)
{
    //Disable host shit.
    if(arg1 && _ui->hostGroupBox->isChecked()) {
        _ui->hostGroupBox->setChecked(false);
    }
}

bool VmuTcpDialog::startTcpServer(void) {
#if 1
    if(!_ui->hostGroupBox->isChecked()) _ui->hostGroupBox->setChecked(true);
    if(gyVmuSerialTcpServerStart(_dev, _ui->listenPortSpinBox->value())) {
        setConnectionStatus(Qt::darkYellow, "Listening for Incoming Connections...");
        _ui->startServerButton->setText("Stop TCP Server");
        _serverActive = true;
        return true;
    } else {
        QMessageBox::critical(nullptr, "Failed to Start", "Failed to launch TCP server. Probably could not bind to port.");
        return false;
    }
#endif
    return true;
}

bool VmuTcpDialog::stopTcpServer(void) {
    #if 1
    bool success = true;

    if(gyVmuSerialConnectionType(_dev) != VMU_SERIAL_CONNECTION_NONE) {
        auto reply = QMessageBox::question(this, "Disconnect Remote Client",
                                           "There is still a remote client connected to the current VMU. Stopping the TCP server will disconnect the client. Continue?",
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::No) return false;

    }


    if(!gyVmuSerialTcpServerStop(_dev)) {
        _gyLog(GY_DEBUG_ERROR, "Something reested when stopping TCP server.");
        success = false;
    }

    _ui->startServerButton->setText("Start TCP Server");
    _serverActive = false;
    updateConnectionStatus();
    return success;
#endif
    return true;
}

void VmuTcpDialog::on_startServerButton_clicked(void)
{
    if(!_serverActive) { //start server
#ifndef GYRO_VMU_TCP_DIALOG_ALLOW_LOOPBACK
        if(_clientActive) {
            auto reply = QMessageBox::question(this, "Disconnect From Server",
                                               "Starting TCP server as host requires the current connection to remote host be terminated. Continue?",
                                                QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::No) return;
            disconnectFromServer();
        }
#endif
        startTcpServer();
    } else { //stop server
        stopTcpServer();
    }

}

bool VmuTcpDialog::connectToServer(void) {
    if(_ui->ipEdit->text().isEmpty()) {
        QMessageBox::critical(nullptr, "Connection Failed!",
                              "Hostname must be specified to connect to remote server!");
        return false;
    }
#if 0
    if(gyVmuSerialTcpOpen(_dev, _ui->ipEdit->text().toStdString().c_str(), _ui->portSpinbox->value())) {
        setConnectionStatus(Qt::darkGreen, "Connected!");
        _ui->connectButton->setText("Disconnect From Server");
        updateConnectionStatus();
        _clientActive = true;
        if(!_ui->clientGroupBox->isChecked()) _ui->clientGroupBox->setChecked(true);
        return true;
    } else {
        QMessageBox::critical(nullptr, "Connection Failure", "Could not connect to the remote host! See debug logs for more information.");
        return false;
    }
#endif
    return true;
}

bool VmuTcpDialog::disconnectFromServer(void) {

    bool success = true;
#if 0
    if(!gyVmuSerialTcpClose(_dev)) {
        _gyLog(GY_DEBUG_ERROR, "Something reested while disconnecting from remote TCP server.");
        success = false;
    }
#endif
    _ui->connectButton->setText("Connect To Server");
    _clientActive = false;
    updateConnectionStatus();
    return success;
}

void VmuTcpDialog::on_connectButton_clicked(void)
{
    if(!_clientActive) {
#ifndef GYRO_VMU_TCP_DIALOG_ALLOW_LOOPBACK
        if(_serverActive) {
            auto reply = QMessageBox::question(this, "Stop TCP Server",
                                               "Connecting to a remote host as client requires stopping the local TCP server. Continue?",
                                                QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::No) return;
            stopTcpServer();

        }
#endif
        connectToServer();
    } else {
        disconnectFromServer();
    }

}


void VmuTcpDialog::on_mostRecentCheckBox_stateChanged(int arg1)
{
    bool checked;
    switch(arg1) {
    case Qt::CheckState::Checked:
        checked = true;
        break;
    case Qt::CheckState::Unchecked:
        checked = false;
    default:
        Q_ASSERT(false); //SHOULD NOT BE FUCKIN TRISTATE!!!!
    }

    _gyLog(GY_DEBUG_VERBOSE, "VMU Serial TCP Server - Use most recent connection: %s", checked? "ON" : "OFF");
#if 1
    gyVmuSerialTcpServerUseMostRecentClientSet(_dev, (int)checked);
#endif
}
