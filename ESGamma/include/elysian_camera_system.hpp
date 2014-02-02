#ifndef ELYSIAN_CAMERA_SYSTEM_HPP
#define ELYSIAN_CAMERA_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_instance_transform.hpp"

#include <gyro.h>

#define ELYSIAN_CAMERA_SYSTEM_STACK_SIZE	3

namespace elysian {
	
	class Entity;
	class GameInput;

	struct CameraState {
		Entity* entity		= nullptr;
		GYVector3 pos		= { 0.0f, 0.0f, 0.0f };
		GYVector3 dir		= { 0.0f, -1.0f, 0.0f };
		GYVector3 up		= { 0.0f, 0.0f, 1.0f };
		float scale			= 1.0f;
		float zOrient		= 0.0f;

		bool defaultProjMat = true;
		/*float fov			= 90.0f;
		float aspect		= 1.0f;
		float znear			= 0.5f;
		float zfar			= 300.0f;*/
		GYMatrix4 projMat;

		float viewWidth		= 640.0f;
		float viewHeight	= 480.0f;
	};

	class CameraSystem: public System { 
	private:
		CameraState				_states[ELYSIAN_CAMERA_SYSTEM_STACK_SIZE];
		CameraState				_debugState;
		CameraState*			_curState		= &_states[0];
		int						_depth			= 0; //depth is technically +1... my code, fuck you.
		bool					_projViewActive = false;
		bool					_debugActive	= false;
		const static GYMatrix4	_identityMat;

		unsigned _scrWidth		= 0;
		unsigned _scrHeight		= 0;
		unsigned _tileStartX	= 0;
		unsigned _tileEndX		= 0;
		unsigned _tileStartY	= 0;
		unsigned _tileEndY		= 0;
		
		unsigned _scrStartX		= 0;
		unsigned _scrStartY		= 0;
		unsigned _scrEndX		= 0;
		unsigned _scrEndY		= 0;

	public:
		CameraSystem(void);

		void processInput(const GameInput& input);

		void setFollowEntity(Entity*const entity) {
			_curState->entity = entity;
		}

		Entity* getFollowEntity(void) const {
			return _curState->entity;
		}

		void setPosition(const GYVector3 &pos) {
			_curState->pos = pos;
		}

		void translate(const GYVector3 &dist) {
			gyVec3Add(&_curState->pos, &_curState->pos, &dist);
		}

		void scale(const float factor) {
			_curState->scale *= factor;
		}

		void rotate(const float delta) {
			_curState->zOrient += delta;
		}

		void setOrientZ(const float angle) {
			_curState->zOrient = angle;
		}

		void setScale(const float factor) {
			_curState->scale = factor;
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
			assert(++_depth < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE);
			_curState = &_states[_depth];
		}

		void popState(void);

		void getVisibleTileRegion(unsigned& startX, unsigned& startY, unsigned& endX, unsigned& endY) const {
			startX = _tileStartX; endX = _tileEndX;
			startY = _tileStartY; endY = _tileEndY;
		}

		void pushViewMatrixToStack(void);
		void popViewMatrixFromStack(void);

		//Transforms a WORLD COORDINATE vector into SCREEN COORDINATES
		void transform(GYVector2* dest, const GYVector2* src);
		void transform(GYVector4* dest, const GYVector4* src);

		void update(void);

		bool isOnscreen(const Entity &entity);



		/* LEGACY
		//const static gyro::Vector2 screenSize;
		//void RenderSprite(gyro::Sprite& sprite) const;
		//void RenderRect(gyro::Rect& rect) const;
		//void Transform(gyro::Matrix3x3& entity) const;
		//gyro::Vector2 Transform(const gyro::Vector2& vec);
		*/
	};
	
}

#endif
