#include <libGyro/gyro_visual_memory_api.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_net_api.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define GY_VISUAL_MEMORY_TCP_HOSTNAME_SIZE  200



static GYVisualMemoryInitParams _initParams     = {
    0
};
static GYVisualMemoryDevice**   _devices        = NULL;
static unsigned                 _deviceCount    = 0;

static void _createDeviceOnPort(unsigned port) {
    assert(!_devices[port]);
    _devices[port] = malloc(sizeof(GYVisualMemoryDevice));
    ++_deviceCount;
}

static void _destroyDeviceOnPort(unsigned port) {
    assert(_devices[port]);
    free(_devices[port]);
    _devices[port] = NULL;
    --_deviceCount;
}

static int _tcpSend(GYVisualMemoryDevice* dev, void* bytes, int size) {
    assert(dev && bytes);
    int bytesSent = 0;
    int success = 0;

    if(dev->tcpSocket) {
        if((bytesSent = gyNetTcpSocketSend(dev->tcpSocket, bytes, size)) != size) {
            _gyLog(GY_DEBUG_ERROR, "[GYRO_VISUAL_MEMORY_API] VMU[%d]: Could not send packet over TCP! DISCONNECTING! [Bytes Sent: %d/%d]", dev->port, bytesSent, size);
            gyVisualMemoryDeviceRemoteDisconnect(dev);
        } else success = 1;
    } else {
        _gyLog(GY_DEBUG_ERROR, "[GYRO_VISUAL_MEMORY_API] VMU[%d]: No open TCP socket! Cannot send packet!", dev->port);
    }

    return success;
}



int gyVisualMemoryInit(const GYVisualMemoryInitParams* params) {
    assert(params);
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_VISUAL_MEMORY_API]: Initializing with [%d] devices max!", params->maxDeviceCount);
    _gyPush();

    memcpy(&_initParams, params, sizeof(GYVisualMemoryInitParams));

    _deviceCount = 0;
    _devices = malloc(sizeof(GYVisualMemoryDevice*)*_initParams.maxDeviceCount);
    memset(_devices, 0, sizeof(GYVisualMemoryDevice*)*_initParams.maxDeviceCount);

    _gyPop(1);
    return 1;
}

int gyVisualMemoryUninit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_VISUAL_MEMORY_API]: Uninitializing");
    for(unsigned i = 0; i < _deviceCount; ++i) {
        //if(_devices[i] && _devices[i]->tcpSocket) DISCONNECT THE BITCH!?!?
        free(_devices[i]);
    }
    free(_devices);
    return 1;
}



int gyVisualMemoryDeviceBlockRead(GYVisualMemoryDevice* device, uint8_t* buffer, uint8_t block) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    frame.payload.blockRead.func            = MAPLE_FUNC_MEMCARD;
    frame.payload.blockRead.part            = 0;
    frame.payload.blockRead.block           = block;
    frame.payload.blockRead.phase           = 0;

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_READ, 0, 0, &frame, 4, &frameBytes);
    _tcpSend(device, (uint8_t*)&frame, frameBytes);

    return 1;
}

int gyVisualMemoryDeviceBlockWrite(GYVisualMemoryDevice* device, const uint8_t* buffer, uint8_t block) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    frame.payload.blockWrite.func           = MAPLE_FUNC_MEMCARD;
    frame.payload.blockWrite.part           = 0;
    frame.payload.blockWrite.block          = block;
    frame.payload.blockWrite.phase          = 0;
    memcpy(&frame.payload.blockWrite.data, buffer, VMU_MAPLE_WRITE_SIZE_MAX);

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_WRITE, 0, 0, &frame, 4, &frameBytes);
    _tcpSend(device, (uint8_t*)&frame, frameBytes);
    return 1;
}

int gyVisualMemoryDeviceDisplayImage(GYVisualMemoryDevice* device, uint8_t* buffer) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    frame.payload.blockWrite.func           = MAPLE_FUNC_LCD;
    frame.payload.blockWrite.part           = 0;
    frame.payload.blockWrite.block          = 0;
    frame.payload.blockWrite.phase          = 0;

    memcpy(&frame.payload.blockWrite.data, buffer, 192);

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_WRITE, MAPLE_ADDR_SUB(2)|MAPLE_ADDR_PORTB, MAPLE_DC_ADDR|MAPLE_ADDR_PORTB, &frame, 196, &frameBytes);
    _tcpSend(device, &frame, frameBytes);

    assert(device && buffer);
    return 1;
}
int gyVisualMemoryDeviceBeep(GYVisualMemoryDevice* device, uint8_t cycleLength, uint8_t pulseWidth) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    frame.payload.blockWrite.func           = MAPLE_FUNC_CLOCK;
    frame.payload.blockWrite.part           = 0;
    frame.payload.blockWrite.block          = 0;
    frame.payload.blockWrite.phase          = 0;

    frame.payload.blockWrite.data[0] = 0x0;
    frame.payload.blockWrite.data[1] = cycleLength;
    frame.payload.blockWrite.data[2] = 0x0;
    frame.payload.blockWrite.data[3] = pulseWidth;

    gyVisualMemoryMapleFrame(MAPLE_CMD_SET_COND, 0, 0, &frame, 8, &frameBytes);
    _tcpSend(device, &frame, frameBytes);

    assert(device);
    return 1;
}

