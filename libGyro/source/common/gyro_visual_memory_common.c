#include <gyro_visual_memory_api.h>
#include <gyro_system_api.h>
#include <gyro_net_api.h>
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

static int _tcpSend(GYVisualMemoryDevice* dev, const unsigned char* bytes, int size) {
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
    uint8_t*        data                    = &frame[MAPLE_FRAME_DATA_OFFSET];
    data[MAPLE_FRAME_DATA_FUNC_OFFSET]      = MAPLE_FUNC_MEMCARD;
    data[MAPLE_FRAME_DATA_PARTITION_OFFSET] = 0;
    data[MAPLE_FRAME_DATA_BLOCK_OFFSET]     = block;
    data[MAPLE_FRAME_DATA_PHASE_OFFSET]     = 0;

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_READ, 0, 0, frame, 4, &frameBytes);
    _tcpSend(device, frame, frameBytes);

    return 1;
}

int gyVisualMemoryDeviceBlockWrite(GYVisualMemoryDevice* device, const uint8_t* buffer, uint8_t block) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    uint8_t*        data                    = &frame[MAPLE_FRAME_DATA_OFFSET];
    data[MAPLE_FRAME_DATA_FUNC_OFFSET]      = MAPLE_FUNC_MEMCARD;
    data[MAPLE_FRAME_DATA_PARTITION_OFFSET] = 0;
    data[MAPLE_FRAME_DATA_BLOCK_OFFSET]     = block;
    data[MAPLE_FRAME_DATA_PHASE_OFFSET]     = 0;

    memcpy(&data[MAPLE_FRAME_DATA_WRITE_OFFSET], buffer, VMU_MAPLE_WRITE_SIZE_MAX);

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_WRITE, 0, 0, frame, 4, &frameBytes);
    _tcpSend(device, frame, frameBytes);
    return 1;
}

int gyVisualMemoryDeviceDisplayImage(GYVisualMemoryDevice* device, uint8_t* buffer) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    uint8_t*        data                    = &frame[MAPLE_FRAME_DATA_OFFSET];
    data[MAPLE_FRAME_DATA_FUNC_OFFSET]      = MAPLE_FUNC_LCD;
    data[MAPLE_FRAME_DATA_PARTITION_OFFSET] = 0;
    data[MAPLE_FRAME_DATA_BLOCK_OFFSET]     = 0;
    data[MAPLE_FRAME_DATA_PHASE_OFFSET]     = 0;

    memcpy(&data[MAPLE_FRAME_DATA_WRITE_OFFSET], buffer, 192);

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_WRITE, MAPLE_ADDR_SUB(2)|MAPLE_ADDR_PORTB, MAPLE_DC_ADDR|MAPLE_ADDR_PORTB, frame, 196, &frameBytes);
    _tcpSend(device, frame, frameBytes);

    assert(device && buffer);
    return 1;
}
int gyVisualMemoryDeviceBeep(GYVisualMemoryDevice* device, uint8_t cycleLength, uint8_t pulseWidth) {
    GYMapleFrame    frame;
    uint32_t        frameBytes;
    uint8_t*        data                    = &frame[MAPLE_FRAME_DATA_OFFSET];
    data[MAPLE_FRAME_DATA_FUNC_OFFSET]      = MAPLE_FUNC_CLOCK;
    data[MAPLE_FRAME_DATA_PARTITION_OFFSET] = 0;
    data[MAPLE_FRAME_DATA_BLOCK_OFFSET]     = 0;
    data[MAPLE_FRAME_DATA_PHASE_OFFSET]     = 0;

    uint8_t* writeData = data[MAPLE_FRAME_DATA_WRITE_OFFSET];
    writeData[0] = 0x0;
    writeData[1] = cycleLength;
    writeData[2] = 0x0;
    writeData[3] = pulseWidth;

    gyVisualMemoryMapleFrame(MAPLE_CMD_BLOCK_WRITE, 0, 0, frame, 8, &frameBytes);
    _tcpSend(device, frame, frameBytes);

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
        success = gyNetTcpSocketClose(device->tcpSocket);
        if(!success) _gyLog(GY_DEBUG_ERROR, "Failed to close socket!");
        device->tcpSocket = NULL;
        device->tcpSettings = NULL;
    }

    _gyPop(1);
    return success;
}

int gyVisualMemoryMapleFrame(uint8_t cmd, uint8_t destAddr, uint8_t srcAddr, void*data, uint32_t dataBytes, uint32_t* totalBytes) {
    const int dataWords = dataBytes/MAPLE_WORD_SIZE;
    if(dataWords > 256) {
        _gyLog(GY_DEBUG_ERROR, "[GYRO_VISUAL_MEMORY]: Data is too large for requested Maple frame: %d", dataBytes);
        *totalBytes = 0;
        return 0;
    } else {
        uint32_t* dataPtr = data;
        dataPtr[0] = MAPLE_HEADER(cmd, destAddr, srcAddr, dataWords);
        *totalBytes = dataBytes + MAPLE_HEADER_SIZE;

        return 1;
    }
}


