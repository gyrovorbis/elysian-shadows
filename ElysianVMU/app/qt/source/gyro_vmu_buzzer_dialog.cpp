/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_vmu_buzzer_dialog.hpp>
#include "ui_gyro_vmu_buzzer_dialog.h"
#include <gyro_vmu_osc.h>
#include <gyro_vmu_buzzer.h>

VmuBuzzerDialog::VmuBuzzerDialog(QWidget* parent):
    QDialog(parent),
    _ui(new Ui::VmuBuzzerDialog)
{
    _ui->setupUi(this);
    setRecommended(true);
    setPeriod(2);
}

VmuBuzzerDialog::~VmuBuzzerDialog(void) {
    delete _ui;
}

void VmuBuzzerDialog::closeEvent(QCloseEvent* event) {
    stopRaw();
}

void VmuBuzzerDialog::saveSettings(QSettings& /*settings*/) {

}

void VmuBuzzerDialog::loadSettings(QSettings& /*settings*/) {

}

bool VmuBuzzerDialog::isPlaying(void) const {
    if(!_dev) return false;
    return gyVmuBuzzerIsSoundPlaying(_dev);
}

uint8_t VmuBuzzerDialog::getDutyCycle(void) const {
    return _ui->dutyCycleSlider->value();
}
void VmuBuzzerDialog::setDutyCycle(uint8_t ms) {
    if(_ui->dutyCycleSlider->value() != ms) _ui->dutyCycleSlider->setValue(ms);
    if(_ui->dutyCycleSpinBox->value() != ms) _ui->dutyCycleSpinBox->setValue(ms);
    if(isPlaying()) playRaw();
    _ui->t1lcValueLabel->setText(QString::number(ms+(256-getPeriod()), 16));
}
uint8_t VmuBuzzerDialog::getPeriod(void) const {
    return _ui->periodSlider->value();
}
void VmuBuzzerDialog::setPeriod(uint8_t ms) {
    _ui->dutyCycleSlider->setMaximum(ms);

    if(getRecommended()) {
        setDutyCycle(ms/2.0f);
    }

    if(_ui->periodSlider->value() != ms) _ui->periodSlider->setValue(ms);
    if(_ui->periodSpinBox->value() != ms) _ui->periodSpinBox->setValue(ms);
    if(isPlaying()) playRaw();

    float freq = (float)((float)VMU_OSC_QUARTZ_FREQ_HZ/6.0f)/(float)(ms);
    _ui->frequencyLabel->setText(QString::number(freq));
    _ui->t1lrValueLabel->setText(QString::number(256-ms, 16));
}
bool VmuBuzzerDialog::getRepeating(void) const {
    return _ui->repeatCheckBox->isChecked();
}
void VmuBuzzerDialog::setRepeating(bool val) {
    _ui->repeatCheckBox->setChecked(val);
}
bool VmuBuzzerDialog::getRecommended(void) const {
    return _ui->recommendedCheckBox->isChecked();
}
void VmuBuzzerDialog::setRecommended(bool val) {
    _ui->recommendedCheckBox->setChecked(val);
}
bool VmuBuzzerDialog::getEmulateVolume(void) const {
    return _ui->volumeCheckBox->isChecked();
}
void VmuBuzzerDialog::setEmulateVolume(bool val) {
    _ui->volumeCheckBox->setChecked(val);
}

void VmuBuzzerDialog::on_recommendedCheckBox_clicked(bool checked) {
    if(checked) {
        _ui->dutyCycleSlider->setValue(_ui->periodSlider->value()/2.0f);
        _ui->dutyCycleSlider->setEnabled(false);
    } else {
        _ui->dutyCycleSlider->setEnabled(true);
    }
}

void VmuBuzzerDialog::on_repeatCheckBox_clicked(bool checked) {

}

void VmuBuzzerDialog::on_volumeCheckBox_clicked(bool checked) {

}

void VmuBuzzerDialog::on_stopPlayButton_toggled(bool checked) {
    if(checked) {
        _ui->stopPlayButton->setText("Stop");
        playRaw();
    } else {
        _ui->stopPlayButton->setText("Play");
        stopRaw();
    }
}

void VmuBuzzerDialog::on_closeButton_clicked(void) {
    close();
}

void VmuBuzzerDialog::playRaw(void) {
    uint8_t newPeriod = getPeriod();
    uint8_t newDutyCycle = getDutyCycle();
    if(_rawPeriod != newPeriod || _rawDutyCycle != newDutyCycle) {
        gyVmuBuzzerSoundPlay(_dev, getPeriod(), getDutyCycle(), 1);
        _rawPeriod = newPeriod;
        _rawDutyCycle = newDutyCycle;
    }
}

void VmuBuzzerDialog::stopRaw(void) {
    gyVmuBuzzerSoundStop(_dev);
    _rawPeriod = 0;
    _rawDutyCycle = 0;
}

void VmuBuzzerDialog::on_periodSlider_valueChanged(int arg1) {
    setPeriod(arg1);
}

void VmuBuzzerDialog::on_dutyCycleSlider_valueChanged(int value) {
    setDutyCycle(value);
}

void VmuBuzzerDialog::on_periodSpinBox_valueChanged(int arg1)
{
    setPeriod(arg1);
}

void VmuBuzzerDialog::on_dutyCycleSpinBox_valueChanged(int arg1)
{
    setDutyCycle(arg1);
}


