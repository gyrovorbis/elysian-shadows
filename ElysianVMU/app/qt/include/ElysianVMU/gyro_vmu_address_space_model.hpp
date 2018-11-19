#ifndef GYRO_VMU_ADDRESS_SPACE_MODEL_HPP
#define GYRO_VMU_ADDRESS_SPACE_MODEL_HPP

#include <QAbstractTableModel>
#include <QTableView>
#include <ElysianVMU/gyro_elysian_vmu_device.hpp>

#define ADDRESS_SPACE_MODEL_BYTES_PER_ROW       2
#define ADDRESS_SPACE_MODEL_HEADER_COL_COUNT    0
#define ADDRESS_SPACE_MODEL_ASCII_COL_COUNT     1
#define ADDRESS_SPACE_MODEL_DATA_COL_COUNT      (ADDRESS_SPACE_MODEL_BYTES_PER_ROW*8)
#define ADDRESS_SPACE_MODEL_COLUMN_COUNT        (ADDRESS_SPACE_MODEL_DATA_COL_COUNT+ADDRESS_SPACE_MODEL_HEADER_COL_COUNT+ADDRESS_SPACE_MODEL_ASCII_COL_COUNT)

class VmuAddressSpaceModel;

enum VMU_MODEL_ADDR_SEGMENT {
    VMU_MODEL_ADDR_SEGMENT_SYSTEM,
    VMU_MODEL_ADDR_SEGMENT_STACK,
    VMU_MODEL_ADDR_SEGMENT_GP,
    VMU_MODEL_ADDR_SEGMENT_SFR,
    VMU_MODEL_ADDR_SEGMENT_XRAM,
    VMU_MODEL_ADDR_SEGMENT_INVALID,
    VMU_MODEL_ADDR_SEGMENT_COUNT
};

struct VmuModelMemoryAddress {

};

struct VmuModelAddrSegmentUIProperties {
    QString prefix;
    QColor  bgColor;
};

enum VMU_ADDRESS_SPACE_MODEL_TYPE {
    VMU_ADDRESS_SPACE_MODEL_CURRENT_RAM,
    VMU_ADDRESS_SPACE_MODEL_FILESYSTEM,
    VMU_ADDRESS_SPACE_MODEL_COUNT
};

class VmuAddressSpaceExplorerWidget: public QTableView {
    VMU_ADDRESS_SPACE_MODEL_TYPE _currentModel = VMU_ADDRESS_SPACE_MODEL_COUNT;
    VmuAddressSpaceModel* _models[VMU_ADDRESS_SPACE_MODEL_COUNT] = { nullptr };

public:
                VmuAddressSpaceExplorerWidget(VMUDevice* device, QWidget* object=nullptr);
    virtual     ~VmuAddressSpaceExplorerWidget(void) override = default;
    void        setAddressSpaceModelType(VMU_ADDRESS_SPACE_MODEL_TYPE type);
    void        setVMUDevice(VMUDevice* device);

};



namespace QESTkUtils {


struct AddressModelData {
    QColor  bgColor;
    QString prefix;
};


class AddressSpaceModel: public QAbstractTableModel {
public:
    enum COL_TYPE {
        //COL_TYPE_HEADER,
        COL_TYPE_DATA,
        COL_TYPE_ASCII,
        COL_TYPE_COUNT,
        COL_TYPE_INVALID
    };
private:
    uintptr_t           _baseAddress            = 0;
    uintptr_t           _addressRange           = 0;
    unsigned            _addrDisplayLength      = 0;

public:
                        AddressSpaceModel(uintptr_t baseAddress, uintptr_t addressRange, QObject* parent=nullptr);
    virtual             ~AddressSpaceModel(void) override = default;

    COL_TYPE            getColType(const QModelIndex& index) const;
    COL_TYPE            getColType(int col) const;
    static uint8_t      byteFromHexVariant(QVariant value);

    //Minimal requirements to power the back-end of the model
    virtual uintptr_t   modelIndexToAddress(const QModelIndex& index) const         = 0;
    virtual uint8_t     readMemory(uintptr_t addr) const                            = 0;
    virtual bool        writeMemory(uintptr_t addr, uint8_t byte)                 = 0;
    virtual auto        modelData(uintptr_t addr) const -> const AddressModelData*  = 0;

    //Overridden from QAbstractTableModel
    //(read-only methods)
    virtual int         rowCount(const QModelIndex& parent=QModelIndex()) const override;
    virtual int         columnCount(const QModelIndex& parent=QModelIndex()) const override;
    virtual QVariant    data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

