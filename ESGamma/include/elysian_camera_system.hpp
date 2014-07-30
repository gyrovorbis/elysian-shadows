#ifndef ELYSIAN_CAMERA_SYSTEM_HPP
#define ELYSIAN_CAMERA_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_instance_transform.hpp"
#include "elysian_subprocess_manager.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_zcoords.hpp"

#include <stdlib.h>

#include <cstdlib>
#include <vector>
#include <gyro.h>

#define ELYSIAN_CAMERA_SYSTEM_STACK_SIZE			5
#define ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE		0.1f
#define ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON	0.01f

namespace elysian {
	
	class Entity;
	class GameInput;
	class CameraSystem;
	struct CameraState;

	class CameraKeyFrame {
		friend class CameraSystem;
		friend struct CameraState;
	private:

        GYVector3	_dir        = GYVector3 { 0.0f, 1.0f, 1.0f };
        GYVector3   _offset     = GYVector3 { 0.0f, 0.0f, 0.0f };
        GYVector3	_pos        = GYVector3 { 0.0f, 0.0f, -ZCoordConstants::mapBase };
        float		_scale      = 1.0f;
        float		_zOrient    = 0.0f;
        float		_xOrient    = 0.0f;
	public:
		void setPos(const float x, const float y, const float z);
        void setOffset(const GYVector3& vec);
		void setScale(const float scale);
		void setZOrient(const float zOrient);
		void setXOrient(const float xOrient);
		void setDir(const float x, const float y, const float z);

		float getScale(void) const;

		void linearInterpolate(const CameraKeyFrame& startKeyFrame, const CameraKeyFrame& endKeyFrame,
							   const float interval, const float elapsed);
	};

    class CameraShakeSubProcess:
            public MonitorableSubProcess<CameraShakeSubProcess, CameraSystem>,
            public EventMonitor
    {
        friend class CameraSystem;
    private:
        enum MOVE_STATE { MOVE_POS, MOVE_NEG, MOVE_ORIG, MOVE_MAX };

        static CameraSystem*    _cameraSys;

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
        void            _start(CameraSystem*const sys);
        void            _stop(CameraSystem*const sys);
        void            _update(CameraSystem*const sys, const float deltaTime);

        //Overridden from EventMonitor
        virtual void	onStart(void)   override;
        virtual void	onFinish(void)  override;

    };

	typedef LinearInterpolatorSubProcess<CameraKeyFrame, CameraSystem> CameraKeyFrameLinearInterpolator;

	struct CameraState: public GYCamera {

		Entity* focalEntity	= nullptr;
		std::vector<Entity*> entities;

		CameraKeyFrame		_keyFrame;
		//GYVector3 dir		= { 0.0f, 1.0f, 1.0f };
		GYVector3 up		= { 0.0f, 0.0f, 1.0f };
		float staticScale	= 1.0f;

		//dynamic camera boundaries
		float maxScale		= 5.0f;
		float minScale		= 0.5f;
		float baseScale		= 1.5f;

		//Visible Tile Region
		unsigned tileStartX	= 0;
		unsigned tileEndX	= 0;
		unsigned tileStartY	= 0;
		unsigned tileEndY	= 0;

		//Bounding Geometry
		int scrStartX	= 0;
		int scrStartY	= 0;
		int scrEndX		= 0;
		int scrEndY		= 0;

		void updateBoundingRegions(const int startX, const int endX, const int startY, const int endY);
		void updateBoundingSubregions(const CameraState& cam);

		bool isWithinScreenRegion(const int x, const int y, const int width, const int height) const;

		void update(const float deltaTime);
	};

