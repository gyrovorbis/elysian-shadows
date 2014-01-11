#include "elysian_camera_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_area.hpp"

namespace elysian {

CameraSystem::CameraSystem(void)  {
	transform._pos.x = transform._pos.y = 200.0f;
	//update();
}

void CameraSystem::PushTransform(void) {
	GYVector4 pos = { transform._pos.x, transform._pos.y, transform.getPosZ(), 1.0f};
/*
	gyMatPush();
	gyMatLoadIdentity();
	gyMatTranslate(_scrWidth/2.0f, _scrHeight/2.0f, 0.0f);
	gyMatScale(transform._scale.x, transform._scale.y, 1.0f);
	gyMatRotateZ(transform._orientZ);
	gyMatTranslate(-transform._pos.x, -transform._pos.y, 0.0f);
	gyMatStore(&mat);
	gyMatPop();

	gyVidCameraSetTransform(mat);
	gyVidCameraSetPosition(&pos);

*/

	gyVidCameraSetViewCenter(&pos, transform._scale.x, transform._orientZ);
}

void CameraSystem::PopTransform() {


}

void CameraSystem::update(void) {
	//Update camera resolution
	getGuiSys()->getResolution(_scrWidth, _scrHeight);

	//Follow entities
	if(_entity) {
		transform.setPos(_entity->transform._pos);
		transform.setPosZ(_entity->transform.getPosZ());
	}

	//Update tile rendering region
	int startX = (transform._pos.x - (0.5f*_scrWidth)/transform._scale.x)/32;
	int startY = (transform._pos.y - (0.5f*_scrHeight)/transform._scale.y)/32;
	int endX = (transform._pos.x + (0.5f*_scrWidth)/transform._scale.x)/32+1;
	int endY = (transform._pos.y + (0.5f*_scrHeight)/transform._scale.y)/32+1;
	if(transform._orientZ != 0.0f) { //insert REAL fix here! Resolution-dependent!
		startY -= 4;
		endY += 4;
	}

	//Convert to unsigned
	_tileStartX = (startX < 0)? 0 : (unsigned)startX;
	_tileStartY = (startY < 0)? 0 : (unsigned)startY;
	_tileEndX = (endX >= _curArea->width)? _curArea->width-1 : (unsigned)endX;
	_tileEndY = (endY >= _curArea->height)? _curArea->height-1 : (unsigned)endY;

	_scrStartX = _tileStartX*32;
	_scrStartY = _tileStartY*32;
	_scrEndX = _tileEndX*32;
	_scrEndY = _tileEndY*32;

}

//VERY INEFFICIENT. BATCH THIS BITCH!
void CameraSystem::Transform(GYVector2* dest, const GYVector2* src) {
	GYVector4 in = { src->x, src->y, 1.0f, 1.0f };
	GYVector4 out = { 0.0f, 0.0f, 0.0f, 0.0f };

	PushTransform();
	gyMatMultVec(&in, &out, 1);
	PopTransform();

	dest->x = out.x; dest->y = out.y;

}

void CameraSystem::Transform(GYVector4* dest, const GYVector4* src) {
	PushTransform();
	gyMatMultVec(src, dest, 1);
	PopTransform();
}

bool CameraSystem::isOnscreen(const Entity &entity) {
	
	if(entity.transform._orientZ == 0.0f) { //Entities without rotation
		return ((entity.transform._pos.x + 0.5f*entity.transform._size.x >= _scrStartX &&
				entity.transform._pos.x - 0.5f*entity.transform._size.x <= _scrEndX) &&
				(entity.transform._pos.y + 0.5f*entity.transform._size.y >= _scrStartY &&
				entity.transform._pos.y - 0.5f*entity.transform._size.y <= _scrEndY));
	} else { //Entities with rotation
		//Approximate region by bounding as a square with largest dimension
		float bigSide = (entity.transform._size.x > entity.transform._size.y)? 
			0.5f*entity.transform._size.x : 0.5f*entity.transform._size.y;

		return ((entity.transform._pos.x + bigSide >= _scrStartX &&
				entity.transform._pos.x - bigSide <= _scrEndX) &&
				(entity.transform._pos.y + bigSide >= _scrStartY &&
				entity.transform._pos.y - bigSide <= _scrEndY));
	}
}

}
