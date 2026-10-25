/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/utility/elysian_camera_shake_subprocess.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>

// ===== STATIC MEMBERS =====

namespace elysian {

CameraSystemBase* CameraShakeSubProcess::_cameraSys = nullptr;

void CameraShakeSubProcess::onStart(void) {}

// ===== CameraShakeSubProcess ======
void CameraShakeSubProcess::_startNextKeyFrame(void) {
    CameraKeyFrame keyFrame = _cameraSys->getKeyFrame();
    GYVector3 translation;
    float time;
    float newMag = _magnitude;
    const float zeroMag = 0.0f;

    if(_fade) {
        gyLinearInterp(&newMag, 1,
                       &_magnitude, &zeroMag,
                       _duration, _elapsedTime);
    }

    switch(_state) {
    case MOVE_ORIG:
        translation = { 0.0f, 0.0f, 0.0f };
        time = _timePerOscillation*0.25f;
        break;
    case MOVE_POS:
        gyVec3Scale(&translation, &_displacement, 0.5f*newMag);
        time = _timePerOscillation*0.25f;
        break;
    case MOVE_NEG:
        gyVec3Scale(&translation, &_displacement, -0.5f*newMag);
        time = _timePerOscillation*0.5f;
        break;
    default: GY_ASSERT(0); //Ya dun fucked up!
    }

    keyFrame.translate(translation);
    _cameraSys->moveToKeyFrame(keyFrame, time, this);
}

void CameraShakeSubProcess::onFinish(void) {
    if(incrementEnum(_state) == MOVE_MAX) {
        _state = MOVE_POS;
        if(--_oscillations == 0) {
            stop(_cameraSys);
            return;
        }
    }
    _startNextKeyFrame();
}

}



