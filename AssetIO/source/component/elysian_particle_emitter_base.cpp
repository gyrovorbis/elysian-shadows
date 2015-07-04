#include <kinetic/gyro_kinetic_physics.h>
#include "elysian_particle_emitter.hpp"
#include "elysian_generic.hpp"
#include "elysian_material.hpp"
#include "elysian_debug.hpp"
#include "elysian_force_generator.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_map_depth_buffer.hpp"
#include "elysian_area.hpp"
#include <elysian_tile.hpp>
#include <elysian_map_depth_buffer.hpp>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ParticleEmitterBase)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ParticleEmitterAttrib)

ParticleEmitterAttrib::ParticleEmitterAttrib(void) {}

ParticleEmitterAttrib::~ParticleEmitterAttrib(void) {
    delete _material;

    gyVidTexDestroy(_normalMap);
    gyVidTexDestroy(_specularMap);

}

auto Particle::getTargetType(void)	const -> TARGET_TYPE { return TARGET_PARTICLE; }
float Particle::getOrientation(void) const { return orientation; }
const GYVector3& Particle::getPosition(void) const { return position; }
const GYStatic* Particle::getStaticAttrib(void) const { return static_cast<const GYStatic*>(attrib); }
bool Particle::getRestingOnBody(void) const { return resting; }

void Particle::integrate(const float deltaTime) {

    if(!resting)
        gyIntegrate(static_cast<GYKinetic*>(this),
                attrib,
                deltaTime,
                &position,
                &orientation);

    (void)deltaTime;

}

void ParticleEmitterInternals::emitFromPointInDir(const GYVector3& pos, const GYVector3& dir, const unsigned intensity, float angle) {
    Particle* part;

    if(_pool.isFull()) return;

    const float scaledIntensity = (float)intensity*_attrib->_intensityFactor;

    for(unsigned i = 0; i < intensity && !_pool.isFull(); ++i) {

        part = &_pool.claimFreeEntry();

        part->clearKinetic();

        part->resting = false;
        part->attrib = _attrib;
        part->flat = (rand()%2 == 1);
        part->position.x = pos.x + ((rand() % 6) - 3);
        part->position.y = pos.y + ((rand() % 6) - 3);
        part->position.z = pos.z + ((rand() % 6) - 3);
        part->life = _attrib->_maxLife;// - (rand() % ((int)((float)scaledIntensity/20.0f) + 1));

        part->animRow = (_attrib->_animGridHeight > 1)? rand()%(_attrib->_animGridHeight) : 0;
        part->frame = _attrib->_animGrid[part->animRow][(_attrib->_randomStartFrame)? rand()%_attrib->_animGridWidth : 0];

        if(part->life <= 0) part->life = 1;
        part->size = _attrib->_startSize;

        const float angleRatio = angle/360.0f;
        int randomMag = angleRatio*(scaledIntensity/(float)(rand()%14 + 10));
        if(randomMag == 0) randomMag = 1;
        const int halfRandomMag = 0.5f * randomMag;

        GYVector3 dirOffset = {
            dirOffset.x = (float)(rand()%randomMag-halfRandomMag),
            dirOffset.y = (float)(rand()%randomMag-halfRandomMag),
            dirOffset.z = (float)(rand()%randomMag-halfRandomMag)
        };

        //HOW ARE WE ROTATING!?
        //part->rotation = -cosTheta*(intensity/100.0f); //tweak me better!


        //relAngle = angle + ((float)(rand()%50)-25)*(M_PI/180.0f);
        //part->orientation = relAngle;

        GYVector3 velocity = {  dir.x * (float)scaledIntensity/(float)(rand()%14 + 10),
                                dir.y * (float)scaledIntensity/(float)(rand()%14 + 10),
                                dir.z * (float)scaledIntensity/(float)(rand()%14 + 10) };

        GYVector3 finalVel;
        gyVec3Add(&finalVel, &velocity, &dirOffset);

        part->setVelocity(finalVel);

/*
#if 1
        part->setVelocity(GYVector3 { cosTheta * (float)intensity/(float)(rand()%14 + 10),
                          sinTheta * (float)intensity/(float)(rand()%14 + 10), -sinTheta * (float)intensity/(float)(rand()%14 + 10) });
#else
        part->setVelocity(GYVector3 { cosTheta * (float)intensity/(float)(10),
                          sinTheta * (float)intensity/(float)(10) });
#endif
*/

        part->color = _attrib->getColor();
        //part->color.r += (rand()%6)-3;
        //part->color.g += (rand()%6)-3;
        //part->color.b += (rand()%6)-3;


    }

}