int gyVisualMemoryDeviceRemoteConnect(unsigned port, const GYVisualMemoryTCPSettings* settings) {
    assert(settings && port < _initParams.maxDeviceCount);
    int success = 1;

    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_VISUAL_MEMORY_API]: Connecting slot[%d] to EVMU remote host.", port);
    _gyPush();

    if(!_devices[port]) {
        _createDeviceOnPort(port);
    }

    GYVisualMemoryDevice* dev = _devices[port];
    dev->tcpSettings = settings;
    assert(dev && settings);

    _gyLog(GY_DEBUG_VERBOSE, "Opening TCP connection to [host: %s, port: %d].", settings->hostname, settings->port);
    dev->tcpSocket = gyNetTcpSocketOpen(settings->hostname, settings->port);
    if(!dev->tcpSocket) {
        _gyPush();
        _gyLog(GY_DEBUG_ERROR, "Failed to create TCP socket!");
        success = 0;
        _gyPop(1);
    } else {
        if(!gyNetMonitoredConnectionsAdd(dev->tcpSocket)) {
            _gyLog(GY_DEBUG_ERROR, "Could not add socket to monitored connection list!");
            success = 0;
        } else {

            //Send connection type to remote immediately.
            char bytes[2] = {
                GY_VMU_TCP_COMMAND_CONNECTION_TYPE,
                GY_VMU_TCP_CONNECTION_DC
            };
            if(!_tcpSend(dev, (unsigned char*)bytes, 2)) success = 0;
        }

    }

    _gyPop(1);
    return success;
}


int gyVisualMemoryDeviceRemoteDisconnect(GYVisualMemoryDevice* device) {
    assert(device && device->tcpSettings);
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_VISUAL_MEMORY_API]: Disconnecting slot[%d] from EVMU remote host.", device->port);
    _gyPush();

    int success = 1;
    if(device->tcpSocket) {
        gyNetMonitoredConnectionsRemove(device->tcpSocket);
        success = gyNetTcpSocketClose(device->tcpSocket);
        if(!success) _gyLog(GY_DEBUG_ERROR, "Failed to close socket!");
        device->tcpSocket = NULL;
        device->tcpSettings = NULL;
    }

    _gyPop(1);
    return success;
}

int gyVisualMemoryMapleFrame(int8_t cmd, uint8_t destAddr, uint8_t srcAddr, GYMapleFrame* frame, uint32_t dataBytes, uint32_t* totalBytes) {
    int dataWords = dataBytes/MAPLE_WORD_SIZE;
    if(dataBytes % MAPLE_WORD_SIZE) {
        ++dataWords;
        dataBytes += MAPLE_WORD_SIZE;
    }
    if(dataWords > 256) {
        _gyLog(GY_DEBUG_ERROR, "[GYRO_VISUAL_MEMORY]: Data is too large for requested Maple frame: %d", dataBytes);
        *totalBytes = 0;
        return 0;
    } else {
        frame->header.cmd       = cmd;
        frame->header.sendAddr  = srcAddr;
        frame->header.recvAddr  = destAddr;
        frame->header.dataWords = dataWords;
        *totalBytes = dataBytes + MAPLE_HEADER_SIZE;
        return 1;
    }
}


int gyVisualMemoryDevicePoll(unsigned port, GYVisualMemoryDevice** device) {
    GYMapleFrame mapleFrame;
    int recvBytes;

    assert(device && port < _initParams.maxDeviceCount);

    GYVisualMemoryDevice* dev = _devices[port];
    *device = _devices[port];
    if(dev && dev->tcpSocket) {


        if(gyNetTcpSocketReady(dev->tcpSocket)) {

            if((recvBytes = gyNetTcpSocketRecv(dev->tcpSocket, &mapleFrame, MAPLE_FRAME_SIZE_MAX)) > 0) {

                _gyLog(GY_DEBUG_VERBOSE, "[GYRO_VMU_API]: Maple Recv (cmd: %d)", mapleFrame.header.cmd);
                _gyPush();
                if(mapleFrame.header.cmd == MAPLE_CMD_DATA_XFER) {
                    _gyLog(GY_DEBUG_VERBOSE, "TRANSFER COMPLETE!!!");

                }
                _gyPop(1);


            } else {
                gyVisualMemoryDeviceRemoteDisconnect(dev);

            }

        }

        return 1;


    }
    return 0;
}
