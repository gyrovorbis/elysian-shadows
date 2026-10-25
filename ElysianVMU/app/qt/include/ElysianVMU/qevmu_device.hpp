#ifndef QEVMU_DEVICE_HPP
#define QEVMU_DEVICE_HPP

#include <evmu-core-cpp/evmu_device.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <ElysianVMU/qevmu_flash.hpp>

class QtVmuDevice: public evmu::VmuDevice {
public:

                        QtVmuDevice(void) = default;
                        QtVmuDevice(VMUDevice* dev);

    static QPixmap      getBuiltinVolumeIcon(unsigned index);

    QPixmap             getVolumeDcIcon(void) const;
    QPixmap             getVolumeVmuIcon(void) const;
    QPixmap             getExtraBgPvrTexture(void) const;

    QColor              getVolumeColor(void) const;
    void                setVolumeColor(QColor color);

    QtVmuFlashDirEntry  getGameFlashDirEntry(void) const;
    QtVmuFlashDirEntry  getIconDataVmsFlashDirEntry(void) const;
    QtVmuFlashDirEntry  getExtraBgPvrFlashDirEntry(void) const;
    QtVmuFlashDirEntry  getFileFlashDirEntry(int index) const;
    QtVmuFlashDirEntry  findFlashDirEntry(const char* name) const;

                        operator VMUDevice*(void) const;
    const QtVmuDevice&  operator =(VMUDevice* dev);
    bool                operator ==(const QtVmuDevice& rhs) const;
    bool                operator !=(const QtVmuDevice& rhs) const;

};



//====== INLINEZ ========
inline QtVmuDevice::QtVmuDevice(VMUDevice* dev):
    evmu::VmuDevice(dev)
{}

inline QtVmuFlashDirEntry QtVmuDevice::getGameFlashDirEntry(void) const {
    return QtVmuFlashDirEntry(_dev, gyVmuFlashDirEntryGame(_dev));
}
inline QtVmuFlashDirEntry QtVmuDevice::getIconDataVmsFlashDirEntry(void) const {
    return QtVmuFlashDirEntry(_dev, gyVmuFlashDirEntryIconData(_dev));
}
inline QtVmuFlashDirEntry QtVmuDevice::getExtraBgPvrFlashDirEntry(void) const {
    return QtVmuFlashDirEntry(_dev, gyVmuFlashDirEntryExtraBgPvr(_dev));
}
inline QtVmuFlashDirEntry QtVmuDevice::getFileFlashDirEntry(int index) const {
    return QtVmuFlashDirEntry(_dev, gyVmuFlashFileAtIndex(_dev, index));
}
inline QtVmuFlashDirEntry QtVmuDevice::findFlashDirEntry(const char* name) const {
    return QtVmuFlashDirEntry(_dev, gyVmuFlashDirEntryFind(_dev, name));
}
inline const QtVmuDevice&  QtVmuDevice::operator =(VMUDevice* dev) {
    *static_cast<evmu::VmuDevice*>(this) = dev;
    return *this;
}

inline QtVmuDevice::operator VMUDevice*(void) const {
    return _dev;
}

inline bool QtVmuDevice::operator ==(const QtVmuDevice& rhs) const {
    return _dev == rhs._dev;
}
inline bool QtVmuDevice::operator !=(const QtVmuDevice& rhs) const {
    return !(*this == rhs);
}


#endif // QEVMU_DEVICE_HPP
