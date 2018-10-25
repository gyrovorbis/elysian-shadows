#ifndef GYRO_VMU_FILESYSTEM_MODEL_HPP
#define GYRO_VMU_FILESYSTEM_MODEL_HPP

#include <QAbstractTableModel>
#include <QTableView>
#include <evmu-core/gyro_vmu_vms.h>

#define VMU_ICON_UPDATE_ANIMATION_INTERVAL_MS   33

struct VMUDevice;
struct VMUFlashDirEntry;
struct VMSFileInfo;
class  QTimer;

class VmuFileSystemTableView: public QTableView {
    Q_OBJECT

public:
    VmuFileSystemTableView(QWidget* widget=nullptr);


  //  virtual void dragEnterEvent(QDragEnterEvent *event) override;
    //virtual void dropEvent(QDropEvent *event) override;

private Q_SLOTS:
        void              _contextMenuRequested(const QPoint& pos);

};


class VmuFileSystemTableModel: public QAbstractTableModel {
    Q_OBJECT
public:
    enum COL_TYPE {
        COL_NAME,
        COL_DC_DESC,
        COL_VM_DESC,
        COL_BLOCKS,
        COL_FILE_TYPE,
        COL_CREATED,
        COL_CRC,
        COL_COPY,
        COL_EYECATCH,
        COL_COUNT,
        COL_DIR_ENTRY,
        COL_FIRST_BLOCK
    };

private:
    VMUDevice*          _dev        = nullptr;
    QTimer*             _animTimer  = nullptr;
    int                 _rowCount   = 0;

private Q_SLOTS:
    void                _updateIcons(void);

public:
                        VmuFileSystemTableModel(VMUDevice* dev=nullptr, QObject* parent=nullptr);
    VMUDevice*          getDevice(void) const;
    void                setDevice(VMUDevice* device);

    //Overridden from QAbstractTableModel
    //(read-only methods)
    virtual int         rowCount(const QModelIndex& parent=QModelIndex()) const override;
    virtual int         columnCount(const QModelIndex& parent=QModelIndex()) const override;
    virtual QVariant    data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

    //(well-behaved method(s))
    virtual QVariant    headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const override;

        virtual Qt::DropActions supportedDropActions(void) const override;
        virtual Qt::DropActions supportedDragActions(void) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool canDropMimeData(const QMimeData *data,
      Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QStringList mimeTypes(void) const override;
};







// ========= INLINEZ ==========


inline VMUDevice* VmuFileSystemTableModel::getDevice(void) const { return _dev; }

#endif // GYRO_VMU_FILESYSTEM_MODEL_HPP
