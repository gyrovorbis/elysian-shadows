#include <ElysianVMU/gyro_vmu_filesystem_model.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <evmu-core/gyro_vmu_flash.h>
#include <evmu-core/gyro_vmu_util.h>
#include <evmu-core/gyro_vmu_icondata.h>
#include <evmu-core/gyro_vmu_extra_bg_pvr.h>
#include <evmu-core/gyro_vmu_vmi.h>
#include <QTextCodec>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QESTkUtils/tk_menu_stack.h>
#include <libGyro/gyro_system_api.h>
#include <QMimeData>
#include <QFileInfo>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <QFileDialog>
#include <ElysianVMU/gyro_vmu_main_window.hpp>
#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QHeaderView>
#include <QMessageBox>
#include <ElysianVMU/qevmu_flash.hpp>

VmuFileSystemTableModel::VmuFileSystemTableModel(VMUDevice* dev, QObject* parent):
    QAbstractTableModel(parent),
    _dev(dev),
    _animTimer(new QTimer)
{
    _animTimer->setInterval(VMU_ICON_UPDATE_ANIMATION_INTERVAL_MS);
    connect(_animTimer, SIGNAL(timeout()), this, SLOT(_updateIcons()));
    if(dev) _animTimer->start();
}

void VmuFileSystemTableModel::setDevice(VMUDevice* dev) {
    _dev = dev;
    if(dev) _animTimer->start();
    else _animTimer->stop();
}


//Overridden from QAbstractTableModels
//(read-only methods)
int VmuFileSystemTableModel::rowCount(const QModelIndex& parent) const {
    return _rowCount;
}

int VmuFileSystemTableModel::columnCount(const QModelIndex& parent) const {
    return COL_COUNT;
}

