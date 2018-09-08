#ifndef GYRO_VISUAL_MEMORY_API_H
#define GYRO_VISUAL_MEMORY_API_H

#include <stdint.h>
#include "gyro_net_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAPLE_CMD_DEV_INFO_REQ                  1
#define MAPLE_CMD_EXT_DEV_INFO_REQ              2
#define MAPLE_CMD_RESET_DEVICE                  3
#define MAPLE_CMD_SHUTDOWN_DEV                  4
#define MAPLE_CMD_DEV_INFO_RESP                 5
#define MAPLE_CMD_EXT_DEV_INFO_RESP             6
#define MAPLE_CMD_ACK                           7
#define MAPLE_CMD_DATA_XFER                     8
#define MAPLE_CMD_GET_COND                      9
#define MAPLE_CMD_GET_MEM_INFO                  10
#define MAPLE_CMD_BLOCK_READ                    11
#define MAPLE_CMD_BLOCK_WRITE                   12
#define MAPLE_CMD_SET_COND                      14
#define MAPLE_CMD_NO_RESP                       -1
#define MAPLE_CMD_FUNC_CODE_UNSUPPORTED_RESP    -2
#define MAPLE_CMD_UNKNOWN_CMD_RESP              -3
#define MAPLE_CMD_RESEND_RESP                   -4
#define MAPLE_CMD_FILE_ERROR_RESP               -5

#define MAPLE_ERROR_CODE_INVALID_PARTITION      0x1
#define MAPLE_ERROR_CODE_PHASE                  0x2
#define MAPLE_ERROR_CODE_INVALID_BLOCK          0x4
#define MAPLE_ERROR_CODE_WRITE_ERROR            0x8
#define MAPLE_ERROR_CODE_INVALID_LENGTH         0x10    //use 192 bytes for LCD writes, 128 bytes for flash writes
#define MAPLE_ERROR_CODE_BAD_CRC                0x20

#define MAPLE_FUNC_CONTROLLER                   0x001
#define MAPLE_FUNC_MEMCARD                      0x002
#define MAPLE_FUNC_LCD                          0x004
#define MAPLE_FUNC_CLOCK                        0x008
#define MAPLE_FUNC_MIC                          0x010
#define MAPLE_FUNC_AR_GUN                       0x020
#define MAPLE_FUNC_KEYBOARD                     0x040
#define MAPLE_FUNC_LIGHT_GUN                    0x080
#define MAPLE_FUNC_PURUPURU                     0x100
#define MAPLE_FUNC_MOUSE                        0x200

#define MAPLE_ADDR_PORT(id)                     ((id)<<6)
#define MAPLE_ADDR_PORTA                        MAPLE_ADDR_PORT(0)
#define MAPLE_ADDR_PORTB                        MAPLE_ADDR_PORT(1)
#define MAPLE_ADDR_PORTC                        MAPLE_ADDR_PORT(2)
#define MAPLE_ADDR_PORTD                        MAPLE_ADDR_PORT(3)
#define MAPLE_ADDR_MAIN                         0x20
#define MAPLE_ADDR_SUB(id)                      ((1)<<id) /* where id is 0 to 4 */

#define MAPLE_WORD_SIZE                         4
#define MAPLE_HEADER_SIZE                       4
#define MAPLE_DC_ADDR                           0
#define MAPLE_FRAME_DATA_WORD_SIZE_MAX          256
#define MAPLE_FRAME_DATA_SIZE_MAX               MAPLE_FRAME_DATA_WORD_SIZE_MAX * MAPLE_WORD_SIZE
#define MAPLE_FRAME_SIZE_MAX                    MAPLE_FRAME_DATA_SIZE_MAX + MAPLE_HEADER_SIZE

#define MAPLE_FRAME_CMD_OFFSET                  0
#define MAPLE_FRAME_DST_ADDR_OFFSET             1
#define MAPLE_FRAME_SRC_ADDR_OFFSET             2
#define MAPLE_FRAME_LENGTH_OFFSET               3
#define MAPLE_FRAME_DATA_OFFSET                 4

