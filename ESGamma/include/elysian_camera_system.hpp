#ifndef ELYSIAN_CAMERA_SYSTEM_HPP
#define ELYSIAN_CAMERA_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_instance_transform.hpp"

#include <gyro.h>

namespace elysian {
	
	class Entity;

	class CameraSystem: public System { 
	private:
		Entity* _entity			= nullptr; //needs to be an array, eventually (battle engine)

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
		InstanceTransform transform;

		void setFollowEntity(Entity*const entity) { _entity = entity; }
		Entity* getFollowEntity(void) const { return _entity; }

		void PushTransform(void);
		void PopTransform(void);

		void getVisibleTileRegion(unsigned& startX, unsigned& startY, unsigned& endX, unsigned& endY) const {
			startX = _tileStartX; endX = _tileEndX;
			startY = _tileStartY; endY = _tileEndY;
		}

		//Transforms a WORLD COORDINATE vector into SCREEN COORDINATES
		void Transform(GYVector2* dest, const GYVector2* src);
		void Transform(GYVector4* dest, const GYVector4* src);

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
