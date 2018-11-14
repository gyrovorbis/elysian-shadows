#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <evmu-core/gyro_vmu_flash.h>
#include <QTextCodec>
#include <QDateTime>
#include <QImage>
#include <gif/qgifimage.h>
#include <evmu-core/gyro_vmu_vms.h>
#include <evmu-core/gyro_vmu_vmi.h>
#include <evmu-core/gyro_vmu_icondata.h>
#include <evmu-core/gyro_vmu_extra_bg_pvr.h>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <libGyro/gyro_system_api.h>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QDesktopServices>

QTextCodec*             VmuQtUtils::japCodec;
QStringList             VmuQtUtils::_supportedFileExtensions;
QString                 VmuQtUtils::_romDir;
VmuIconDataImageBank    VmuQtUtils::_iconDataBank;
VmuExtraBgPvrBank       VmuQtUtils::_extraBgPvrBank;
QString                 VmuQtUtils::_defaultImageTypeFilter = "Portable Network Graphics Format(*.png)"; //;;Jpeg Images (*.jpg);;GIF Images (*.gif);;Bitmap Images (*.bmp);;PPM Images (*.ppm)";

bool VmuQtUtils::initialize(void) {
    japCodec = QTextCodec::codecForName(VMU_TEXT_CODEC);

    _supportedFileExtensions << "vmu"
                             << "bin"
                             << "vmi"
                             << "vms"
                             << "dci"
                             << "dcm"
                             << "lcd";

    return true;
}

QMap<QString, VmuAnimatedIconCacheEntry*> VmuAnimatedIconCache::_iconCache;
QMap<QString, QPixmap> VmuEyecatchCache::_eyecatchCache;

bool VmuAnimatedIconCacheEntry::exportAsGif(QString path) {
    bool success = true;

    _gyLog(GY_DEBUG_VERBOSE, "Exporting icons to animated GIF: [%s]", Q_CSTR(path));
    _gyPush();

    QGifImage* gif = new QGifImage(QSize(VMU_VMS_ICON_BITMAP_WIDTH, VMU_VMS_ICON_BITMAP_HEIGHT));

    QVector<QRgb> gifPalette;
    for(int i = 0; i < VMU_VMS_ICON_PALETTE_SIZE; ++i) {
        uint16_t color = palette[i];
        auto rgb = qRgb(((color&VMU_VMS_ICON_PALETTE_RED_MASK)>>VMU_VMS_ICON_PALETTE_RED_POS)<<4,
               ((color&VMU_VMS_ICON_PALETTE_GREEN_MASK)>>VMU_VMS_ICON_PALETTE_GREEN_POS)<<4,
               ((color&VMU_VMS_ICON_PALETTE_BLUE_MASK)>>VMU_VMS_ICON_PALETTE_BLUE_POS)<<4
                );
        gifPalette.push_back(rgb);

        qDebug() << color;
    }

    gif->setGlobalColorTable(std::move(gifPalette), Qt::white);

    //gif->setDefaultTransparentColor(Qt::white);
    gif->setDefaultDelay(100);

    for(auto& icon : pixmaps) {
        gif->addFrame(icon.toImage(), animSpeed * 16);
    }

    if(gif->save(path)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        return true;
    }
    return false;
}


QPixmap VmuAnimatedIconCacheEntry::getPixmap(void) {
    qint64 curMs = QDateTime::currentMSecsSinceEpoch();
    qint64 delta = (curMs - lastMs);
    if(delta > animSpeed*16) {
        if(++currentFrame >= pixmaps.size()) {
            currentFrame = 0;
        }
        lastMs = curMs;
    }
    return pixmaps[currentFrame];
}

