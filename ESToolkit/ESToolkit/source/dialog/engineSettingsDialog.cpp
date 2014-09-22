#include "engineSettingsDialog.h"
#include "estoolkit.h"
#include "ui_engineSettings.h"
#include "engineDialog.h"
#include <QSettings>
#include <QDebug>
#include <QFileDialog>

EngineSettingsDialog::EngineSettingsDialog(QWidget *parent): QDialog(parent), _ui(new Ui::engine_settings_dialog) {
    _ui->setupUi(this);
	//setFocusPolicy(Qt::StrongFocus);

    this->setWindowTitle("Engine Settings");

    connect(_ui->gammaBin_edit,SIGNAL(editingFinished()),this,SLOT(syncEnginePathx86()));
    connect(_ui->gammaBin_button, SIGNAL(clicked()),this,SLOT(onEnginePathx86Browse()));
    connect(_ui->gammaBinDefault_button, SIGNAL(clicked()),this,SLOT(onEnginePathx86Default()));

    connect(_ui->dcIpaddress_ledit,SIGNAL(editingFinished()),this,SLOT(syncDcIp()));
    connect(_ui->dcMacaddress_ledit,SIGNAL(editingFinished()),this,SLOT(syncDcMac()));
    connect(_ui->sudopassword_ledit,SIGNAL(editingFinished()),this,SLOT(syncSudoPswd()));

    connect(_ui->done_button,SIGNAL(clicked()),this,SLOT(onDone()));

}

void EngineSettingsDialog::saveSettings() {
    QSettings settings("Elysian Team",QString("ESToolkit ")+QString(ESTOOLKIT_VERSION));
    settings.setValue("engine/settings/dcIp",_ui->dcIpaddress_ledit->text());
    settings.setValue("engine/settings/dcMac",_ui->dcMacaddress_ledit->text());
    settings.setValue("engine/settings/x86GammaBin",_ui->gammaBin_edit->text());
    //settings.setValue("engine/setting/sudopwd",_ui->sudopassword_ledit->text()); //FUCKING ETHICS, MAN
}
void EngineSettingsDialog::restoreSettings() {
    QSettings settings("Elysian Team",QString("ESToolkit ")+QString(ESTOOLKIT_VERSION));
    setDcIp(settings.value("engine/settings/dcIp").toString());
    setDcMac(settings.value("engine/settings/dcMac").toString());
    setEnginePathx86(settings.value("engine/settings/x86GammaBin", ENGINE_DIALOG_DEFAULT_PATH).toString());
    //_sudoPswd=settings.value("engine/setting/sudopwd").toString();//FUCKING ETHICS, MAN
    syncUI();
}
void EngineSettingsDialog::syncUI() {
    _ui->dcIpaddress_ledit->setText(_dcIpAddr);
    _ui->dcMacaddress_ledit->setText(_dcMacAddr);
    _ui->sudopassword_ledit->setText(_sudoPswd);
    _ui->gammaBin_edit->setText(_enginePathx86);
}
void EngineSettingsDialog::syncDcMac() {
    setDcMac(_ui->dcMacaddress_ledit->text());
}
void EngineSettingsDialog::syncDcIp() {
    setDcIp(_ui->dcIpaddress_ledit->text());
}
void EngineSettingsDialog::syncSudoPswd() {
    setSudoPswd(_ui->sudopassword_ledit->text());
}
void EngineSettingsDialog::syncEnginePathx86() {
    setEnginePathx86(_ui->gammaBin_edit->text());
}

void EngineSettingsDialog::onDone(){
    if(_ui->savesettings_cbox->isChecked()) saveSettings();
    close();
}
QString EngineSettingsDialog::getEnginePathx86() {
    return _enginePathx86;
}

QString EngineSettingsDialog::getDcIp() {
    return _dcIpAddr;
}

QString EngineSettingsDialog::getDcMac() {
    return _dcMacAddr;
}

QString EngineSettingsDialog::getSudoPswd() {
    return _sudoPswd;
}

void EngineSettingsDialog::setDcIp(QString ip) {
    if(_dcIpAddr==ip) {
        //they were the same, avoid infinite looping
        return;
    } else  {
        _dcIpAddr=ip;
        emit dcIpChanged(_dcIpAddr);
    }
}
void EngineSettingsDialog::setDcMac(QString mac) {
    if(_dcMacAddr==mac) {
        //they were the same, avoid infinite looping
        return;
    } else  {
        _dcMacAddr=mac;
        emit dcMacChanged(_dcMacAddr);
    }
}
void EngineSettingsDialog::setSudoPswd(QString pswd) {
    if(_sudoPswd==pswd) {
        //they were the same, avoid infinite looping
        return;
    } else  {
        _sudoPswd=pswd;
        emit sudoPswdChanged(_sudoPswd);
    }
}

void EngineSettingsDialog::setEnginePathx86(QString path) {
    if(_enginePathx86==path) {
        //they were the same, avoid infinite looping
        return;
    } else  {
        _enginePathx86=path;
        if(_ui->gammaBin_edit->text() != path) _ui->gammaBin_edit->setText(path);
        emit enginePathx86Changed(path);
    }
}

void EngineSettingsDialog::onEnginePathx86Default(void) {
    setEnginePathx86(ENGINE_DIALOG_DEFAULT_PATH);
    if(_ui->gammaBin_edit->text() != ENGINE_DIALOG_DEFAULT_PATH) _ui->gammaBin_edit->setText(ENGINE_DIALOG_DEFAULT_PATH);
}

void EngineSettingsDialog::onEnginePathx86Browse(void) {
    QString filename;

    filename = QFileDialog::getOpenFileName(this, tr("Select ESGamma Binary"));

    if(filename.isEmpty()) return;


    setEnginePathx86(filename);

}

