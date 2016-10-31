#ifndef ELYSIAN_CAMERA_SYSTEM_BASE_HPP
#define ELYSIAN_CAMERA_SYSTEM_BASE_HPP

#include <elysian_system.hpp>
#include <primitive/elysian_instance_transform.hpp>
#include <utility/elysian_subprocess_manager.hpp>
#include <level/elysian_zcoords.hpp>
#include <utility/elysian_camera_shake_subprocess.hpp>

#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>

#define ELYSIAN_CAMERA_SYSTEM_STACK_SIZE			5
#define ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE		0.1f
#define ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON	0.01f

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class   Keyboard;
    class   CameraSystemBase;
    struct  Camera;

    class CameraKeyFrame: public InstanceTransform {
        friend class            CameraSystemBase;
        friend struct           Camera;
    private:

        float                   _fov        = 45.0f;
    public:

        float                   getFov(void) const;
        void                    setFov(const float fov);

        void                    linearInterpolate(const CameraKeyFrame& startKeyFrame, const CameraKeyFrame& endKeyFrame,
                                    const float interval, const float elapsed);
    };

    using CameraKeyFrameLinearInterpolator = LinearInterpolatorSubProcess<CameraKeyFrame, CameraSystemBase>;


    struct Camera: public CameraKeyFrame, public GYCamera {

        Entity*                 focalEntity     = nullptr;
        std::vector<Entity*>    entities;

        //GYVector3 dir		= { 0.0f, 1.0f, 1.0f };
        GYVector3               up              = { 0.0f, 0.0f, 1.0f };
        float                   staticScale     = 1.0f;

        //dynamic camera boundaries
        float                   maxScale		= 5.0f;
        float                   minScale		= 0.5f;
        float                   baseScale		= 1.0f;

        //Bounding Geometry
        int                     scrStartX       = 0;
        int                     scrStartY       = 0;
        int                     scrEndX         = 0;
        int                     scrEndY         = 0;

        void                    updateViewVolumeBoundingRegion(void);
        void                    clipBoundingRegion(const int startX, const int endX, const int startY, const int endY);
        void                    clipBoundingRegion(const Camera& cam);

        bool                    isWithinScreenRegion(const float x, const float y, const float width, const float height) const;
    };

    class CameraSystemBase:
            public SubProcessManager<CameraSystemBase,
                                    CameraKeyFrameLinearInterpolator,
                                    CameraShakeSubProcess>,
            public System
    {
        friend                  CameraKeyFrameLinearInterpolator;

    protected:
        Camera                  _internalStates[ELYSIAN_CAMERA_SYSTEM_STACK_SIZE];
        Camera*                 _states[ELYSIAN_CAMERA_SYSTEM_STACK_SIZE];
        Camera                  _debugState;
        Camera                  _uiCamera;
        Camera*                 _curState           = &_internalStates[0];
        int						_depth              = 0; //depth is technically +1... my code, fuck you.
        bool					_projViewActive     = false;
        bool					_debugActive        = false;
        bool					_customState        = false;
        bool                    _followEnabled      = true;

        unsigned				_scrWidth           = 0;
        unsigned				_scrHeight          = 0;

        int						_topBoundary        = -1;
        int						_bottomBoundary     = -1;
        int						_leftBoundary       = -1;
        int						_rightBoundary      = -1;

        bool					_dynamic            = true;
        bool                    _boundByMap         = true;

        void                    _updateState(Camera*const state, const float deltaTime);

    public:
        enum    SUBPROC_ID      { SUBPROC_KEY_FR_INTERP, SUBPROC_CAM_SHAKE };

                                CameraSystemBase(void);

        void                    initializeProjMatrix(void);
        bool                    isBoundByMap(void) const;
        void                    setBoundByMap(const bool val);
        bool                    isDebug(void) const;
        bool                    isFollowEnabled(void) const;
        void                    setFollowEnabled(const bool val);

        void                    setCurrentIndex(const unsigned index);
        int                     getCurrentIndex(void) const;

        void                    pushCustomCamera(Camera*const state);
        void                    pushPerspectiveState(void);
        void                    setCurrentCamera(Camera*const state);
        void                    processInput(const Keyboard* input);
        CameraKeyFrame          getKeyFrame(void) const;
        void                    setFollowEntity(Entity*const entity);
        void                    setBaseScale(const float scale);
        Entity*                 getFollowEntity(void) const;
        void                    addEntity(Entity*const entity) const;
        void                    removeEntity(Entity*const entity) const;
        void                    setPosition(const GYVector3& pos);
        void                    translate(const GYVector3& dist);
        void                    translateOffset(const GYVector3& dist);
        void                    scale(const float factor, const float time=0.0f, EventMonitor*const monitor=nullptr);
        void                    moveToKeyFrame(const CameraKeyFrame& frame, const float time=0.0f, EventMonitor*const monitor=nullptr);
        void                    shake(const GYVector3& displacement, unsigned int oscillations, const float time, const bool fade=false, EventMonitor*const monitor=nullptr);
        void                    staticScale(const float factor);
        void                    setStaticScale(const float factor);
        float                   getStaticScale(void) const;
        void                    setScale(const float /*factor*/, const float time=0.0f, EventMonitor*const monitor=nullptr);
        void                    setPerspectiveFrustum(const float fov, const float aspect, const float znear, const float zfar);
        void                    setDirection(const GYVector3 &dir);
        void                    setUpDirection(const GYVector3 &dir);
        void                    pushViewProjMatrix(void);
        void                    popViewProjMatrix(void);
        void                    pushState(void);
        void                    popState(void);
        Camera*                 getCurrentState(void) const;
        bool                    isOnscreen(const int x, const int y, const int width, const int height) const;
        void                    getVisibleTileRegion(int& startX, int& startY, int& endX, int& endY, const Camera* state=nullptr) const;
        void                    pushViewMatrixToStack(void);
        void                    popViewMatrixFromStack(void);

        //Transforms a WORLD COORDINATE vector into SCREEN COORDINATES
        void                    transform(GYVector3* dest, const GYVector3* src);
        void                    transform(GYVector4* dest, const GYVector4* src);

        //Transforms a SCREEN COORDINATE vector into WORLD COORDINATES
        void                    inverseTransform(GYVector3* dest, const GYVector3* src);

        void                    update(void);
        bool                    isOnscreen(const Entity &entity);
        void                    setBoundaries(const float top, const float bottom, const float left, const float right);
        void                    setDynamic(const bool value);


    };









    // ======== INLINEZ =======
