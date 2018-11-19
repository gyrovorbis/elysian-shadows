#include <ElysianVMU/gyro_elysian_vmu_flash.hpp>
#include <evmu-core/gyro_vmu_icondata.h>
#include <evmu-core/gyro_vmu_util.h>
#include <evmu-core/gyro_vmu_pso_img.h>

bool EVmuFlashDirEntry::isIconDataVms(void) const {
    return (getFileName() == VMU_ICONDATA_VMS_FILE_NAME);
}

bool EVmuFlashDirEntry::isExtraBgPvr(void) const {
    return (getFileName() == GYRO_VMU_EXTRA_BG_PVR_FILE_NAME);
}

bool EVmuFlashDirEntry::isPsoImg(void) const {
    return (getFileName() == VMU_PSO_IMG_FILE_NAME);
}

QString EVmuFlashDirEntry::getFileTypeStr(void) const {
    switch(getFileType()) {
    case VMU_FLASH_FILE_TYPE_DATA: return "DATA";
    case VMU_FLASH_FILE_TYPE_GAME: return "GAME";
    case VMU_FLASH_FILE_TYPE_NONE: return "NONE";
    default:                       return "INVALID";
    }
}

QString EVmuFlashDirEntry::getCreationDateStr(void) const {
    return QString("%1/%2/%3 %4:%5").
                arg(gyVmuFromBCD(_dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_MONTH])).
                arg(gyVmuFromBCD(_dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_DAY])).
                arg(gyVmuFromBCD(_dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_YEAR])).
                arg(gyVmuFromBCD(_dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_HOUR])).
                arg(gyVmuFromBCD(_dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_MINUTE]));
}

QString EVmuFlashDirEntry::getCreationDayOfWeekStr(void) const {
    return gyVmuWeekDayStr(_dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_WEEKDAY]);
}


QPixmap EVmuFlashDirEntry::getFileIcon(void) const {
    if(!isIconDataVms()) {
        return VmuAnimatedIconCache::getIcon(_dev, _dirEntry);
    }
}

QPixmap EVmuFlashDirEntry::getEyeCatch(void) const {
    if(!isIconDataVms()) {
        return VmuEyecatchCache::getIcon(_dev, _dirEntry);
    }
}

bool EVmuFlashDirEntry::readFile(uint8_t* buffer) const {
    return gyVmuFlashFileRead(_dev, _dirEntry, buffer, 1);
}

QString EVmuIconDataVmsFileInfo::getVmuDescription(void) const {
    char buffer[VMU_ICONDATA_DESC_SIZE+1] = { '\0' };
    memcpy(buffer, _info->desc, VMU_ICONDATA_DESC_SIZE);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

bool EVmuIconDataVmsFileInfo::isSecretBiosUnlocked(void) const {
    return gyVmuIconDataSecretBiosUnlocked(_info);
}
void EVmuIconDataVmsFileInfo::setSecretBiosUnlocked(bool unlocked) const {
    gyVmuIconDataSecretBiosSetUnlocked(_info, unlocked);
}

const VmuPsoImgBankEntry* EVmuFlashDirEntry::getPsoImgBankEntry(void) const {
    return isPsoImg()? VmuQtUtils::getPsoImgBank().getEntry(_dev, _dirEntry) : nullptr;
}

const uint8_t* EVmuIconDataVmsFileInfo::getVmuIconData(void) const {
    return gyVmuIconDataVmuIconData(_info);
}
const uint8_t* EVmuIconDataVmsFileInfo::getDcIconData(void) const {
    return gyVmuIconDataDcIconData(_info);
}
const uint16_t* EVmuIconDataVmsFileInfo::getDcPalette(void) const {
    return gyVmuIconDataDcPalette(_info);
}

uint16_t* EVmuIconDataVmsFileInfo::createVmuIconImageArgb4444(void) const {
    return gyVmuIconDataVmuIconImageARGB4444(_info);
}
uint16_t* EVmuIconDataVmsFileInfo::createDcIconImageArgb4444(void) const {
    return gyVmuIconDataDcIconImageARGB4444(_info);
}

size_t EVmuIconDataVmsFileInfo::getVmuIconOffset(void) const {
    return _info->vmuIconOffset;
}
size_t EVmuIconDataVmsFileInfo::getDcIconOffset(void) const {
    return _info->dcIconOffset;
}