QVariant VmuFileSystemTableModel::data(const QModelIndex& index, int role) const {
    int row = index.row();

    auto dirEntry   = QtVmuFlashDirEntry(_dev.getFileFlashDirEntry(row));
    if(!dirEntry) return QVariant();

    auto vmsHeader  = QtVmuVmsHeader(dirEntry.getVmsHeader());
    auto iconData   = QtVmuIconDataVmsFileInfo(dirEntry.getIconDataFileInfo());
    auto extraBgPvr = dirEntry.isExtraBgPvr();
    //if(!vmsHeader) return QVariant(); IconData has no VMS header!!!

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case COL_NAME:
            if(iconData) return VMU_ICONDATA_VMS_FILE_NAME;
            else return dirEntry.getFileName();

        case COL_VM_DESC:
            return iconData? iconData.getVmuDescription() :
                             vmsHeader.getVmuDescription();

        case COL_DC_DESC:
            if(!iconData) return vmsHeader.getVmuDescription();
            else return QString("-");

        case COL_APP:
            if(!iconData) return vmsHeader.getCreatorApp();
            else return QString("-");

        case COL_BLOCKS:
            return dirEntry.getFileSizeBlocks();

        case COL_FILE_TYPE:
            if(iconData)  return "ICONDATA";
            else return QString::fromStdString(dirEntry.getFileTypeStr());

        case COL_CREATED: //Mouse-over to give entire, full-detail (Century, day of week, second)
            return QString::fromStdString(dirEntry.getCreationDateStr());

        case COL_CRC:
            if(!iconData) return vmsHeader.getCrc();
            else return QString("-");

        case COL_COPY:
             return dirEntry.isCopyProtected()? "No" : "Yes";

        case COL_DIR_ENTRY:
            return dirEntry.getFileIndex();

        case COL_FIRST_BLOCK:
            return dirEntry.getFirstBlock();

        case COL_COUNT:
        default: return QVariant("");
        }

    } else if(role == Qt::DecorationRole) {

        switch(index.column()) {
        case COL_NAME:
             if(iconData) return _dev.getVolumeDcIcon();
             else return dirEntry.getFileIcon();

        case COL_IMAGE:
            if(iconData) return _dev.getVolumeVmuIcon();
            else if(extraBgPvr) return _dev.getExtraBgPvrTexture();
            else if(dirEntry.isPsoImg()) return dirEntry.getPsoImgBankEntry()->image;
            else return dirEntry.getEyeCatch();

        default:
            return QVariant();
        }

    } else if(role == Qt::ToolTipRole) {

        switch(index.column()) {
        case COL_NAME:
            return iconData? VMU_ICONDATA_VMS_FILE_NAME :
                             dirEntry.getFileName();
        case COL_VM_DESC:
            return iconData? iconData.getVmuDescription() :
                             vmsHeader.getVmuDescription();
        case COL_DC_DESC:
            if(!iconData) return vmsHeader.getVmuDescription();
            else return QString("ICONDATA.VMS has no DC description.");

        case COL_APP:

            if(!iconData) return vmsHeader.getCreatorApp();
            else return QString("ICONDATA.VMS has no application field.");

        case COL_BLOCKS:
            return QString("%1 Byte(s)").arg(dirEntry.getFileSizeBytes());

        case COL_FILE_TYPE:
            if(iconData) return "Custom VMU Icon (Hidden File)";
            else return QString::fromStdString(dirEntry.getFileTypeStr());

        case COL_CREATED: //Mouse-over to give entire, full-detail (Century, day of week, second)
            return QString::fromStdString(dirEntry.getCreationDateStr()) +
                   QString(" (") + QString::fromStdString(dirEntry.getCreationDayOfWeekStr()) +
                    QString(")");

        case COL_CRC:
            if(!iconData) {
                if(dirEntry.getFileType() == VMU_FLASH_FILE_TYPE_GAME) {
                    return QString("GAME files don't require a CRC.");
                } else {
                    if(dirEntry.calculateCrc() == vmsHeader.getCrc()) {
                        return QString("CRC is valid.");
                    } else return QString("CRC is incorrect and must be fixed!");
                }
            } else return QString("ICONDATA.VMS does not have a CRC field.");
        case COL_COPY:
            return dirEntry.isCopyProtected()? "File cannot be copied on VMU+DC" :
                                               "File can be copied on DC+VMU.";
        case COL_DIR_ENTRY:
            return dirEntry.getFileIndex();

        case COL_FIRST_BLOCK:
            return dirEntry.getFirstBlock();

        case COL_IMAGE:
            if(iconData) {
                if(_dev.getVolumeVmuIcon().isNull()) return QString("Custom Monochrome VMU Icon");
                else return QString("No Monochrome VMU Icon found!");
            } else if(extraBgPvr) {
                if(_dev.getExtraBgPvrTexture().isNull()) return QString("Extra Dreamcast BIOS Background PVR Texture");
                else return QString("No Dreamcast PVR Texture for Extra Bios Background!");
            } else if(dirEntry.isPsoImg()) {
                auto* entry = dirEntry.getPsoImgBankEntry();
                if(entry) {
                    QString str("PSO Screenshot with");
                    int count = 0;
                    for(auto& it : entry->characterNames) {
                        if(count++) {
                            str += QString(",");
                        }
                        str += QString(" ") + it;
                    }
                    return str;
                }
            } else {
                if(dirEntry.getEyeCatch().isNull()) return QString("No Extra Eyecatch Image");
                else return QString("Additional Eyecatch Image");
            }

        case COL_COUNT:
        default: return QVariant("");
        }

    } else if(role == Qt::ForegroundRole) {

        switch(index.column()) {
        case COL_DC_DESC:
            if(!iconData)  return QColor(Qt::black);
            else return QColor(Qt::gray);

        case COL_APP:
            if(!iconData) return QColor(Qt::black);
            else return QColor(Qt::gray);

        case COL_FILE_TYPE:
            if(!iconData) return QColor(Qt::black);
            else return QColor(Qt::gray);

        case COL_CRC:
            if(iconData) return QColor(Qt::gray);
            else if(dirEntry.getFileType() == VMU_FLASH_FILE_TYPE_GAME) return QColor(Qt::black);
            else if(dirEntry.calculateCrc() == vmsHeader.getCrc()) return QColor(Qt::darkGreen);
            else return QColor(Qt::red);

        default: break;
        }
    }
    return QVariant();
}

