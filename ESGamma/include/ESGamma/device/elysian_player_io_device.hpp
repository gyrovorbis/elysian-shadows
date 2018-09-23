#ifndef ELYSIAN_PLAYER_IO_DEVICE_HPP
#define ELYSIAN_PLAYER_IO_DEVICE_HPP

#include <ESGamma/system/elysian_event_system.hpp>

namespace elysian {

    //Base event for all changes of input device state
    class InputEvent: public Event {
        int     _playerIndex    = -1;
    public:
                InputEvent(EventType eventType, int playerIndex);
        int     getPlayerIndex(void) const;
    };

    //Base event for all attach/detach/overall device state changes
    class InputDeviceEvent: public InputEvent {
    public:
        enum DEVICE_EVENT_TYPE {
            ATTACHED,
            REMOVED,
            STATE_CHANGED,
            UNKNOWN
        };
    private:
        DEVICE_EVENT_TYPE    _deviceEventType   = UNKNOWN;
    public:
                            InputDeviceEvent(EventType eventType, int playerIndex, DEVICE_EVENT_TYPE devType);
        DEVICE_EVENT_TYPE   getDeviceEventType(void) const;
    };

    class PlayerIODevice    {
    protected:
        bool        _attached       = false;
        unsigned    _playerIndex;

    public:
        bool        isAttached(void) const;
        unsigned    getPlayerIndex(void) const;
        void        setPlayerIndex(unsigned index);

    };


    //===== EVENT INLINEZ =======

    //InputEvent
    inline InputEvent::InputEvent(EventType eventType, int playerIndex):
        Event(eventType), _playerIndex(playerIndex) {}
    inline int InputEvent::getPlayerIndex(void) const { return _playerIndex; }

    //InputDeviceEvent
    inline InputDeviceEvent::InputDeviceEvent(EventType eventType, int playerIndex, DEVICE_EVENT_TYPE devType):
        InputEvent(eventType, playerIndex), _deviceEventType(devType) {}
    inline auto InputDeviceEvent::getDeviceEventType(void) const -> DEVICE_EVENT_TYPE { return _deviceEventType; }

    //PlayerIODevice
    inline bool PlayerIODevice::isAttached(void) const { return _attached; }
    inline void PlayerIODevice::setPlayerIndex(unsigned index) { _playerIndex = index; }
    inline unsigned PlayerIODevice::getPlayerIndex(void) const { return _playerIndex; }


}

#endif // ELYSIAN_PLAYER_IO_DEVICE_HPP
