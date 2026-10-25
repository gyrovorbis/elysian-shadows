/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ESGamma/tk_camera_system.h>
#include <ESToolkit/core/tk_assets.h>

TkCameraSystem::TkCameraSystem(void) {

    //Make sure the rest of the stack is initialized, don't give a shit when we're aren't using EStk.
    for(unsigned i = 1; i < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE; ++i) {
        _states[i] = _states[0];
        _states[i]->updateViewVolumeBoundingRegion();
    }

    connect(getAssets(), SIGNAL(areaChanging(Area*const)), this, SLOT(onAreaChanging(Area*const)));

}

void TkCameraSystem::setCurrentIndex(const unsigned index) {

    elysian::CameraSystemBase::setCurrentIndex(index);
    Q_EMIT cameraIndexChanged(index);
}

void TkCameraSystem::onAreaChanging(Area *const area) {
    //Store current keyframe state within old area (if there was one)
    if(_curArea) {
        _curArea->setCameraKeyFrame(getKeyFrame());
    }

    //Load new keyframe state stored in new area (if there is one)
    if(area) {
        moveToKeyFrame(area->getCameraKeyFrame());
    }
}
