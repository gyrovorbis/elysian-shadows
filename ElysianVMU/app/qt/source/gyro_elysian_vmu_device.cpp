#include <ElysianVMU/gyro_elysian_vmu_device.hpp>
#include <libGyro/gyro_system_api.h>
#include <QESTkUtils/tk_debug_log_stack.h>


bool EVmuDevice::saveState(QString path) const {
    return gyVmuDeviceSaveState(_dev, Q_CSTR(path));
}

bool EVmuDevice::loadState(QString path) const {
    return gyVmuDeviceLoadState(_dev, Q_CSTR(path));
}


QPixmap EVmuDevice::getVolumeDcIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();

    if(!iconDataEntry.isNull()) {
        return VmuQtUtils::getIconDataBank().getEntry(_dev, iconDataEntry)->dcIcon;
    } else return QPixmap();
}

QPixmap EVmuDevice::getVolumeVmuIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();

    if(!iconDataEntry.isNull()) {
        return VmuQtUtils::getIconDataBank().getEntry(_dev, iconDataEntry)->vmuIcon;
    } else return QPixmap();
}

QPixmap EVmuDevice::getExtraBgPvrTexture(void) const {
    auto extraBgPvrEntry = getExtraBgPvrFlashDirEntry();
    auto* bgEntry = VmuQtUtils::getExtraBgPvrBank().getEntry(_dev, extraBgPvrEntry);
    return bgEntry->image;
}