#if 0
    inline float CameraKeyFrame::getScale(void) const { return _scale; }
    inline void CameraKeyFrame::setScale(const float sc) { _scale = sc; }
    inline void CameraKeyFrame::setPos(const float x, const float y, const float z) { _pos.x = x; _pos.y = y; _pos.z = z;}
    inline void CameraKeyFrame::setZOrient(const float zOrient) { _zOrient = zOrient; }
    inline void CameraKeyFrame::setXOrient(const float xOrient) { _xOrient = xOrient; }
    inline void CameraKeyFrame::setDir(const float x, const float y, const float z) { _dir.x = x; _dir.y = y; _dir.z = z; }
    inline void CameraKeyFrame::setOffset(const GYVector3 &vec) { _offset = vec; }
    inline constexpr float CameraKeyFrame::getAngle(void) const { return _angle; }
    inline void CameraKeyFrame::setAngle(const float angle) { _angle = angle; }
    inline void CameraKeyFrame::setOffset(const float x, const float y, const float z) { _offset.x = x; _offset.y = y; _offset.z = z; }
    inline void CameraKeyFrame::getPos(float& x, float& y, float& z) const { x = _pos.x; y = _pos.y; z = _pos.z; }
    inline void CameraKeyFrame::setPos(const float x, const float y) { _pos.x = x; _pos.y = y; }
    inline void CameraKeyFrame::getOffset(float& x, float& y, float& z) const { x = _offset.x; y = _offset.y; z = _offset.z; }
    constexpr inline float CameraKeyFrame::getZOrient(void) const { return _zOrient; }
    constexpr inline float CameraKeyFrame::getXOrient(void) const { return _xOrient; }
    inline void CameraKeyFrame::getDir(float& x, float& y, float& z) const { x = _dir.x; y = _dir.y; z = _dir.z; }
    inline void CameraKeyFrame::getPos(float &x, float &y) const { x = _pos.x; y = _pos.y; }
#else
    inline float CameraKeyFrame::getFov(void) const { return _fov; }
    inline void CameraKeyFrame::setFov(const float fov) { _fov = fov; }
