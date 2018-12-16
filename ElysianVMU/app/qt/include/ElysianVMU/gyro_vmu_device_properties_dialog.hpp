#ifndef GYRO_VMU_DEVICE_PROPERTIES_DIALOG_HPP
#define GYRO_VMU_DEVICE_PROPERTIES_DIALOG_HPP

#include <QDialog>
#include <gyro_elysian_vmu_device.hpp>
#include <evmu-core/gyro_vmu_flash.h>

#define DC_VOLUME_ICONS_BUILTIN_BASE_PATH ":/volume_icons"

class   QSettings;
struct  VMUDevice;

namespace Ui {
    class VmuDevicePropertiesDialog;
}

class VmuDevicePropertiesDialog : public QDialog {
    Q_OBJECT
private:
    Ui::VmuDevicePropertiesDialog*  _ui = nullptr;
    EVmuDevice                      _dev;

    static QPixmap*                  _builtinVolIcons[VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX];


    static bool        _loadBuiltinVolumeIcons(void);




    void        _updateFormatDate(void);
    void        _updateFileCount(void);
    void        _updateCapacity(void);
    void        _updateAvailable(void);
    void        _updateVolumeColor(void);
    void        _updateVolumeIcon(void);
    void        _updateVmuIcon(void);
    void        _updateBiosBg(void);
    void        _updateAll(void);

public:
    explicit            VmuDevicePropertiesDialog(QWidget* parent=nullptr);


    void                setDevice(EVmuDevice dev);



    static QPixmap      getBuiltinVolumeIcon(unsigned index);

public Q_SLOTS:
    void onVolumeIconChanged(int index);
    void onVolumeColorSpinboxChanged(int value);

};




//====== INLINEZ =======
inline void VmuDevicePropertiesDialog::setDevice(EVmuDevice dev) {
    //if(_dev != dev) {
        _dev = dev;
        _updateAll();
    //}
}



#endif // GYRO_VMU_DEVICE_PROPERTIES_DIALOG_HPP
