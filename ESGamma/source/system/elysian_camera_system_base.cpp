#include <crtp/elysian_camera_system.hpp>
#include <crtp/elysian_entity.hpp>
#include "elysian_gui_system_base.hpp"
#include "elysian_debug.hpp"
#include <crtp/elysian_area.hpp>
#include <primitive/elysian_rect.hpp>
#include <crtp/elysian_level.hpp>
#include <elysian_timer_system.hpp>
#include <system/elysian_input_system_base.hpp>
#include <system/elysian_event_system.hpp>

namespace elysian {



// ===== INSTANCE MEMBERS =====

void Camera::updateViewVolumeBoundingRegion(void) {
    scrStartX   = INT_MIN;
    scrStartY   = INT_MIN;
    scrEndX     = INT_MAX;
    scrEndY     = INT_MAX;

    clipBoundingRegion(*this);
}

void Camera::clipBoundingRegion(const int xMin, const int xMax, const int yMin, const int yMax) {
    //Clip to 0
    const int clipXMin = (xMin < 0)? 0 : xMin;
    const int clipYMin = (yMin < 0)? 0 : yMin;

    if(scrStartX < clipXMin)    scrStartX   = clipXMin;
    if(scrEndX > xMax)          scrEndX     = xMax;
    if(scrStartY < clipYMin)    scrStartY   = clipYMin;
    if(scrEndY > yMax)          scrEndY     = yMax;

    //assert(scrStartY <= scrEndY);
    //assert(scrStartX <= scrEndX);

    //tileStartX	= scrStartX/32;
    //tileEndX	= scrEndX/32;

    //tileStartY	= scrStartY/32;
   //tileEndY	= scrEndY/32;
}

void Camera::clipBoundingRegion(const Camera &cam) {
    int startX	= INT_MAX;
    int startY	= INT_MAX;
    int endX	= INT_MIN;
    int endY	= INT_MIN;

    auto checkPointBounds = [&](const GYVector3& point) {
        if(point.x < (float)startX) startX  = (int)point.x;
        if(point.x > (float)endX)   endX    = (int)point.x;

        if(point.y < (float)startY) startY  = (int)point.y;
        if(point.y > (float)endY)   endY     = (int)point.y;
    };

    //Find X/Y extremes given each vertex to create bounding box
    checkPointBounds(cam.viewProjVolume.ftr);
    checkPointBounds(cam.viewProjVolume.ftl);
    checkPointBounds(cam.viewProjVolume.fbr);
    checkPointBounds(cam.viewProjVolume.fbl);
    checkPointBounds(cam.viewProjVolume.ntr);
    checkPointBounds(cam.viewProjVolume.ntl);
    checkPointBounds(cam.viewProjVolume.nbr);
    checkPointBounds(cam.viewProjVolume.nbl);

    clipBoundingRegion(startX, endX, startY, endY);
}

void CameraSystemBase::_updateState(Camera*const state, const float /*deltaTime*/) {
    auto &interp = get<SUBPROC_KEY_FR_INTERP>(static_cast<SubProcParent&>(*this));

    auto [width, height] = getViewportSize();

    int boundTop = (_topBoundary == -1)? -16 : _topBoundary*32-16;
    int boundBottom = (_bottomBoundary == -1)? getCurrentArea()->height*32-16 : _bottomBoundary*32-16;
    int boundLeft = (_leftBoundary == -1)? -16 : _leftBoundary*32-16;
    int boundRight = (_rightBoundary == -1)? getCurrentArea()->width*32-16 : _rightBoundary*32-16;

    int boundHeight = boundBottom - boundTop;
    int boundWidth  = boundRight - boundLeft;
   // if(boundHeight < height) {
        float minScaleY = (float)height/(float)boundHeight;
   // }
   // if(boundWidth < width) {
        float minScaleX = (float)width/(float)boundWidth;
    //}

    state->minScale = max(minScaleY, minScaleX);
   // dbgprintf("MinScale - %f", state->minScale);


    float maxHeight = boundBottom	- boundTop;
    float maxWidth	= boundRight	- boundLeft;

    if(_boundByMap) {
        if(state->baseScale < state->minScale) state->baseScale = state->minScale;
        if(state->getScaleX() < state->minScale || state->getScaleY() < state->minScale) {
            state->setScaleX(state->minScale);
            state->setScaleY(state->minScale);

        }
    }




    if(!_dynamic) {
#if 0
        boundTop    += (float)height/_curState->_scale.y*0.5f;
        boundBottom -= (float)height/_curState->_scale.y*0.5f;
        boundLeft   += (float)width/_curState->_scale.x*0.5f;
        boundRight  -= (float)width/_curState->_scale.x*0.5f;


        if(_boundByMap) {
            if(state->_pos.y < boundTop)
                _curState->_pos.y = boundTop;
            if(state->_pos.y > boundBottom)
                _curState->_pos.y = boundBottom;
            if(state->_pos.x < boundLeft)
                _curState->_pos.x = boundLeft;
            if(state->_pos.x > boundRight)
                _curState->_pos.x = boundRight;
        }
#endif
        return;
    }

    //return to base zoom
    if(state->entities.size() == 0) {
        if(fabs(state->baseScale - interp.getEndKeyFrame()._scale.x) > ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON) {
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
            float xDist = fabs(entity->_transform._pos.x-state->_pos.x);
            float yDist = fabs(entity->_transform._pos.y-state->_pos.y);

            //we can fit this fucker onscreen
             if(xDist <= maxXDist && yDist <= maxYDist) {

                if(xDist > newXDist) newXDist = xDist;
                if(yDist > newYDist) newYDist = yDist;

            }
        };

        for(auto it : state->entities) fitEntity(it);

        //Debug::printf(Debug::LOG_VERBOSE, "<%f, %f>, <%f, %f>", newXDist, newYDist, maxXDist, maxYDist);

        //convert dists to widths by adding back margins

        //const float scaleX = (newXDist*2.0f)/((float)baseWidth-(2.0f*(float)baseWidth*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE));
        //const float scaleY = (newYDist*2.0f)/((float)baseHeight-(2.0f*(float)baseHeight*ELYSIAN_CAMERA_STATE_ENTITY_MARGIN_SIZE));
        const float scaleX = (baseWidth)/(newXDist*2.0f);
        const float scaleY = (baseHeight)/(newYDist*2.0f);

        float nextScale = (scaleX < scaleY)? scaleX : scaleY;
       /*
        if(nextScale >= state->minScale) {
            if(fabs(nextScale - interp.getEndKeyFrame()._scale.x) > ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON) {
                setScale(nextScale, 4.0f);
            }
        } else {
            setScale(state->minScale);
        }*/

        if(nextScale < state->minScale) nextScale = state->minScale;




        if(fabs(nextScale - interp.getEndKeyFrame()._scale.x) > ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON) {
                   // dbgprintf("ScALING!!!");
                    //Debug::printf(Debug::LOG_VERBOSE, "SCALE - %f", nextScale);
                    //Debug::printf(Debug::LOG_VERBOSE, "<%f, %f>", scaleX, scaleY);

                    //startScaleInterp(_keyFrame._scale, nextScale, 4.0f);
                    setScale(nextScale, 4.0f);
        }

                    boundTop    += (float)height/_curState->_scale.y*0.5f;
                    boundBottom -= (float)height/_curState->_scale.y*0.5f;
                    boundLeft   += (float)width/_curState->_scale.x*0.5f;
                    boundRight  -= (float)width/_curState->_scale.x*0.5f;


                    if(_boundByMap) {
                        if(state->_pos.y < boundTop)
                            _curState->_pos.y = boundTop;
                        if(state->_pos.y > boundBottom)
                            _curState->_pos.y = boundBottom;
                        if(state->_pos.x < boundLeft)
                            _curState->_pos.x = boundLeft;
                        if(state->_pos.x > boundRight)
                            _curState->_pos.x = boundRight;
                    }

                    if(_boundByMap) {
                        auto& pos = interp.getEndKeyFrame()._pos;
                        if(pos.y < boundTop)
                            pos.y = boundTop;
                        if(pos.y > boundBottom)
                            pos.y = boundBottom;
                        if(pos.x < boundLeft)
                            pos.x = boundLeft;
                        if(pos.x > boundRight)
                            pos.x = boundRight;
                    }


               // }

        //setScale(nextScale);

        //float boundWidth = ;
        //float boundHeight = ;

        //Debug::printf(Debug::LOG_VERBOSE, "%f, %f - NEWSCALE - %f", scaleX, scaleY, nextScale);


        //Check if we need to bother updating
        //if(fabs(nextScale - interp.getEndKeyFrame()._scale.x) > ELYSIAN_CAMERA_STATE_SCALE_INTERP_EPSILON) {
           // dbgprintf("ScALING!!!");
            //Debug::printf(Debug::LOG_VERBOSE, "SCALE - %f", nextScale);
            //Debug::printf(Debug::LOG_VERBOSE, "<%f, %f>", scaleX, scaleY);

            //curInterp.startScaleInterp(_keyFrame._scale, nextScale, 4.0f);
            //setScale(nextScale, 4.0f);

       // }


    }

    //_keyFrame._scale = curInterp.getNewScale();

}


CameraSystemBase::CameraSystemBase(void) {
    CameraShakeSubProcess::_cameraSys = this;

    //initializeProjMatrix();

    for(unsigned i = 0; i < ELYSIAN_CAMERA_SYSTEM_STACK_SIZE; ++i) {
        _states[i] = &_internalStates[i];
    }
}

void CameraSystemBase::initializeProjMatrix(void) {
    static const GYMatrix4 identity4 = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
    unsigned w, h;
    //Initialize default camera
    gyVidResolution(&w, &h);
    //w = 640; h = 480;
    gyVidCamOrthoProj(_curState, 0.0f, (float)w, (float)h, 0.0f, -1500.0f, 1500.0f);
    gyVidCamOrthoLookAt(_curState, &_curState->_pos, 1.0f, 0.0f, &_curState->_orient);

    memcpy(_uiCamera.viewMat, identity4, sizeof(GYMatrix4));
    gyVidCamOrthoProj(&_uiCamera, 0.0f, (float)w, (float)h, 0.0f, -999.0f, 1000.0f);

    GYVector3 orient = { 0.0f, 1.0f, 1.0f };
    gyVec3Normalize(&orient);

    _debugState.setOrient(orient.x, orient.y, orient.z);
    const GYVector3 zeroPos = { 0.0f, 0.0f, 0.0f };
    gyVidCamFrustumLookAt(&_debugState, &zeroPos, &_debugState._orient, &_debugState.up);
    gyVidCamFrustum(&_debugState, 45.0f, 1.0f, 0.5f, 9000.0f);

    _curState->updateViewVolumeBoundingRegion();
    //_curState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);

}

Vector2 CameraSystemBase::getScreenResolution(void) const {
    unsigned w, h;
    gyVidResolution(&w, &h);
    return Vector2(w, h);
}

void CameraSystemBase::ensureVisibleRect(const Rectf& in, Rectf& out, const Rectf margins) const {
    Vector2 res = getScreenResolution();
    Rectf boundaries(margins.top, res.getY() - margins.bottom, margins.left, res.getX() - margins.right);
    out = in;

    if(in.getLeft() < boundaries.getLeft()) {
        out.translate(boundaries.getLeft() - in.getLeft(), 0.0f);
    }

    if(in.getRight() > boundaries.getRight()) {
        out.translate(boundaries.getRight() - in.getRight(), 0.0f);
    }

    if(in.getTop() < boundaries.getTop()) {
        out.translate(0.0f, boundaries.getTop() - in.getTop());
    }

    if(in.getBottom() > boundaries.getBottom()) {
        out.translate(0.0f, boundaries.getBottom() - in.getBottom());
    }
}

void CameraSystemBase::setCurrentCamera(Camera*const state) {
    _states[_depth] = state;
    static_cast<CameraSystem*>(this)->setCurrentIndex(_depth);
}

void CameraSystemBase::pushViewProjMatrix(void) {


    gyVidCamUpdate(_curState);


    _projViewActive = true;
}

void CameraSystemBase::getVisibleTileRegion(int& startX, int& startY, int& endX, int& endY, const Camera* state) const {
    if(!state) state = _curState;
    startX = state->scrStartX / ELYSIAN_TILE_WIDTH; endX = state->scrEndX / ELYSIAN_TILE_WIDTH;
    startY = state->scrStartY / ELYSIAN_TILE_HEIGHT; endY = state->scrEndY / ELYSIAN_TILE_HEIGHT;

    //assert(startX >= 0);
    //assert(startY >= 0);
    if(++endX >= _curArea->width) endX = _curArea->width-1;
    if(++endY >= _curArea->height) endY = _curArea->height-1;
    //if(startX < 0) startX = 0;
    //if(endX < 0) endX = 0;
    //if(startY < 0) startY = 0;
    //if(endY < 0) endY = 0;
/*
    if(endX < startX) {
        std::swap(startX, endX);
    }

    if(endY < startY) {
        std::swap(startY, endY);
    }*/
}

void CameraSystemBase::popViewProjMatrix(void) {
    //Set View matrix to identity Matrix
    //Set proj matrix to ortho perspective

    //gyVidProjectionDefaultMatrix();
    //gyVidCameraCustomViewMatrix(_identityMat);

    gyVidCamUpdate(&_uiCamera);

    _projViewActive = false;
}


void CameraSystemBase::setPerspectiveFrustum(const float /*fov*/, const float /*aspect*/, const float /*znear*/, const float /*zfar*/) {
    //_curState->defaultProjMat = false;

    //gyVidViewFrustumMat(_curState->projMat, fov, aspect, znear, zfar);
    //gyVidProjectionCustomMatrix(_curState->projMat);
}

void CameraSystemBase::popState(void) {
    --_depth;
    GY_ASSERT(_depth >= 0);
    if(_depth <= 0) _depth = 0;
    //_states[_depth] = &_internalStates[_depth];

    setCurrentCamera(_states[_depth]);
    _customState = false;
    //restore projection matrix if it's supposed to be on the stack is this even really necessary?
    /*if(_projViewActive) {
        if(_curState->defaultProjMat) gyVidProjectionDefaultMatrix();
        else gyVidProjectionCustomMatrix(_curState->projMat);
    }*/
}

void CameraSystemBase::processInput(const Keyboard* input) {

    if(input->isTapped(GY_KEY_P)) {
        _debugActive = !_debugActive;

        if(_debugActive) { //enable
#if 1
            _debugState.focalEntity = _curState->focalEntity;

            _debugState._pos = GYVector3 {
                _curState->_pos.x-500.0f,
                _curState->_pos.y+1000.0f,
                _curState->_pos.z+500.0f
            };

           // _curState = &_debugState;
            //setPerspectiveFrustum(90.0f, 1.0f, 0.5f, 900.0f);
            //setEngineState(ENGINE_CAMERA_DEBUG);
            pushCustomCamera(&_debugState);
            gyVidCamFrustum(_curState, 45.0f, 1.0f, 0.5f, 900.0f);
            _curState->updateViewVolumeBoundingRegion();
            _curState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);

#else
        gyVidCamFrustum(_curState, 45.0f, 1.0f, 0.5f, 900.0f);
        _curState->updateViewVolumeBoundingRegion();
        _curState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);

#endif

        } else { //disable
            if(_curState == &_debugState) //Only pop if debug is current camera
                popState();
            //setEngineState(ENGINE_NORMAL);
        }
    }