	class CameraSystem:
			public SubProcessManager<CameraSystem,
                                    CameraKeyFrameLinearInterpolator,
                                    CameraShakeSubProcess>,
			public System
	{
		friend CameraKeyFrameLinearInterpolator;

	private:
		CameraState				_internalStates[ELYSIAN_CAMERA_SYSTEM_STACK_SIZE];
		CameraState*			_states[ELYSIAN_CAMERA_SYSTEM_STACK_SIZE];
		CameraState				_debugState;
		CameraState				_uiCamera;
		CameraState*			_curState		= &_internalStates[0];
		int						_depth			= 0; //depth is technically +1... my code, fuck you.
		bool					_projViewActive = false;
		bool					_debugActive	= false;
		bool					_customState	= false;

		unsigned				_scrWidth		= 0;
		unsigned				_scrHeight		= 0;

		int						_topBoundary	=	-1;
		int						_bottomBoundary =	-1;
		int						_leftBoundary	=	-1;
		int						_rightBoundary	=	-1;

		bool					_dynamic		= true;

		void _updateState(CameraState*const state, const float deltaTime);

	public:
        enum SUBPROC_ID { SUBPROC_KEY_FR_INTERP, SUBPROC_CAM_SHAKE };

		CameraSystem(void);

		void pushCustomCameraState(CameraState*const state) {
			++_depth;
			assert(_depth < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE);
			_states[_depth] = state;
			_curState = state;
			_customState = true;
		}

		void pushPerspectiveState(void) {
			_debugActive = true;
			pushCustomCameraState(&_debugState);
		}

		void setCurrentCameraState(CameraState*const state) {
			_states[_depth] = state;
			_curState = state;
		}

		void processInput(const GameInput& input);

		CameraKeyFrame getKeyFrame(void) const {
			return _curState->_keyFrame;
		}

		void setFollowEntity(Entity*const entity) {
			_curState->focalEntity = entity;
		}

		void setBaseScale(const float scale) {
			_curState->baseScale = scale;
		}

		Entity* getFollowEntity(void) const {
			return _curState->focalEntity;
		}

		void addEntity(Entity*const entity) const {
			_curState->entities.push_back(entity);
		}

		void removeEntity(Entity*const entity) const {
			for(int i = 0; i < (int)_curState->entities.size(); ++i)
				if(_curState->entities[i] == entity) {
					_curState->entities.erase(_curState->entities.begin()+i);
					return;
				}
        }

		void setPosition(const GYVector3 &pos) {
			_curState->_keyFrame._pos = pos;
		}

		void translate(const GYVector3 &dist) {
            gyVec3Add(&_curState->_keyFrame._pos, &_curState->_keyFrame._pos, &dist);
		}

        void translateOffset(const GYVector3& dist) {
            gyVec3AddAccum(&_curState->_keyFrame._offset, &dist);
        }

		void scale(const float factor, const float time=0.0f, EventMonitor*const monitor=nullptr) {
            auto &interp = get<SUBPROC_KEY_FR_INTERP>(static_cast<SubProcParent& >(*this));
			//if(interp.getProcState() == PROC_STATE_RUNNING)

			CameraKeyFrame frame = getKeyFrame();
			frame.setScale(frame.getScale()*factor);
			interp.setKeyFrames(&_curState->_keyFrame, frame);
			interp.setTimeInterval(time);
			interp.setEventMonitor(monitor);
			interp.start(this);
		}

		void moveToKeyFrame(const CameraKeyFrame& frame, const float time=0.0f, EventMonitor*const monitor=nullptr);

        void shake(const GYVector3& displacement, unsigned int oscillations, const float time, const bool fade=false, EventMonitor*const monitor=nullptr);

		void staticScale(const float factor) {
			_curState->staticScale *= factor;
		}

		void setStaticScale(const float factor) {
			_curState->staticScale = factor;
		}

		void rotateZ(const float delta) {
			_curState->_keyFrame._zOrient += delta;
		}

		void rotateX(const float delta) {
			_curState->_keyFrame._xOrient += delta;
		}

		void setOrientX(const float angle) {
			_curState->_keyFrame._xOrient = angle;
		}

		void setOrientZ(const float angle) {
			_curState->_keyFrame._zOrient = angle;
		}

		void setScale(const float factor, const float time=0.0f, EventMonitor*const monitor=nullptr) {
            auto &interp = get<SUBPROC_KEY_FR_INTERP > ( static_cast<SubProcParent&>(*this));

			if(interp.getProcState() == PROC_STATE_RUNNING) return;

			CameraKeyFrame frame = getKeyFrame();
			frame.setScale(factor);
			interp.setKeyFrames(&_curState->_keyFrame, frame);
			interp.setTimeInterval(time);
			interp.setEventMonitor(monitor);
			interp.start(this);
		}

		//Perspective functions
		void setPerspectiveFrustum(const float fov, const float aspect, const float znear, const float zfar);

		void setDirection(const GYVector3 &dir) {
			_curState->_keyFrame._dir = dir;
		}

		void setUpDirection(const GYVector3 &dir) {
			_curState->up = dir;
		}

		void pushViewProjMatrix(void);

		void popViewProjMatrix(void);

		void pushState(void) {
			++_depth;
			assert(_depth < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE);
			_states[_depth] = &_internalStates[_depth];
			_curState = _states[_depth];
		}

		void popState(void);

		CameraState* getCurrentState(void) const { return _curState; }

		void getVisibleTileRegion(unsigned& startX, unsigned& startY, unsigned& endX, unsigned& endY) const {
            startX = _curState->tileStartX; endX = _curState->tileEndX;
            startY = _curState->tileStartY; endY = _curState->tileEndY;
			//startX = 0; startY = 0;
			//endX = 49; endY = 49;
            //startX = 0;
            //startY = 0;
            //endX = getCurArea()->width-1;
            //endY = getCurArea()->height-1;
		}

		void pushViewMatrixToStack(void);
		void popViewMatrixFromStack(void);

		//Transforms a WORLD COORDINATE vector into SCREEN COORDINATES
		void transform(GYVector3* dest, const GYVector3* src);
		void transform(GYVector4* dest, const GYVector4* src);

		void update(void);

		bool isOnscreen(const Entity &entity);

		void isEntityWithinViewVolume(const Entity& entity) const;
		void isTileWithinViewVolume(const unsigned l, const unsigned x, const unsigned y) const;

		void setBoundaries(const float top, const float bottom, const float left, const float right) {
			_topBoundary = top; _bottomBoundary = bottom; _leftBoundary = left; _rightBoundary = right;
		}

		void setDynamic(const bool value) { _dynamic = value; }


		/* LEGACY
		//const static gyro::Vector2 screenSize;
		//void RenderSprite(gyro::Sprite& sprite) const;
		//void RenderRect(gyro::Rect& rect) const;
		//void Transform(gyro::Matrix3x3& entity) const;
		//gyro::Vector2 Transform(const gyro::Vector2& vec);
		*/
	};






