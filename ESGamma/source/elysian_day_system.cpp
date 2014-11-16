#include "elysian_day_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_light_source.hpp"
#include "elysian_debug.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_area.hpp", "area.h")
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")
namespace elysian {

void DaySystem::update(void) {

	TimerSystem* timerSys = getTimerSys();

	_curSecs += timerSys->getDeltaTime() * _timeSpeed;

	if(_curSecs >= _secsPerDay) {
		_curSecs -= _secsPerDay;
        //Debug::printf(Debug::VERBOSE, "DAY WENT BY");
		++_days;
	}

	_time = _curSecs/_secsPerDay;
	_minutes = _curSecs/60.0f;
	_hours = _minutes/60.0f;
	_minutes %= 60;

    const float theta = _time*2.0f*M_PI - 0.5f*M_PI;

	GYVector4 dir;

	gyMatPush();
	gyMatLoadIdentity();
	gyMatRotate(theta, _axis.x, _axis.y, _axis.z);
	gyMatMultVec(&_baseDir, &dir, 1);
	gyMatPop();

	//update sun's direction vector
	gyVec3Normalize((GYVector3*)&dir);
	_sunLightSrc->setDirection(*(GYVector3*)&dir);
}


void DaySystem::setSunEnabled(const bool val) {
	if(_sunEnabled == val) return;

	if(val) {
		onAreaEnter();
	} else {
		onAreaExit();
	}

	_sunEnabled = val;
}


void DaySystem::onAreaEnter(void) {
	getCurArea()->addEntity(_sunEntity);
	_sunActive = true;
}

void DaySystem::onAreaExit(void) {
	getCurArea()->removeEntity(_sunEntity);
	_sunActive = false;
}

DaySystem::DaySystem(void) {
	GYVector3 dir3 = { _baseDir.x, _baseDir.y, _baseDir.z };
	_sunLightAttrib = new LightAttrib;
	_sunLightAttrib->setAmbientColor(GYVector4{0.5f, 0.5f, 0.5f, 1.0f});
	_sunLightAttrib->setDiffuseColor(GYVector4{0.5f, 0.5f, 0.5f, 1.0f});
	_sunLightAttrib->setSpecularColor(GYVector4{1.0f, 1.0f, 1.0f, 1.0f});


	_sunEntity = new Entity();
	_sunEntity->setName("Sun");
	_sunLightSrc = _sunEntity->addLightSource();
	_sunLightSrc->setLightType(GY_LIGHT_DIRECTIONAL);
	_sunLightSrc->setDirection(dir3);
	_sunLightSrc->setShadowCaster(true);
	_sunLightSrc->setAttrib(_sunLightAttrib);
}

DaySystem::~DaySystem(void) {
	delete _sunEntity;
	delete _sunLightAttrib;
}


}
