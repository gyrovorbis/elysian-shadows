#include <gyro_vmu_address_space_model.hpp>
#include <gyro_vmu_device.h>
#include <gyro_vmu_sfr.h>
#include <gyro_vmu_flash.h>
#include <gyro_vmu_memory.h>
#include <QFontDatabase>
#include <QHeaderView>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <libGyro/gyro_system_api.h>
#include <ElysianVMU/qevmu_flash.hpp>


const VmuModelAddrSegmentUIProperties VmuCurrentRamAddressSpaceModel::_addrSegProperties[VMU_MODEL_ADDR_SEGMENT_COUNT] {
#if 0
    { "RAM0",   QColor(0xf0fff0) },
    { " STK",   QColor(0xe0eee0) },
    { "RAM1",   QColor(0xf0fff0) },
    { " SFR",   QColor(0xc1cdc1) },
    { " XRM",   QColor(0x838b83) },
    { "?",      Qt::white}
#else
    { "RAM0",   QColor(0xf0fff0) },
    { " STK",   QColor(0xf0f8ff) },
    { "RAM1",   QColor(0xf0fff0) },
    { " SFR",   QColor(0xe6e6fa) },
    { " XRM",   QColor(0xffe4e1) },
    { "?",      Qt::white}
#endif
};

namespace QESTkUtils {

    uint8_t AddressSpaceModel::byteFromHexVariant(QVariant value) {
       QString string = value.value<QString>();
       string.truncate(2);
       QByteArray byte = QByteArray::fromHex(string.toUtf8());
       if(!byte.size()) return false;
       return static_cast<uint8_t>(byte.data()[0]);
    }
}

VmuAddressSpaceExplorerWidget::VmuAddressSpaceExplorerWidget(VMUDevice* dev, QWidget* parent):
    QTableView(parent)
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
    resizeColumnsToContents();

    _models[VMU_ADDRESS_SPACE_MODEL_CURRENT_RAM]    = new VmuCurrentRamAddressSpaceModel();
    _models[VMU_ADDRESS_SPACE_MODEL_FILESYSTEM]     = new VmuFileSystemAddressSpaceModel;

    setVMUDevice(dev);
}


void VmuAddressSpaceExplorerWidget::setVMUDevice(VMUDevice* device) {
    for(unsigned i = 0; i < VMU_ADDRESS_SPACE_MODEL_COUNT; ++i) {
        _models[i]->setVMUDevice(device);
    }
}

void VmuAddressSpaceExplorerWidget::setAddressSpaceModelType(VMU_ADDRESS_SPACE_MODEL_TYPE type) {
    if(_currentModel != type) {
        Q_ASSERT(type < VMU_ADDRESS_SPACE_MODEL_COUNT);
        _currentModel = type;
        Q_ASSERT(_models[type]);
        setModel(_models[type]);
        resizeColumnsToContents();
    }
}


