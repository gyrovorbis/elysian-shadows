#ifndef TK_CAMERA_DOCK_WIDGET_H
#define TK_CAMERA_DOCK_WIDGET_H

#include <QDockWidget>
#include <asset/tk_asset_manager.h>
#include <QSlider>

#define TK_CAMERA_DOCK_WIDGET_SLIDER_PRECISION  1000

namespace Ui {
    class CameraDockWidget;
}

namespace elysian {
    struct  Camera;
}

class   TkCameraSystem;

class CameraDockWidget: public QDockWidget, public AssetManager {
    Q_OBJECT
private:
    Ui::CameraDockWidget*   _ui             = nullptr;
    TkCameraSystem*         _cameraSystem   = nullptr;
    elysian::Camera*        _cameraState    = nullptr;

   void                     _updatePosX(const float x);
   void                     _updatePosY(const float y);
   void                     _setPosZ(const float z);


    //void setEnabled(const bool value); Disabling all
protected Q_SLOTS:
    void onCameraIndexChange(const unsigned index);
    void onEngineUpdated(void);

public:
    explicit        CameraDockWidget(QWidget* parent=nullptr);

    static float    normValue(const float value, const QSlider* slider);

public Q_SLOTS:


    void onEngineInitialize(void);

    void on_posZSlider_sliderMoved(int position);
    void on_posYSlider_sliderMoved(int position);
    void on_posXSlider_sliderMoved(int position);
    void on_angleSlider_sliderMoved(int position);

    void on_orientZSlider_sliderMoved(int position);
    void on_orientYSlider_sliderMoved(int position);
    void on_orientXSlider_sliderMoved(int position);

    void on_posZEdit_editingFinished(void);
    void on_posYEdit_editingFinished(void);
    void on_posXEdit_editingFinished(void);

    void on_orientZEdit_editingFinished(void);
    void on_orientYEdit_editingFinished(void);
    void on_orientXEdit_editingFinished(void);
    void on_angleEdit_selectionChanged(void);

    void on_resetTransformButton_clicked(void);
    void on_perspectiveButton_clicked(bool checked);
    void on_orthoButton_clicked(bool checked);
    void on_cameraComboBox_currentIndexChanged(int index);

};




#endif // TK_CAMERA_DOCK_WIDGET_H

