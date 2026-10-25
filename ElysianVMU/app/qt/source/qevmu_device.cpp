#include <ElysianVMU/qevmu_device.hpp>

//===== STATIX ======
QPixmap QtVmuDevice::getBuiltinVolumeIcon(unsigned index) {
    if(index <= VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX) {
        return QPixmap(QString(ELYSIAN_VMU_DEVICE_BUILTIN_ICON_RSRC_DIR)
                       + QString("%1.png").arg(index));

    } else return QPixmap();
}


QPixmap QtVmuDevice::getVolumeDcIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();

    if(!iconDataEntry.isNull()) {
        return VmuQtUtils::getIconDataBank().getEntry(_dev, iconDataEntry)->dcIcon;
    } else return getBuiltinVolumeIcon(getFlashRootBlock()->iconShape);
}

QPixmap QtVmuDevice::getVolumeVmuIcon(void) const {
    auto iconDataEntry = getIconDataVmsFlashDirEntry();

    if(!iconDataEntry.isNull()) {
        return VmuQtUtils::getIconDataBank().getEntry(_dev, iconDataEntry)->vmuIcon;
    } else return getBuiltinVolumeIcon(getFlashRootBlock()->iconShape);
}

QPixmap QtVmuDevice::getExtraBgPvrTexture(void) const {
    auto extraBgPvrEntry = getExtraBgPvrFlashDirEntry();
    auto* bgEntry = VmuQtUtils::getExtraBgPvrBank().getEntry(_dev, extraBgPvrEntry);
    return bgEntry->image;
}


QColor QtVmuDevice::getVolumeColor(void) const {
    VMUFlashRootBlock* root = getFlashRootBlock();
    return QColor(root->volumeLabel.vmu.r,
                  root->volumeLabel.vmu.g,
                  root->volumeLabel.vmu.b,
                  root->volumeLabel.vmu.a);
}

void QtVmuDevice::setVolumeColor(QColor color) {
    int r, g, b, a;
    color.getRgb(&r, &g, &b, &a);
    VMUFlashRootBlock* root = getFlashRootBlock();
    root->volumeLabel.vmu.a = a;
    root->volumeLabel.vmu.r = r;
    root->volumeLabel.vmu.g = g;
    root->volumeLabel.vmu.b = b;
}