#if 1
    if(_curState == &_debugState) {

        if(input->isPressed(GY_KEY_O)) {
            GYVector3 dist = { 0.0f, 0.0f, 5.0f };
            //GYVector3 delta = { 0.0f, 0.0f, -0.01f};
            //gyVec3Add(&_curState->dir, &_curState->dir, &delta);
            //gyVec3Normalize(&_curState->dir);
            translateOffset(dist);
        }
        if(input->isPressed(GY_KEY_L)) {
            GYVector3 dist = { 0.0f, 0.0f, -5.0f };
            //GYVector3 delta = { 0.0f, 0.0f, 0.01f };
            //gyVec3Add(&_curState->dir, &_curState->dir, &delta);
            //gyVec3Normalize(&_curState->dir);
            translateOffset(dist);
        }
        if(input->isPressed(GY_KEY_DPAD_L)) {
            //GYVector3 delta = { -0.01f, 0.0f, 0.0f };
            //gyVec3Add(&_curState->dir, &_curState->dir, &delta);
            //gyVec3Normalize(&_curState->dir);
        }
        if(input->isPressed(GY_KEY_DPAD_R)) {
            //GYVector3 delta = { 0.01f, 0.0f, 0.0f };
            //gyVec3Add(&_curState->dir, &_curState->dir, &delta);
            //gyVec3Normalize(&_curState->dir);
        }

        if(input->isPressed(GY_KEY_W)) {
            translateOffset(GYVector3{ 0.0f, -5.0f, 0.0f});

        }
        if(input->isPressed(GY_KEY_A)) {

            translateOffset(GYVector3{ -2.0f, 0.0f, 0.0f});
        }
        if(input->isPressed(GY_KEY_S)) {

            translateOffset(GYVector3{ 0.0f, 5.0f, 0.0f});

        }
        if(input->isPressed(GY_KEY_D)) {
            translateOffset(GYVector3{ 2.0f, 0.0f, 0.0f});
        }

        if(input->isPressed(GY_KEY_Z)) {
 //           _curState->_keyFrame._dir.y -= 0.01f;
 //           _curState->_keyFrame._dir.z += 0.01f;
  //          gyVec3Normalize(&_curState->_keyFrame._dir);
        }

        if(input->isPressed(GY_KEY_X)) {
    //        _curState->_keyFrame._dir.y += 0.01f;
  //          _curState->_keyFrame._dir.z -= 0.01f;
  //          gyVec3Normalize(&_curState->_keyFrame._dir);
        }
    }
