#include <gyro_vmu_address_space_model.hpp>
#include <gyro_vmu_device.h>
#include <gyro_vmu_memory.h>
#include <QFontDatabase>

VmuMemoryBrowserWidget::VmuMemoryBrowserWidget(VMUDevice* dev, QObject* object):
    _model(new VmuAddressSpaceModel(dev))
{
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(10);
    setFont(f);
    setContentsMargins(0, 0, 0, 0);
    setViewportMargins(0, 0, 0, 0);
    QPalette p = palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    setPalette(p);
    setAlternatingRowColors(true);
    setMouseTracking(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setModel(_model);
}

VmuAddressSpaceModel::VmuAddressSpaceModel(VMUDevice* dev, QObject* parent):
    QAbstractTableModel(parent),
    _dev(dev)
{
    setHeaderData(0, Qt::Horizontal, "Address", Qt::DisplayRole);
    setHeaderData(1, Qt::Horizontal, "Data", Qt::DisplayRole);
    setHeaderData(2, Qt::Horizontal, "ASCII", Qt::DisplayRole);
#if 0
    for(unsigned i = 0; i < ADDRESS_SPACE_MODEL_COLUMN_COUNT; ++i) {
        char buff[40];
        sprintf(buff, "%x", i);
        setHeaderData(i, Qt::Vertical, buff);
    }
#endif

}

auto VmuAddressSpaceModel::flags(const QModelIndex& index) const -> Qt::ItemFlags {
    return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

int VmuAddressSpaceModel::rowCount(const QModelIndex&) const {
    return (VMU_MEM_SEG_COUNT * VMU_MEM_SEG_SIZE)/(ADDRESS_SPACE_MODEL_BYTES_PER_ROW*8);
}

int VmuAddressSpaceModel::columnCount(const QModelIndex& parent) const {
    return ADDRESS_SPACE_MODEL_COLUMN_COUNT;
}


//RETURN ASCII-FIED SHIT
QVariant VmuAddressSpaceModel::data(const QModelIndex& index, int role) const {
    char buff[7];
    int addr = modelIndexToMemAddr(index);
    int val;
    switch(role) {
    case Qt::DisplayRole:
        if(addr != -1) {

            int val = gyVmuMemRead(_dev, modelIndexToMemAddr(index)) & 0xff;
            sprintf(buff, "%x", val);
            return buff;
        } else {
            return "?";
        }
    default:
        break;
    }
    return QVariant();
}

bool VmuAddressSpaceModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    bool ok = false;
    int addr = modelIndexToMemAddr(index);
    if(addr != -1) {
        gyVmuMemWrite(_dev, index.row(), value.toInt(&ok));
    }
    return ok;
}

QVariant VmuAddressSpaceModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if(orientation == Qt::Horizontal) {
        switch(role) {
        default:
        case Qt::DisplayRole:
            switch(getColType(section)) {
            case COL_TYPE_HEADER:  return "Address";
            case COL_TYPE_DATA:    return "Value";
            case COL_TYPE_ASCII:   return "ASCII";
            default:               return "Invalid";
            }
        }
    } else if(orientation == Qt::Vertical) {
        switch(role) {
        default:
        case Qt::DisplayRole: {
            char buff[40];
            sprintf(buff, "%x", section);
            return buff;
        }
        }
    }
    return QVariant();
}



