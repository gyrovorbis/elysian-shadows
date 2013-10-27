#ifndef ELYSIAN_WEATHER_SYSTEM_HPP
#define ELYSIAN_WEATHER_SYSTEM_HPP
#include <vector>
#include "elysian_system.hpp"
#include "gyro.h"

namespace elysian {
// Precipitation type.
enum {
	PT_NONE = 0x00,
	PT_RAIN = 0x01,
	PT_SNOW = 0x02,
	PT_HAIL = 0x04
};

struct WeatherParticle : public System {
	signed int	type;
	GYVector2	pos;
	GYVector2	dir;
	GYVector2	velocity;
	float		lifeTime;
	float		lifeSpan;
	float		length;
	float		opacity;
};

struct MistParticle {
	GYVector2	pos;
	GYVector2	dir;
	float		lifeTime;
	float		lifeSpan;
	float		length;
};

class WeatherSystem {
public:
	WeatherSystem(void);
	void destroy(void);

	void update(float dt);
	void render(void);

	void generateParticles(void);
	void resetParticles(void);
	void resetParticles(int precipType);

	// Get tone modifier.
	GYColor4 getToneModifier(void) const;

	int	getPrecipType(void) const		{ return _precipType; }
	void setPrecipType(int type)		{ _precipType = type; }

	int getMaxParticles(void) const		{ return _maxParticles; }
	void setMaxParticles(int max)		{ _maxParticles = max; }

	float getRainMass(void) const		{ return _rainMass; }
	void setRainMass(float mass)		{ _rainMass = mass; }

	float getSnowMass(void) const		{ return _rainMass; }
	void setSnowMass(float mass)		{ _snowMass = mass; }

	float getHailMass(void) const		{ return _rainMass; }
	void setHailMass(float mass)		{ _hailMass = mass; }

	bool isMisty(void) const			{ return _isMisty; }
	void setMisty(bool misty)			{ _isMisty = misty; }

private:
	void _renderRainParticles(void);
	void _renderSnowParticles(void);
	void _renderHailParticles(void);

	int _precipType;
	int _maxParticles;
	float _rainMass;
	float _snowMass;
	float _hailMass;
	std::vector<WeatherParticle> _particles;

	bool _isMisty;

	// Texture.
	GYTexture _snowTexture;
};

} // Namespace elysian.

#endif // ELYSIAN_WEATHER_SYSTEM_HPP
