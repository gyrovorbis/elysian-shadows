#ifndef GYRO_ELYSIAN_VMU_FLASH_HPP
#define GYRO_ELYSIAN_VMU_FLASH_HPP

#include <stdint.h>
#include <stdlib.h>
#include <evmu-core/gyro_vmu_flash.h>
#include <evmu-core/gyro_vmu_extra_bg_pvr.h>
#include <evmu-core/gyro_vmu_vms.h>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <QString>
#include <QTextCodec>
#include <cassert>

class   EVmuDevice;
struct  VMUFlashDirEntry;
struct  VMSFileInfo;
struct  IconDataFileInfo;
struct  ExtraBgPvrFileInfo;
struct  VmuExtraBgPvrFileInfo;

class EVmuIconDataVmsFileInfo {
    IconDataFileInfo*   _info = nullptr;

public:
                    EVmuIconDataVmsFileInfo(IconDataFileInfo* info);
    bool            isNull(void) const;

    QString         getVmuDescription(void) const;

    bool            isSecretBiosUnlocked(void) const;
    void            setSecretBiosUnlocked(bool unlocked) const;

    size_t          getVmuIconOffset(void) const;
    size_t          getDcIconOffset(void) const;

    const uint8_t*  getVmuIconData(void) const;
    const uint8_t*  getDcIconData(void) const;
    const uint16_t* getDcPalette(void) const;

    uint16_t*       createVmuIconImageArgb4444(void) const;
    uint16_t*       createDcIconImageArgb4444(void) const;

    operator        IconDataFileInfo*(void) const;
    operator        bool(void) const;


};

class EVmuExtraBgPvrFileInfo {
public:
    VmuExtraBgPvrFileInfo   _info;

                            EVmuExtraBgPvrFileInfo(const VMSFileInfo* info);
    bool                    isNull(void) const;

    VMU_PVR_TEX_FMT         getTextureFormat(void) const;
    VMU_PVR_TEX_LAYOUT      getTextureLayout(void) const;
    uint16_t                getWidth(void) const;
    uint16_t                getHeight(void) const;

    const uint8_t*          getTextureData(void) const;
    const uint8_t*          createTextureImageRgb888(void) const;

    operator                VmuExtraBgPvrFileInfo(void) const;
    operator                bool(void) const;

};

class EVmuVmsHeader {
private:
    VMSFileInfo*        _info = nullptr;

public:
                        EVmuVmsHeader(VMSFileInfo* info);
     bool               isNull(void) const;

    size_t              getHeaderSize(void) const;
    size_t              getPayloadSize(void) const;

    bool                copyPayloadData(uint8_t* buffer) const;

    //Properties
    QString             getVmuDescription(void) const;
    void                setVmuDescription(QString desc) const;

    QString             getDcDescription(void) const;
    void                setDcDescription(QString desc) const;

    QString             getCreatorApp(void) const;
    void                setCreatorApp(QString app);

    uint16_t            getCrc(void) const;
    void                setCrc(uint16_t value) const;

    //Icons / Eyecatch
    uint16_t            getIconCount(void) const;
    uint16_t            getAnimSpeed(void) const;
    bool                hasEyeCatch(void) const;
    VMS_EYECATCH_MODE   getEyeCatchType(void) const;

    /* This shit requires the data to be stored consecutively, so we
     * would have to self-extract, and fuck that for now
     * uint16_t            getPaletteEntry(unsigned index) const;
     * uint16_t**          createIconsImagesArgb444(void) const;
     * uint16_t*           createEyeCatchArgb444(void) const;
     */

    operator            VMSFileInfo*(void) const;
    operator            bool(void) const;

};






class EVmuFlashDirEntry {
private:
    VMUDevice*              _dev 		= nullptr;
    VMUFlashDirEntry*       _dirEntry 	= nullptr;
public:
                            EVmuFlashDirEntry(VMUDevice* dev, VMUFlashDirEntry* dirEntry);

    bool                    isNull(void) const;

    bool					isIconDataVms(void) const;
    bool					isExtraBgPvr(void) const;
    bool                    isPsoImg(void) const;
    bool					isGame(void) const;
    bool					isData(void) const;

