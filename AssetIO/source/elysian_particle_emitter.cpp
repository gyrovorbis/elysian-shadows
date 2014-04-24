#include "elysian_particle_emitter.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"
#include "elysian_entity.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_material.hpp"
#include "elysian_force_generator.hpp"
#include "elysian_debug.hpp"


namespace elysian {

ParticleEmitterAttrib::ParticleEmitterAttrib(void):
	GYStatic({1.0f/500.0f,
			 1.0f,
			 0.99f,
			 1.0f/5000.0f,
			 0.99f
	}) {}

ParticleEmitterAttrib::~ParticleEmitterAttrib(void) {
	delete _material;
	delete _specularMap;
}

auto Particle::getTargetType(void)	const -> TARGET_TYPE { return TARGET_PARTICLE; }
float Particle::getOrientation(void) const { return orientation; }
const GYVector3& Particle::getPosition(void) const { return position; }
const GYStatic* Particle::getStaticAttrib(void) const { return static_cast<const GYStatic*>(attrib); }

void Particle::integrate(const float deltaTime) {
	gyIntegrate(static_cast<GYKinetic*>(this),
				attrib,
				deltaTime,
				&position,
				&orientation);
}

void ParticleEmitterInternals::emitFromPoint(const GYVector3& pos, const float angle, const unsigned intensity) {
	Particle* part;
	float relAngle;
	float sinTheta, cosTheta;


	if(_pool.isFull()) return;

	for(unsigned i = 0; i < intensity && !_pool.isFull(); ++i) {

		part = &_pool.claimFreeEntry();

		part->clearKinetic();

		part->attrib = _attrib;
		part->flat = (rand()%2 == 1);
		part->position.x = pos.x + ((rand() % 6) - 3);
		part->position.y = pos.y + ((rand() % 6) - 3);
		part->position.z = pos.z + ((rand() % 6) - 3);
		part->life = _attrib->_maxLife - (rand() % ((int)((float)intensity/20.0f) + 1));

		if(_attrib->_minFrame != _attrib->_maxFrame) { //use range
			part->frame = rand()%(_attrib->_maxFrame-_attrib->_minFrame)+_attrib->_minFrame;
		} else { //use static
			part->frame = _attrib->getFrame();
		}

		if(part->life <= 0) part->life = 1;
		part->size = _attrib->_size;
		if(part->size < 1) part->size = 1;
		//if(part->size > 200) part->size = 200;

		//doesn't center the angle!
		relAngle = angle + ((float)(rand()%50)-25)*(M_PI/180.0f);
		part->orientation = relAngle;

		gySinCos(relAngle, &sinTheta, &cosTheta);

		part->rotation = -cosTheta*(intensity/100.0f); //tweak me better!


#if 1
		part->setVelocity(GYVector3 { cosTheta * (float)intensity/(float)(rand()%14 + 10),
						  sinTheta * (float)intensity/(float)(rand()%14 + 10), -sinTheta * (float)intensity/(float)(rand()%14 + 10) });
#else
		part->setVelocity(GYVector3 { cosTheta * (float)intensity/(float)(10),
						  sinTheta * (float)intensity/(float)(10) });
#endif

		part->color = _attrib->getColor();
		part->color.r += (rand()%6)-3;
		part->color.g += (rand()%6)-3;
		part->color.b += (rand()%6)-3;


	}

}

void ParticleEmitterInternals::update(const CameraState*const cam, const float deltaTime) {
	GYVector3 newVel;
	//Check for periodic emission!

	for(auto it = _pool.begin(); it < _pool.end(); ++it) {

		//apply bullshit gravity
		GYVector3 bullshitGravity;

		if((*it).position.z > ZCoordConstants::mapBase) {
			gyVec3Scale(&bullshitGravity, &_attrib->_acceleration, deltaTime);

			gyVec3Add(&newVel, &(*it).getVelocity(), &bullshitGravity);
			(*it).setVelocity(newVel);
			//integrate
			(*it).integrate(deltaTime);
		} else {
			(*it).position.z = ZCoordConstants::mapBase;
		}

		if(!cam->isWithinScreenRegion((*it).position.x, (*it).position.y, (*it).size, (*it).size)) {
			_pool.remove(it);
			continue;
		}

		if(--(*it).life == 0) {
			_pool.remove(it);
			continue;
		}
		
		(*it).zIndex = (((*it).position.y - 0.5f*(*it).size)-(float)cam->scrStartY)/32.0f + ((*it).position.z-ZCoordConstants::mapBase)/32.0f;
		if((*it).zIndex < 0) (*it).zIndex = 0;
		if((*it).zIndex >= ELYSIAN_DEPTH_BUFFER_HEIGHT) (*it).zIndex = ELYSIAN_DEPTH_BUFFER_HEIGHT;

		if(_attrib->_fadeOut) (*it).color.a = 255.0f*((float)_attrib->_maxLife/(float)(*it).life);
	}
}

void ParticleEmitterInternals::applyForce(ForceGenerator*const force, const float deltaTime) {
	for(auto it = _pool.begin(); it < _pool.end(); ++it) {

		force->applyForce(&(*it), deltaTime);
	}

}

void ParticleEmitterInternals::render(void) {

	//Probably could be optimized as shit to not bother using
	//internal matrix stack!

	gyVidTexBind(_attrib->getTexture());
	gyVidMaterialBind(_attrib->_material);
	gyVidAlphaLevel(GY_ALPHA_FULL);
	gyVidRectNormalType(GY_RECT_NORMAL_FRONT);

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

void ParticleEmitterInternals::renderAtElev(const unsigned char elev) {
	gyVidTexBind(_attrib->getTexture());
	gyVidMaterialBind(_attrib->_material);
	gyVidAlphaLevel(GY_ALPHA_FULL);

	//loop unroll my bitch ass

	for(auto it = _pool.begin(); it < _pool.end(); ++it) {

		//Check if we are within the range of this level of elevation
		if((*it).zIndex != elev)  continue;

		gyVidRectNormalType((*it).flat? GY_RECT_NORMAL_FLAT : GY_RECT_NORMAL_FRONT);

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

ParticleEmitterAttrib* ParticleEmitterInternals::getAttrib(void) const {
	return _attrib;
}

void ParticleEmitterInternals::allocatePool(const unsigned size) {
	_pool.resize(size);
}

void ParticleEmitterInternals::setAttrib(ParticleEmitterAttrib * const attrib) {
	_attrib = attrib;
}

ParticleEmitterInternals::~ParticleEmitterInternals(void) {}


ParticleEmitter::ParticleEmitter(Entity& entity): Component(entity) {}

void ParticleEmitter::emitFromEntity(const GYVector3 &pos, const float angle, const unsigned intensity) {
	GYVector4 localPos = { pos.x, pos.y, pos.z, 1.0f };
	GYVector4 worldPos;

	gyMatPush();
	gyMatLoadIdentity();

	_entity->transform.apply();

	gyMatMultVec(&localPos, &worldPos, 1);
	gyMatPop();

	emitFromPoint(*(GYVector3*)&worldPos, angle, intensity);

}

bool ParticleEmitter::addToLevel(void) {
	_curArea->particleEmitters.push_back(this);
	return true;
}

bool ParticleEmitter::removeFromLevel(void) {
	containerEraseOnMatch(this, _curArea->particleEmitters);
	return true;
}


bool ParticleEmitter::canAddTo(Entity& /*entity*/) { return true; }
bool ParticleEmitter::load(FILE* /*file*/) { return true; }
bool ParticleEmitter::save(FILE* /*file*/) { return true; }
auto ParticleEmitter::getType(void) const -> TYPE { return Component::PARTICLE_EMITTER; }

}