	// ======== INLINEZ =======

	inline float CameraKeyFrame::getScale(void) const { return _scale; }
	inline void CameraKeyFrame::setScale(const float sc) { _scale = sc; }
	inline void CameraKeyFrame::setPos(const float x, const float y, const float z) {
		_pos.x = x; _pos.y = y; _pos.z = z;
	}
	inline void CameraKeyFrame::setZOrient(const float zOrient) { _zOrient = zOrient; }
	inline void CameraKeyFrame::setXOrient(const float xOrient) { _xOrient = xOrient; }
	inline void CameraKeyFrame::setDir(const float x, const float y, const float z) {
		_dir.x = x; _dir.y = y; _dir.z = z;
	}
    inline void CameraKeyFrame::setOffset(const GYVector3 &vec) {
        _offset = vec;
    }

	inline void CameraKeyFrame::linearInterpolate(const CameraKeyFrame& startKeyFrame, const CameraKeyFrame& endKeyFrame,
										const float interval, const float elapsed) {

		/* Do some fucking terrible, type-unsafe casting to batch process linear interpolation
		 * of all floats in the struct with the SIMD instructions. Ugly, but fast as shit. */
		gyLinearInterp((float*)&_pos.x, 6,
					   (float*)&startKeyFrame._pos.x, (float*)&endKeyFrame._pos.x,
					   interval, elapsed);

		gyLinearInterp((float*)&_dir.x, 3,
					   (float*)&startKeyFrame._dir.x, (float*)&endKeyFrame._dir.x,
					   interval, elapsed);

        gyLinearInterp((float*)&_offset.x, 3,
                       (float*)&startKeyFrame._offset.x, (float*)&endKeyFrame._offset.x,
                       interval, elapsed);

		gyVec3Normalize(&_dir);
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

	inline bool CameraState::isWithinScreenRegion(const int x, const int y, const int halfWidth, const int halfHeight) const {

		const int distX = abs((int)x-(int)_keyFrame._pos.x);
		const int distY = abs((int)y-(int)_keyFrame._pos.y);

		const int maxDistX = halfWidth + (scrEndX-scrStartX)*0.5f;
		const int maxDistY = halfHeight + (scrEndY-scrStartY)*0.5f;

		return(distY < maxDistY && distX < maxDistX);

	}

	inline void CameraSystem::moveToKeyFrame(const CameraKeyFrame &frame, const float time, EventMonitor * const monitor) {
        auto &interp = get<SUBPROC_KEY_FR_INTERP>(static_cast<SubProcParent&>(*this));
		interp.setKeyFrames(&_curState->_keyFrame, frame);
		interp.setTimeInterval(time);
        interp.setEventMonitor(monitor);
		interp.start(this);
	}

    inline void CameraSystem::shake(const GYVector3& displacement, unsigned int oscillations, const float time, const bool fade, EventMonitor*const monitor) {
       auto &camShake = get<SUBPROC_CAM_SHAKE>(static_cast<SubProcParent& >(*this));
       camShake.setDisplacement(displacement);
       camShake.setOscillations(oscillations);
       camShake.setDuration(time);
       camShake.setFade(fade);
       camShake.setEventMonitor(monitor);
       camShake.start(this);
    }

    //===== CameraShakeSubProcess Inlinez =====

    inline void CameraShakeSubProcess::setDisplacement(const GYVector3& disp) { _displacement = disp; }
    inline void CameraShakeSubProcess::setDuration(const float dur) { _duration = dur; }
    inline void CameraShakeSubProcess::setOscillations(const unsigned osc) { _oscillations = osc; }
    inline void CameraShakeSubProcess::setFade(const bool fade) { _fade = fade; }
    inline void CameraShakeSubProcess::_start(CameraSystem*const sys) {
        _timePerOscillation = (float)(_duration / ((float)_oscillations));
        _state = MOVE_POS;
        _elapsedTime = 0.0f;
        _magnitude = gyVec3MagAndNormalize(&_displacement);
        _startNextKeyFrame();
    }
    inline void CameraShakeSubProcess::_stop(CameraSystem*const sys) {}
    inline void CameraShakeSubProcess::_update(CameraSystem*const sys, const float deltaTime) {
        _elapsedTime += deltaTime;
        if(_elapsedTime >= _duration) _elapsedTime = _duration;
    }




	
}

#endif
