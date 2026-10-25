/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ElysianVMU/gyro_vmu_device_model.hpp>
#include <ElysianVMU/qevmu_flash.hpp>
#include <QESTkUtils/tk_menu_stack.h>
#include <QApplication>
#include <QFontDatabase>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMimeData>
#include <QUrl>

VmuDeviceTableModel::VmuDeviceTableModel(VMUDevice* dev, QObject* parent):
    QAbstractTableModel(parent),
    _dev(dev)
{}

int VmuDeviceTableModel::rowCount(const QModelIndex&) const {
    return _rowCount;
}

int VmuDeviceTableModel::columnCount(const QModelIndex&) const {
    return COL_COUNT;
}

QVariant VmuDeviceTableModel::data(const QModelIndex& index, int role) const {
    int row = index.row();

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case COL_NAME: return "Untitled";

        default: return QVariant("");
        }

    } else if(role == Qt::DecorationRole) {

        switch(index.column()) {
        case COL_NAME:
             return _dev.getVolumeDcIcon();
        default:
            return QVariant();
        }

    } else if(role == Qt::ToolTipRole) {

        switch(index.column()) {
        case COL_NAME:
            return "/assets/full/file/path/shit/Untitled.vmu";
        case COL_COUNT:
        default: return QVariant("");
        }

    } else if(role == Qt::ForegroundRole) {

        switch(index.column()) {

        default: break;
        }
    } else if(role == Qt::BackgroundRole) {
        switch(index.column()) {
        case COL_NAME:
             return _dev.getVolumeColor();
        default:
            return QVariant();

        }
    }
    return QVariant();
}

//(well-behaved method(s))
QVariant VmuDeviceTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case COL_NAME:          return "Name";
            default:                return "?";
            }
        }
    } else if(role == Qt::ToolTipRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case COL_NAME:          return "Flash image filename";
            default:                return "?";
            }

        }

    }

    return QVariant();
}


VmuDeviceTableView::VmuDeviceTableView(QWidget* widget):
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
    setWindowTitle("EVMU Device Manager");
    //setWindowIcon(QIcon(ICON_FILE_PATH));
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

Qt::DropActions VmuDeviceTableModel::supportedDragActions(void) const {
    return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction);
}

Qt::DropActions VmuDeviceTableModel::supportedDropActions(void) const {
    return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction);
}

Qt::ItemFlags VmuDeviceTableModel::flags(const QModelIndex& index) const {
    return (Qt::ItemIsSelectable|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
}

QStringList VmuDeviceTableModel::mimeTypes(void) const {
    QStringList list;
    list << "text/plain" << "text/uri-list";
    return list;
}

bool VmuDeviceTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
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

bool VmuDeviceTableModel::canDropMimeData(const QMimeData *data,
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

bool VmuDeviceTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    bool success = true;
    for(unsigned i = 0; i < count; ++i) {
        beginInsertRows(parent, row+i, row+i);

        ++_rowCount;

        endInsertRows();
    }
    return true;
}

bool VmuDeviceTableModel::removeRows(int row, int count, const QModelIndex &parent) {
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

void VmuDeviceTableView::_contextMenuRequested(const QPoint& pos) {
    QModelIndex index(indexAt(pos));
    if(!index.isValid()) return;

    QESTkUtils::MenuStack stack("Flash Entry");


    stack.push("Export Image");
    stack.pop();

    stack.execDeleteLater(mapToGlobal(pos));
}

#if 0
void VmuDeviceTableView::dragEnterEvent(QDragEnterEvent *event) {
      return QTableView::dragEnterEvent(event);
    // event->acceptProposedAction();
    _gyLog(GY_DEBUG_VERBOSE,"trolzy");

}
void VmuDeviceTableView::dropEvent(QDropEvent *event) {
    _gyLog(GY_DEBUG_VERBOSE, "LOLOL");
    return QTableView::dropEvent(event);
    //event->acceptProposedAction();
}
#endif