VmuAnimatedIconCacheEntry* VmuAnimatedIconCache::loadPixmaps(VMUDevice* dev, const VMUFlashDirEntry* dirEntry) {
    VmuAnimatedIconCacheEntry* entry    = nullptr;
    const VMSFileInfo* vmsHeader        = gyVmuFlashDirEntryVmsHeader(dev, dirEntry);
    uint16_t** iconsData                = gyVmuVMSFileInfoCreateIconsARGB444(dev, dirEntry);

    if(iconsData) {
        entry = new VmuAnimatedIconCacheEntry();
        entry->currentFrame = 0;
        entry->animSpeed = vmsHeader->animSpeed;
        entry->lastMs = QDateTime::currentMSecsSinceEpoch();
         memcpy(entry->palette, vmsHeader->palette, sizeof(uint16_t)*VMU_VMS_ICON_PALETTE_SIZE);
        for(unsigned i = 0; i < vmsHeader->iconCount; ++i) {
            QImage image((uchar*)iconsData[i], VMU_VMS_ICON_BITMAP_WIDTH, VMU_VMS_ICON_BITMAP_HEIGHT, QImage::Format_ARGB4444_Premultiplied);
            entry->pixmaps.push_back(QPixmap::fromImage(image) /*.scaled(21, 21, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)*/);
        }

    }
    return entry;
}


VmuAnimatedIconCacheEntry* VmuAnimatedIconCache::getEntry(VMUDevice* dev, const VMUFlashDirEntry* dirEntry) {
    char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1];
    gyVmuFlashDirEntryName(dirEntry, buffer);
    QString name(buffer);
    auto it = _iconCache.find(name);
    if(it != _iconCache.end()) {
        return *it;
    } else {
        return nullptr;
    }
}

QPixmap VmuAnimatedIconCache::getIcon(VMUDevice* dev, const VMUFlashDirEntry* dirEntry, VmuAnimatedIconCacheEntry** newEntry) {
    VmuAnimatedIconCacheEntry* entry;

    if(!(entry = getEntry(dev, dirEntry))) {
        char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1];
        gyVmuFlashDirEntryName(dirEntry, buffer);
        QString name(buffer);
        entry = loadPixmaps(dev, dirEntry);
        _iconCache[name] = entry;
    }

    if(newEntry) *newEntry = entry;
    return entry? entry->getPixmap() : QPixmap();
}

bool VmuAnimatedIconCache::exportAsGIF(const VMUFlashDirEntry* dirEntry, QString filename) {
    char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1];
    gyVmuFlashDirEntryName(dirEntry, buffer);
    QString name(buffer);
    VmuAnimatedIconCacheEntry* entry;
    auto it = _iconCache.find(name);

    if(it != _iconCache.end()) {
        entry = *it;
        return entry->exportAsGif(filename);
    } else {
        _gyLog(GY_DEBUG_ERROR, "Icons could not be exported to animated GIF, icon data was not found within cache!");
        return false;
    }
}

QPixmap VmuEyecatchCache::loadPixmap(VMUDevice* dev, const VMUFlashDirEntry* dirEntry) {
    QPixmap pixmap;
    const VMSFileInfo*  vmsHeader   = gyVmuFlashDirEntryVmsHeader(dev, dirEntry);
    uint16_t*           iconData    = gyVmuVMSFileInfoCreateEyeCatchARGB444(dev, dirEntry);

    if(iconData) {
        QImage image((uchar*)iconData, VMU_VMS_EYECATCH_BITMAP_WIDTH, VMU_VMS_EYECATCH_BITMAP_HEIGHT, QImage::Format_ARGB4444_Premultiplied);
        pixmap = QPixmap::fromImage(image);// /*.scaled(21, 21, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)*/);
    }
    return pixmap;
}

QPixmap VmuEyecatchCache::getIcon(VMUDevice* dev, const VMUFlashDirEntry* dirEntry) {
    QPixmap pixmap;
    char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1];
    gyVmuFlashDirEntryName(dirEntry, buffer);
    QString name(buffer);

    auto it = _eyecatchCache.find(name);
    if(it != _eyecatchCache.end()) {
        pixmap = *it;
    } else {
        pixmap = loadPixmap(dev, dirEntry);
        _eyecatchCache[name] = pixmap;

    }

    return pixmap;
}

bool VmuQtUtils::canOpenFile(QString filePath) {
    return _supportedFileExtensions.contains(filePath.split(".").last(), Qt::CaseInsensitive);
}

