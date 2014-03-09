#include "elysian_particle_system.hpp"
#include "elysian_camera_system.hpp"


namespace elysian {

void ParticleEmitterInternals::emitFrom(const GYVector3& pos, const float angle, const unsigned intensity) {
	Particle* part;
	unsigned curIndex = 0;
	float relAngle;
	float sinTheta, cosTheta;

	auto nextFreeIndex = [&](unsigned& index) {
		for(; curIndex < _poolSize; ++curIndex) {
			if(!_pool[curIndex].active) {
				index = curIndex;
				return true;
			}
		}
		return false;
	};

	if(_activeCount >= _poolSize) return; //shit's full

	for(unsigned i = 0; i < intensity; ++i) {
		unsigned newIndex;

		if(!nextFreeIndex(newIndex)) break;
		part = &_pool[newIndex];

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

		part->active = true;

		++_activeCount;
	}


}

void ParticleEmitterInternals::update(const CameraState*const cam) {
	Particle* part;


	//Check for periodic emission!

	//loop unroll my bitch ass
	if(_activeCount == 0) return;
	for(unsigned i = 0; i < _poolSize; ++i) {
		if(!_pool[i].active) continue;

		part = &_pool[i];

		gyVec3AddAccum(&part->position, &part->velocity);

		if(!cam->isWithinScreenRegion(part->position.x, part->position.y, part->size, part->size)) {
			part->active = false;
			--_activeCount;
			continue;
		}

		gyVec3AddAccum(&part->velocity, &_attrib->gravity);

		if(--part->life == 0) {
			part->active = false;
			--_activeCount;
		}

	}
}

void ParticleEmitterInternals::render(void) {
	Particle* part;

	//Probably could be optimized as shit to not bother using
	//internal matrix stack!

	if(_activeCount == 0) return;

	gyVidTexBind(nullptr);
	gyVidMaterialBind(nullptr);

	//loop unroll my bitch ass
	for(unsigned i = 0; i < _poolSize; ++i) {
		if(!_pool[i].active) continue;
		part = &_pool[i];

		gyMatPush();
		gyVidQuadListBegin(&part->color);

		gyMatTranslateScale(part->position.x, part->position.y, part->position.z, part->size, part->size, part->size);
		gyVidRect(0);

		gyVidQuadListEnd();
		gyMatPop();

	}
}

ParticleEmitterAttrib* ParticleEmitterInternals::getAttrib(void) const {
	return _attrib;
}

void ParticleEmitterInternals::allocatePool(const unsigned size) {
	delete[] _pool;

	_pool = new Particle[size];
	_poolSize = size;
	_activeCount = 0;
}

void ParticleEmitterInternals::setAttrib(ParticleEmitterAttrib * const attrib) {
	_attrib = attrib;
}

ParticleEmitterInternals::~ParticleEmitterInternals(void) {
	delete[] _pool;
}

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
