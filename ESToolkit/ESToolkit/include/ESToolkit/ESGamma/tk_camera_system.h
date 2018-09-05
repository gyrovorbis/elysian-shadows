#ifndef TK_CAMERA_SYSTEM_H
#define TK_CAMERA_SYSTEM_H

#include <QObject>
#include <ESGamma/system/elysian_camera_system_base.hpp>
#include <ESToolkit/core/tk_asset_manager.h>

class TkCameraSystem: public QObject, public elysian::CameraSystemBase, public AssetManager {
Q_OBJECT

public:
        TkCameraSystem(void);

    void setCurrentIndex(const unsigned index);

private Q_SLOTS:
    void onAreaChanging(Area*const area);

Q_SIGNALS:

    void cameraIndexChanged(const unsigned index);
    void cameraStateAdded(void);
    void cameraStateRemoved(void);

};

#endif // TK_CAMERA_SYSTEM_H