bool VmuEyecatchCache::exportAsPNG(const VMUFlashDirEntry* dirEntry, QString filename) {
    QPixmap pixmap;
    char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1];
    gyVmuFlashDirEntryName(dirEntry, buffer);
    QString name(buffer);

    auto it = _eyecatchCache.find(name);
    if(it != _eyecatchCache.end()) {
        pixmap = *it;
    } else {
        _gyLog(GY_DEBUG_ERROR, "Icons could not be exported to animated PNG, icon data was not found within cache!");
        return false;
    }

    if(pixmap.toImage().save(filename)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
        return true;
    }
    return false;

}

bool VmuQtUtils::loadVMUImage(VMUDevice* dev,
                              QString filepath,
                              VMUFlashDirEntry** entry,
                              bool replaceGame,
                              ReloadState reloadInfo)
{
    if(filepath.isEmpty()) return false;

    QFileInfo info(filepath);
    _romDir = info.absoluteDir().absolutePath();

    VMU_LOAD_IMAGE_STATUS stats;
    auto* dirEntry = gyVmuFlashLoadImage(dev, Q_CSTR(filepath), &stats);
    if(entry) *entry = dirEntry;

    std::function<bool(VMU_LOAD_IMAGE_STATUS, ReloadState)> statusMsg = [&](VMU_LOAD_IMAGE_STATUS status, ReloadState reloadState) {
        switch(status) {
        case VMU_LOAD_IMAGE_SUCCESS: return true;
        case VMU_LOAD_IMAGE_FILES_MAXED:
            QMessageBox::critical(nullptr, "Too many files", "The current VMU image already has the maximum allowable number of files.");
            return false;
        case VMU_LOAD_IMAGE_OPEN_FAILED:
            QMessageBox::critical(nullptr, "File Error", "ElysianVMU was unable to open the specified file!");
            return false;
        case VMU_LOAD_IMAGE_READ_FAILED:
            QMessageBox::critical(nullptr, "File Error", "ElysianVMU failed to read from the specified file!");
            return false;
        case VMU_LOAD_IMAGE_INADEQUATE_FREE_BLOCKS:
            QMessageBox::critical(nullptr, "Not Enough Space", QString(gyVmuFlashLoadImageLastErrorMessage()));
            return false;
        case VMU_LOAD_IMAGE_VMS_NO_VMI: {

            if(!reloadState.reloading) {
                reloadState.reloading = true;
                QMessageBox msgBox;
                msgBox.setText("VMI File Not Found!");
                msgBox.setInformativeText("A .VMS file requires a corresponding .VMI file describing its contents, and no VMI file was found. "
                                          "You can either manually specify the location of the VMI file, or"
                                          "(only in the case of GAME files), you can have a default one autogenerated for you.");

                QAbstractButton*   buttonCancel    = msgBox.addButton("Cancel",        QMessageBox::NoRole);
                QAbstractButton*   buttonGenerate  = msgBox.addButton("Autogenerate",  QMessageBox::NoRole);
                QAbstractButton*   buttonManual    = msgBox.addButton("Browse",        QMessageBox::NoRole);

                msgBox.exec();
                if(msgBox.clickedButton() == buttonManual) {
                    reloadState.vmiPath = QFileDialog::getOpenFileName(nullptr,
                                                                   "Browse for the VMI file",
                                                                   _romDir,
                                                                   "VMU metadata files (*.vmi *.VMI)");

                } else if(msgBox.clickedButton() == buttonGenerate) {
                    reloadState.autogenerate = true;
                } else {
                    return false;
                }
            }

            if(!reloadState.vmiPath.isEmpty()) {
                auto* entryRet = gyVmuFlashLoadImageVmiVms(dev, Q_CSTR(reloadState.vmiPath), Q_CSTR(filepath), &status);
                if(entry) *entry = entryRet;
                return statusMsg(status, reloadState);
            } else if(reloadState.autogenerate) {
                _gyLog(GY_DEBUG_VERBOSE, "Auto-generating VMI information [GAME]");
                _gyPush();
                VMIFileInfo vmi;
                size_t vmsFileSize = 0;
                uint8_t* vmsBytes = gyVmuFlashLoadVMS(Q_CSTR(filepath), &vmsFileSize);
                gyVmuVmiGenerateFromVms(&vmi, (const VMSFileInfo*)&vmsBytes[VMU_FLASH_GAME_VMS_HEADER_OFFSET], vmsFileSize, VMI_FILE_MODE_GAME_GAME);
                auto* entryRet = gyVmuFlashCreateFileVmiVms(dev, &vmi, vmsBytes, &status);
                if(entry) *entry = entryRet;
                _gyPop(1);
                return statusMsg(status, reloadState);
            }

            return false;

        }
        case VMU_LOAD_IMAGE_VMI_NO_VMS: {
            if(!reloadState.reloading) {
                reloadState.reloading = true;

                if(QMessageBox::question(nullptr, "VMS File Not Found",
                                    "A .VMI file simply describes and points to a .VMS file, which holds the actual data contents. "
                                    "No corresponding VMS file could be autodetected. Would you like to specify one manually?")
                        == QMessageBox::Yes)
                {
                    reloadState.vmsPath = QFileDialog::getOpenFileName(nullptr,
                                                                   "Browse for the VMS file",
                                                                   _romDir,
                                                                   "VMU data files (*.vms *.VMS)");

                }
            }

            if(!reloadState.vmsPath.isEmpty()) {
                auto* entryRet = gyVmuFlashLoadImageVmiVms(dev, Q_CSTR(filepath), Q_CSTR(reloadState.vmsPath), &status);
                if(entry) *entry = entryRet;
                return statusMsg(status, reloadState);
            }

            return false;

        }
        case VMU_LOAD_IMAGE_GAME_DUPLICATE:
            //Don't bitch if it's the only game file!!
            if(replaceGame && gyVmuFlashFileCount(dev) == 1) {
                auto* dirEntry = gyVmuFlashDirEntryGame(dev);
                if(gyVmuFlashFileDelete(dev, dirEntry)) {
                    return loadVMUImage(dev, filepath, entry, replaceGame, reloadState);
                }

            } else QMessageBox::critical(nullptr, "Game Exists", "The current VMU image already contains a GAME file, please delete it and try again.");
            return false;
        case VMU_LOAD_IMAGE_NAME_DUPLICATE:
            QMessageBox::critical(nullptr, "Duplicate File", "The image could not be loaded, because there is already a file in the filesystem with the same name. ");
            return false;
        case VMU_LOAD_IMAGE_UNKNOWN_FORMAT:
            QMessageBox::critical(nullptr, "Unknown Format", "The image could not be loaded, because it is in an unknown or unsupported file format!");
            return false;
        case VMU_LOAD_IMAGE_DEVICE_READ_ERROR:
        case VMU_LOAD_IMAGE_DEVICE_WRITE_ERROR:
        case VMU_LOAD_IMAGE_DEFRAG_FAILED:
            QMessageBox::critical(nullptr, "Failure", "The image couldn't be loaded due to an internal error. Please submit the debug log file and bug report.");
            return false;
        default:
            QMessageBox::critical(nullptr, "Failure", "The image couldn't be loaded due an unknown error.");
            return false;
        }
    };

    return statusMsg(stats, reloadInfo);
}