void ParticleEmitterInternals::emitFromPoint(const GYVector3 &pos, const float angle, const unsigned intensity) {
    float relAngle;
    float sinTheta, cosTheta;

    relAngle = angle + ((float)(rand()%50)-25)*(M_PI/180.0f);
    //part->orientation = relAngle;

    gySinCos(relAngle, &sinTheta, &cosTheta);

    GYVector3 dir = {
        cosTheta, sinTheta, -sinTheta
    };

    emitFromPointInDir(pos, dir, intensity, 180.0f);

}


void ParticleEmitterInternals::update(const CameraState*const cam, const float deltaTime) {
    GYVector3 newVel;
    //Check for periodic emission!

    for(auto it = _pool.begin(); it < _pool.end(); ++it) {

        //apply bullshit gravity
        GYVector3 bullshitGravity;

        if((*it).position.z > ZCoordConstants::mapBase+1) {
            gyVec3Scale(&bullshitGravity, &_attrib->_acceleration, deltaTime);

            GYVector3 vel = (*it).getVelocity();
            gyVec3Add(&newVel, &vel, &bullshitGravity);
            (*it).setVelocity(newVel);
            //integrate
            (*it).integrate(deltaTime);
        } else if(_attrib->_stick){
            (*it).position.z = ZCoordConstants::mapBase+0.01;
            (*it).zIndex = 0;
            (*it).resting = true;
        } else { //we don't stick
            _pool.remove(it);
        }

        if(!cam->isWithinScreenRegion((*it).position.x, (*it).position.y, (*it).size, (*it).size)) {
            _pool.remove(it);
            continue;
        }

        if(--(*it).life == 0) {
            _pool.remove(it);
            continue;
        }

        //Calculate depth buffer internals
        if(!(*it).resting) {
            (*it).zIndex = (((*it).position.y - 0.5f*(*it).size)-(float)cam->scrStartY)/32.0f + ((*it).position.z-ZCoordConstants::mapBase)/32.0f;
            if((*it).zIndex < 0) (*it).zIndex = 0;
            if((*it).zIndex >= ELYSIAN_DEPTH_BUFFER_HEIGHT) (*it).zIndex = ELYSIAN_DEPTH_BUFFER_HEIGHT;
        }

        const int curLife = _attrib->_maxLife - (*it).life;

        //Linearly interpolate shit
        GYRO_LINEAR_INTERP((*it).size, _attrib->_startSize, _attrib->_endSize, _attrib->_maxLife, curLife);

        GYRO_LINEAR_INTERP((*it).color.a, _attrib->getColor().a, _attrib->_endColor.a, _attrib->_maxLife, curLife);
        GYRO_LINEAR_INTERP((*it).color.r, _attrib->getColor().r, _attrib->_endColor.r, _attrib->_maxLife, curLife);
        GYRO_LINEAR_INTERP((*it).color.g, _attrib->getColor().g, _attrib->_endColor.g, _attrib->_maxLife, curLife);
        GYRO_LINEAR_INTERP((*it).color.b, _attrib->getColor().b, _attrib->_endColor.b, _attrib->_maxLife, curLife);

        unsigned animCol;
        unsigned animDuration = (_attrib->_animFrameDuration > 0)? _attrib->_animFrameDuration : _attrib->_maxLife;
        if(_attrib->getTexture()) {
            GYRO_LINEAR_INTERP(animCol, 0, _attrib->_animGridWidth, animDuration, curLife);
            if(_attrib->_animGridWidth != 0) animCol %= _attrib->_animGridWidth;
            else animCol = 0;
            (*it).frame = _attrib->_animGrid[(*it).animRow][animCol];
        }
    }

}