char lcd_data_image[1921] = {
                0x00, 0x07, 0xFF, 0xFF, 0xF0, 0x00,
                0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
                0x00, 0x10, 0x3E, 0x00, 0x04, 0x00,
                0x00, 0x20, 0x55, 0x80, 0x02, 0x00,
                0x00, 0x21, 0xAA, 0xFF, 0xC1, 0x00,
                0x00, 0x23, 0xD5, 0x55, 0xF8, 0x80,
                0x00, 0x27, 0xEA, 0xAB, 0xFC, 0x80,
                0x00, 0x27, 0xFF, 0xFF, 0xFE, 0x80,
                0x00, 0x2F, 0xFF, 0xFF, 0xFE, 0x80,
                0x00, 0x2F, 0xFF, 0xFF, 0xFE, 0x80,
                0x00, 0x24, 0xFF, 0xFF, 0xF2, 0x80,
                0x00, 0x24, 0x07, 0xFF, 0xE4, 0x80,
                0x00, 0x26, 0x00, 0x00, 0x04, 0x80,
                0x00, 0x12, 0x7F, 0xFF, 0xF9, 0x00,
                0x00, 0x11, 0x80, 0x00, 0x01, 0x00,
                0x00, 0x10, 0x00, 0x00, 0x01, 0x00,
                0x00, 0x10, 0x00, 0x00, 0x01, 0x00,
                0x00, 0x10, 0x78, 0x50, 0xF1, 0x00,
                0x00, 0x08, 0x84, 0x01, 0x09, 0x00,
                0x00, 0x08, 0xB4, 0x01, 0x69, 0x00,
                0x00, 0x04, 0x84, 0x01, 0x09, 0x00,
                0x00, 0x02, 0x78, 0x00, 0xF2, 0x00,
                0x00, 0x01, 0x00, 0x00, 0x02, 0x00,
                0x00, 0x01, 0x00, 0x00, 0x02, 0x00,
                0x00, 0x01, 0x80, 0x00, 0x04, 0x00,
                0x00, 0x00, 0x40, 0x03, 0x84, 0x00,
                0x00, 0x00, 0x40, 0x1E, 0x08, 0x00,
                0x00, 0x03, 0xA0, 0x00, 0x30, 0x00,
                0x00, 0x02, 0x10, 0x00, 0xC0, 0x00,
                0x00, 0x02, 0x00, 0x07, 0x00, 0x00,
                0x00, 0x01, 0xC1, 0xF8, 0x00, 0x00,
                0x00, 0x00, 0x3E, 0x00, 0x00, 0x00,
        };