VmuIconDataBankKey VmuIconDataImageBank::createKey(VMUDevice* dev, const VMUFlashDirEntry* dirEntry) {
    Q_ASSERT(dev && dirEntry);
    return VmuIconDataBankKey {
        dirEntry,
        *gyVmuFlashBlockFATEntry(dev, dirEntry->firstBlock),
        gyVmuFlashDirEntryIndex(dev, dirEntry),
        gyVmuIconDataSize((IconDataFileInfo*)gyVmuFlashDirEntryVmsHeader(dev, dirEntry))
    };
}


VmuIconDataBankEntry* VmuIconDataImageBank::createEntry(VMUDevice* dev, const VMUFlashDirEntry* dirEntry) {
    Q_ASSERT(dev && dirEntry);

    _gyLog(GY_DEBUG_VERBOSE, "Loading ICONDATA_VMS Images.");
    _gyPush();

    VmuIconDataBankEntry*   entry       = nullptr;
    const VMSFileInfo*      vms         = gyVmuFlashDirEntryVmsHeader(dev, dirEntry);
    uint8_t*                dataBuffer  = nullptr;
    const uint8_t*          vmuIcnData  = nullptr;
    const uint8_t*          dcIcnData   = nullptr;
    const IconDataFileInfo* iconData    = (const IconDataFileInfo*)vms;
    size_t                  fileSize    = gyVmuIconDataSize(iconData);

    if(!vms) {
        _gyLog(GY_DEBUG_ERROR, "Failed to retrieve VMS file header!");
        goto end;
    }

    if(!fileSize) {
        _gyLog(GY_DEBUG_ERROR, "File size returned 0 bytes!");
        goto end;
    }

    entry = new VmuIconDataBankEntry;

    dataBuffer = new uint8_t[fileSize];
    if(gyVmuFlashFileReadBytes(dev, dirEntry, dataBuffer, fileSize, 0, 1)
            != fileSize)
    {
        _gyLog(GY_DEBUG_ERROR, "Failed to read file contents!");
        goto clean_buffer;
    }

    vmuIcnData = gyVmuIconDataVmuIconData(reinterpret_cast<const IconDataFileInfo*>(dataBuffer));
    if(!vmuIcnData) {
        _gyLog(GY_DEBUG_WARNING, "No Monochrome VMU Icon? Wtf? I guess that's valid...");
    } else {
        QImage image = iconDataVmuIconImage(dev, iconData);
        if(image.isNull()) {
            _gyLog(GY_DEBUG_ERROR, "Something was wrong with creating the VMU Icon image!");
        } else {
            entry->vmuIcon = QPixmap::fromImage(image);
        }
    }

    dcIcnData = gyVmuIconDataDcIconData(reinterpret_cast<const IconDataFileInfo*>(dataBuffer));
    if(!dcIcnData) {
        _gyLog(GY_DEBUG_VERBOSE, "No color DC Icon.");
    } else {
        QImage image = iconDataDcIconImage(dev, reinterpret_cast<const IconDataFileInfo*>(dataBuffer));
        if(image.isNull()) {
            _gyLog(GY_DEBUG_ERROR, "Something was wrong with creating the DC Icon image!");
        } else {
            entry->dcIcon = QPixmap::fromImage(image);
        }
    }

clean_buffer:
    delete[] dataBuffer;
end:
    _gyPop(1);
    return entry;
}