    EVmuVmsHeader			getVmsHeader(void) const;
    EVmuIconDataVmsFileInfo	getIconDataFileInfo(void) const;
    EVmuExtraBgPvrFileInfo	getExtraBgPvrFileInfo(void) const;

    QPixmap                 getFileIcon(void) const;
    QPixmap                 getEyeCatch(void) const;
    const VmuPsoImgBankEntry*
                            getPsoImgBankEntry(void) const;

    void                    setCopyProtected(bool enabled) const;

    size_t					getFileSizeBytes(void) const;
    bool					isCrcValid(void) const;
    uint16_t				calculateCrc(void) const;
    void					fixCrc(void) const;

    //DirectoryEntry
    unsigned				getFileIndex(void) const;
    VMU_FLASH_FILE_TYPE		getFileType(void) const;
    QString                 getFileTypeStr(void) const;
    QString					getFileName(void) const;
    QString                 getCreationDateStr(void) const;
    QString                 getCreationDayOfWeekStr(void) const;
    bool					isCopyProtected(void) const;
    uint16_t				getFirstBlock(void) const;
    uint16_t				getFileSizeBlocks(void) const;
    uint16_t				getHeaderBlockOffset(void) const;
    bool					deleteFile(void);


    bool                    readFile(uint8_t* buffer) const;
    size_t					readFileBytes(uint8_t* buffer, size_t bytes, size_t offset, bool includeHeader=true) const;
    uint8_t					readFileByte(uint32_t offset);
    bool					writeFileByte(uint32_t offset, uint8_t value);

    //QPixmap				getVmsIcon(unsigned index) const;
    //QPixmap				getEyecatch(void) const;
    //QPixmap				getIconDataVmuIcon(void) const;
    //QPixmap				getIconDataDcIcon(void) const;
    //QPixmap				getExtraBgPvrTexture(void) const;


    //Type-compatible with evmu-core C API
    operator                    VMUFlashDirEntry*(void) const;
    operator                    bool(void) const;
    const EVmuFlashDirEntry&    operator=(VMUFlashDirEntry* dirEntry);
};


inline bool EVmuFlashDirEntry::isNull(void) const {
    return !_dev || !_dirEntry || _dirEntry->fileType == VMU_FLASH_FILE_TYPE_NONE;
}

inline EVmuFlashDirEntry::EVmuFlashDirEntry(VMUDevice* dev, VMUFlashDirEntry* entry):
    _dev(dev),
    _dirEntry(entry)
{
    assert(static_cast<VMUDevice*>(dev));
}

inline EVmuFlashDirEntry::operator VMUFlashDirEntry *(void) const {
    return _dirEntry;
}

inline EVmuFlashDirEntry::operator bool(void) const {
    return !isNull();
}

inline QString EVmuFlashDirEntry::getFileName(void) const {
    char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1] = { '\0' };
    memcpy(buffer, _dirEntry->fileName, VMU_FLASH_DIRECTORY_FILE_NAME_SIZE);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

inline VMU_FLASH_FILE_TYPE EVmuFlashDirEntry::getFileType(void) const {
    return static_cast<VMU_FLASH_FILE_TYPE>(_dirEntry->fileType);
}

inline bool EVmuFlashDirEntry::isCopyProtected(void) const {
    return (_dirEntry->copyProtection == VMU_FLASH_COPY_PROTECTION_COPY_PROTECTED);
}

inline void EVmuFlashDirEntry::setCopyProtected(bool value) const {
    _dirEntry->copyProtection = value? VMU_FLASH_COPY_PROTECTION_COPY_PROTECTED :
                                       VMU_FLASH_COPY_PROTECTION_COPY_OK;
}

inline uint16_t EVmuFlashDirEntry::getFirstBlock(void) const {
    return _dirEntry->firstBlock;
}

inline uint16_t EVmuFlashDirEntry::getFileSizeBlocks(void) const {
    return _dirEntry->fileSize;
}
inline uint16_t EVmuFlashDirEntry::getHeaderBlockOffset(void) const {
    return _dirEntry->headerOffset;
}

inline bool EVmuFlashDirEntry::isGame(void) const {
    return _dirEntry->fileType == VMU_FLASH_FILE_TYPE_GAME;
}

