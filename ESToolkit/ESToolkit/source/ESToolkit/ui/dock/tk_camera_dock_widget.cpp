#include <ESToolkit/ui/dock/tk_camera_dock_widget.h>
#include <ui_tk_camera_dock_widget.h>
#include <system/elysian_engine.hpp>
#include <ESGamma/tk_camera_system.h>
#include <system/elysian_light_system.hpp>

float CameraDockWidget::normValue(const float position, const QSlider* slider) {
    return (float)((float)position/(float) ( ((float)slider->maximum()) -((float)slider->minimum()) )); //TROLOLO
}

CameraDockWidget::CameraDockWidget(QWidget *parent): QDockWidget(parent), _ui(new Ui::CameraDockWidget) {
    _ui->setupUi(this);

    _ui->angleSlider->setMaximum(TK_CAMERA_DOCK_WIDGET_SLIDER_PRECISION);
    _ui->posXSlider->setMaximum(ELYSIAN_AREA_MAP_WIDTH*32);
    _ui->posYSlider->setMaximum(ELYSIAN_AREA_MAP_HEIGHT*32);
    _ui->posZSlider->setMinimum(1);
    _ui->posZSlider->setMaximum(ELYSIAN_AREA_MAP_HEIGHT*32);
    _ui->orientXSlider->setMaximum(TK_CAMERA_DOCK_WIDGET_SLIDER_PRECISION);
    _ui->orientYSlider->setMaximum(TK_CAMERA_DOCK_WIDGET_SLIDER_PRECISION);
    _ui->orientZSlider->setMaximum(TK_CAMERA_DOCK_WIDGET_SLIDER_PRECISION);

    for(unsigned i = 0; i < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE; ++i) {
        _ui->cameraComboBox->addItem(QString::number(i));
    }

    connect(getAssets(), SIGNAL(engineInitialize()), this, SLOT(onEngineInitialize()));
    connect(getAssets(), SIGNAL(engineUpdated()), this, SLOT(onEngineUpdated()));
}

void CameraDockWidget::onEngineInitialize(void) {
    typedef elysian::Camera Camera;
    _cameraSystem = getAssets()->getEngine()->getCameraSys();
    _cameraState = _cameraSystem->getCurrentState();
    connect(_cameraSystem, SIGNAL(cameraIndexChanged(unsigned)), this, SLOT(onCameraIndexChange(unsigned)));

}


void CameraDockWidget::onEngineUpdated(void) {
    if(!isVisible()) return;

    GYVector3 pos, dir;

    //grab keyframe values for current frame
    _cameraState->getPos(pos.x, pos.y, pos.z);
    _cameraState->getOrient(dir.x, dir.y, dir.z);

    //update textboxes
    if(!_ui->posXEdit->hasFocus()) _ui->posXEdit->setText(QString::number(pos.x));
    if(!_ui->posYEdit->hasFocus()) _ui->posYEdit->setText(QString::number(pos.y));
    if(!_ui->posZEdit->hasFocus()) _ui->posZEdit->setText(QString::number(pos.z));

    if(!_ui->orientXEdit->hasFocus()) _ui->orientXEdit->setText(QString::number(dir.x));
    if(!_ui->orientYEdit->hasFocus()) _ui->orientYEdit->setText(QString::number(dir.y));
    if(!_ui->orientZEdit->hasFocus()) _ui->orientZEdit->setText(QString::number(dir.z));
    if(!_ui->angleEdit->hasFocus()) _ui->angleEdit->setText(QString::number(_cameraState->getAngle()));

    //update sliders
    if(!_ui->posXSlider->hasFocus()) _ui->posXSlider->setValue(pos.x);
    if(!_ui->posYSlider->hasFocus()) _ui->posYSlider->setValue(pos.y);
    if(!_ui->posZSlider->hasFocus()) _ui->posZSlider->setValue(pos.z);

    if(!_ui->angleSlider->hasFocus()) {
        float val = _cameraState->getAngle();
        float normValue = val / (2.0f*M_PI);
        float value = normValue * (float)_ui->angleSlider->maximum();
        _ui->angleSlider->setValue(value);
    }

    if(!_ui->orientXSlider->hasFocus()) {
        float normValue = dir.x;
        float value = normValue * (float)_ui->orientXSlider->maximum();
        _ui->orientXSlider->setValue(value);
    }
    if(!_ui->orientYSlider->hasFocus()) {
        float normValue = dir.y;
        float value = normValue * (float)_ui->orientYSlider->maximum();
        _ui->orientYSlider->setValue(value);
    }
    if(!_ui->orientZSlider->hasFocus()) {
        float normValue = dir.z;
        float value = normValue * (float)_ui->orientZSlider->maximum();
        _ui->orientZSlider->setValue(value);
    }

}

void CameraDockWidget::onCameraIndexChange(const unsigned index) {
    //if(getAssets()->getEngine()->getLightSys()->isShadowPass()) return;
    _cameraState = _cameraSystem->getCurrentState();

    bool blocked = _ui->cameraComboBox->blockSignals(true);
    _ui->cameraComboBox->setCurrentIndex(index);
    _ui->cameraComboBox->blockSignals(blocked);

    //Update camera boundaries
   _cameraState->updateViewVolumeBoundingRegion();
   _cameraState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);
}


