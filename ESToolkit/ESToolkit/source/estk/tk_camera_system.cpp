#include <estk/tk_camera_system.h>

TkCameraSystem::TkCameraSystem(void) {

    //Make sure the rest of the stack is initialized, don't give a shit when we're aren't using EStk.
    for(unsigned i = 1; i < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE; ++i) {
        _states[i] = _states[0];
        _states[i]->updateViewVolumeBoundingRegion();
    }

}

void TkCameraSystem::setCurrentIndex(const unsigned index) {

    elysian::CameraSystemBase::setCurrentIndex(index);
    Q_EMIT cameraIndexChanged(index);
}