inline bool	EVmuFlashDirEntry::isData(void) const {
    return _dirEntry->fileType == VMU_FLASH_FILE_TYPE_DATA;
}

inline EVmuVmsHeader EVmuFlashDirEntry::getVmsHeader(void) const {
    switch(getFileType()) {
    case VMU_FLASH_FILE_TYPE_GAME:
    case VMU_FLASH_FILE_TYPE_DATA:
        return isIconDataVms()? nullptr : gyVmuFlashDirEntryVmsHeader(_dev, _dirEntry);
    default:
        return nullptr;
    }
}

inline size_t EVmuFlashDirEntry::getFileSizeBytes(void) const {
    switch(getFileType()) {
    case VMU_FLASH_FILE_TYPE_GAME:
        return getFileSizeBlocks() * VMU_FLASH_BLOCK_SIZE;
    case VMU_FLASH_FILE_TYPE_DATA: {
        auto hdr = getVmsHeader();
        return hdr.getHeaderSize() + hdr.getPayloadSize();
    }
    default:
        return 0;
    }
}

inline bool	EVmuFlashDirEntry::isCrcValid(void) const {
    auto vms = getVmsHeader();
    if(!vms.isNull()) {
        return vms.getCrc() == gyVmuFlashFileCalculateCRC(_dev, _dirEntry);
    } else return false;
}

inline uint16_t	EVmuFlashDirEntry::calculateCrc(void) const {
    return gyVmuFlashFileCalculateCRC(_dev, _dirEntry);
}

inline void	EVmuFlashDirEntry::fixCrc(void) const {
    auto vms = getVmsHeader();
    if(!vms.isNull()) {
        vms.setCrc(gyVmuFlashFileCalculateCRC(_dev, _dirEntry));
    }
}

inline unsigned EVmuFlashDirEntry::getFileIndex(void) const {
   return gyVmuFlashDirEntryIndex(_dev, _dirEntry);
}

inline bool EVmuFlashDirEntry::deleteFile(void) {
    bool success = gyVmuFlashFileDelete(_dev, _dirEntry);
    _dirEntry = nullptr;
    return success;
}

inline EVmuIconDataVmsFileInfo	EVmuFlashDirEntry::getIconDataFileInfo(void) const {
    if(isIconDataVms()) {
        return EVmuIconDataVmsFileInfo(reinterpret_cast<IconDataFileInfo*>(gyVmuFlashDirEntryVmsHeader(_dev, _dirEntry)));
    } else return EVmuIconDataVmsFileInfo(nullptr);
}

inline EVmuExtraBgPvrFileInfo EVmuFlashDirEntry::getExtraBgPvrFileInfo(void) const {
    if(isExtraBgPvr()) {
        return EVmuExtraBgPvrFileInfo(getVmsHeader());
    } else return EVmuExtraBgPvrFileInfo(nullptr);
}



//===========   ICONDATA_VMS ===========
inline EVmuIconDataVmsFileInfo::EVmuIconDataVmsFileInfo(IconDataFileInfo* info):
    _info(info)
{}
inline bool EVmuIconDataVmsFileInfo::isNull(void) const {
    return !_info;
}
inline EVmuIconDataVmsFileInfo::operator bool(void) const {
    return !isNull();
}

inline EVmuIconDataVmsFileInfo::operator IconDataFileInfo*(void) const {
    return _info;
}


//============= EXTRA BG PVR ==============

inline EVmuExtraBgPvrFileInfo::EVmuExtraBgPvrFileInfo(const VMSFileInfo* vmsHeader) {
    if(vmsHeader) gyVmuExtraBgPvrFileInfo(vmsHeader, &_info);
    else memset(&_info, 0, sizeof(VmuExtraBgPvrFileInfo));
}


inline bool EVmuExtraBgPvrFileInfo::isNull(void) const {
    return !_info.graphicHeader;
}

inline VMU_PVR_TEX_FMT EVmuExtraBgPvrFileInfo::getTextureFormat(void) const {
    return static_cast<VMU_PVR_TEX_FMT>(_info.graphicHeader->texFormat);
}