    //(well-behaved method(s))
    virtual QVariant    headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const override;

    //(editable)
    virtual bool        setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole) override;
    virtual auto        flags(const QModelIndex& index) const -> Qt::ItemFlags override;
};

}


class VmuAddressSpaceModel: public QESTkUtils::AddressSpaceModel {
protected:
    EVmuDevice          _dev;
public:

                        VmuAddressSpaceModel(VMUDevice* dev, uintptr_t baseAddress, uintptr_t addressRange, QObject* parent=nullptr);
    VMUDevice*          getVMUDevice(void) const;
    void                setVMUDevice(VMUDevice* dev);

    //Base, generic implementation using RAM address space
    virtual uintptr_t   modelIndexToAddress(const QModelIndex& index) const override;
    virtual uint8_t     readMemory(uintptr_t addr) const override;
    virtual bool        writeMemory(uintptr_t addr, uint8_t byte) override;
    virtual auto        modelData(uintptr_t addr) const -> const QESTkUtils::AddressModelData* override;
};

class VmuCurrentRamAddressSpaceModel: public VmuAddressSpaceModel {
protected:
    static const VmuModelAddrSegmentUIProperties
                        _addrSegProperties[VMU_MODEL_ADDR_SEGMENT_COUNT];
public:
                    //VmuCurrentRamAddressSpaceModel(VMUDevice* dev=nullptr, uintptr_t baseAddress=0, uintptr_t addressRange=0, QObject* parent=nullptr);
                    VmuCurrentRamAddressSpaceModel(VMUDevice* device=nullptr);

    static VMU_MODEL_ADDR_SEGMENT
                        getAddrSegment(VMUDevice* dev, uintptr_t address);

    static const VmuModelAddrSegmentUIProperties&
                        getAddrSegmentProperties(VMU_MODEL_ADDR_SEGMENT segment);



    virtual auto        modelData(uintptr_t addr) const -> const QESTkUtils::AddressModelData* override;

};

class VmuFileSystemAddressSpaceModel: public VmuAddressSpaceModel {
private:

    virtual uint8_t     readMemory(uintptr_t addr) const override;
    virtual bool        writeMemory(uintptr_t addr, uint8_t byte) override;
public:
                        VmuFileSystemAddressSpaceModel(void);
    virtual             ~VmuFileSystemAddressSpaceModel(void) override = default;

   virtual auto        modelData(uintptr_t addr) const -> const QESTkUtils::AddressModelData* override;
};







inline const VmuModelAddrSegmentUIProperties&
VmuCurrentRamAddressSpaceModel::getAddrSegmentProperties(VMU_MODEL_ADDR_SEGMENT segment) {
    return _addrSegProperties[segment];
}

inline VMUDevice* VmuAddressSpaceModel::getVMUDevice(void) const {
    return _dev;
}

inline void VmuAddressSpaceModel::setVMUDevice(VMUDevice* dev) {
    _dev = dev;
}


inline uintptr_t VmuAddressSpaceModel::modelIndexToAddress(const QModelIndex& index) const {
    uintptr_t addr = 0;
    switch(getColType(index)) {
    case COL_TYPE_DATA:
        addr = (uintptr_t)(((uintptr_t)index.row()) << 4);
        addr += (uintptr_t)index.column();
        break;
    case COL_TYPE_ASCII:
        addr = (uintptr_t)(((uintptr_t)index.row()) << 4);
    default:
        break;
    }
    return addr;
}

namespace QESTkUtils {

inline auto AddressSpaceModel::getColType(const QModelIndex& index) const -> COL_TYPE {
    return getColType(index.column());
}

inline auto AddressSpaceModel::getColType(int col) const -> COL_TYPE {
    if(col >= ADDRESS_SPACE_MODEL_COLUMN_COUNT) return COL_TYPE_INVALID;
    else if(col < 0) return COL_TYPE_INVALID;

    //if(col < ADDRESS_SPACE_MODEL_HEADER_COL_COUNT) return COL_TYPE_HEADER;
    else if(col < ADDRESS_SPACE_MODEL_HEADER_COL_COUNT + ADDRESS_SPACE_MODEL_DATA_COL_COUNT) return COL_TYPE_DATA;
    else if(col < ADDRESS_SPACE_MODEL_HEADER_COL_COUNT + ADDRESS_SPACE_MODEL_DATA_COL_COUNT + ADDRESS_SPACE_MODEL_ASCII_COL_COUNT) return COL_TYPE_ASCII;
    return COL_TYPE_INVALID;
}

}



#endif // GYRO_VMU_ADDRESS_SPACE_MODEL_HPP