#endif
    inline void CameraKeyFrame::linearInterpolate(const CameraKeyFrame& startKeyFrame, const CameraKeyFrame& endKeyFrame,
                                        const float interval, const float elapsed) {

        /* Do some fucking terrible, type-unsafe casting to batch process linear interpolation
         * of all floats in the struct with the SIMD instructions. Ugly, but fast as shit. */
        gyLinearInterp((float*)&_pos.x, 6,
                       (float*)&startKeyFrame._pos.x, (float*)&endKeyFrame._pos.x,
                       interval, elapsed);
#if 0
        gyLinearInterp((float*)&_dir.x, 3,
                       (float*)&startKeyFrame._dir.x, (float*)&endKeyFrame._dir.x,
                       interval, elapsed);

        gyLinearInterp((float*)&_offset.x, 3,
                       (float*)&startKeyFrame._offset.x, (float*)&endKeyFrame._offset.x,
                       interval, elapsed);

        gyVec3Normalize(&_dir);
#endif
/*
        gyLinearInterp((float*)&_scale, 1,
                       (float*)&startKeyFrame._scale, (float*)&endKeyFrame._scale,
                       interval, elapsed);

        gyLinearInterp((float*)&_xOrient, 1,
                       (float*)&startKeyFrame._xOrient, (float*)&endKeyFrame._xOrient,
                       interval, elapsed);

        gyLinearInterp((float*)&_zOrient, 1,
                       (float*)&startKeyFrame._zOrient, (float*)&endKeyFrame._zOrient,
                       interval, elapsed);*/
    }

    inline bool CameraSystemBase::isOnscreen(const int x, const int y, const int halfWidth, const int halfHeight) const {
#if 0
        const int distX = abs((int)x-(int)_curState->_keyFrame._pos.x);
        const int distY = abs((int)y-(int)_curState->_keyFrame._pos.y)/(_curState->_keyFrame._scale*_curState->staticScale);

        const int maxDistX = (halfWidth + (_curState->scrEndX-_curState->scrStartX)*0.5f)/(_curState->_keyFrame._scale*_curState->staticScale);
        const int maxDistY = (halfHeight + (_curState->scrEndY-_curState->scrStartY)*0.5f)/(_curState->_keyFrame._scale*_curState->staticScale);

        return (distY < maxDistY && distX < maxDistX);
#else
        (void)x;
        (void)y;
        (void)halfWidth;
        (void)halfHeight;
        return true;
#endif

    }

    inline bool Camera::isWithinScreenRegion(const float x, const float y, const float halfWidth, const float halfHeight) const {
#if 1

        const float scrX = ((float)scrStartX + (float)scrEndX)/2.0f;
        const float scrY = ((float)scrStartY + (float)scrEndY)/2.0f;

        const float distX = fabs(x-scrX);
        const float distY = fabs(y-scrY);

        const float maxDistX = ((float)scrEndX-(float)scrStartX)*0.5f + halfWidth;
        const float maxDistY = ((float)scrEndY-(float)scrStartY)*0.5f + halfHeight;

        return (distY < maxDistY && distX < maxDistX);
#else
        (void)x;
        (void)y;
        (void)halfWidth;
        (void)halfHeight;

        return true;
#endif
    }

    inline void CameraSystemBase::moveToKeyFrame(const CameraKeyFrame &frame, const float time, EventMonitor * const monitor) {
        auto &interp = get<SUBPROC_KEY_FR_INTERP>(static_cast<SubProcParent&>(*this));
        interp.setKeyFrames(_curState, frame);
        interp.setTimeInterval(time);
        interp.setEventMonitor(monitor);
        interp.start(this);
    }

    inline void CameraSystemBase::shake(const GYVector3& displacement, unsigned int oscillations, const float time, const bool fade, EventMonitor*const monitor) {
       auto &camShake = get<SUBPROC_CAM_SHAKE>(static_cast<SubProcParent& >(*this));
       camShake.setDisplacement(displacement);
       camShake.setOscillations(oscillations);
       camShake.setDuration(time);
       camShake.setFade(fade);
       camShake.setEventMonitor(monitor);
       camShake.start(this);
    }

    inline bool CameraSystemBase::isBoundByMap(void) const { return _boundByMap; }
    inline void CameraSystemBase::setBoundByMap(const bool val) { _boundByMap = val; }

    //doesn't take camera rotation into account yet!
    inline void CameraSystemBase::inverseTransform(GYVector3 *dest, const GYVector3 *src) {
        unsigned w, h;
        gyVidResolution(&w, &h);
        auto screenOffset = GYVector3 { (float)w, (float)h, 0.0f };

        *dest = *src;
        gyVec3AddScaled(dest, &screenOffset, -0.5f);
        gyVec3ScaleAccum(dest, 1.0f/(_curState->getScale().x*_curState->staticScale));
        gyVec3AddAccum(dest, &_curState->_pos);
    }

    inline bool CameraSystemBase::isDebug(void) const { return (_curState == &_debugState); }

    inline bool CameraSystemBase::isFollowEnabled(void) const { return _followEnabled; }
    inline void CameraSystemBase::setFollowEnabled(const bool val) { _followEnabled = val; }

    inline void CameraSystemBase::setCurrentIndex(const unsigned index) {
        _depth = index;
        _curState = _states[_depth];
    }
    inline int CameraSystemBase::getCurrentIndex(void) const { return _depth; }

    inline void CameraSystemBase::pushCustomCamera(Camera*const state) {
        ++_depth;
        assert(_depth < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE);
        setCurrentCamera(state);
        //_curState = state;
        _customState = true;
    }

    inline void CameraSystemBase::pushPerspectiveState(void) {
        _debugActive = true;
        pushCustomCamera(&_debugState);
    }

    inline CameraKeyFrame CameraSystemBase::getKeyFrame(void) const {
        return *static_cast<CameraKeyFrame*>(_curState);
    }

    inline void CameraSystemBase::setFollowEntity(Entity*const entity) {
        _curState->focalEntity = entity;
    }

    inline void CameraSystemBase::setBaseScale(const float scale) {
        _curState->baseScale = scale;
    }

    inline Entity* CameraSystemBase::getFollowEntity(void) const {
        return _curState->focalEntity;
    }

    inline void CameraSystemBase::addEntity(Entity*const entity) const {
        _curState->entities.push_back(entity);
    }

    inline void CameraSystemBase::removeEntity(Entity*const entity) const {
        for(int i = 0; i < (int)_curState->entities.size(); ++i)
            if(_curState->entities[i] == entity) {
                _curState->entities.erase(_curState->entities.begin()+i);
                return;
            }
    }

    inline void CameraSystemBase::setPosition(const GYVector3 &pos) {
        _curState->_pos = pos;
    }

    inline void CameraSystemBase::translate(const GYVector3 &dist) {
        gyVec3Add(&_curState->_pos, &_curState->_pos, &dist);
    }

    inline void CameraSystemBase::translateOffset(const GYVector3& dist) {
        gyVec3AddAccum(&_curState->_pos, &dist);
    }

    inline void CameraSystemBase::scale(const float factor, const float time, EventMonitor*const monitor) {
        auto &interp = get<SUBPROC_KEY_FR_INTERP>(static_cast<SubProcParent&>(*this));
        //if(interp.getProcState() == PROC_STATE_RUNNING)

        CameraKeyFrame frame = getKeyFrame();
        frame.setScale(frame.getScale().x*factor, frame.getScale().y*factor, frame.getScale().z*factor);
        interp.setKeyFrames(_curState, frame);
        interp.setTimeInterval(time);
        interp.setEventMonitor(monitor);
        interp.start(this);
    }

    inline void CameraSystemBase::staticScale(const float factor) {
        _curState->staticScale *= factor;
    }

    inline void CameraSystemBase::setStaticScale(const float factor) {
        _curState->staticScale = factor;
    }

    inline float CameraSystemBase::getStaticScale(void) const { return _curState->staticScale; }

    inline void CameraSystemBase::setScale(const float /*factor*/, const float time, EventMonitor*const monitor) {
        auto &interp = get<SUBPROC_KEY_FR_INTERP> ( static_cast<SubProcParent&>(*this));

        if(interp.getProcState() == PROC_STATE_RUNNING) return;

        CameraKeyFrame frame = getKeyFrame();
        //frame.setScale(factor);
        interp.setKeyFrames(_curState, frame);
        interp.setTimeInterval(time);
        interp.setEventMonitor(monitor);
        interp.start(this);
    }

    inline void CameraSystemBase::setDirection(const GYVector3 &dir) {
        _curState->setOrient(dir.x, dir.y, dir.z);
       // _curState->_keyFrame._dir = dir;
    }

    inline void CameraSystemBase::setUpDirection(const GYVector3 &dir) {
        _curState->up = dir;
    }

    inline void CameraSystemBase::pushState(void) {
        ++_depth;
        if(_depth <= 0) _depth = 0;
        //assert(_depth < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE);
        _states[_depth] = &_internalStates[_depth];
        setCurrentCamera(_states[_depth]);
    }

    inline Camera* CameraSystemBase::getCurrentState(void) const { return _curState; }

    inline void CameraSystemBase::setBoundaries(const float top, const float bottom, const float left, const float right) {
        _topBoundary = top; _bottomBoundary = bottom; _leftBoundary = left; _rightBoundary = right;
    }

    inline void CameraSystemBase::setDynamic(const bool value) { _dynamic = value; }

}


#endif
