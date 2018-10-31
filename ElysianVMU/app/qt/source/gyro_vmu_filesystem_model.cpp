#include <ElysianVMU/gyro_vmu_filesystem_model.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <evmu-core/gyro_vmu_flash.h>
#include <evmu-core/gyro_vmu_util.h>
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

    const VMUFlashDirEntry* dirEntry = gyVmuFlashFileAtIndex(_dev, row);
    if(!dirEntry) return QVariant();
    const VMSFileInfo* vmsHeader = gyVmuFlashDirEntryVmsHeader(_dev, dirEntry);
    if(!vmsHeader) return QVariant();

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case COL_NAME: {
            char buffer[VMU_FLASH_DIRECTORY_FILE_NAME_SIZE+1];
            gyVmuFlashDirEntryName(dirEntry, buffer);
            return VmuQtUtils::japCodec->toUnicode(buffer);
        }
        case COL_VM_DESC: {
            char buffer[VMU_VMS_FILE_INFO_VMU_DESC_SIZE+1];
            gyVmuVmsHeaderVmuDescGet(vmsHeader, buffer);
            return VmuQtUtils::japCodec->toUnicode(buffer);
        }
        case COL_DC_DESC: {
            char buffer[VMU_VMS_FILE_INFO_DC_DESC_SIZE+1];
            gyVmuVmsHeaderDcDescGet(vmsHeader, buffer);
            return VmuQtUtils::japCodec->toUnicode(buffer);
        }
        case COL_APP: {
            char buffer[VMU_VMS_FILE_INFO_CREATOR_APP_SIZE];
            gyVmuVmsHeaderCreatorAppGet(vmsHeader, buffer);
            return VmuQtUtils::japCodec->toUnicode(buffer);
        }
        case COL_BLOCKS:
            return dirEntry? dirEntry->fileSize : 0;
        case COL_FILE_TYPE:
            if(!dirEntry) return "?";
            switch(dirEntry->fileType) {
            case VMU_FLASH_FILE_TYPE_DATA: return "DATA";
            case VMU_FLASH_FILE_TYPE_GAME: return "GAME";
            case VMU_FLASH_FILE_TYPE_NONE: return "NONE";
            default:                       return "INVALID";
            }
        case COL_CREATED: {//Mouse-over to give entire, full-detail (Century, day of week, second)
            if(!dirEntry) return "?";
            return QString("%1/%2/%3 %4:%5").
                    arg(gyVmuFromBCD(dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_MONTH])).
                    arg(gyVmuFromBCD(dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_DAY])).
                    arg(gyVmuFromBCD(dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_YEAR])).
                    arg(gyVmuFromBCD(dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_HOUR])).
                    arg(gyVmuFromBCD(dirEntry->timeStamp[VMU_FLASH_DIRECTORY_DATE_MINUTE]));
            }
        case COL_CRC:
                return vmsHeader? vmsHeader->crc : 0;
        case COL_COPY:
            return dirEntry? dirEntry->copyProtection? "No Copy" : "Full" : "?";
        case COL_DIR_ENTRY:
            return dirEntry? gyVmuFlashDirEntryIndex(_dev, dirEntry) : 0;
        case COL_FIRST_BLOCK:
            return dirEntry? dirEntry->firstBlock : 0;
        case COL_COUNT:
        default: return QVariant("");
        }
    } else if(role == Qt::DecorationRole) {
        switch(index.column()) {
        case COL_NAME:
            return VmuAnimatedIconCache::getIcon(_dev, dirEntry);
        case COL_EYECATCH:
            return VmuEyecatchCache::getIcon(_dev, dirEntry);
        default:
            return QVariant();
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
            case COL_APP:           return "Application";
            case COL_BLOCKS:        return "Blocks";
            case COL_FILE_TYPE:     return "Type";
            case COL_CREATED:       return "Date Created";
            case COL_CRC:           return "CRC";
            case COL_COPY:          return "Permissions";
            case COL_DIR_ENTRY:     return "Directory Index";
            case COL_FIRST_BLOCK:   return "First Block";
            case COL_EYECATCH:      return "Eyecatch";
            default:                return "?";
            }

        }
    }

    return QVariant();
}

void VmuFileSystemTableModel::_updateIcons() {
    if(!_dev) return;

    int fileCount = gyVmuFlashFileCount(_dev);
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
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMaximizeButtonHint);
    move(qApp->desktop()->availableGeometry(this).center()-geometry().center());

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

    std::vector<VMUFlashDirEntry*> entries;
    for(int r = 0; r < count; ++r) {
        entries.push_back(gyVmuFlashFileAtIndex(_dev, row-r));
    }

    for(int i = 0; i < entries.size(); ++i) {
        auto* entry = entries[i];
        beginRemoveRows(parent, row-i, row-i);
        if(entry) {
            success &= gyVmuFlashFileDelete(_dev, entry);
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

    VmuFileSystemTableModel* fsModel = static_cast<VmuFileSystemTableModel*>(model());
    VMUDevice* dev = fsModel->getDevice();
    int row = index.row();
    const VMUFlashDirEntry* dirEntry = gyVmuFlashFileAtIndex(dev, row);
    const VMSFileInfo* vmsHeader = gyVmuFlashDirEntryVmsHeader(dev, dirEntry);
#if 0
    stack.push("Export File");
    stack.insertAction("VMI/VMS",
        [=](bool) {

            if(index.isValid()) {

            }

        });
    stack.insertAction("DCI",
        [=](bool) {

            if(index.isValid()) {

            }

        });
    stack.pop();
#endif

    stack.push("Export Graphics");

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
        });

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


        });

    stack.pop();

    stack.insertSeparator();
#if 0
    stack.insertAction("Fix CRC",
        [=](bool) {

            if(index.isValid()) {

            }


        });
    stack.insertSeparator();
#endif
    stack.insertAction("Delete",
        [=](bool) {

            if(index.isValid()) {
                model()->removeRows(index.row(), 1);
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