int gyVisualMemoryDevicePoll(unsigned port, GYVisualMemoryDevice** device) {
    GYMapleFrame mapleFrame;
    int recvBytes;

    static unsigned first = 0;
    assert(device && port < _initParams.maxDeviceCount);

    GYVisualMemoryDevice* dev = _devices[port];
    if(dev && dev->tcpSocket) {

            ++first;


            static uint8_t val = 0xff;
            int byte = first;
            if(byte > 192) {
                first = 0;
                val = ~val;
                byte = 0;
            }
            if(first % 20 == 0) {
                    lcd_data_image[8+byte] = val;
                    gyVisualMemoryDeviceDisplayImage(dev, lcd_data_image);
            }



        if(gyNetTcpSocketReady(dev->tcpSocket)) {

            if((recvBytes = gyNetTcpSocketRecv(dev->tcpSocket, mapleFrame, MAPLE_FRAME_SIZE_MAX)) > 0) {
                char cmd                        = mapleFrame[MAPLE_FRAME_CMD_OFFSET];
                uint8_t       dstAddr           = mapleFrame[MAPLE_FRAME_DST_ADDR_OFFSET];
                uint8_t       srcAddr           = mapleFrame[MAPLE_FRAME_SRC_ADDR_OFFSET];
                uint8_t       dataWords         = mapleFrame[MAPLE_FRAME_LENGTH_OFFSET];
                uint8_t*      data              = &mapleFrame[MAPLE_FRAME_DATA_OFFSET];

                //Extract Maple Data
                unsigned char func              = data[MAPLE_FRAME_DATA_FUNC_OFFSET];
                unsigned char partitionNumber   = data[MAPLE_FRAME_DATA_PARTITION_OFFSET];      //should be 0?
                unsigned char phase             = data[MAPLE_FRAME_DATA_PHASE_OFFSET];          //sequence number for piecewise block access
                unsigned char block             = data[MAPLE_FRAME_DATA_BLOCK_OFFSET];          //0 for LCD screens (when cmd is WRITE)
                unsigned char* blockWriteData   = &data[MAPLE_FRAME_DATA_WRITE_OFFSET];

                uint32_t      dataBytes         = dataWords * MAPLE_WORD_SIZE;
                uint32_t      mapleFrameSize    = dataBytes + MAPLE_HEADER_SIZE;
                uint32_t      dataWriteBytes    = dataBytes - MAPLE_FRAME_DATA_WRITE_OFFSET;

                _gyLog(GY_DEBUG_VERBOSE, "[GYRO_VMU_API]: Maple Recv (cmd: %d)", cmd);
                _gyPush();
                if(cmd == MAPLE_CMD_DATA_TRANSFER) {
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






#if 0
int gyVisualMemoryDevicePoll(unsigned port, GYVisualMemoryDevice** device) {
    static unsigned first = 0;
    assert(device && port < _initParams.maxDeviceCount);

    GYVisualMemoryDevice* dev = _devices[port];
    if(dev && dev->tcpSocket) {
        ++first;
        if(first% 100 == 0) {
        uint32_t mapleFrame =
            MAPLE_HEADER(MAPLE_CMD_DEV_INFO_REQ,
                        MAPLE_ADDR_SUB(0)   | MAPLE_ADDR_PORTA,
                        MAPLE_DC_ADDR       | MAPLE_ADDR_PORTA,
                        0);
            if(_tcpSend(dev, (unsigned char*)&mapleFrame, sizeof(mapleFrame))) {

            }
        }

        if(first%500==0) {
            unsigned char subAddr = MAPLE_ADDR_SUB((first/500)-2);
            _gyLog(GY_DEBUG_VERBOSE, "sending icon to subAddr - %hhx", subAddr);

            const char lcd_data_image[200+4] = {
                MAPLE_CMD_BLOCK_WRITE,
                subAddr | MAPLE_ADDR_PORTB,
                MAPLE_DC_ADDR | MAPLE_ADDR_PORTB,
                48 + 2,
                0x00, 0x00, 0x00, 0x04,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x07, 0xFF, 0xFF, 0xF0, 0x00,
                0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
                0x00, 0x10, 0x3E, 0x00, 0x04, 0x00,
                0x00, 0x20, 0x55, 0x80, 0x02, 0x00,
                0x00, 0x21, 0xAA, 0xFF, 0xC1, 0x00,
                0x00, 0x23, 0xD5, 0x55, 0xF8, 0x80,
                0x00, 0x27, 0xEA, 0xAB, 0xFC, 0x80,
                0x00, 0x27, 0xFF, 0xFF, 0xFE, 0x80,
                0x00, 0x2F, 0xFF, 0xFF, 0xFE, 0x80,
                0x00, 0x2F, 0xFF, 0xFF, 0xFE, 0x80,
                0x00, 0x24, 0xFF, 0xFF, 0xF2, 0x80,
                0x00, 0x24, 0x07, 0xFF, 0xE4, 0x80,
                0x00, 0x26, 0x00, 0x00, 0x04, 0x80,
                0x00, 0x12, 0x7F, 0xFF, 0xF9, 0x00,
                0x00, 0x11, 0x80, 0x00, 0x01, 0x00,
                0x00, 0x10, 0x00, 0x00, 0x01, 0x00,
                0x00, 0x10, 0x00, 0x00, 0x01, 0x00,
                0x00, 0x10, 0x78, 0x50, 0xF1, 0x00,
                0x00, 0x08, 0x84, 0x01, 0x09, 0x00,
                0x00, 0x08, 0xB4, 0x01, 0x69, 0x00,
                0x00, 0x04, 0x84, 0x01, 0x09, 0x00,
                0x00, 0x02, 0x78, 0x00, 0xF2, 0x00,
                0x00, 0x01, 0x00, 0x00, 0x02, 0x00,
                0x00, 0x01, 0x00, 0x00, 0x02, 0x00,
                0x00, 0x01, 0x80, 0x00, 0x04, 0x00,
                0x00, 0x00, 0x40, 0x03, 0x84, 0x00,
                0x00, 0x00, 0x40, 0x1E, 0x08, 0x00,
                0x00, 0x03, 0xA0, 0x00, 0x30, 0x00,
                0x00, 0x02, 0x10, 0x00, 0xC0, 0x00,
                0x00, 0x02, 0x00, 0x07, 0x00, 0x00,
                0x00, 0x01, 0xC1, 0xF8, 0x00, 0x00,
                0x00, 0x00, 0x3E, 0x00, 0x00, 0x00,
        };

            _tcpSend(dev, lcd_data_image, 204);

        }



            if(gyNetTcpSocketReady(dev->tcpSocket)) {
                _gyLog(GY_DEBUG_VERBOSE, "[GY_VISUAL_MEMORY]: ReadyReceive!");
            uint8_t recvBuff[300];
            unsigned recvBytes = gyNetTcpSocketRecv(dev->tcpSocket, &recvBuff, 300);
            _gyLog(GY_DEBUG_VERBOSE, "RECV BYTES - %d", recvBytes);
             if(recvBytes) {
                 _gyLog(GY_DEBUG_VERBOSE, "FUNC - %hhx", recvBuff[4] | recvBuff[5] << 8);
                 for(unsigned i = 0; i < recvBytes; ++i) {
                     _gyLog(GY_DEBUG_VERBOSE, "BYTE[%d] = %hhx", recvBuff[i]);
                 }
             }
            }

             return 1;


    }
    return 0;
}
#endif
