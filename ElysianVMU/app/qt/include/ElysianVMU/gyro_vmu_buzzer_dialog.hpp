/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VMU_BUZZER_DIALOG_HPP
#define GYRO_VMU_BUZZER_DIALOG_HPP

#include <QDialog>

class   QSettings;
struct  VMUDevice;

namespace Ui {
    class VmuBuzzerDialog;
}

class VmuBuzzerDialog : public QDialog {
    Q_OBJECT
public:
    explicit        VmuBuzzerDialog(QWidget* parent = nullptr);
    virtual         ~VmuBuzzerDialog(void);

    virtual void    closeEvent(QCloseEvent *event) override;

    void            saveSettings(QSettings& settings);
    void            loadSettings(QSettings& settings);

    void            setVmuDevice(VMUDevice* dev);
    VMUDevice*      getVmuDevice(void) const;

    uint8_t         getDutyCycle(void) const;
    void            setDutyCycle(uint8_t ms);
    uint8_t         getPeriod(void) const;
    void            setPeriod(uint8_t ms);
    bool            getRepeating(void) const;
    void            setRepeating(bool val);
    bool            getRecommended(void) const;
    void            setRecommended(bool val);
    bool            getEmulateVolume(void) const;
    void            setEmulateVolume(bool val);
    bool            isPlaying(void) const;

    void            playRaw(void);
    void            stopRaw(void);

private Q_SLOTS:

    void            on_recommendedCheckBox_clicked(bool checked);
    void            on_repeatCheckBox_clicked(bool checked);
    void            on_volumeCheckBox_clicked(bool checked);
    void            on_stopPlayButton_toggled(bool checked);
    void            on_closeButton_clicked(void);
    void            on_periodSlider_valueChanged(int value);
    void            on_dutyCycleSlider_valueChanged(int value);
    void            on_dutyCycleSpinBox_valueChanged(int arg1);
    void            on_periodSpinBox_valueChanged(int arg1);

private:
    VMUDevice*          _dev            = nullptr;
    Ui::VmuBuzzerDialog*_ui             = nullptr;
    uint8_t             _rawPeriod      = 0;
    uint8_t             _rawDutyCycle   = 0;

};







//========= INLINEZ =========
inline void VmuBuzzerDialog::setVmuDevice(VMUDevice* dev) {
    _dev = dev;
}

inline VMUDevice* VmuBuzzerDialog::getVmuDevice(void) const {
    return _dev;
}



#endif