QImage VmuIconDataImageBank::iconDataDcIconImage(const VMUDevice* dev, const IconDataFileInfo* icnDat) {
    Q_ASSERT(dev && icnDat);

    QImage img(reinterpret_cast<const uint8_t*>(gyVmuIconDataDcIconImageARGB4444(icnDat)),
               VMU_VMS_ICON_BITMAP_WIDTH,
               VMU_VMS_ICON_BITMAP_HEIGHT,
               QImage::Format_ARGB4444_Premultiplied,
               [](void* data) {
                    free(data); //free the ElysianVMU image data when we're done with it.
                });
    return img;

}

QImage VmuIconDataImageBank::iconDataVmuIconImage(const VMUDevice* dev, const IconDataFileInfo* icnDat) {
   const  uint16_t* imgData = gyVmuIconDataVmuIconImageARGB4444(icnDat);

    QImage img(reinterpret_cast<const uint8_t*>(imgData),
               VMU_VMS_ICON_BITMAP_WIDTH,
               VMU_VMS_ICON_BITMAP_HEIGHT,
               QImage::Format_ARGB4444_Premultiplied,
               [](void* data) {
                    free(data); //free the ElysianVMU image data when we're done with it.
                });
    return img;
}


bool VmuIconDataBankEntry::exportVmuIcon(QString filename) const {
    bool success = true;
    _gyLog(GY_DEBUG_VERBOSE, "Exporting Custom Monochrome LCD VMU Icon: [%s]", Q_CSTR(filename));
    _gyPush();

    if(vmuIcon.toImage().save(filename)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));

    } else {
        _gyLog(GY_DEBUG_ERROR, "Failed to export image!");
    }

    _gyPop(1);
    return success;
}

