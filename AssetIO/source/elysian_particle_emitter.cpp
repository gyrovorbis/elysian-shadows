#include "elysian_particle_emitter.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"
#include "elysian_entity.hpp"

namespace elysian {

ParticleEmitter::ParticleEmitter(Entity& entity): Component(entity) {}

void ParticleEmitter::emitFromEntity(const GYVector3 &pos, const float angle, const unsigned intensity) {
	GYVector4 localPos = { pos.x, pos.y, pos.z, 0.0f };
	GYVector4 worldPos;

	gyMatPush();
	gyMatLoadIdentity();
	gyMatInstance(&_entity->transform);
	gyMatMultVec(&localPos, &worldPos, 1);
	gyMatPop();

	emitFromEntity(*(GYVector3*)&worldPos, angle, intensity);

}

bool ParticleEmitter::addToLevel(void) {
	_curArea->particleEmitters.push_back(this);
	return true;
}

bool ParticleEmitter::removeFromLevel(void) {
	containerEraseOnMatch(this, _curArea->particleEmitters);
	return true;
}


bool ParticleEmitter::canAddTo(Entity& entity) { return true; }
bool ParticleEmitter::load(std::ifstream& stream) { return true; }
bool ParticleEmitter::save(std::ofstream& stream) { return true; }
auto ParticleEmitter::getType(void) const -> TYPE { return Component::PARTICLE_EMITTER; }

}