#define MAPLE_FRAME_DATA_FUNC_OFFSET            3
#define MAPLE_FRAME_DATA_PARTITION_OFFSET       2
#define MAPLE_FRAME_DATA_PHASE_OFFSET           1
#define MAPLE_FRAME_DATA_BLOCK_OFFSET           0
#define MAPLE_FRAME_DATA_WRITE_OFFSET           4

#define MAPLE_HEADER(cmd,dst_addr,src_addr,len)	( (((cmd)&0xfful)<<24) | (((dst_addr)&0xfful)<<16) | (((src_addr)&0xfful)<<8) | ((len)&0xff))

#define VMU_MAPLE_WRITE_SIZE_MAX                128
#define VMU_BLOCK_SIZE                          256

#define GY_VISUAL_MEMORY_TCP_HOSTNAME_SIZE      200

#define GY_MAPLE_FUNCTION_DATA_SIZE             3
#define GY_MAPLE_PRODUCT_NAME_SIZE              30
#define GY_MAPLE_PRODUCT_LICENSE_SIZE           60

#define GY_MAPLE_DEVICE_INFO_SIZE               (28 * MAPLE_WORD_SIZE)

typedef struct GYMapleDeviceInfo {
    int32_t     func;                                           // function codes supported by this peripheral (or:ed together) (big endian)
    int32_t     function_data[GY_MAPLE_FUNCTION_DATA_SIZE];     // additional info for the supported function codes (3 max) (big endian)
    int8_t      area_code;                                      // regional code of peripheral
    int8_t      connector_direction;                            // physical orientation of bus connection
    char        product_name[GY_MAPLE_PRODUCT_LICENSE_SIZE];    // name of peripheral
    char        product_license[GY_MAPLE_PRODUCT_LICENSE_SIZE]; // license statement
    int16_t     standby_power;                                  // standby power consumption (little endian)
    int16_t     max_power;                                      // maximum power consumption (little endian)
} GYMapleDeviceInfo;

#define MAPLE_PAYLOAD_SIZE MAPLE_FRAME_SIZE_MAX-MAPLE_HEADER_SIZE

//MAPLE_CMD_BLOCK_WRITE
typedef struct GYMapleMemoryPayload {
    uint8_t func;
    uint8_t part;
    uint8_t phase;
    uint8_t block;
    uint8_t data[MAPLE_PAYLOAD_SIZE-4];
} GYMapleMemoryPayload;

//MAPLE_CMD_EXT_DEV_INFO_RESP
typedef struct GYMapleExtDevInfoPayload {
    GYMapleDeviceInfo   devInfo;
    uint8_t             versionStr[MAPLE_PAYLOAD_SIZE-GY_MAPLE_DEVICE_INFO_SIZE];
} GYMapleExtDevInfoPayload;

typedef struct GYMapleCondPayload {
    uint8_t func;
    uint8_t cond[MAPLE_PAYLOAD_SIZE-1];
} GYMapleCondPayload;


