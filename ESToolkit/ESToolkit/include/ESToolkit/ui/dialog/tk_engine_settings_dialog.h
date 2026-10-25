/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

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
    Q_PROPERTY(QString _enginePathx86 READ getEnginePathx86 WRITE setEnginePathx86 NOTIFY enginePathx86Changed)
Q_SIGNALS:
    void dcIpChanged(QString);
    void dcMacChanged(QString);
    void sudoPswdChanged(QString);
    void enginePathx86Changed(QString);
public:
    EngineSettingsDialog(QWidget *parent=0);
    void restoreSettings();
    void saveSettings();
    void syncUI();

    QString getDcIp();
    QString getDcMac();
    QString getSudoPswd();
    QString getEnginePathx86();

    void setDcIp(QString ip);
    void setDcMac(QString mac);
    void setSudoPswd(QString pswd);
    void setEnginePathx86(QString path);
private:
    Ui::engine_settings_dialog *_ui;
    QString _dcIpAddr, _dcMacAddr, _sudoPswd, _enginePathx86;
public Q_SLOTS:
    void syncDcIp();
    void syncDcMac();
    void syncSudoPswd();
    void syncEnginePathx86();

private Q_SLOTS:
      void onDone();
      void onEnginePathx86Default();
      void onEnginePathx86Browse();
};


#endif // ENGINE_DIALOG_H
