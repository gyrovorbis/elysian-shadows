#include <ElysianVMU/gyro_elysian_vmu_device.hpp>
#include <libGyro/gyro_system_api.h>
#include <QESTkUtils/tk_debug_log_stack.h>

//===== STATIX ======
QPixmap EVmuDevice::getBuiltinVolumeIcon(unsigned index) {
    if(index <= VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX) {
        return QPixmap(QString(ELYSIAN_VMU_DEVICE_BUILTIN_ICON_RSRC_DIR)
                       + QString("%1.png").arg(index));

    } else return QPixmap();
}


//===== INSTANCED ======


bool EVmuDevice::saveState(QString path) const {
    return gyVmuDeviceSaveState(_dev, Q_CSTR(path));
}

bool EVmuDevice::loadState(QString path) const {
    return gyVmuDeviceLoadState(_dev, Q_CSTR(path));
}

bool EVmuDevice::hasCustomVolumeColor(void) const {
    return getFlashRootBlock()->volumeLabel.vmu.customColor;
}

bool EVmuDevice::hasCustomVolumeIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();
    if(iconDataEntry.isNull()) return false;
    if(iconDataEntry.getIconDataFileInfo().isNull()) return false;
    return iconDataEntry.getIconDataFileInfo().getDcIconData();
}

QPixmap EVmuDevice::getVolumeDcIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();

    if(!iconDataEntry.isNull()) {
        return VmuQtUtils::getIconDataBank().getEntry(_dev, iconDataEntry)->dcIcon;
    } else return getBuiltinVolumeIcon(getFlashRootBlock()->iconShape);
}

QPixmap EVmuDevice::getVolumeVmuIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();

    if(!iconDataEntry.isNull()) {
        return VmuQtUtils::getIconDataBank().getEntry(_dev, iconDataEntry)->vmuIcon;
    } else return getBuiltinVolumeIcon(getFlashRootBlock()->iconShape);
}

QPixmap EVmuDevice::getExtraBgPvrTexture(void) const {
    auto extraBgPvrEntry = getExtraBgPvrFlashDirEntry();
    auto* bgEntry = VmuQtUtils::getExtraBgPvrBank().getEntry(_dev, extraBgPvrEntry);
    return bgEntry->image;
}


uint16_t EVmuDevice::getVolumeIconShape(void) const {
    VMUFlashRootBlock* root = getFlashRootBlock();
    return root->iconShape;
}

void EVmuDevice::setVolumeIconShape(uint16_t index) {
    VMUFlashRootBlock* root = getFlashRootBlock();
    root->iconShape = index;
}


QColor EVmuDevice::getVolumeColor(void) const {
    VMUFlashRootBlock* root = getFlashRootBlock();
    return QColor(root->volumeLabel.vmu.r,
                  root->volumeLabel.vmu.g,
                  root->volumeLabel.vmu.b,
                  root->volumeLabel.vmu.a);
}

void EVmuDevice::setVolumeColor(QColor color) {
    int r, g, b, a;
    color.getRgb(&r, &g, &b, &a);
    VMUFlashRootBlock* root = getFlashRootBlock();
    root->volumeLabel.vmu.a = a;
    root->volumeLabel.vmu.r = r;
    root->volumeLabel.vmu.g = g;
    root->volumeLabel.vmu.b = b;
}