#endif
}

void CameraSystemBase::update(void) {
    const float deltaTime = getTimerSys()->getDeltaTime();

    //Update camera resolution
    unsigned scrW, scrH;
    gyVidResolution(&scrW, &scrH);

    if(scrW != _viewportSize.first || scrH != _viewportSize.second) {
        _viewportSize = std::make_pair(scrW, scrH);
        getEventSys()->broadcastEvent(Event(EVENT_VIEWPORT_SIZE_CHANGED));
    }

    if(_customState) return;

    if(_curState->projType == GY_PROJECTION_ORTHO) {
        updateSubProcesses(deltaTime);
        //Follow entities
        if(_followEnabled && _curState->focalEntity) {
            _curState->_pos = GYVector3 {
                _curState->focalEntity->_transform._pos.x,
                _curState->focalEntity->_transform._pos.y - _curState->focalEntity->_transform._pos.z,
                1.0f//-_curState->focalEntity->_transform.getPosZ()//*///ZCoordConstants::mapBase,
                //0.0f
            };
        } else {
        //    _curState->_keyFrame._pos = GYVector3 { 0.0f, 0.0f, 0.0f };
        }

      //  gyVec3AddAccum(&_curState->_keyFrame._pos, &_curState->_keyFrame._offset);

        //if(_dynamic) {

			_updateState(_curState, deltaTime);
        //    _curState->updateBoundingRegions(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);
	//	}

        //gyVidProjectionDefaultMatrix();
        //gyVidCameraSetViewCenter(&_curState->pos, _curState->scale, _curState->zOrient);
#if 0
        gyVidCamOrthoLookAt(_curState, &_curState->_keyFrame._pos, _curState->_keyFrame._scale*_curState->staticScale*_curState->_keyFrame._pos.z, _curState->_keyFrame._zOrient, _curState->_keyFrame._xOrient);
#else
        gyVidCamOrthoLookAt(_curState, &_curState->_pos, _curState->_scale.x*_curState->staticScale*_curState->_pos.z, _curState->_angle, &_curState->_orient);
#endif

        gyMatPush();
        gyMatLoad(&_curState->projViewMat);
        gyMatShearZ(0.0f, -1.0f);

        gyMatStore(&_curState->projViewMat);
        gyMatPop();

        gyVidSetCamWorldPos(&_curState->_pos);


    } else { //PERSPECTIVE PROJECTION

  //  if(_curState == &_debugState) {
        GYVector3 worldPos = _curState->_pos;

        if(_curState->focalEntity) {
            _curState->_pos = GYVector3 {
                _curState->focalEntity->_transform._pos.x,
                _curState->focalEntity->_transform._pos.y +1000.0f,
                /*_curState->focalEntity->transform.getPosZ()*/-ZCoordConstants::mapBase+400.0f+ _curState->focalEntity->_transform._pos.z,
                //0.0f
            };
        }

       // gyVec3Add(&worldPos, &_curState->_keyFrame._pos, &_curState->_keyFrame._offset);


        /*	_debugState.pos = GYVector3{
                -_curState->focalEntity->transform._pos.x,
                -_curState->focalEntity->transform._pos.y+30.0f,
                -(_curState->focalEntity->getElev()+1)*32.0f-ZCoordConstants::mapBase
            };*/


            //gyVidCamFrustumLookAt(_curState, &pos, &_curState->dir, &_curState->up);
        //gyVidSetCamWorldPos(&_curState->_keyFrame._pos);

        gyMatPush();

        //Create View Matrix
        const GYVector3 zeroPos = { 0.0f, 0.0f, 0.0f };
        gyVidCamFrustumLookAt(_curState, &zeroPos, &_curState->_orient, &_curState->up);
        //gyVidLookAtMat(camMat, 0.0f, 0.0f, 0.0f,
                     //  _curState->dir.x, _curState->dir.y, _curState->dir.z,
                     //  _curState->up.x, _curState->up.y, _curState->up.z);
        gyMatLoad(&_curState->viewMat);



        gyMatScale(1.0f, 1.0f, -1.0f);

        gyMatShearZ(0.0f, -1.0f);

        gyMatTranslate(-worldPos.x, -worldPos.y, -worldPos.z);
        gyMatStore(&_curState->viewMat);
        gyMatPop();

        gyMatMultMat(_curState->projViewMat, _curState->projMat, _curState->viewMat);

        gyVidSetCamWorldPos(&worldPos);
        updateSubProcesses(deltaTime);

       // }
    }
    //Update camera boundaries
   _curState->updateViewVolumeBoundingRegion();
   _curState->clipBoundingRegion(0, getCurrentArea()->getMapWidth()*32, 0, getCurrentArea()->getMapHeight()*32);
}

