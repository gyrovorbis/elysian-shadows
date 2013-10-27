#include <GL/gl.h>
#include "elysian_weather_system.hpp"
#include "elysian_camera_system.hpp"
namespace elysian {

WeatherSystem::WeatherSystem(void) :
	_precipType(PT_NONE),
	_maxParticles(128),
	_rainMass(1),
	_snowMass(1),
	_hailMass(1),
	_isMisty(false) {

	// Load in some texture.
}

void WeatherSystem::destroy(void) {
	// Do we need to remove a texture?
	delete this;
}

void WeatherSystem::update(float dt) {
	// Update particles.
	generateParticles();

	std::vector<WeatherParticle>::iterator it = _particles.begin();
	while(it != _particles.end()) {
		WeatherParticle& particle = *it;

		// TODO: Calculate position based on delta time.

		particle.lifeTime += dt;

		if(particle.lifeTime >= particle.lifeSpan) {
			it = _particles.erase(it);
		} else {
			it++;
		}
	}
}

void WeatherSystem::generateParticles(void) {
	if(_precipType == PT_NONE) {
		return;
	}

	int toGen = _maxParticles - _particles.size();

	// TODO: Get camera position here... -- Come back later.


}

void WeatherSystem::resetParticles(void) {
	_particles.clear();
}

void WeatherSystem::resetParticles(int precipType) {
	std::vector<WeatherParticle>::iterator it = _particles.begin();
	while(it != _particles.end()) {
		WeatherParticle& particle = *it;

		if(precipType & particle.type) {
			it = _particles.erase(it);
		} else {
			it++;
		}
	}
}

void WeatherSystem::render(void) {
	_renderRainParticles();
	_renderSnowParticles();
	_renderHailParticles();
}

void WeatherSystem::_renderRainParticles(void) {
	// TODO: Bring this inline with gyro_video or whatever is used.
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
		std::vector<WeatherParticle>::iterator it = _particles.begin();
		while(it != _particles.end()) {
			WeatherParticle& particle = *it;

			if(particle.type == PT_RAIN) {
				// TODO: Colour, Size, direction, position.
			}
			it++;
		}

	glEnd();
}

void WeatherSystem::_renderSnowParticles(void) {
	// TODO
}

void WeatherSystem::_renderHailParticles(void) {
	// TODO
}

GYColor4 WeatherSystem::getToneModifier(void) const {
	if(_precipType & PT_RAIN || _precipType & PT_SNOW || _precipType & PT_HAIL) {
		// TODO: Return colour (0.8f, 0.8f, 0.8f)
	}
	// TODO: return colour white?
}

} // Namespace elysian.
