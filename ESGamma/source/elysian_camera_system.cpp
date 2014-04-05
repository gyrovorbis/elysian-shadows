#include "elysian_camera_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_area.hpp"
#include "elysian_game_input.hpp"

namespace elysian {

void CameraState::updateBoundingRegions(const int xMin, const int xMax, const int yMin, const int yMax) {
	int startX	= xMax;
	int startY	= yMax;
	int endX	= xMin;
	int endY	= yMin;

	auto checkPointBounds = [&](const GYVector3& point) {
		if(point.x < (float)startX) startX = (int)point.x;
		else if(point.x > (float)endX) endX = (int)point.x;
		if(point.y < (float)startY) startY = (int)point.y;
		else if(point.y > (float)endY) endY = (int) point.y;
	};

	//Find X/Y extremes given each vertex to create bounding box
	checkPointBounds(viewVolume.ftr);
	checkPointBounds(viewVolume.ftl);
	checkPointBounds(viewVolume.fbr);
	checkPointBounds(viewVolume.fbl);
	checkPointBounds(viewVolume.ntr);
	checkPointBounds(viewVolume.ntl);
	checkPointBounds(viewVolume.nbr);
	checkPointBounds(viewVolume.nbl);

	if(startX < 0) startX = 0;
	if(startY < 0) startY = 0;
	if(endX < 0) endX = 0;
	if(endY < 0) endY = 0;

	//Convert from world to tile coordinates (and signed to unsigned)
	scrStartX = (startX < xMin)? xMin : (unsigned)startX;
	scrStartY = (startY < yMin)? yMin : (unsigned)startY;
	scrEndX = (endX > xMax)? xMax : (unsigned)endX;
	scrEndY = (endY > yMax)? yMax : (unsigned)endY;


	//tileStartX = 0;
	//tileStartY = 0;
	//tileEndX = 49;
	//tileEndY = 49;

	tileStartX	= scrStartX/32;
	tileEndX	= scrEndX/32+1;
	tileStartY	= scrStartY/32;
	tileEndY	= scrEndY/32+1;
}

void CameraState::updateBoundingSubregions(const CameraState &cam) {
	/**** FUCKING FIX ME!!!!! ****/

//	memcpy(&viewVolume, &cam.viewVolume, sizeof(GYViewVolume));


	/***** END REEST ******/

	updateBoundingRegions(cam.scrStartX, cam.scrEndX, cam.scrStartY, cam.scrEndY);


}


CameraSystem::CameraSystem(void) {
	static const GYMatrix4 identity4 = {
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	unsigned w, h;

	//Initialize default camera
	gyVidResolution(&w, &h);
	gyVidCamOrthoProj(_curState, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 1000.0f);
	gyVidCamOrthoLookAt(_curState, &_curState->pos, 1.0f, 0.0f, 0.0f);

	memcpy(_uiCamera.viewMat, identity4, sizeof(GYMatrix4));
	gyVidCamOrthoProj(&_uiCamera, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 1000.0f);

	gyVidCamFrustum(&_debugState, 90.0f, (float)w/(float)h, 0.5f, 900.0f);
	_debugState.dir = { 0.0f, 1.0f, 1.0f };

	_states[0] = &_internalStates[0];
}

void CameraSystem::pushViewProjMatrix(void) {

	if(_curState->projType == GY_PROJECTION_ORTHO) {
		//gyVidProjectionDefaultMatrix();
		//gyVidCameraSetViewCenter(&_curState->pos, _curState->scale, _curState->zOrient);
		gyVidCamOrthoLookAt(_curState, &_curState->pos, _curState->scale, _curState->zOrient, 0.0f);
		gyVidSetCamWorldPos(&_curState->pos);

	} else { //PERSPECTIVE PROJECTION

		if(_curState == &_debugState) {
			GYVector3 pos = {
				-_curState->entity->transform._pos.x,
				-_curState->entity->transform._pos.y+30.0f,
				-(_curState->entity->getElev()+1)*32.0f-ZCoordConstants::mapBase
			};

			gyVidCamFrustumLookAt(_curState, &pos, &_curState->dir, &_curState->up);
			gyVidSetCamWorldPos(&pos);
		}

#if 0
		GYMatrix4 camMat;
		gyMatPush();
		gyMatLoadIdentity();

		//Create View Matrix
		gyVidLookAtMat(camMat, 0.0f, 0.0f, 0.0f,
					   _curState->dir.x, _curState->dir.y, _curState->dir.z,
					   _curState->up.x, _curState->up.y, _curState->up.z);
		gyMatApply(camMat);

		gyMatScale(1.0f, -1.0f, -1.0f);
		gyMatTranslate(-_curState->pos.x, -_curState->pos.y, -_curState->pos.z);
		gyMatStore(&camMat);
		gyMatPop();
#else
/*
		GYMatrix4 camMat;
		gyMatPush();
		gyMatLoadIdentity();

		//Create View Matrix
		gyVidLookAtMat(camMat, 0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 1.0f,
					   _curState->up.x, _curState->up.y, _curState->up.z);
		gyMatApply(camMat);

		//PRETTY SURE THIS SCALAR BULLSHIT IS WRONG ON THE DREAMCAST!!!
		gyMatScale(1.0f, 1.0f, -1.0f);
		gyMatTranslate(-_curState->entity->transform._pos.x, -_curState->entity->transform._pos.y+30.0f, -(_curState->entity->getElev()+1)*32.0f-ZCoordConstants::mapBase);
		gyMatStore(&camMat);
		gyMatPop();*/


#endif





	}

	gyVidCamUpdate(_curState);


	_projViewActive = true;
}

void CameraSystem::popViewProjMatrix(void) {
	//Set View matrix to identity Matrix
	//Set proj matrix to ortho perspective

	//gyVidProjectionDefaultMatrix();
	//gyVidCameraCustomViewMatrix(_identityMat);

	gyVidCamUpdate(&_uiCamera);

	_projViewActive = false;
}


void CameraSystem::setPerspectiveFrustum(const float /*fov*/, const float /*aspect*/, const float /*znear*/, const float /*zfar*/) {
	//_curState->defaultProjMat = false;

	//gyVidViewFrustumMat(_curState->projMat, fov, aspect, znear, zfar);
	//gyVidProjectionCustomMatrix(_curState->projMat);
}

void CameraSystem::popState(void) {
	--_depth;
	assert(_depth >= 0);
	_curState = _states[_depth];

	//restore projection matrix if it's supposed to be on the stack is this even really necessary?
	/*if(_projViewActive) {
		if(_curState->defaultProjMat) gyVidProjectionDefaultMatrix();
		else gyVidProjectionCustomMatrix(_curState->projMat);
	}*/
}

void CameraSystem::processInput(const GameInput& input) {
	if(input.getTapped(GameInput::BUT_P)) {
		_debugActive = !_debugActive;

		if(_debugActive) { //enable
			_debugState.entity = _curState->entity;

			//_curState = &_debugState;
			//setPerspectiveFrustum(90.0f, 1.0f, 0.5f, 900.0f);
		//	setEngineState(ENGINE_CAMERA_DEBUG);
			pushCustomCameraState(&_debugState);

		} else { //disable
			if(_curState == &_debugState) //Only pop if debug is current camera
				popState();
		//	setEngineState(ENGINE_NORMAL);
		}
	}
#if 0
	if(_debugActive) {

		if(input.getDown(GameInput::BUT_DPAD_U)) {
			GYVector3 delta = { 0.0f, 0.0f, -0.01f};
			gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			gyVec3Normalize(&_curState->dir);
		}
		if(input.getDown(GameInput::BUT_DPAD_D)) {
			GYVector3 delta = { 0.0f, 0.0f, 0.01f };
			gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			gyVec3Normalize(&_curState->dir);
		}
		if(input.getDown(GameInput::BUT_DPAD_L)) {
			GYVector3 delta = { -0.01f, 0.0f, 0.0f };
			gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			gyVec3Normalize(&_curState->dir);
		}
		if(input.getDown(GameInput::BUT_DPAD_R)) {
			GYVector3 delta = { 0.01f, 0.0f, 0.0f };
			gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			gyVec3Normalize(&_curState->dir);
		}

		if(input.getDown(GameInput::BUT_W)) {
			GYVector3 delta = _curState->dir;
			gyVec3Scale(&delta, -3.0f);
			translate(delta);

		}
		if(input.getDown(GameInput::BUT_A)) {

			translate(GYVector3{ -3.0f, 0.0f, 0.0f});
		}
		if(input.getDown(GameInput::BUT_S)) {

			GYVector3 delta = _curState->dir;
			gyVec3Scale(&delta, 3.0f);
			translate(delta);

		}
		if(input.getDown(GameInput::BUT_D)) {

			translate(GYVector3{ 3.0f, 0.0f, 0.0f});
		}
	}
#endif

}

void CameraSystem::update(void) {
	//Update camera resolution
	getGuiSys()->getResolution(_scrWidth, _scrHeight);

	//Follow entities
	if(_curState->entity) {
		_curState->pos = GYVector3 {
			_curState->entity->transform._pos.x,
			_curState->entity->transform._pos.y,
			//_curState->entity->transform.getPosZ(),
			0.0f
		};
	}

	_curState->updateBoundingRegions(0, getCurArea()->getMapWidth()*32, 0, getCurArea()->getMapHeight()*32);

/*
	//Update tile rendering region
	int startX = (_curState->pos.x - (0.5f*_scrWidth)/_curState->scale)/32;
	int startY = (_curState->pos.y - (0.5f*_scrHeight)/_curState->scale)/32;
	int endX = (_curState->pos.x + (0.5f*_scrWidth)/_curState->scale)/32+1;
	int endY = (_curState->pos.y + (0.5f*_scrHeight)/_curState->scale)/32+1;
	if(_curState->zOrient != 0.0f) { //insert REAL fix here! Resolution-dependent!
		startY -= 4;
		endY += 4;
	}

	//Convert to unsigned
	_tileStartX = (startX < 0)? 0 : (unsigned)startX;
	_tileStartY = (startY < 0)? 0 : (unsigned)startY;
	_tileEndX = (endX >= _curArea->width)? _curArea->width-1 : (unsigned)endX;
	_tileEndY = (endY >= _curArea->height)? _curArea->height-1 : (unsigned)endY;

	//////FUCKING FIX ME!!!!!!!!!
	_tileStartX = 0;
	_tileStartY = 0;
	_tileEndX = _curArea->width-1;
	_tileEndY = _curArea->height-1;

	_scrStartX = _tileStartX*32;
	_scrStartY = _tileStartY*32;
	_scrEndX = _tileEndX*32;
	_scrEndY = _tileEndY*32;

	*/

}

//VERY INEFFICIENT. BATCH THIS BITCH! THESE DON'T WORK ANYMORE!!!!
void CameraSystem::transform(GYVector3* dest, const GYVector3* src) {
	GYVector4 in = { src->x, src->y, src->z, 1.0f };
	GYVector4 out;

	gyMatPush();
	gyMatLoadIdentity();
	gyMatLoad(&_curState->viewMat);
	gyMatMultVec(&in, &out, 1);
	gyMatPop();

	dest->x = out.x; dest->y = out.y; dest->z = out.z;
}

void CameraSystem::transform(GYVector4* dest, const GYVector4* src) {
	//PushTransform();
	gyMatMultVec(src, dest, 1);
	//PopTransform();
}

bool CameraSystem::isOnscreen(const Entity &entity) {
	
	if(entity.transform._orientZ == 0.0f) { //Entities without rotation
		return ((entity.transform._pos.x + 0.5f*entity.transform._size.x >= _curState->scrStartX &&
				entity.transform._pos.x - 0.5f*entity.transform._size.x <= _curState->scrEndX) &&
				(entity.transform._pos.y + 0.5f*entity.transform._size.y >= _curState->scrStartY &&
				entity.transform._pos.y - 0.5f*entity.transform._size.y <= _curState->scrEndY));
	} else { //Entities with rotation
		//Approximate region by bounding as a square with largest dimension
		float bigSide = (entity.transform._size.x > entity.transform._size.y)? 
			0.5f*entity.transform._size.x : 0.5f*entity.transform._size.y;

		return ((entity.transform._pos.x + bigSide >= _curState->scrStartX &&
				entity.transform._pos.x - bigSide <= _curState->scrEndX) &&
				(entity.transform._pos.y + bigSide >= _curState->scrStartY &&
				entity.transform._pos.y - bigSide <= _curState->scrEndY));
	}
}




void CameraSystem::isEntityWithinViewVolume(const Entity& /*entity*/) const {

}

void CameraSystem::isTileWithinViewVolume(const unsigned /*l*/, const unsigned /*x*/, const unsigned /*y*/) const {

}




}
