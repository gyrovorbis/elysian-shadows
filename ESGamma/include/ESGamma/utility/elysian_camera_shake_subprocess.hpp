#ifndef ELYSIAN_CAMERA_SHAKE_SUBPROCESS_HPP
#define ELYSIAN_CAMERA_SHAKE_SUBPROCESS_HPP

#include <libGyro/gyro_matrix_api.h>
#include <ESGamma/utility/elysian_subprocess_manager.hpp>

namespace elysian {

    class CameraSystemBase;

    class CameraShakeSubProcess:
            public MonitorableSubProcess<CameraShakeSubProcess, CameraSystemBase>,
            public EventMonitor
    {
        friend class CameraSystemBase;
    private:
        enum MOVE_STATE { MOVE_POS, MOVE_NEG, MOVE_ORIG, MOVE_MAX };

        static CameraSystemBase*    _cameraSys;

        GYVector3               _displacement;
        unsigned                _oscillations;
        float                   _duration;
        float                   _elapsedTime;
        float                   _timePerOscillation;
        float                   _magnitude;
        MOVE_STATE              _state;
        bool                    _fade;

        void                    _startNextKeyFrame(void);

    public:
        void            setDisplacement(const GYVector3& disp);
        void            setDuration(const float dur);
        void            setOscillations(const unsigned osc);
        void            setFade(const bool fade);

        //Overridden from MonitorableSubProcess
        void            _start(CameraSystemBase*const sys);
        void            _stop(CameraSystemBase*const sys);
        void            _update(CameraSystemBase*const sys, const float deltaTime);

        //Overridden from EventMonitor
        virtual void	onStart(void)   override;
        virtual void	onFinish(void)  override;

    };


    //===== CameraShakeSubProcess Inlinez =====

    inline void CameraShakeSubProcess::setDisplacement(const GYVector3& disp) { _displacement = disp; }
    inline void CameraShakeSubProcess::setDuration(const float dur) { _duration = dur; }
    inline void CameraShakeSubProcess::setOscillations(const unsigned osc) { _oscillations = osc; }
    inline void CameraShakeSubProcess::setFade(const bool fade) { _fade = fade; }
    inline void CameraShakeSubProcess::_start(CameraSystemBase*const /*sys*/) {
        _timePerOscillation = (float)(_duration / ((float)_oscillations));
        _state = MOVE_POS;
        _elapsedTime = 0.0f;
        _magnitude = gyVec3MagAndNormalize(&_displacement);
        _startNextKeyFrame();
    }
    inline void CameraShakeSubProcess::_stop(CameraSystemBase*const /* sys */) {}
    inline void CameraShakeSubProcess::_update(CameraSystemBase*const /*sys*/, const float deltaTime) {
        _elapsedTime += deltaTime;
        if(_elapsedTime >= _duration) _elapsedTime = _duration;
    }
}

#endif // ELYSIAN_CAMERA_SHAKE_SUBPROCESS_HPP