typedef struct GYMapleFrame {
    union {
        struct {
            struct {                            //header
                union {
                    struct {
                        int8_t                              cmd;
                        uint8_t                             recvAddr;
                        uint8_t                             sendAddr;
                        uint8_t                             dataWords;
                    };
                    uint8_t     bytes[MAPLE_HEADER_SIZE];
                };
            }                                   header;
            struct {                            //payload
                union {
                    GYMapleDeviceInfo                       devInfoResp;                    //(5) MAPLE_CMD_DEV_INFO_RESP
                    GYMapleExtDevInfoPayload                extDevInfoResp;                 //(6) MAPLE_CMD_EXT_DEV_INFO_RESP
                    GYMapleCondPayload                      getCondDataXferResp;            //(8) MAPLE_CMD_DATA_XFER [(9) MAPLE_CMD_GET_COND resp]
                    GYMapleMemoryPayload                    blockReadDataXferResp;          //(8) MAPLE_CMD_DATA_XFER [(11) MAPLE_CMD_BLOCK_READ resp]
                    struct {
                        uint8_t cond;
                    }                                       getCond;                        //(9) MAPLE_CMD_GET_COND
                    struct {
                        uint8_t func;
                        uint8_t part;
                    }                                       getMemInfo;                     //(10) MAPLE_CMD_GET_MEM_INFO
                    struct {
                        uint8_t func;
                        uint8_t part;
                        uint8_t phase;
                        uint8_t block;
                    }                                       blockRead;                      //(11) MAPLE_CMD_BLOCK_READ
                    GYMapleMemoryPayload                    blockWrite;                     //(12) MAPLE_CMD_BLOCK_WRITE
                    GYMapleCondPayload                      setCond;                        //(14) MAPLE_CMD_SET_COND
                    struct {
                        uint8_t                             error;
                    }                                       fileErrorResp;                  //(-5) MAPLE_CMD_FILE_ERROR_RESP
                    uint8_t                                 bytes[MAPLE_PAYLOAD_SIZE];      //raw access
                };
            }                                   payload;
        };
       uint8_t                                  bytes[MAPLE_FRAME_SIZE_MAX];
    };
} GYMapleFrame;


typedef enum GY_VMU_TCP_COMMAND_TYPE {
    GY_VMU_TCP_COMMAND_CONNECTION_TYPE
} GY_VMU_TCP_COMMAND_TYPE;

typedef enum GY_VMU_TCP_CONNECTION_TYPE {
    GY_VMU_TCP_CONNECTION_NONE,
    GY_VMU_TCP_CONNECTION_VMU,
    GY_VMU_TCP_CONNECTION_DC
} GY_VMU_TCP_CONNECTION_TYPE;

typedef struct GYVisualMemoryTCPSettings {
    char        hostname[GY_VISUAL_MEMORY_TCP_HOSTNAME_SIZE];
    unsigned    port;
} GYVisualMemoryTCPSettings;

typedef enum GY_VMU_STATE {
    GY_VMU_STATE_INACTIVE,
    GY_VMU_STATE_WAIT_DEV_INFO,
    GY_VMU_STATE_WAIT_EXT_DEV_INFO,
    GY_VMU_STATE_WAIT_ACK
} GY_VMU_STATE;

typedef struct GYVisualMemoryDevice {
    GYVisualMemoryTCPSettings*  tcpSettings;
    GYTcpSocket*                tcpSocket;
    unsigned                    port;
    void*                       _physicalDevHandle;
    float                       elapsedTime;
} GYVisualMemoryDevice;

typedef struct GYVisualMemoryInitParams {
    unsigned maxDeviceCount;
} GYVisualMemoryInitParams;

int gyVisualMemoryInit(const GYVisualMemoryInitParams* params);
int gyVisualMemoryUninit(void);

int gyVisualMemoryDevicePoll(unsigned port, GYVisualMemoryDevice** device);

int gyVisualMemoryDeviceBlockRead(GYVisualMemoryDevice* device, uint8_t* buffer, uint8_t blocks);
int gyVisualMemoryDeviceBlockWrite(GYVisualMemoryDevice* device, const uint8_t* buffer, uint8_t block);
int gyVisualMemoryDeviceDisplayImage(GYVisualMemoryDevice* device, uint8_t* buffer);
int gyVisualMemoryDeviceBeep(GYVisualMemoryDevice* device, uint8_t cycleLength, uint8_t pulseWidth);

int gyVisualMemoryDeviceRemoteConnect(unsigned port, const GYVisualMemoryTCPSettings* settings);
int gyVisualMemoryDeviceRemoteDisconnect(GYVisualMemoryDevice* device);

int gyVisualMemoryMapleFrame(int8_t cmd, uint8_t destAddr, uint8_t srcAddr, GYMapleFrame* frame, uint32_t dataBytes, uint32_t* totalBytes);


#ifdef __cplusplus
}
#endif

#endif // GYRO_VISUAL_MEMORY_API_H
