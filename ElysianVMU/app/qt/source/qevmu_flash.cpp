/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ElysianVMU/qevmu_flash.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <evmu-core/gyro_vmu_icondata.h>


const VmuPsoImgBankEntry* QtVmuFlashDirEntry::getPsoImgBankEntry(void) const {
    return isPsoImg()? VmuQtUtils::getPsoImgBank().getEntry(_dev, _dirEntry) : nullptr;
}

QPixmap QtVmuFlashDirEntry::getFileIcon(void) const {
    if(!isIconDataVms()) {
        return VmuAnimatedIconCache::getIcon(_dev, _dirEntry);
    }
}

QPixmap QtVmuFlashDirEntry::getEyeCatch(void) const {
    if(!isIconDataVms()) {
        return VmuEyecatchCache::getIcon(_dev, _dirEntry);
    }
}


QString QtVmuIconDataVmsFileInfo::getVmuDescription(void) const {
    char buffer[VMU_ICONDATA_DESC_SIZE+1] = { '\0' };
    memcpy(buffer, _info->desc, VMU_ICONDATA_DESC_SIZE);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}