//(well-behaved method(s))
QVariant VmuFileSystemTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case COL_NAME:          return "Name";
            case COL_VM_DESC:       return "VMU Info";
            case COL_DC_DESC:       return "DC Info";
            case COL_APP:           return "App";
            case COL_BLOCKS:        return "Blocks";
            case COL_FILE_TYPE:     return "Type";
            case COL_CREATED:       return "Date";
            case COL_CRC:           return "CRC";
            case COL_COPY:          return "Copy";
            case COL_DIR_ENTRY:     return "Directory Index";
            case COL_FIRST_BLOCK:   return "First Block";
            case COL_IMAGE:         return "Image";
            default:                return "?";
            }
        }
    } else if(role == Qt::ToolTipRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case COL_NAME:          return "Actual filename in FAT filesystem and VMU BIOS";
            case COL_VM_DESC:       return "Description shown in VMU BIOS";
            case COL_DC_DESC:       return "Description shown in Dreamcast BIOS";
            case COL_APP:           return "Application associated with file";
            case COL_BLOCKS:        return "File size in Blocks (256 bytes)";
            case COL_FILE_TYPE:     return "GAME or DATA file types";
            case COL_CREATED:       return "Creation Date";
            case COL_CRC:           return "Checksum";
            case COL_COPY:          return "Copy Permissions (enforced by VMU/DC BIOS)";
            case COL_DIR_ENTRY:     return "FAT8 Directory Entry Index";
            case COL_FIRST_BLOCK:   return "First Block of Image in FAT filesystem";
            case COL_IMAGE:         return "Eyecatch, Custom Icons, Custom DC BIOS Textures";
            default:                return "?";
            }

        }

    }

    return QVariant();
}

void VmuFileSystemTableModel::_updateIcons() {
    if(!_dev) return;

    unsigned fileCount = _dev.getFlashFileCount();
    if(_rowCount != fileCount) {
        if(_rowCount > fileCount) {
            removeRows(rowCount()-1, _rowCount-fileCount, QModelIndex());
        } else {
            insertRows(rowCount(), fileCount-_rowCount, QModelIndex());
        }
    }


    QModelIndex topLeft = createIndex(0, COL_NAME);
    QModelIndex botRight = createIndex(rowCount(), COL_NAME);
    QVector<int> roles = {
        Qt::DecorationRole
    };

    Q_EMIT dataChanged(topLeft, botRight, roles);
}


VmuFileSystemTableView::VmuFileSystemTableView(QWidget* widget):
   QTableView(widget)
{
    setSelectionMode(QTableView::SingleSelection);
    setSelectionBehavior(QTableView::SelectRows);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setAlternatingRowColors(true);
    auto rect = geometry();
    rect.setSize(QSize(800, 300));
    setGeometry(rect);
    setWindowTitle("EVMU Filesystem Manager");
    setWindowIcon(QIcon(ICON_FILE_PATH));
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowMaximizeButtonHint);
    auto flags = windowFlags();
    flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    move(qApp->desktop()->availableGeometry(this).center()-geometry().center());

    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(10);
    setFont(f);
    setContentsMargins(0, 0, 0, 0);
    setViewportMargins(0, 0, 0, 0);

    //horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    resizeColumnsToContents();

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(_contextMenuRequested(const QPoint&)));
}

Qt::DropActions VmuFileSystemTableModel::supportedDragActions(void) const {
    return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction);
}

Qt::DropActions VmuFileSystemTableModel::supportedDropActions(void) const {
    return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction);
}

