#ifndef ENGINE_SETTINGS_DIALOG_H
#define ENGINE_SETTINGS_DIALOG_H

#include <QDialog>

namespace Ui {
    class engine_settings_dialog;
}
class EngineSettingsDialog : public QDialog {
    Q_OBJECT
    Q_PROPERTY(QString _dcIpAddr READ getDcIp WRITE setDcIp NOTIFY dcIpChanged)
    Q_PROPERTY(QString _dcMacAddr READ getDcMac WRITE setDcMac NOTIFY dcMacChanged)
    Q_PROPERTY(QString _sudoPswd READ getSudoPswd WRITE setSudoPswd NOTIFY sudoPswdChanged)
signals:
    void dcIpChanged(QString);
    void dcMacChanged(QString);
    void sudoPswdChanged(QString);
public:
    EngineSettingsDialog(QWidget *parent=0);
    void restoreSettings();
    void saveSettings();
    void syncUI();

    QString getDcIp();
    QString getDcMac();
    QString getSudoPswd();

    void setDcIp(QString ip);
    void setDcMac(QString mac);
    void setSudoPswd(QString pswd);
private:
    Ui::engine_settings_dialog *_ui;
    QString _dcIpAddr, _dcMacAddr, _sudoPswd;
public slots:
    void syncDcIp();
    void syncDcMac();
    void syncSudoPswd();

    void onDone();
};


#endif // ENGINE_DIALOG_H
