#include "elysian_camera_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_area.hpp"
#include "elysian_game_input.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_debug.hpp"

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

	//Fuckin' weird, hacky, and reesty... but okay.
	tileStartX	= scrStartX/32;
	tileEndX	= scrEndX/32+1;
	while(tileEndX*32+16 > (unsigned)xMax) {
		--tileEndX;
		if(tileEndX == 0) break;
	}
	tileStartY	= scrStartY/32;
	tileEndY	= scrEndY/32+1;
	while(tileEndY*32+16 > (unsigned)yMax) {
		--tileEndY;
		if(tileEndY == 0) break;
	}
}

void CameraState::updateBoundingSubregions(const CameraState &cam) {
	/**** FUCKING FIX ME!!!!! ****/

//	memcpy(&viewVolume, &cam.viewVolume, sizeof(GYViewVolume));


	/***** END REEST ******/

	updateBoundingRegions(cam.scrStartX, cam.scrEndX, cam.scrStartY, cam.scrEndY);


}

void CameraSystem::_updateState(CameraState*const state, const float /*deltaTime*/) {
    auto &interp = get<SUBPROC_KEY_FR_INTERP>(static_cast<SubProcParent&>(*this));

	unsigned width, height;

	gyVidResolution(&width, &height);

	int boundTop = (_topBoundary == -1)? -16 : _topBoundary*32-16;
	int boundBottom = (_bottomBoundary == -1)? getCurArea()->height*32-16 : _bottomBoundary*32-16;
	int boundLeft = (_leftBoundary == -1)? -16 : _leftBoundary*32-16;
	int boundRight = (_rightBoundary == -1)? getCurArea()->width*32-16 : _rightBoundary*32-16;

	float maxHeight = boundBottom	- boundTop;
	float maxWidth	= boundRight	- boundLeft;

	boundTop += (float)height/_curState->_keyFrame._scale*0.5f;
	boundBottom -= (float)height/_curState->_keyFrame._scale*0.5f;
	boundLeft += (float)width/_curState->_keyFrame._scale*0.5f;
	boundRight -= (float)width/_curState->_keyFrame._scale*0.5f;

	if(state->_keyFrame._pos.y < boundTop) _curState->_keyFrame._pos.y = boundTop;
	if(state->_keyFrame._pos.y > boundBottom) _curState->_keyFrame._pos.y = boundBottom;
	if(state->_keyFrame._pos.x < boundLeft) _curState->_keyFrame._pos.x = boundLeft;
	if(state->_keyFrame._pos.x > boundRight) _curState->_keyFrame._pos.x = boundRight;

	if(!_dynamic) return;

	//return to base zoom
	if(state->entities.size() == 0) {
		if(fabs(state->baseScale - interp.getEndKeyFrame()._scale) > ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON) {
			setScale(state->baseScale, 4.0f);
		}
	}
	else {

		float baseWidth, baseHeight, minWidth, minHeight;
		baseWidth = minWidth = (float)width;
		baseHeight = minHeight = (float)height;

		baseWidth	-= 2.0f*baseWidth*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE;
		baseHeight	-= 2.0f*baseHeight*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE;

		maxWidth	-= 2.0f*maxWidth*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE;
		maxHeight	-= 2.0f*maxHeight*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE;

		minWidth	/= state->baseScale;
		minHeight	/= state->baseScale;
		minWidth	-= 2.0f*minWidth*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE;
		minHeight	-= 2.0f*minHeight*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE;

		const float maxXDist = 0.5f*maxWidth;
		const float maxYDist = 0.5f*maxHeight;

		float newXDist = 0.5f*minWidth;
		float newYDist = 0.5f*minHeight;

		auto fitEntity = [&](const Entity*const entity) {
			float xDist = fabs(entity->transform._pos.x-state->_keyFrame._pos.x);
			float yDist = fabs(entity->transform._pos.y-state->_keyFrame._pos.y);

			//we can fit this fucker onscreen
			if(xDist <= maxXDist && yDist <= maxYDist) {

				if(xDist > newXDist) newXDist = xDist;
				if(yDist > newYDist) newYDist = yDist;

			}
		};

		for(auto it : state->entities) fitEntity(it);

		//Debug::printf(Debug::VERBOSE, "<%f, %f>, <%f, %f>", newXDist, newYDist, maxXDist, maxYDist);

		//convert dists to widths by adding back margins

		//const float scaleX = (newXDist*2.0f)/((float)baseWidth-(2.0f*(float)baseWidth*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE));
		//const float scaleY = (newYDist*2.0f)/((float)baseHeight-(2.0f*(float)baseHeight*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE));
		const float scaleX = (baseWidth)/(newXDist*2.0f);
		const float scaleY = (baseHeight)/(newYDist*2.0f);

		const float nextScale = (scaleX < scaleY)? scaleX : scaleY;

		//float boundWidth = ;
		//float boundHeight = ;

		//Debug::printf(Debug::VERBOSE, "%f, %f - NEWSCALE - %f", scaleX, scaleY, nextScale);

		//Check if we need to bother updating
		if(fabs(nextScale - interp.getEndKeyFrame()._scale) > ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON) {
			Debug::printf(Debug::VERBOSE, "SCALE - %f", nextScale);
			Debug::printf(Debug::VERBOSE, "<%f, %f>", scaleX, scaleY);

			//curInterp.startScaleInterp(_keyFrame._scale, nextScale, 4.0f);
			setScale(nextScale, 4.0f);

		}

	}

	//_keyFrame._scale = curInterp.getNewScale();

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
	gyVidCamOrthoProj(_curState, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 0.0f);
	gyVidCamOrthoLookAt(_curState, &_curState->_keyFrame._pos, 1.0f, 0.0f, _curState->_keyFrame._xOrient);

	memcpy(_uiCamera.viewMat, identity4, sizeof(GYMatrix4));
	gyVidCamOrthoProj(&_uiCamera, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 1000.0f);

	gyVidCamFrustum(&_debugState, 45.0f, 1.0f, 0.5f, 900.0f);
	//_debugState.dir = { 0.0f, 1.0f, 1.0f };

	_states[0] = &_internalStates[0];
}