Vector3 CameraSystemBase::transformWorldToScreen(const Vector3& vec) {
   GYVector3 out;
   GYVector3 in = { vec.getX(), vec.getY(), vec.getZ() };
   transform(&out, &in);
   return out;
}

void CameraSystemBase::transformWorldToScreen(const Rectf &in, Rectf &out) {
    GYVector3 topLeft = { in.getLeft(), in.getTop(), 0.0f };
    GYVector3 bottomRight = { in.getRight(), in.getBottom(), 0.0f };
    GYVector3 scrTopLeft, scrBotRight;
    transform(&scrTopLeft, &topLeft);
    transform(&scrBotRight, &bottomRight);
    out.setBoundaries(scrTopLeft.y, scrBotRight.y, scrTopLeft.x, scrBotRight.x);
}


//VERY INEFFICIENT. BATCH THIS BITCH! THESE DON'T WORK ANYMORE!!!!
void CameraSystemBase::transform(GYVector3* dest, const GYVector3* src) {
    GYVector4 in = { src->x, src->y, src->z, 1.0f };
    GYVector4 out;

    gyMatPush();
    //gyMatLoadIdentity();
    gyMatLoad(&_curState->viewMat);
    gyMatMultVec(&in, &out, 1);
    gyMatPop();

    dest->x = out.x; dest->y = out.y; dest->z = out.z;
}