inline VMU_PVR_TEX_LAYOUT EVmuExtraBgPvrFileInfo::getTextureLayout(void) const {
    return static_cast<VMU_PVR_TEX_LAYOUT>(_info.graphicHeader->texLayout);
}
inline uint16_t EVmuExtraBgPvrFileInfo::getWidth(void) const {
    return _info.graphicHeader->width;
}
inline uint16_t EVmuExtraBgPvrFileInfo::getHeight(void) const {
    return _info.graphicHeader->height;
}

inline EVmuExtraBgPvrFileInfo::operator VmuExtraBgPvrFileInfo(void) const {
    return _info;
};

inline EVmuExtraBgPvrFileInfo::operator bool(void) const {
    return !isNull();
}

inline const uint8_t* EVmuExtraBgPvrFileInfo::getTextureData(void) const {
    return gyVmuExtraBgPvrImageData(&_info);
}
inline const uint8_t* EVmuExtraBgPvrFileInfo::createTextureImageRgb888(void) const {
    return gyVmuExtraBgPvrImageRGB888(&_info);
}



//============== VMS HEADER =============


inline EVmuVmsHeader::EVmuVmsHeader(VMSFileInfo* info):
    _info(info) {}

inline bool EVmuVmsHeader::isNull(void) const {
    return !_info;
}

inline size_t EVmuVmsHeader::getHeaderSize(void) const {
    return gyVmuVmsFileInfoHeaderSize(_info);
}
inline size_t EVmuVmsHeader::getPayloadSize(void) const {
    return _info->dataBytes;
}

//Properties
inline QString EVmuVmsHeader::getVmuDescription(void) const {
    char buffer[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
    gyVmuVmsHeaderVmuDescGet(_info, buffer);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

inline void EVmuVmsHeader::setVmuDescription(QString desc) const {
    desc.truncate(VMU_VMS_FILE_INFO_VMU_DESC_SIZE);
    memcpy(_info->vmuDesc, Q_CSTR(desc), VMU_VMS_FILE_INFO_VMU_DESC_SIZE);
}

inline QString EVmuVmsHeader::getDcDescription(void) const {
    char buffer[VMU_VMS_FILE_INFO_DC_DESC_SIZE+1];
    gyVmuVmsHeaderDcDescGet(_info, buffer);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

inline void EVmuVmsHeader::setDcDescription(QString desc) const {
    desc.truncate(VMU_VMS_FILE_INFO_DC_DESC_SIZE);
    memcpy(_info->dcDesc, Q_CSTR(desc), VMU_VMS_FILE_INFO_DC_DESC_SIZE);
}

inline QString EVmuVmsHeader::getCreatorApp(void) const {
    char buffer[VMU_VMS_FILE_INFO_CREATOR_APP_SIZE+1];
    gyVmuVmsHeaderCreatorAppGet(_info, buffer);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}
inline void EVmuVmsHeader::setCreatorApp(QString app) {
    app.truncate(VMU_VMS_FILE_INFO_CREATOR_APP_SIZE);
    memcpy(_info->creatorApp, Q_CSTR(app), VMU_VMS_FILE_INFO_CREATOR_APP_SIZE);
}

inline uint16_t EVmuVmsHeader::getCrc(void) const {
    return _info->crc;
}
inline void EVmuVmsHeader::setCrc(uint16_t value) const {
    _info->crc = value;
}

inline uint16_t EVmuVmsHeader::getIconCount(void) const {
    return _info->iconCount;
}
inline uint16_t EVmuVmsHeader::getAnimSpeed(void) const {
    return _info->animSpeed;
}
inline bool EVmuVmsHeader::hasEyeCatch(void) const {
    return getEyeCatchType() != VMS_EYECATCH_NONE;
}
inline VMS_EYECATCH_MODE EVmuVmsHeader::getEyeCatchType(void) const {
    return static_cast<VMS_EYECATCH_MODE>(_info->eyecatchType);
}

inline EVmuVmsHeader::operator VMSFileInfo*(void) const {
    return _info;
}

inline EVmuVmsHeader::operator bool(void) const {
    return !isNull();
}



#endif // GYRO_ELYSIAN_VMU_FLASH_HPP