void CameraDockWidget::on_cameraComboBox_currentIndexChanged(int index)
{
    if(!_cameraSystem) return;
    _cameraSystem->popState();
    _cameraSystem->pushState();
  //  const bool wasActive = _ui->cameraComboBox->blockSignals(true);
    _cameraSystem->setCurrentIndex(index);
  //  _ui->cameraComboBox->blockSignals(wasActive);
}

void CameraDockWidget::on_orthoButton_clicked(bool checked)
{
    if(checked) {
        unsigned w, h;
        gyVidResolution(&w, &h);
        gyVidCamOrthoProj(_cameraState, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 1000.0f);
        _ui->perspectiveButton->setChecked(false);
    } else {
        _ui->perspectiveButton->click();
    }
    _cameraState->updateViewVolumeBoundingRegion();
    _cameraState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);
}

void CameraDockWidget::on_perspectiveButton_clicked(bool checked)
{
    if(checked) {
        unsigned w, h;
        gyVidResolution(&w, &h);
        gyVidCamFrustum(_cameraState, 45.0f, 1.0f, 0.5f, 900.0f);
        _ui->orthoButton->setChecked(false);
    } else {
        _ui->orthoButton->click();
    }
    _cameraState->updateViewVolumeBoundingRegion();
    _cameraState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);
}

void CameraDockWidget::on_resetTransformButton_clicked()
{

}

void CameraDockWidget::on_orientXEdit_editingFinished()
{
    bool valid;
    const float orient = _ui->orientXEdit->text().toFloat(&valid);

    if(valid) {
        float x, y, z;
        _cameraState->getOrient(x, y, z);
        _cameraState->setOrient(orient, y, z);
    }
}

void CameraDockWidget::on_orientYEdit_editingFinished()
{
    bool valid;
    const float orient = _ui->orientYEdit->text().toFloat(&valid);

    if(valid) {
        float x, y, z;
        _cameraState->getOrient(x, y, z);
        _cameraState->setOrient(x, orient, z);
    }
}

void CameraDockWidget::on_orientZEdit_editingFinished()
{
    bool valid;
    const float orient = _ui->orientZEdit->text().toFloat(&valid);

    if(valid) {
        float x, y, z;
        _cameraState->getOrient(x, y, z);
        _cameraState->setOrient(x, y, orient);
    }
}

void CameraDockWidget::on_posXEdit_editingFinished()
{
    bool valid;
    const float pos = _ui->posXEdit->text().toFloat(&valid);

    if(valid) {
        float x, y;
        _cameraState->getPos(x, y);
        _cameraState->setPos(pos, y);
    }
}

void CameraDockWidget::on_posYEdit_editingFinished()
{
    bool valid;
    const float pos = _ui->posYEdit->text().toFloat(&valid);

    if(valid) {
        float x, y;
        _cameraState->getPos(x, y);
        _cameraState->setPos(x, pos);
    }
}

void CameraDockWidget::on_posZEdit_editingFinished()
{
    bool valid;
    const float pos = _ui->posZEdit->text().toFloat(&valid);

    if(valid) {
        float x, y;
        _cameraState->getPos(x, y);
        _cameraState->setPos(x, y, pos);
    }
}

void CameraDockWidget::on_orientXSlider_sliderMoved(int position)
{

    float x, y, z;
    _cameraState->getOrient(x, y, z);

    GYVector3 orient = { normValue(position, _ui->orientXSlider), y, z};

    if(orient.x == 0.0f && orient.y == 0.0f && orient.z == 0.0f) return;

    gyVec3Normalize(&orient);

    _cameraState->setOrient(orient.x, orient.y, orient.z);
}

void CameraDockWidget::on_orientYSlider_sliderMoved(int position)
{
    float x, y, z;
    _cameraState->getOrient(x, y, z);


    GYVector3 orient = { x, normValue(position, _ui->orientYSlider), z};
    if(orient.x == 0.0f && orient.y == 0.0f && orient.z == 0.0f) return;
    gyVec3Normalize(&orient);

    _cameraState->setOrient(orient.x, orient.y, orient.z);
}

void CameraDockWidget::on_orientZSlider_sliderMoved(int position)
{
    float x, y, z;
    _cameraState->getOrient(x, y, z);


    GYVector3 orient = { x, y, normValue(position, _ui->orientZSlider)};

    if(orient.x == 0.0f && orient.y == 0.0f && orient.z == 0.0f) return;
    gyVec3Normalize(&orient);

    _cameraState->setOrient(orient.x, orient.y, orient.z);
}

void CameraDockWidget::on_posXSlider_sliderMoved(int position)
{
    float x, y;
    _cameraState->getPos(x, y);
    _cameraState->setPos(position, y);
}

void CameraDockWidget::on_posYSlider_sliderMoved(int position)
{
    float x, y;
    _cameraState->getPos(x, y);
    _cameraState->setPos(x, position);
}

void CameraDockWidget::on_posZSlider_sliderMoved(int position)
{
    float x, y;

    const float z = _cameraState->projType == GY_PROJECTION_ORTHO ?
                1.0f-normValue(position, _ui->posZSlider) : position;

    _cameraState->getPos(x, y);
    _cameraState->setPos(x, y, z);
}

void CameraDockWidget::on_angleEdit_selectionChanged()
{
    bool valid;
    const float angle = _ui->angleEdit->text().toFloat(&valid);
    _cameraState->setAngle(angle);
}

void CameraDockWidget::on_angleSlider_sliderMoved(int position)
{

    _cameraState->setAngle(normValue(position, _ui->angleSlider)*2.0f*M_PI);
}