void CameraSystem::pushViewProjMatrix(void) {


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
	_customState = false;
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
			//_debugState.focalEntity = _curState->focalEntity;

			_debugState._keyFrame._pos = GYVector3 {
				_curState->_keyFrame._pos.x,
				_curState->_keyFrame._pos.y,
				ZCoordConstants::mapBase+150.0f
			};

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
#if 1
	if(_debugActive) {

		if(input.getDown(GameInput::BUT_O)) {
			GYVector3 dist = { 0.0f, 0.0f, 5.0f };
			//GYVector3 delta = { 0.0f, 0.0f, -0.01f};
			//gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			//gyVec3Normalize(&_curState->dir);
			translate(dist);
		}
		if(input.getDown(GameInput::BUT_L)) {
			GYVector3 dist = { 0.0f, 0.0f, -5.0f };
			//GYVector3 delta = { 0.0f, 0.0f, 0.01f };
			//gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			//gyVec3Normalize(&_curState->dir);
			translate(dist);
		}
		if(input.getDown(GameInput::BUT_DPAD_L)) {
			//GYVector3 delta = { -0.01f, 0.0f, 0.0f };
			//gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			//gyVec3Normalize(&_curState->dir);
		}
		if(input.getDown(GameInput::BUT_DPAD_R)) {
			//GYVector3 delta = { 0.01f, 0.0f, 0.0f };
			//gyVec3Add(&_curState->dir, &_curState->dir, &delta);
			//gyVec3Normalize(&_curState->dir);
		}

		if(input.getDown(GameInput::BUT_W)) {
			translate(GYVector3{ 0.0f, -5.0f, 0.0f});

		}
		if(input.getDown(GameInput::BUT_A)) {

			translate(GYVector3{ -5.0f, 0.0f, 0.0f});
		}
		if(input.getDown(GameInput::BUT_S)) {

			translate(GYVector3{ 0.0f, 5.0f, 0.0f});

		}
		if(input.getDown(GameInput::BUT_D)) {
			translate(GYVector3{ 5.0f, 0.0f, 0.0f});
		}

		if(input.getDown(GameInput::BUT_Z)) {
			_curState->dir.y -= 0.01f;
			_curState->dir.z += 0.01f;
			gyVec3Normalize(&_curState->dir);
		}

		if(input.getDown(GameInput::BUT_X)) {
			_curState->dir.y += 0.01f;
			_curState->dir.z -= 0.01f;
			gyVec3Normalize(&_curState->dir);
		}
	}
#endif

}

