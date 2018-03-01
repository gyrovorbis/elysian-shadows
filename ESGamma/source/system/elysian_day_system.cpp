#include "elysian_day_system.hpp"
#include "elysian_timer_system.hpp"
#include <crtp/elysian_light_source.hpp>
#include "elysian_debug.hpp"
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_entity.hpp>
#include <elysian_event_system.hpp>

namespace elysian {

void DaySystem::update(void) {
    TimerSystem* timerSys = getTimerSys();

    if(_curArea->getUseSun() && !_sunActive) onAreaEnter();
    else if(!_curArea->getUseSun() && _sunActive) onAreaExit();

    _curSecs += timerSys->getDeltaTime() * _timeSpeed;

    if(_curSecs >= _secsPerDay) {
        _curSecs -= _secsPerDay;
        ++_days;
    }

    _time       =   _curSecs/_secsPerDay;
    _minutes    =   _curSecs/60.0f;
    _hours      =   _minutes/60.0f;
    _minutes    %=  60;

    const float theta = _time*2.0f*M_PI - 0.5f*M_PI;

    _updateTimeInterval();

    GYVector4 dir;

    gyMatPush();
    gyMatLoadIdentity();
    gyMatRotate(theta, _axis.x, _axis.y, _axis.z);
    gyMatMultVec(&_baseDir, &dir, 1);
    gyMatPop();

    _sunLightSrc->setDirection(*(GYVector3*)&dir);
}

void DaySystem::onAreaEnter(void) {
    if(_curArea->getUseSun() && !_sunActive && _sunEntity->getArea() != _curArea) {
        getCurrentArea()->addEntity(_sunEntity);
        _sunActive = true;
    }
}

void DaySystem::onAreaExit(void) {
    if(_sunActive) {
        getCurrentArea()->removeEntity(_sunEntity);
        _sunActive = false;
    }
}

DaySystem::DaySystem(void) {
    GYVector3 dir3 = { _baseDir.x, _baseDir.y, _baseDir.z };
    _sunLightAttrib = std::make_shared<LightAttrib>();
    _sunLightAttrib->setAmbientColor(GYVector4{0.5f, 0.5f, 0.5f, 1.0f});
    _sunLightAttrib->setDiffuseColor(GYVector4{0.5f, 0.5f, 0.5f, 1.0f});
    _sunLightAttrib->setSpecularColor(GYVector4{1.0f, 1.0f, 1.0f, 1.0f});

    _sunEntity = new Entity();
    _sunEntity->setName("_Sun");
    _sunLightSrc = _sunEntity->addLightSource();
    _sunLightSrc->setLightType(GY_LIGHT_DIRECTIONAL);
    _sunLightSrc->setDirection(dir3);
    _sunLightSrc->setShadowCaster(true);
    _sunLightSrc->setAttrib(_sunLightAttrib);

    System::getEventSys()->addEventObserver(this, EVENT_AREA_ENTER);
    System::getEventSys()->addEventObserver(this, EVENT_AREA_EXIT);
}

DaySystem::~DaySystem(void) {
    delete _sunEntity;
}


void DaySystem::_updateTimeInterval(void) {
    if(_hours < TIME_HOUR_DAWN) {
        if(_timeInterval == TIME_INTERVAL_NIGHT_PM) System::getEventSys()->broadcastEvent(Event(EVENT_GAME_TIME_MIDNIGHT));
        _timeInterval = TIME_INTERVAL_NIGHT_AM;

    } else if(_hours >= TIME_HOUR_DAWN && _hours < TIME_HOUR_NOON) {
        if(_timeInterval == TIME_INTERVAL_NIGHT_AM) System::getEventSys()->broadcastEvent(Event(EVENT_GAME_TIME_DAWN));
        _timeInterval = TIME_INTERVAL_DAY_AM;

    } else if(_hours >= TIME_HOUR_NOON && _hours < TIME_HOUR_DUSK) {
        if(_timeInterval == TIME_INTERVAL_DAY_AM) System::getEventSys()->broadcastEvent(Event(EVENT_GAME_TIME_NOON));
        _timeInterval = TIME_INTERVAL_DAY_PM;

    } else if(_hours >= TIME_HOUR_DUSK && _hours < TIME_HOUR_MIDNIGHT) {
        if(_timeInterval == TIME_INTERVAL_DAY_PM) System::getEventSys()->broadcastEvent(Event(EVENT_GAME_TIME_DUSK));
        _timeInterval = TIME_INTERVAL_NIGHT_PM;
    }
}

void DaySystem::onObserveEvent(Event *event, EventTarget*) {
    switch(event->getEventType()) {
    case EVENT_AREA_ENTER:
        onAreaEnter();
        break;
    case EVENT_AREA_EXIT:
        onAreaExit();
        break;
    default:break;
    }
}


}
