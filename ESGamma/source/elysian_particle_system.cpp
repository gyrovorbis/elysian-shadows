#include "elysian_particle_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_debug.hpp"


namespace elysian {

void ParticleEmitterInternals::emitFrom(const GYVector3& pos, const float angle, const unsigned intensity) {
	Particle* part;
	float relAngle;
	float sinTheta, cosTheta;

    if(_pool.isFull()) {
        Debug::printf(Debug::VERBOSE, "FULL");
    }


    if(_pool.isFull()) return;

    if(_pool.size() == 0)
            Debug::printf(Debug::VERBOSE, "EMPTY");


	for(unsigned i = 0; i < intensity; ++i) {

        part = &_pool.claimFreeEntry();

		part->position.x = pos.x + ((rand() % 6) - 3);
		part->position.y = pos.y + ((rand() % 6) - 3);
		part->life = _attrib->maxLife - (rand() % ((int)((float)intensity/20.0f) + 1));

		if(part->life < 0) part->life = 0;
		part->size = _attrib->size;
		if(part->size < 1) part->size = 1;
		if(part->size > 200) part->size = 200;

		//doesn't center the angle!
		relAngle = angle;// + rand()%55;

		gySinCos(angle, &sinTheta, &cosTheta);

		part->velocity.x = cosTheta * (float)intensity/(float)(rand()%14 + 10);
		part->velocity.y = sinTheta * (float)intensity/(float)(rand()%14 + 10);
		part->color = _attrib->color;
		part->color.r += (rand()%6)-3;
		part->color.g += (rand()%6)-3;
		part->color.b += (rand()%6)-3;


    }

}

void ParticleEmitterInternals::update(const CameraState*const cam) {

	//Check for periodic emission!

    for(auto it = _pool.begin(); it < _pool.end(); ++it) {

        gyVec3AddAccum(&it->position, &it->velocity);

        if(!cam->isWithinScreenRegion(it->position.x, it->position.y, it->size, it->size)) {
            _pool.remove(it);
			continue;
		}

        gyVec3AddAccum(&it->velocity, &_attrib->gravity);

        if(--it->life == 0) {
            _pool.remove(it);
        }
	}
}

void ParticleEmitterInternals::render(void) {

	//Probably could be optimized as shit to not bother using
	//internal matrix stack!

	gyVidTexBind(nullptr);
	gyVidMaterialBind(nullptr);

	//loop unroll my bitch ass

    for(auto it = _pool.begin(); it < _pool.end(); ++it) {

		gyMatPush();
        gyVidQuadListBegin(&it->color);

        gyMatTranslateScale(it->position.x, it->position.y, it->position.z, it->size, it->size, it->size);
		gyVidRect(0);

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

ParticleSystem::ParticleSystem(void) {
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].setAttrib(new ParticleEmitterAttrib);
		_emitters[i].allocatePool(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
	}
}

void ParticleSystem::emitFrom(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity) {
	_emitters[index].emitFrom(pos, angle, intensity);
}

ParticleEmitterAttrib* ParticleSystem::getAttrib(const unsigned index) const {
	return _emitters[index].getAttrib();
}

void ParticleSystem::resizePool(const unsigned index, const unsigned size) {
	_emitters[index].allocatePool(size);
}

void ParticleSystem::update(void) {
	CameraState* state = getCameraSys()->getCurrentState();

	//Update internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].update(state);
	}

	//update entity emitters
}


void ParticleSystem::render(void) {

	//render internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].render();
	}

	//render entity emitters

}


}
