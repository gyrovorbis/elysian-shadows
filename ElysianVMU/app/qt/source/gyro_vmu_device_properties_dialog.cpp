#include <ElysianVMU/gyro_vmu_device_properties_dialog.hpp>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <QDir>
#include "ui_gyro_vmu_device_properties_dialog.h"

using namespace QESTkUtils;

QPixmap* VmuDevicePropertiesDialog::_builtinVolIcons[VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX] = { nullptr };


VmuDevicePropertiesDialog::VmuDevicePropertiesDialog(QWidget* parent):
    QDialog(parent),
    _ui(new Ui::VmuDevicePropertiesDialog)
{
    _ui->setupUi(this);

    if(!_builtinVolIcons[0] || _builtinVolIcons[0]->isNull()) {
        _loadBuiltinVolumeIcons();
    }
    for(unsigned i = 0; i < VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX; ++i) {
        _ui->volumeIconCombobox->addItem(QString::number(i));
        _ui->volumeIconCombobox->setItemIcon(i, getBuiltinVolumeIcon(i));
    }

    connect(_ui->volumeIconCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(onVolumeIconChanged(int)));
    connect(_ui->rSpinbox, SIGNAL(valueChanged(int)), this, SLOT(onVolumeColorSpinboxChanged(int)));
    connect(_ui->gSpinbox, SIGNAL(valueChanged(int)), this, SLOT(onVolumeColorSpinboxChanged(int)));
    connect(_ui->bSpinbox, SIGNAL(valueChanged(int)), this, SLOT(onVolumeColorSpinboxChanged(int)));
    connect(_ui->aSpinbox, SIGNAL(valueChanged(int)), this, SLOT(onVolumeColorSpinboxChanged(int)));


    //SET VOLUME COLOR
}

void VmuDevicePropertiesDialog::onVolumeIconChanged(int index) {
    _dev.setVolumeIconShape(index);
    _ui->volumeIcon->setPixmap(getBuiltinVolumeIcon(index));

}

void VmuDevicePropertiesDialog::_updateAll(void) {
    _updateFormatDate();
    _updateFileCount();
    _updateCapacity();
    _updateAvailable();
    _updateVolumeColor();
    _updateVolumeIcon();
    _updateVmuIcon();
    _updateBiosBg();
}

bool VmuDevicePropertiesDialog::_loadBuiltinVolumeIcons(void) {
    bool success = true;

    Debug::log(Debug::LOG_VERBOSE, "VmuDevicePropertiesDialog: Loading built-in DC volume icons from [%s]", DC_VOLUME_ICONS_BUILTIN_BASE_PATH);
    Debug::push();

    QDir root(DC_VOLUME_ICONS_BUILTIN_BASE_PATH);

    if(!root.exists()) {
        Debug::log(Debug::LOG_ERROR, "Folder doesn't even exist!");
        success = false;
    } else {

        for(unsigned i = 0; i < VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX; ++i) {

            _builtinVolIcons[i] = new QPixmap();
            if(!_builtinVolIcons[i]->load(root.filePath(QString("%1.png").arg(i)))) {
                Debug::log(Debug::LOG_ERROR, "Failed to load icon[%u]!", i);
                success = false;
            }
        }
    }

    Debug::pop();
    return success;

}

inline QPixmap VmuDevicePropertiesDialog::getBuiltinVolumeIcon(unsigned index) {
    Q_ASSERT(index < VMU_FLASH_ROOT_BLOCK_ICON_SHAPE_MAX);
    return _builtinVolIcons[index]? *_builtinVolIcons[index] : QPixmap();
}

void VmuDevicePropertiesDialog::_updateFormatDate(void) {

}
void VmuDevicePropertiesDialog::_updateFileCount(void) {
    _ui->fileCountValue->setText(QString::number(_dev.getFlashFileCount()));
}
void VmuDevicePropertiesDialog::_updateCapacity(void) {
    auto mem = _dev.getFlashMemoryUsage();
    unsigned capacity = mem.blocksDamaged + mem.blocksFree + mem.blocksUsed;
    _ui->capacityValue->setText(QString::number(capacity));
}
void VmuDevicePropertiesDialog::_updateAvailable(void) {
    auto mem = _dev.getFlashMemoryUsage();
    _ui->availableValue->setText(QString::number(mem.blocksFree));
}
void VmuDevicePropertiesDialog::_updateVolumeColor(void) {
    auto* root = _dev.getFlashRootBlock();
#if 0
    _ui->rSpinbox->setValue(root->volumeLabel.vmu.r);
    _ui->gSpinbox->setValue(root->volumeLabel.vmu.g);
    _ui->bSpinbox->setValue(root->volumeLabel.vmu.b);
    _ui->aSpinbox->setValue(root->volumeLabel.vmu.a);
#else
   auto color = _dev.getVolumeColor();
   int a, r, g, b;
   color.getRgb(&r, &g, &b, &a);
   _ui->rSpinbox->setValue(r);
   _ui->gSpinbox->setValue(g);
   _ui->bSpinbox->setValue(b);
   _ui->aSpinbox->setValue(a);
#endif
}

void VmuDevicePropertiesDialog::_updateVolumeIcon(void) {
    uint16_t shape = _dev.getVolumeIconShape();
    _ui->volumeIconCombobox->setCurrentIndex(shape);
    _ui->volumeIcon->setPixmap(getBuiltinVolumeIcon(shape));
}
void VmuDevicePropertiesDialog::_updateVmuIcon(void) {

}
void VmuDevicePropertiesDialog::_updateBiosBg(void) {

}

void VmuDevicePropertiesDialog::onVolumeColorSpinboxChanged(int value) {
    _dev.setVolumeColor(QColor(_ui->rSpinbox->value(),
                               _ui->gSpinbox->value(),
                               _ui->bSpinbox->value(),
                               _ui->aSpinbox->value()));

}