bool VmuIconDataBankEntry::exportDcIcon(QString filename) const {
    bool success = true;
    _gyLog(GY_DEBUG_VERBOSE, "Exporting Custom Color VMU Volume Icon [%s]", Q_CSTR(filename));
    _gyPush();

    if(dcIcon.toImage().save(filename)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));

    } else {
        _gyLog(GY_DEBUG_ERROR, "Failed to export image!");
    }

    _gyPop(1);
    return success;
}


QImage VmuExtraBgPvrBank::extraBgPvrImage(const VMUDevice* dev, const VmuExtraBgPvrFileInfo* info) {
    _gyLog(GY_DEBUG_VERBOSE, "Creating QImage texture for EXTRA.BG.PVR!");
    _gyPush();

    const uint8_t* imgData = reinterpret_cast<const uint8_t*>(gyVmuExtraBgPvrImageRGB888(info));

    QImage img(reinterpret_cast<const uint8_t*>(imgData),
               info->graphicHeader->width,
               info->graphicHeader->height,
               QImage::Format_RGB888,
               [](void* data) {
                    free(data); //free the ElysianVMU image data when we're done with it.
                });

    img.save("~/pvr_bg.png");

    _gyPop(1);

    return img;
}

VmuExtraBgPvrBankKey VmuExtraBgPvrBank::createKey(VMUDevice* device, const VMUFlashDirEntry* dirEntry) {
    return VmuExtraBgPvrBankKey {
        dirEntry
    };
}

VmuExtraBgPvrBankEntry* VmuExtraBgPvrBank::createEntry(VMUDevice* device, const VMUFlashDirEntry* dirEntry) {
    QImage                  img;
    VmuExtraBgPvrBankEntry* entry       = nullptr;
    uint8_t*                vms         = nullptr;
    size_t                  bytesRead   = 0;
    size_t                  fileSize    = 0;

    _gyLog(GY_DEBUG_VERBOSE, "Creating Image Bank/Cache Entry for EXTRA.BG.PVR!");
    _gyPush();

    if(!dirEntry || !dirEntry->fileSize) {
        _gyLog(GY_DEBUG_ERROR, "Could not create entry for EXTRA.BG.PVR, because size of file on device is 0 bytes!");
        goto done;
    }

    fileSize = dirEntry->fileSize * VMU_FLASH_BLOCK_SIZE;
    vms = new uint8_t[fileSize];

    bytesRead = gyVmuFlashFileReadBytes(device, dirEntry, vms, fileSize, 0, 1);

    if(bytesRead != fileSize)
    {
        _gyLog(GY_DEBUG_ERROR, "Failed to read entire image contents! [read/total: %u/%u]", bytesRead, fileSize);
        goto done;
    }

    VmuExtraBgPvrFileInfo pvrInfo;
    gyVmuExtraBgPvrFileInfo(reinterpret_cast<const VMSFileInfo*>(vms), &pvrInfo);

    if(!pvrInfo.graphicHeader) {
        _gyLog(GY_DEBUG_ERROR, "Never found actual graphic header. Bailing.");
        goto done;
    }

    img = extraBgPvrImage(device, &pvrInfo);

    if(img.isNull()) {
        _gyLog(GY_DEBUG_ERROR, "Failed to create QImage from EXTRA.BG.PVR raw data!");
        goto done;
    }

    entry = new VmuExtraBgPvrBankEntry {
        QPixmap::fromImage(img)
    };

done:
    delete[] vms;
    _gyPop(1);
    return entry;
}


bool VmuExtraBgPvrBankEntry::exportImage(QString filepath) const {
    bool success = true;
    _gyLog(GY_DEBUG_VERBOSE, "Exporting EXTRA.BG.PVR: [%s]", Q_CSTR(filepath));
    _gyPush();

    if(image.toImage().save(filepath)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));

    } else {
        _gyLog(GY_DEBUG_ERROR, "Failed to export image!");
    }

    _gyPop(1);
    return success;
}
