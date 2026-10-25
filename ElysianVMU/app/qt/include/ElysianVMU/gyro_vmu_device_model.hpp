#ifndef GYRO_VMU_DEVICE_MODEL_HPP
#define GYRO_VMU_DEVICE_MODEL_HPP

#include <QAbstractTableModel>
#include <ElysianVMU/qevmu_device.hpp>
#include <QTableView>
#include <QPixmap>

class VmuDeviceTableView: public QTableView {
    Q_OBJECT

public:
                            VmuDeviceTableView(QWidget* widget=nullptr);


  //  virtual void dragEnterEvent(QDragEnterEvent *event) override;
    //virtual void dropEvent(QDropEvent *event) override;

private Q_SLOTS:
        void              _contextMenuRequested(const QPoint& pos);

};


class VmuDeviceTableModel: public QAbstractTableModel {
    Q_OBJECT
public:
    enum COL_TYPE {
        COL_NAME,
        COL_BLOCKS,
        COL_COUNT
    };

private:
    QtVmuDevice             _dev;
    QPixmap                 _volumeIcon;
    int                     _rowCount   = 1;

public:
                            VmuDeviceTableModel(VMUDevice* dev=nullptr, QObject* parent=nullptr);
    VMUDevice*              getDevice(void) const;
    void                    setDevice(VMUDevice* device);

    //Overridden from QAbstractTableModel
    //(read-only methods)
    virtual int             rowCount(const QModelIndex& parent=QModelIndex()) const override;
    virtual int             columnCount(const QModelIndex& parent=QModelIndex()) const override;
    virtual QVariant        data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

    //(well-behaved method(s))
    virtual QVariant        headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const override;

    virtual Qt::DropActions supportedDropActions(void) const override;
    virtual Qt::DropActions supportedDragActions(void) const override;

    virtual bool            insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool            removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool            canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    virtual bool            dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    virtual Qt::ItemFlags   flags(const QModelIndex& index) const override;
    virtual QStringList     mimeTypes(void) const override;
};







// ========= INLINEZ ==========


inline VMUDevice* VmuDeviceTableModel::getDevice(void) const { return _dev; }
inline void VmuDeviceTableModel::setDevice(VMUDevice* dev) { _dev = dev; }


#endif // GYRO_VMU_DEVICE_MODEL_HPP
