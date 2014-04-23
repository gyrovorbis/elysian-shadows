#ifndef ELYSIAN_CAMERA_SYSTEM_HPP
#define ELYSIAN_CAMERA_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_instance_transform.hpp"
#include "elysian_subprocess_manager.hpp"

#include <vector>
#include <gyro.h>

#define ELYSIAN_CAMERA_SYSTEM_STACK_SIZE			5
#define ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE		0.1f
#define ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON	0.01f

namespace elysian {
	
	class Entity;
	class GameInput;
	class EventMonitor;


	class CameraSystem;
	class CameraKeyFrameLinearInterpolator;

	class CameraKeyFrame {
		friend class CameraSystem;
		friend class CameraState;
		friend class CameraKeyFrameLinearInterpolator;
	private:
		GYVector3	_pos = GYVector3 { 0.0f, 0.0f, 0.0f };
		float		_scale = 1.0f;
		float		_zOrient = 0.0f;
		float		_xOrient = 0.0f;
	public:
		void setPos(const float x, const float y, const float z);
		void setScale(const float scale);
		void setZOrient(const float zOrient);
		void setXOrient(const float xOrient);
		float getScale(void) const;
	};


	struct CameraKeyFrameLinearInterpolator:
			public MonitorableSubProcess<CameraKeyFrameLinearInterpolator, CameraSystem> {
		friend class CameraSystem;
	private:
		CameraKeyFrame	_startFrame;
		CameraKeyFrame	_endFrame;
		CameraKeyFrame	_curFrame;
		float			_timeInterval;
		float			_timeElapsed;
	public:

		void start(CameraSystem * const m);
		void update(CameraSystem * const m);
		void stop(CameraSystem * const m);

		void setKeyFrames(const CameraKeyFrame& start, const CameraKeyFrame& end);
		void setTimeInterval(const float time);
	};


	struct CameraState: public GYCamera {

		Entity* focalEntity	= nullptr;
		std::vector<Entity*> entities;

		CameraKeyFrame	_keyFrame;
		GYVector3 dir		= { 0.0f, 1.0f, 1.0f };
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
		unsigned scrStartX	= 0;
		unsigned scrStartY	= 0;
		unsigned scrEndX	= 0;
		unsigned scrEndY	= 0;

		void updateBoundingRegions(const int startX, const int endX, const int startY, const int endY);
		void updateBoundingSubregions(const CameraState& cam);

		bool isWithinScreenRegion(const int x, const int y, const int width, const int height) const;

		void update(const float deltaTime);
	};

	class CameraSystem:
			public SubProcessManager<CameraSystem,
									CameraKeyFrameLinearInterpolator>,
			public System
	{
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

		void _updateState(CameraState*const state, const float deltaTime);

	public:
		enum SUBPROC_ID { SUBPROC_KEY_FR_INTERP };

		CameraSystem(void);

		void pushCustomCameraState(CameraState*const state) {
			++_depth;
			assert(_depth < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE);
			_states[_depth] = state;
			_curState = state;
			_customState = true;
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

		void scale(const float factor, const float time) {
			CameraKeyFrame frame = getKeyFrame();
			frame.setScale(frame.getScale()*factor);
			auto &interp = get<SUBPROC_KEY_FR_INTERP>(*this);
			interp.setKeyFrames(_curState->_keyFrame, frame);
			interp.setTimeInterval(time);
			interp.start(this);
		}

		void moveToKeyFrame(const CameraKeyFrame& frame, const float time, EventMonitor*const monitor=nullptr);

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

		void setScale(const float factor, const float time) {
			CameraKeyFrame frame = getKeyFrame();
			frame.setScale(factor);
			auto &interp = get<SUBPROC_KEY_FR_INTERP>(*this);
			interp.setKeyFrames(_curState->_keyFrame, frame);
			interp.setTimeInterval(time);
			interp.start(this);
		}

		//Perspective functions
		void setPerspectiveFrustum(const float fov, const float aspect, const float znear, const float zfar);

		void setDirection(const GYVector3 &dir) {
			_curState->dir = dir;
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

	inline void CameraKeyFrameLinearInterpolator::setKeyFrames(const CameraKeyFrame& start, const CameraKeyFrame& end) {
		_startFrame = start; _endFrame = end;
	}

	inline void CameraKeyFrameLinearInterpolator::setTimeInterval(const float time) {
		_timeInterval = time;
	}

	inline void CameraKeyFrameLinearInterpolator::start(CameraSystem * const m) {
		_timeElapsed = 0;
		_curFrame = _startFrame;
	}

	inline bool CameraState::isWithinScreenRegion(const int x, const int y, const int halfWidth, const int halfHeight) const {

		const int distX = abs((int)x-(int)_keyFrame._pos.x);
		const int distY = abs((int)y-(int)_keyFrame._pos.y);

		const int maxDistX = halfWidth + (scrEndX-scrStartX)*0.5f;
		const int maxDistY = halfHeight + (scrEndY-scrStartY)*0.5f;

		return(distY < maxDistY && distX < maxDistX);

	}

	inline void CameraSystem::moveToKeyFrame(const CameraKeyFrame &frame, const float time, EventMonitor * const monitor) {
		auto &interp = get<SUBPROC_KEY_FR_INTERP>(*this);
		interp.setKeyFrames(_curState->_keyFrame, frame);
		interp.setTimeInterval(time);
		interp.start(this);
		interp.setEventMonitor(monitor);
	}



	
}

#endif
