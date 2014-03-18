#include "elysian_particle_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_area.hpp"

namespace elysian {

ParticleEmitterAttrib::~ParticleEmitterAttrib(void) {
	delete _material;
}

void ParticleEmitterInternals::emitFromPoint(const GYVector3& pos, const float angle, const unsigned intensity) {
	Particle* part;
	float relAngle;
	float sinTheta, cosTheta;


    if(_pool.isFull()) return;

	for(unsigned i = 0; i < intensity; ++i) {

        part = &_pool.claimFreeEntry();

		part->position.x = pos.x + ((rand() % 6) - 3);
		part->position.y = pos.y + ((rand() % 6) - 3);
		part->position.z = pos.z;
		part->life = _attrib->_maxLife - (rand() % ((int)((float)intensity/20.0f) + 1));

		if(_attrib->_minFrame != _attrib->_maxFrame) { //use range
			part->frame = rand()%(_attrib->_maxFrame-_attrib->_minFrame)+_attrib->_minFrame;
		} else { //use static
			part->frame = _attrib->getFrame();
		}

		if(part->life < 0) part->life = 0;
		part->size = _attrib->_size;
		if(part->size < 1) part->size = 1;
		if(part->size > 200) part->size = 200;

		//doesn't center the angle!
		relAngle = angle;// + (rand()%5-10)/10.0f;
		part->orientation = relAngle;

		gySinCos(angle, &sinTheta, &cosTheta);

		part->rotation = -cosTheta*(intensity/100.0f); //tweak me better!

		part->velocity.x = cosTheta * (float)intensity/(float)(rand()%14 + 10);
		part->velocity.y = sinTheta * (float)intensity/(float)(rand()%14 + 10);
		part->color = _attrib->getColor();
		part->color.r += (rand()%6)-3;
		part->color.g += (rand()%6)-3;
		part->color.b += (rand()%6)-3;


    }

}

void ParticleEmitterInternals::update(const CameraState*const cam) {

	//Check for periodic emission!

    for(auto it = _pool.begin(); it < _pool.end(); ++it) {

		//integrate
        gyVec3AddAccum(&it->position, &it->velocity);

        if(!cam->isWithinScreenRegion(it->position.x, it->position.y, it->size, it->size)) {
            _pool.remove(it);
			continue;
		}

		it->orientation += it->rotation;
		it->rotation *= _attrib->_angularDamping;

		gyVec3AddAccum(&it->velocity, &_attrib->_acceleration);

        if(--it->life == 0) {
            _pool.remove(it);
			continue;
        }

		if(_attrib->_fadeOut) it->color.a = 255.0f*((float)_attrib->_maxLife/(float)it->life);
	}
}

void ParticleEmitterInternals::render(void) {

	//Probably could be optimized as shit to not bother using
	//internal matrix stack!

	gyVidTexBind(_attrib->getTexture());
	gyVidMaterialBind(nullptr);
	gyVidAlphaLevel(GY_ALPHA_FULL);

	//loop unroll my bitch ass

    for(auto it = _pool.begin(); it < _pool.end(); ++it) {

		gyMatPush();
        gyVidQuadListBegin(&it->color);

		gyMatTranslate(it->position.x, it->position.y, it->position.z);
		gyMatScale(it->size, it->size, 1.0f);
		gyMatRotateZ(it->orientation);

		gyVidRect(it->frame);

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

void ParticleSystem::emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity) {
	_emitters[index].emitFromPoint(pos, angle, intensity);
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
	auto emitters = getCurArea()->particleEmitters;
	for(auto it = emitters.begin(); it != emitters.end(); ++it) {
//		it->update();
	}
}


void ParticleSystem::render(void) {

	//render internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].render();
	}

	//render entity emitters (no, entities do this)

}


}