void ParticleEmitterInternals::applyForce(ForceGenerator*const force, const float deltaTime) {
    for(auto it = _pool.begin(); it < _pool.end(); ++it) {
        force->applyForce(&(*it), deltaTime);
    }

}

void ParticleEmitterInternals::render(void) {
    if(gyVidShadowPass() && !_attrib->_shadowVolume) return;

    //Probably could be optimized as shit to not bother using
    //internal matrix stack!

    if(_pool.size() > 0) {
        gyVidTexBind(_attrib->getTexture());

        gyVidNormalMapBind(_attrib->_normalMap);
        gyVidSpecularMapBind(_attrib->_specularMap);

        gyVidMaterialBind(_attrib->_material);
        gyVidAlphaLevel(GY_ALPHA_FULL);
        gyVidRectPlaneType(GY_RECT_PLANE_FRONT);

        gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
        gyVidAlphaLevel(GY_ALPHA_FULL);

        //loop unroll my bitch ass

        for(auto it = _pool.begin(); it < _pool.end(); ++it) {

            gyMatPush();
            gyVidQuadListBegin(&(*it).color);

            gyMatTranslate((*it).position.x, (*it).position.y, (*it).position.z);
            //gyMatRotateZ((*it).orientation);
            gyMatScale((*it).size, (*it).size, (*it).size);

            gyVidRect((*it).frame);

            gyVidQuadListEnd();
            gyMatPop();

        }
    }
}

void ParticleEmitterInternals::renderAtElev(const unsigned char elev) {
    if(gyVidShadowPass() && !_attrib->_shadowVolume) return;
    bool initialized = false;

    //loop unroll my bitch ass

    for(auto it = _pool.begin(); it < _pool.end(); ++it) {

        //Check if we are within the range of this level of elevation
        if((*it).zIndex != elev)  continue;

        if(!initialized) {
            gyVidTexBind(_attrib->getTexture());

            gyVidNormalMapBind(_attrib->_normalMap);
            gyVidSpecularMapBind(_attrib->_specularMap);

            gyVidMaterialBind(_attrib->_material);
            gyVidAlphaLevel(GY_ALPHA_FULL);

            gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
            initialized = true;

        }

       // gyVidRectPlaneType((*it).flat? GY_RECT_PLANE_BOTTOM : GY_RECT_PLANE_FRONT);

        gyMatPush();
        gyVidQuadListBegin(&(*it).color);

        gyMatTranslate((*it).position.x, (*it).position.y, (*it).position.z);
        //gyMatRotateZ((*it).orientation);
        gyMatScale((*it).size, (*it).size, (*it).size);

        gyVidRect((*it).frame);

        gyVidQuadListEnd();
        gyMatPop();

    }
}

void ParticleEmitterBase::emitFromEntity(const GYVector3 &pos, const float angle, const unsigned intensity) {
    GYVector4 localPos = { pos.x, pos.y, pos.z, 1.0f };
    GYVector4 worldPos;

    gyMatPush();
    gyMatLoadIdentity();

    _entity->_transform.apply();

    gyMatMultVec(&localPos, &worldPos, 1);
    gyMatPop();

    emitFromPoint(*(GYVector3*)&worldPos, angle, intensity);

}


ParticleEmitterAttrib* ParticleEmitterInternals::getAttrib(void) const {
    return _attrib;
}

void ParticleEmitterInternals::allocatePool(const unsigned size) {
    _pool.resize(size);
}

void ParticleEmitterInternals::setAttrib(ParticleEmitterAttrib * const attrib) {
    //delete _attrib;
	_attrib = attrib;
}

ParticleEmitterInternals::~ParticleEmitterInternals(void) {}


ParticleEmitterBase::ParticleEmitterBase(Entity& entity): Component(entity) {}


}
