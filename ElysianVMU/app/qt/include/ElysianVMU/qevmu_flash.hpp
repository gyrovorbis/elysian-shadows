#ifndef QEVMU_FLASH_HPP
#define QEVMU_FLASH_HPP

#include <evmu-core-cpp/evmu_flash.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <QTextCodec>



class QtVmuIconDataVmsFileInfo: public evmu::VmuIconDataVmsFileInfo {

public:
                    QtVmuIconDataVmsFileInfo(const evmu::VmuIconDataVmsFileInfo& info);

    QString         getVmuDescription(void) const;

};

class QtVmuVmsHeader: public evmu::VmuVmsHeader {
public:

                        QtVmuVmsHeader(VMSFileInfo* info);
                        QtVmuVmsHeader(const evmu::VmuVmsHeader& rhs);

    QString             getVmuDescription(void) const;
    void                setVmuDescription(QString desc) const;

    QString             getDcDescription(void) const;
    void                setDcDescription(QString desc) const;

    QString             getCreatorApp(void) const;
    void                setCreatorApp(QString app);

};

class QtVmuFlashDirEntry: public evmu::VmuFlashDirEntry {
public:

                            QtVmuFlashDirEntry(VMUDevice* dev, VMUFlashDirEntry* dirEntry);
                            QtVmuFlashDirEntry(const evmu::VmuFlashDirEntry& rhs);

    QPixmap                 getFileIcon(void) const;
    QPixmap                 getEyeCatch(void) const;
    const VmuPsoImgBankEntry*
                            getPsoImgBankEntry(void) const;

    QString					getFileName(void) const;
    QtVmuVmsHeader			getVmsHeader(void) const;

};




inline QtVmuVmsHeader::QtVmuVmsHeader(const evmu::VmuVmsHeader& rhs):
    evmu::VmuVmsHeader(rhs)
{}

inline QtVmuVmsHeader::QtVmuVmsHeader(VMSFileInfo* info):
    evmu::VmuVmsHeader(info)
{}

inline QtVmuIconDataVmsFileInfo::QtVmuIconDataVmsFileInfo(const evmu::VmuIconDataVmsFileInfo& info):
    evmu::VmuIconDataVmsFileInfo(info)
{}

inline QtVmuFlashDirEntry::QtVmuFlashDirEntry(VMUDevice* dev, VMUFlashDirEntry* dirEntry):
    evmu::VmuFlashDirEntry(dev, dirEntry) {}


inline QtVmuFlashDirEntry::QtVmuFlashDirEntry(const evmu::VmuFlashDirEntry& rhs):
    evmu::VmuFlashDirEntry(rhs) {}

inline QString QtVmuFlashDirEntry::getFileName(void) const {
    char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1] = { '\0' };
    memcpy(buffer, _dirEntry->fileName, VMU_FLASH_DIRECTORY_FILE_NAME_SIZE);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

inline QString QtVmuVmsHeader::getVmuDescription(void) const {
    char buffer[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
    gyVmuVmsHeaderVmuDescGet(_info, buffer);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

inline void QtVmuVmsHeader::setVmuDescription(QString desc) const {
    desc.truncate(VMU_VMS_FILE_INFO_VMU_DESC_SIZE);
    memcpy(_info->vmuDesc, Q_CSTR(desc), VMU_VMS_FILE_INFO_VMU_DESC_SIZE);
}

inline QString QtVmuVmsHeader::getDcDescription(void) const {
    char buffer[VMU_VMS_FILE_INFO_DC_DESC_SIZE+1];
    gyVmuVmsHeaderDcDescGet(_info, buffer);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}

inline void QtVmuVmsHeader::setDcDescription(QString desc) const {
    desc.truncate(VMU_VMS_FILE_INFO_DC_DESC_SIZE);
    memcpy(_info->dcDesc, Q_CSTR(desc), VMU_VMS_FILE_INFO_DC_DESC_SIZE);
}

inline QString QtVmuVmsHeader::getCreatorApp(void) const {
    char buffer[VMU_VMS_FILE_INFO_CREATOR_APP_SIZE+1];
    gyVmuVmsHeaderCreatorAppGet(_info, buffer);
    return VmuQtUtils::japCodec->toUnicode(buffer);
}
inline void QtVmuVmsHeader::setCreatorApp(QString app) {
    app.truncate(VMU_VMS_FILE_INFO_CREATOR_APP_SIZE);
    memcpy(_info->creatorApp, Q_CSTR(app), VMU_VMS_FILE_INFO_CREATOR_APP_SIZE);
}
inline QtVmuVmsHeader QtVmuFlashDirEntry::getVmsHeader(void) const {
    switch(getFileType()) {
    case VMU_FLASH_FILE_TYPE_GAME:
    case VMU_FLASH_FILE_TYPE_DATA:
        return isIconDataVms()? nullptr : gyVmuFlashDirEntryVmsHeader(_dev, _dirEntry);
    default:
        return nullptr;
    }
}




#endif // QEVMU_FLASH_HPP
