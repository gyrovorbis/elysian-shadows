#include "elysian_particle_emitter.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"

namespace elysian {

ParticleEmitter::ParticleEmitter(Entity& entity): Component(entity) {}

void ParticleEmitter::emitFromEntity(const GYVector3 &pos, const float angle, const unsigned intensity) {
	//GYVector3 worldPos = ;

	//emitFromPoint();

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