namespace QESTkUtils {


AddressSpaceModel::AddressSpaceModel(uintptr_t baseAddress,
                                     uintptr_t addressRange,
                                     QObject* parent):
    QAbstractTableModel(parent),
    _baseAddress(baseAddress),
    _addressRange(addressRange)
{

    uintptr_t longestPtr = _baseAddress+_addressRange;
    _addrDisplayLength = 0;

    do {
        ++_addrDisplayLength;
    } while((longestPtr >>= 4));

    setHeaderData(0, Qt::Horizontal, "Address", Qt::DisplayRole);
    setHeaderData(1, Qt::Horizontal, "Data", Qt::DisplayRole);
    setHeaderData(2, Qt::Horizontal, "ASCII", Qt::DisplayRole);

    for(unsigned i = 0; i < ADDRESS_SPACE_MODEL_COLUMN_COUNT; ++i) {
        char buff[40];
        sprintf(buff, "%x", i);
        setHeaderData(i, Qt::Vertical, buff);
    }
}

auto AddressSpaceModel::flags(const QModelIndex& index) const -> Qt::ItemFlags {
    return (Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}

int AddressSpaceModel::rowCount(const QModelIndex&) const {
    return (_addressRange)/(ADDRESS_SPACE_MODEL_BYTES_PER_ROW*8);
}

int AddressSpaceModel::columnCount(const QModelIndex& parent) const {
    return ADDRESS_SPACE_MODEL_COLUMN_COUNT;
}

//RETURN ASCII-FIED SHIT
QVariant AddressSpaceModel::data(const QModelIndex& index, int role) const {
    uintptr_t addr = modelIndexToAddress(index);
    const AddressModelData* addrData = modelData(addr);
    auto colType = getColType(index);
    uint32_t baseAddr = (unsigned)(((unsigned)index.row()) << 4);
    char buff[7];

    switch(role) {
    case Qt::BackgroundRole:
        if(colType == COL_TYPE_ASCII) {
            addrData = modelData(baseAddr);
        }

        if(addrData) return addrData->bgColor;
        break;
#if 0
        segment = getAddrSegment(_dev, addr);
        return getAddrSegmentProperties(segment).bgColor;
#endif
    case Qt::EditRole:
    case Qt::DisplayRole:
        switch(getColType(index)) {
         case COL_TYPE_ASCII: {
            char byteData[16 + 1] =  { 0 };

            for(int b = 0; b < 16; ++b) {
                char byte = (char)readMemory(baseAddr + b);
                byteData[b] = byte? byte : '.';
            }

            byteData[16] = '\0';

            return QString(byteData);
        }
        default: {

            if(addr >= _baseAddress && addr < _baseAddress + _addressRange) {

                int  val = readMemory(modelIndexToAddress(index)) & 0xff;
                sprintf(buff, "%02x", val);
                return buff;
            } else {
                return "?";
            }
        }
        break;
    }

        break;
    default:
        break;
    }
    return QVariant();
}

bool AddressSpaceModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    uintptr_t addr = modelIndexToAddress(index);
    if(role == Qt::EditRole) {
        QString str = value.toString();

        switch(getColType(index)) {
        case COL_TYPE_ASCII:
            for(unsigned b = 0; b < 0x10; ++b) {
                if(b >= str.size()) {
                    writeMemory(addr + b, '\0');
                } else {
                    writeMemory(addr + b, QChar(str[b]).toLatin1());
                }
            }
            break;
        case COL_TYPE_DATA:

            uint8_t byte = byteFromHexVariant(value);
            writeMemory(addr, byte);
        }

    }
}

QVariant AddressSpaceModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if(orientation == Qt::Horizontal) {
        switch(role) {
        case Qt::DisplayRole:
            switch(getColType(section)) {
            //case COL_TYPE_HEADER:  return "Address";
            case COL_TYPE_DATA: {
                char strBuff[3];
                int addrVal = section - COL_TYPE_DATA;
                snprintf(strBuff, sizeof(strBuff), "%01x", addrVal);
                return QString(strBuff);
            }
            case COL_TYPE_ASCII:   return "ASCII";
            default:               return "xx";
            }
        }
    } else if(orientation == Qt::Vertical) {
        switch(role) {
        case Qt::DisplayRole: {
            char buff[40];
            uintptr_t address = modelIndexToAddress(createIndex(section, 0));
#if 0
            section - COL_TYPE_DATA + _baseAddress;
            auto segment = getAddrSegment(_dev, address<<4);
            const auto& properties = getAddrSegmentProperties(segment);
#else
            const auto* addrData = modelData(address);
#endif
            if(addrData && !addrData->prefix.isEmpty()) sprintf(buff, "%s:%0*x", Q_CSTR(addrData->prefix), _addrDisplayLength, section);
            else sprintf(buff, "%0*x", _addrDisplayLength, section);

            return QString(buff);
        }
        default: return QVariant();
        }
    }
    return QVariant();
}

}

VmuAddressSpaceModel::VmuAddressSpaceModel(VMUDevice* dev,
                                           uintptr_t baseAddress,
                                           uintptr_t addressRange,
                                           QObject* parent):
    QESTkUtils::AddressSpaceModel(baseAddress,
                                  addressRange,
                                  parent),
    _dev(dev)
{}

