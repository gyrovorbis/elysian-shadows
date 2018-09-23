#ifndef ELYSIAN_VISUAL_MEMORY_DEVICE_HPP
#define ELYSIAN_VISUAL_MEMORY_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>
#include <libGyro/gyro_visual_memory_api.h>

namespace elysian {
    class VisualMemoryUnit;

    class VisualMemoryUnitEvent: public Event {
     private:
        VisualMemoryUnit* _vmu;
    public:
                            VisualMemoryUnitEvent(EventType type, VisualMemoryUnit* vmu);
        VisualMemoryUnit*   getVMU(void) const;
    };


    class VisualMemoryUnit: public PlayerIODevice {
    public:
        enum VMU_TYPE {
            VMU_NONE,
            VMU_PHYSICAL,
            VMU_VIRTUAL
        };
    private:
        uint8_t                     _frameBuffer[32][6]     = { { 0 } };
        GYVisualMemoryTCPSettings*  _tcpSettings            = nullptr;
        GYVisualMemoryDevice*       _gyHandle               = nullptr;
        VMU_TYPE                    _vmuType                = VMU_NONE;
        bool                        _fbChanged              = false;
    public:
        bool                        readBlock(uint16_t blockNum, uint8_t* buffer) const;
        bool                        writeBlock(uint16_t blockNum, const uint8_t* buffer);
        bool                        beep(uint8_t pulseLength, uint8_t dutyCycle);
        bool                        setFramebuffer(const char* buff);
        bool                        loadIcon(const char* filePath);
        bool                        frameBufferReadPixel(uint8_t x, uint8_t y) const;
        void                        frameBufferWritePixel(uint8_t x, uint8_t y, bool pixelVal);
        bool                        pollDevice(void);
        bool                        isRemoteConnected(void) const;
        VMU_TYPE                    getVmuType(void) const;
        bool                        connectToEVMU(const char* hostName, unsigned port);
        bool                        disconnectFromEVMU(void);
    };


    //====== VMU INLINEZ =======

    inline VisualMemoryUnit* VisualMemoryUnitEvent::getVMU(void) const { return _vmu; }
    inline VisualMemoryUnitEvent::VisualMemoryUnitEvent(EventType type, VisualMemoryUnit* vmu):
        Event(type),
        _vmu(vmu)
    {}
}

#endif // ELYSIAN_VISUAL_MEMORY_DEVICE_HPP