Qt::ItemFlags VmuFileSystemTableModel::flags(const QModelIndex& index) const {
    return (Qt::ItemIsSelectable|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
}

QStringList VmuFileSystemTableModel::mimeTypes(void) const {
    QStringList list;
    list << "text/plain" << "text/uri-list";
    return list;
}

bool VmuFileSystemTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                  int row, int column, const QModelIndex &parent) {
    bool success = true;
      if(data->hasUrls()) {
          for(auto& url : data->urls()) {
              if(url.isLocalFile()) {
                  QString path = url.toLocalFile();
                  if(VmuQtUtils::canOpenFile(path)) {
                      //VMU_LOAD_IMAGE_STATUS status;
                      //beginInsertRows(QModelIndex(), rowCount(), rowCount());
                      return VmuQtUtils::loadVMUImage(_dev, path);
                      /*endInsertRows();

                      if(!success) {
                            beginRemoveRows(QModelIndex(), rowCount(), 1);
                            endRemoveRows();
                      }*/
                  }
              }
          }
      }


    return success;
}

bool VmuFileSystemTableModel::canDropMimeData(const QMimeData *data,
  Qt::DropAction action, int row, int column, const QModelIndex &parent) const {
    if(data->hasUrls()) {
        for(auto& url : data->urls()) {
            if(url.isLocalFile()) {
                QString path = url.toLocalFile();
                if(VmuQtUtils::canOpenFile(path)) return true;
            }
        }
    }
    return false;
}

bool VmuFileSystemTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    bool success = true;
    for(unsigned i = 0; i < count; ++i) {
        beginInsertRows(parent, row+i, row+i);

        ++_rowCount;

        endInsertRows();
    }
    return true;
}

bool VmuFileSystemTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    bool success = true;

    std::vector<QtVmuFlashDirEntry> entries;
    for(int r = 0; r < count; ++r) {
        entries.push_back(_dev.getFileFlashDirEntry(row-r));
    }

    for(int i = 0; i < entries.size(); ++i) {
        auto& entry = entries[i];
        beginRemoveRows(parent, row-i, row-i);
        if(entry) {
            success &= entry.deleteFile();
        }
        --_rowCount;
        endRemoveRows();
    }

    return success;
}

