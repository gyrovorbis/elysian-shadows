#ifndef GYRO_VMU_ADDRESS_SPACE_MODEL_HPP
#define GYRO_VMU_ADDRESS_SPACE_MODEL_HPP

#include <QAbstractTableModel>
#include <QTableView>

struct VMUDevice;



#define ADDRESS_SPACE_MODEL_BYTES_PER_ROW       2
#define ADDRESS_SPACE_MODEL_HEADER_COL_COUNT    0
#define ADDRESS_SPACE_MODEL_ASCII_COL_COUNT     1
#define ADDRESS_SPACE_MODEL_DATA_COL_COUNT      (ADDRESS_SPACE_MODEL_BYTES_PER_ROW*8)
#define ADDRESS_SPACE_MODEL_COLUMN_COUNT        (ADDRESS_SPACE_MODEL_DATA_COL_COUNT+ADDRESS_SPACE_MODEL_HEADER_COL_COUNT+ADDRESS_SPACE_MODEL_ASCII_COL_COUNT)

class VmuAddressSpaceModel;

class VmuMemoryBrowserWidget: public QTableView {
    VmuAddressSpaceModel* _model = nullptr;

public:
    VmuMemoryBrowserWidget(VMUDevice* device, QObject* object);

};

class VmuAddressSpaceModel: public QAbstractTableModel {
    enum COL_TYPE {
        COL_TYPE_HEADER,
        COL_TYPE_DATA,
        COL_TYPE_ASCII,
        COL_TYPE_COUNT,
        COL_TYPE_INVALID
    };

private:
    struct VMUDevice*       _dev            = nullptr;
public:
                        VmuAddressSpaceModel(VMUDevice* dev, QObject* parent=nullptr);

    virtual             ~VmuAddressSpaceModel(void) override = default;

    VMUDevice*          getVMUDevice(void) const;
    void                setVMUDevice(VMUDevice* dev);

    int                 modelIndexToMemAddr(const QModelIndex& index) const;
    COL_TYPE            getColType(const QModelIndex& index) const;
    COL_TYPE            getColType(int col) const;

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




inline VMUDevice* VmuAddressSpaceModel::getVMUDevice(void) const {
    return _dev;
}

inline void VmuAddressSpaceModel::setVMUDevice(VMUDevice* dev) {
    _dev = dev;
}


inline int VmuAddressSpaceModel::modelIndexToMemAddr(const QModelIndex& index) const {
    int addr = -1;
    switch(getColType(index)) {
    case COL_TYPE_DATA:
        addr = (unsigned)(((unsigned)index.row()) << 4);
        addr |= (unsigned)index.column();
        break;
    default:
        break;
    }
    return addr;
}

inline auto VmuAddressSpaceModel::getColType(const QModelIndex& index) const -> COL_TYPE {
    return getColType(index.column());
}

inline auto VmuAddressSpaceModel::getColType(int col) const -> COL_TYPE {
    if(col >= ADDRESS_SPACE_MODEL_COLUMN_COUNT) return COL_TYPE_INVALID;
    else if(col < 0) return COL_TYPE_INVALID;

    if(col < ADDRESS_SPACE_MODEL_HEADER_COL_COUNT) return COL_TYPE_HEADER;
    else if(col < ADDRESS_SPACE_MODEL_HEADER_COL_COUNT + ADDRESS_SPACE_MODEL_DATA_COL_COUNT) return COL_TYPE_DATA;
    else if(col < ADDRESS_SPACE_MODEL_HEADER_COL_COUNT + ADDRESS_SPACE_MODEL_DATA_COL_COUNT + ADDRESS_SPACE_MODEL_ASCII_COL_COUNT) return COL_TYPE_ASCII;
    return COL_TYPE_INVALID;
}



#endif // GYRO_VMU_ADDRESS_SPACE_MODEL_HPP