void CameraSystem::update(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	updateSubProcesses(deltaTime);

	//Update camera resolution
	getGuiSys()->getResolution(_scrWidth, _scrHeight);

	if(_customState) return;


	//Follow entities
	if(_curState->focalEntity) {
		_curState->_keyFrame._pos = GYVector3 {
			_curState->focalEntity->transform._pos.x,
            _curState->focalEntity->transform._pos.y - _curState->focalEntity->transform._pos.z,
			/*_curState->focalEntity->transform.getPosZ()*/-ZCoordConstants::mapBase,
			//0.0f
		};
	}

	_curState->updateBoundingRegions(0, getCurArea()->getMapWidth()*32, 0, getCurArea()->getMapHeight()*32);

	if(_curState->projType == GY_PROJECTION_ORTHO) {
		//if(_dynamic) {
			_updateState(_curState, deltaTime);
			_curState->updateBoundingRegions(0, getCurArea()->getMapWidth()*32, 0, getCurArea()->getMapHeight()*32);
	//	}

		//gyVidProjectionDefaultMatrix();
		//gyVidCameraSetViewCenter(&_curState->pos, _curState->scale, _curState->zOrient);
		gyVidCamOrthoLookAt(_curState, &_curState->_keyFrame._pos, _curState->_keyFrame._scale*_curState->staticScale, _curState->_keyFrame._zOrient, _curState->_keyFrame._xOrient);

		gyMatPush();
		gyMatLoad(&_curState->projViewMat);
		gyMatShearZ(0.0f, -1.0f);

		gyMatStore(&_curState->projViewMat);
		gyMatPop();


	} else { //PERSPECTIVE PROJECTION

	if(_curState == &_debugState) {
		/*	_debugState.pos = GYVector3{
				-_curState->focalEntity->transform._pos.x,
				-_curState->focalEntity->transform._pos.y+30.0f,
				-(_curState->focalEntity->getElev()+1)*32.0f-ZCoordConstants::mapBase
			};*/


			//gyVidCamFrustumLookAt(_curState, &pos, &_curState->dir, &_curState->up);
		gyVidSetCamWorldPos(&_curState->_keyFrame._pos);

		gyMatPush();

		//Create View Matrix
		const GYVector3 zeroPos = { 0.0f, 0.0f, 0.0f };
		gyVidCamFrustumLookAt(_curState, &zeroPos, &_curState->dir, &_curState->up);
		//gyVidLookAtMat(camMat, 0.0f, 0.0f, 0.0f,
					 //  _curState->dir.x, _curState->dir.y, _curState->dir.z,
					 //  _curState->up.x, _curState->up.y, _curState->up.z);
		gyMatLoad(&_curState->viewMat);

		gyMatScale(1.0f, 1.0f, -1.0f);
		gyMatTranslate(-_curState->_keyFrame._pos.x, -_curState->_keyFrame._pos.y, -_curState->_keyFrame._pos.z);
		gyMatStore(&_curState->viewMat);
		gyMatPop();

		gyMatMultMat(_curState->projViewMat, _curState->projMat, _curState->viewMat);

#if 0
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
	}

	gyVidSetCamWorldPos(&_curState->_keyFrame._pos);


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