VMU_MODEL_ADDR_SEGMENT VmuCurrentRamAddressSpaceModel::getAddrSegment(VMUDevice* dev, uintptr_t address) {
    VMU_MODEL_ADDR_SEGMENT seg = VMU_MODEL_ADDR_SEGMENT_INVALID;
    int ramBank = (gyVmuMemRead(dev, SFR_OFFSET(SFR_ADDR_PSW)) & SFR_PSW_RAMBK0_MASK) >> SFR_PSW_RAMBK0_POS;

    //RAM Segments
    if(address < RAM_SFR_ADDR_BASE) {

        //System Memory
        if(!ramBank) {
            if(address < RAM_STACK_ADDR_BASE) {
                seg = VMU_MODEL_ADDR_SEGMENT_SYSTEM;
            } else {
                seg = VMU_MODEL_ADDR_SEGMENT_STACK;
            }

        } else { //Userspace
            seg = VMU_MODEL_ADDR_SEGMENT_GP;
        }

    } else if (address < SFR_ADDR_XRAM_BASE) { //SFRs
        seg = VMU_MODEL_ADDR_SEGMENT_SFR;

        //XRAM
    } else if(address <= SFR_ADDR_XRAM_END) {
        seg = VMU_MODEL_ADDR_SEGMENT_XRAM;
    }

    return seg;
}


uint8_t VmuAddressSpaceModel::readMemory(uintptr_t addr) const {
    return _dev.readMemoryByte(addr);
}

bool VmuAddressSpaceModel::writeMemory(uintptr_t addr, uint8_t value) {
    return _dev.writeMemoryByte(addr, value);
}

auto VmuAddressSpaceModel::modelData(uintptr_t addr) const -> const QESTkUtils::AddressModelData* {
    return nullptr;
}

VmuCurrentRamAddressSpaceModel::VmuCurrentRamAddressSpaceModel(VMUDevice* device):
    VmuAddressSpaceModel(device, 0, VMU_MEM_ADDR_SPACE_RANGE)
{}

auto VmuCurrentRamAddressSpaceModel::modelData(uintptr_t addr) const -> const QESTkUtils::AddressModelData* {
    static QESTkUtils::AddressModelData addrData;

    auto seg = getAddrSegment(_dev, addr);
    auto& props = getAddrSegmentProperties(seg);


    addrData = QESTkUtils::AddressModelData {
        props.bgColor,
        props.prefix
    };

    return &addrData;
}

auto VmuFileSystemAddressSpaceModel::modelData(uintptr_t addr) const -> const QESTkUtils::AddressModelData* {

    if(!_dev.isFlashFormatted()) return  nullptr;

    unsigned block = addr/VMU_FLASH_BLOCK_SIZE;
    uint16_t fatEntry = _dev.getFatEntry(block);

    static auto addrData = QESTkUtils::AddressModelData {
        Qt::white,
        QString("")
    };

    switch(fatEntry) {
    case VMU_FLASH_BLOCK_FAT_UNALLOCATED:
        addrData.bgColor = QColor(0xe6e6fa);
        break;
    case VMU_FLASH_BLOCK_FAT_DAMAGED:
        addrData.bgColor = QColor(Qt::red);
        break;
    default:
        addrData.bgColor = QColor(0xffe4e1);
    }

    addrData.prefix = QString::asprintf("[BLK%03u]", block);


    return &addrData;
}

VmuFileSystemAddressSpaceModel::VmuFileSystemAddressSpaceModel(void):
    VmuAddressSpaceModel(nullptr, 0, FLASH_SIZE) {}


uint8_t VmuFileSystemAddressSpaceModel::readMemory(uintptr_t addr) const {
    return _dev.readFlashByte(addr);
}


bool VmuFileSystemAddressSpaceModel::writeMemory(uintptr_t addr, uint8_t value) {
    return _dev.writeFlashByte(addr, value);
}

