#include <ESGamma/device/elysian_visual_memory_device.hpp>
#include <libGyro/gyro_file_api.h>

namespace elysian {

bool VisualMemoryUnit::frameBufferReadPixel(uint8_t x, uint8_t y) const {
    if(x >= 48 || y >= 32) {
        errprintf("Attempting to read oustide of VMU framebuffer boundaries: <%d, %d>", x, y);
        return false;
    }
    const int byte  = x / 8;
    const int bit   = x % 8;
    return (bool)(_frameBuffer[y][byte]>>bit)&0x1;
}
void VisualMemoryUnit::frameBufferWritePixel(uint8_t x, uint8_t y, bool pixelVal) {
    if(x >= 48 || y >= 32) {
        errprintf("Attempting to write oustide of VMU framebuffer boundaries: <%d, %d>", x, y);
        return;
    }
    const uint8_t byte  = x / 8;
    const uint8_t bit   = x % 8;
    const uint8_t prevVal = _frameBuffer[y][byte];
    if(pixelVal) {
        _frameBuffer[y][byte] |= (0x1<<bit);
    } else {
        _frameBuffer[y][byte] &= ~(0x1<<bit);
    }
    if(_frameBuffer[y][byte] != prevVal) _fbChanged = true;
}

bool VisualMemoryUnit::readBlock(uint16_t blockNum, uint8_t* buffer) const {
    return _gyHandle? gyVisualMemoryDeviceBlockRead(_gyHandle, buffer, blockNum) : false;
}

bool VisualMemoryUnit::writeBlock(uint16_t blockNum, const uint8_t* buffer) {
    return _gyHandle? gyVisualMemoryDeviceBlockWrite(_gyHandle, buffer, blockNum) : false;
}
bool VisualMemoryUnit::beep(uint8_t pulseLength, uint8_t dutyCycle) {
    return _gyHandle? gyVisualMemoryDeviceBeep(_gyHandle, pulseLength, dutyCycle) : false;
}
bool VisualMemoryUnit::loadIcon(const char* filePath) {
    GYFile *fp = NULL;
    bool success = true;
    dbgprintf("VisualMemoryUnit[%d]::loadIcon(%s)", getPlayerIndex(), filePath);
    dbgpush();

    if (!gyFileOpen(filePath, "rb", &fp)) {
        Debug::printf(Debug::LOG_ERROR, "FAILURE!");
        success = false;
    } else {
        size_t bytesRead;
        gyFileRead(fp, _frameBuffer, 192, &bytesRead);
        if(bytesRead != 192) {
            errprintf("Could only read %d of %d bytes from file!", bytesRead, 192);
            success = false;
        }
        if(bytesRead > 0) _fbChanged = true;
    }
    dbgpop();
    return success;
}

bool VisualMemoryUnit::pollDevice(void) {
    _attached = gyVisualMemoryDevicePoll(_playerIndex, &_gyHandle);
    if(_attached && _gyHandle) {
        if(_fbChanged) {
            gyVisualMemoryDeviceDisplayImage(_gyHandle, (uint8_t*)_frameBuffer);
            _fbChanged = false;
        }
    }
    return _attached;
}

bool VisualMemoryUnit::connectToEVMU(const char* hostName, unsigned port) {
    _tcpSettings = (GYVisualMemoryTCPSettings*)malloc(sizeof(GYVisualMemoryTCPSettings));
    strcpy(_tcpSettings->hostname, hostName);
    _tcpSettings->port = port;
    return gyVisualMemoryDeviceRemoteConnect(_playerIndex, _tcpSettings);
}

bool VisualMemoryUnit::disconnectFromEVMU(void) {
    bool success = true;
    if(_gyHandle) success = gyVisualMemoryDeviceRemoteDisconnect(_gyHandle);
    free(_tcpSettings);
    _tcpSettings = nullptr;
    return success;
}

}