void CameraSystemBase::transform(GYVector4* /*dest*/, const GYVector4* /*src*/) {
/*
    gyMatPush();
    //gyMatLoadIdentity();
    gyMatLoad(&_curState->viewMat);
    gyMatMultVec(&dest, &src, 1);
    gyMatPop();
&*/
}

//NEEDS TO RESPECT ELEVATION + DIFFERENT PLANE TYPES
bool CameraSystemBase::isOnscreen(const Entity &entity) {
#if 1
    if(entity._transform._orient.z == 0.0f) { //Entities without rotation
        return _curState->isWithinScreenRegion(entity._transform._pos.x, entity._transform._pos.y-entity._transform._pos.z,
                                    entity._transform._size.x * entity._transform._scale.x * 0.5f,
                                    entity._transform._size.y * entity._transform._scale.y * 0.5f);
    } else { //Entities with rotation
        //Approximate region by bounding as a square with largest dimension
        const float xSize = entity._transform._size.x * entity._transform._scale.x * 0.5f;
        const float ySize = entity._transform._size.y * entity._transform._scale.y * 0.5f;

        const float length = (xSize > ySize)? xSize : ySize;

        return _curState->isWithinScreenRegion(entity._transform._pos.x, entity._transform._pos.y,
                                    length,
                                    length);
    }
#else/*
    if(entity._transform._orientZ == 0.0f) { //Entities without rotation
        return ((entity._transform._pos.x + 0.5f*entity._transform._size.x >= (float)_curState->scrStartX &&
                entity._transform._pos.x - 0.5f*entity._transform._size.x <= (float)_curState->scrEndX) &&
                (entity._transform._pos.y + 0.5f*entity._transform._size.y >= (float)_curState->scrStartY &&
                entity._transform._pos.y - 0.5f*entity._transform._size.y <= (float)_curState->scrEndY));
    } else { //Entities with rotation
        //Approximate region by bounding as a square with largest dimension
        float bigSide = (entity._transform._size.x > entity._transform._size.y)?
            0.5f*entity._transform._size.x : 0.5f*entity._transform._size.y;

        return ((entity._transform._pos.x + bigSide >= _curState->scrStartX &&
                entity._transform._pos.x - bigSide <= _curState->scrEndX) &&
                (entity._transform._pos.y + bigSide >= _curState->scrStartY &&
                entity._transform._pos.y - bigSide <= _curState->scrEndY));
    }*/

    return true;
#endif
}


void CameraSystemBase::centerOnEntity(const Entity& entity, float time, EventMonitor*const monitor) {
    moveToPosition(entity._transform._pos.x, entity._transform._pos.y, entity._transform._pos.z, time, monitor);
}

inline void CameraSystemBase::setScale(const float factor, const float time, EventMonitor*const monitor) {
    auto &interp = get<SUBPROC_KEY_FR_INTERP> ( static_cast<SubProcParent&>(*this));

    if(interp.getProcState() == PROC_STATE_RUNNING && System::getEngineState() == System::ENGINE_WARPING) return;

    CameraKeyFrame frame = getKeyFrame();
    frame.setScale(factor, factor);
    interp.setKeyFrames(_curState, frame);
    interp.setTimeInterval(time);
    interp.setEventMonitor(monitor);
    interp.start(this);
}


}