void VmuFileSystemTableView::_contextMenuRequested(const QPoint& pos) {
    QModelIndex index(indexAt(pos));
    if(!index.isValid()) return;

    QESTkUtils::MenuStack stack("Flash Entry");

    VmuFileSystemTableModel*    fsModel     = static_cast<VmuFileSystemTableModel*>(model());
    QtVmuDevice                 dev         = fsModel->getDevice();
    int                         row         = index.row();
    auto                        dirEntry    = dev.getFileFlashDirEntry(row);
    auto                        vmsHeader   = dirEntry.getVmsHeader();
    auto                        icnData     = dirEntry.getIconDataFileInfo();
    bool                        pvrInfo     = dirEntry.isExtraBgPvr();

    stack.push("Export File");

    stack.insertAction("VMI/VMS",
        [=](bool) {

            if(index.isValid()) {
                QString path = QFileDialog::getSaveFileName(nullptr,
                                                            "Choose Location to export VMI+VMS files.",
                                                            VmuQtUtils::getRomDir());
                if(path.isEmpty()) return;

                QString name = path.split("/").last();
                if(name.length() > VMU_VMI_FILE_INFO_VMS_RESOURCE_NAME_SIZE) {
                    QString newName = name;
                    newName.truncate(VMU_VMI_FILE_INFO_VMS_RESOURCE_NAME_SIZE - 1);

                    if(QMessageBox::question(nullptr, "Filename too long!",
                                        QString("The filename provided is too long. A valid .VMI/.VMS pair that is complaint with Dreamcast"
                                             "web browsers must have matching filenames no longer than ") + QString(VMU_VMI_FILE_INFO_VMS_RESOURCE_NAME_SIZE) +
                                             QString(" characters. Automatically truncate the given filename to ") + QString(newName) + QString("?"))
                        == QMessageBox::No)
                    {
                            return;
                    }

                    path.replace(name, newName);

                }

                QString vmiPath = QString(path + QString(".vmi"));
                QString vmsPath = QString(path + QString(".vms"));

                QFileInfo vmiInfo(vmiPath);
                QFileInfo vmsInfo(vmsPath);

                if(vmiInfo.exists() || vmsInfo.exists()) {
                    QString nouns;
                    if(vmiInfo.exists()) {
                        nouns += vmiPath;
                        if(vmsInfo.exists()) nouns += QString(" and ");
                    }
                    if(vmsInfo.exists()) {
                        nouns += vmsPath;
                    }

                    if(QMessageBox::question(nullptr, "Confirm Overwrite",
                                    QString("File(s): ") + nouns + QString(" already exist and will be overwritten by proceeding. Continue?"))
                                             == QMessageBox::No)
                    {
                        return;
                    }

                }

                bool success = true;

                success |= gyVmuFlashExportVmi(dev, dirEntry, Q_CSTR(vmiPath));
                success |= gyVmuFlashExportVms(dev, dirEntry, Q_CSTR(vmsPath));

                if(!success) {
                    _gyLog(GY_DEBUG_ERROR, "Failed to export VMI/VMS pair!");
                    QMessageBox::critical(nullptr, "Not Enough Space", QString(gyVmuFlashLastErrorMessage()));
                }
            }

        });

    stack.insertAction("DCI",
        [=](bool) {

            if(index.isValid()) {
                QString filter = QString("Nexus Files (*.dci)");
                QString path = QFileDialog::getSaveFileName(nullptr,
                                                            "Choose Location to export DCI image",
                                                            VmuQtUtils::getRomDir(),
                                                            filter,
                                                            &filter);

                if(path.isEmpty()) return;

                if(!gyVmuFlashExportDci(dev, dirEntry, Q_CSTR(path))) {
                    QMessageBox::critical(nullptr, "Export Failed", "Failed to export the selected item to a DCI file!");
                }
            }

        });

    if(dirEntry.getFileType() == VMU_FLASH_FILE_TYPE_DATA) {
        stack.insertAction("Raw",
            [=](bool) {

                if(index.isValid()) {
                    QString filter = QString("Binary File (*.bin)");
                    QString path = QFileDialog::getSaveFileName(nullptr,
                                                                "Choose Location to export raw binary",
                                                                VmuQtUtils::getRomDir(),
                                                                filter,
                                                                &filter);

                    if(path.isEmpty()) return;

                    if(!gyVmuFlashExportRaw(dev, dirEntry, Q_CSTR(path))) {
                        QMessageBox::critical(nullptr, "Export Failed", "Failed to export the selected item to a raw binary file!");
                    }
                }

            });
    }

    stack.pop();

    stack.push("Export Graphics");

    if(icnData) {

        stack.insertAction("Custom VMU LCD Icon",
            [=](bool) {
                QString defaultSelected = "Portable Network Graphics Format(*.png)";
                QString path = QFileDialog::getSaveFileName(nullptr,
                                                            "Choose Location to export image",
                                                            VmuQtUtils::getRomDir(),
                                                            VmuQtUtils::getDefaultImageTypeFilter(),
                                                            &defaultSelected);
                if(path.isEmpty()) return;

                auto* entry = VmuQtUtils::getIconDataBank().getEntry(dev, dirEntry);
                if(entry) {
                    entry->exportVmuIcon(path);
                }

            }, icnData.getVmuIconData() != 0);

        stack.insertAction("Custom Volume Icon",
            [=](bool) {
                QString defaultSelected = "Portable Network Graphics Format(*.png)";
                QString path = QFileDialog::getSaveFileName(nullptr,
                                                            "Choose Location to export image",
                                                            VmuQtUtils::getRomDir(),
                                                            VmuQtUtils::getDefaultImageTypeFilter(),
                                                            &defaultSelected);
                if(path.isEmpty()) return;

                auto* entry = VmuQtUtils::getIconDataBank().getEntry(dev, dirEntry);
                if(entry) {
                    entry->exportDcIcon(path);
                }

            }, icnData.getDcIconOffset());


    } else {
        if(pvrInfo) {
            stack.insertAction("Custom BIOS BG Texture",
                [=](bool) {
                    QString defaultSelected = "Portable Network Graphics Format(*.png)";
                    QString path = QFileDialog::getSaveFileName(nullptr,
                                                                "Choose Location to export image",
                                                                VmuQtUtils::getRomDir(),
                                                                VmuQtUtils::getDefaultImageTypeFilter(),
                                                                &defaultSelected);
                    if(path.isEmpty()) return;

                    auto* entry = VmuQtUtils::getExtraBgPvrBank().getEntry(dev, dirEntry);
                    if(entry) {
                        entry->exportImage(path);
                    }
                });

        } else if(dirEntry.isPsoImg()) {
            stack.insertAction("PSO Screenshot",
                [=](bool) {
                    QString defaultSelected = "Portable Network Graphics Format(*.png)";
                    QString path = QFileDialog::getSaveFileName(nullptr,
                                                                "Choose Location to export image",
                                                                VmuQtUtils::getRomDir(),
                                                                VmuQtUtils::getDefaultImageTypeFilter(),
                                                                &defaultSelected);
                    if(path.isEmpty()) return;

                    auto* entry = QtVmuFlashDirEntry(dirEntry).getPsoImgBankEntry();
                    if(entry) {
                        entry->exportImage(path);
                    }
                });
        }

        stack.insertAction("Icon(s)",
            [=](bool) {
            QString filter = "Animated GIF (*.gif)";
                QString path = QFileDialog::getSaveFileName(nullptr,
                                                            "Choose Location to export GIF",
                                                            VmuQtUtils::getRomDir(),
                                                            filter,
                                                            &filter);
                if(path.isEmpty()) return;

                VmuAnimatedIconCache::exportAsGIF(dirEntry, path);
            }, vmsHeader.getIconCount() != 0);

        stack.insertAction("Eyecatch",
            [=](bool) {
                QString filter = "PNG (*.png)";
                QString path = QFileDialog::getSaveFileName(nullptr,
                                                            "Choose Location to export eyecatch",
                                                            VmuQtUtils::getRomDir(),
                                                            filter,
                                                            &filter);
                if(path.isEmpty()) return;

                VmuEyecatchCache::exportAsPNG(dirEntry, path);


            }, vmsHeader.hasEyeCatch());



    }

    stack.pop();

    if(icnData) {
        bool biosSecretUnlocked = icnData.isSecretBiosUnlocked();
        stack.insertAction(biosSecretUnlocked? "Disable Secret BIOS" : "Unlock Secret BIOS",
            [=](bool) {
                icnData.setSecretBiosUnlocked(!biosSecretUnlocked);
            });
    }

    stack.insertSeparator();

    stack.insertAction("Fix CRC",
        [=](bool) {
            if(index.isValid()) {
               dirEntry.fixCrc();
            }
        }, !icnData);

    stack.insertAction(dirEntry.isCopyProtected()? "Disable Copy Protection" : "Enable Copy Protection",
        [=](bool) {
            if(index.isValid()) {
               dirEntry.setCopyProtected(!dirEntry.isCopyProtected());
            }
        });

    stack.insertSeparator();

    stack.insertAction("Delete",
        [=](bool) {
            if(index.isValid()) {
                if(QMessageBox::question(nullptr, "Confirm Delete",
                                  "Are you SURE you want to delete the selected file from the current Flash image? This cannot be undone.")
                    == QMessageBox::Yes)
                {
                    model()->removeRows(index.row(), 1);
                }
            }
        });

    stack.execDeleteLater(mapToGlobal(pos));
}

#if 0
void VmuFileSystemTableView::dragEnterEvent(QDragEnterEvent *event) {
      return QTableView::dragEnterEvent(event);
    // event->acceptProposedAction();
    _gyLog(GY_DEBUG_VERBOSE,"trolzy");

}
void VmuFileSystemTableView::dropEvent(QDropEvent *event) {
    _gyLog(GY_DEBUG_VERBOSE, "LOLOL");
    return QTableView::dropEvent(event);
    //event->acceptProposedAction();
}
#endif
