#ifndef GYRO_VISUAL_MEMORY_API_H
#define GYRO_VISUAL_MEMORY_API_H

#include <stdint.h>
#include <gyro_net_api.h>

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
#define MAPLE_CMD_DATA_TRANSFER                 8
#define MAPLE_CMD_GET_CONDITION                 9
#define MAPLE_CMD_GET_MEMORY_INFO               10
#define MAPLE_CMD_BLOCK_READ                    11
#define MAPLE_CMD_BLOCK_WRITE                   12
#define MAPLE_CMD_SET_CONDITION                 14
#define MAPLE_CMD_NO_RESP                       -1
#define MAPLE_CMD_FUNC_CODE_UNSUPPORTED_RESP    -2
#define MAPLE_CMD_UNKNOWN_CMD_RESP              -3
#define MAPLE_CMD_RESEND_RESP                   -4
#define MAPLE_CMD_FILE_ERROR                    -5

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

#define MAPLE_FRAME_CMD_OFFSET                  3
#define MAPLE_FRAME_DST_ADDR_OFFSET             2
#define MAPLE_FRAME_SRC_ADDR_OFFSET             1
#define MAPLE_FRAME_LENGTH_OFFSET               0
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

typedef unsigned char GYMapleFrame[MAPLE_FRAME_SIZE_MAX];

typedef enum GY_VMU_TCP_COMMAND_TYPE {
    GY_VMU_TCP_COMMAND_CONNECTION_TYPE,
    GY_VMU_TCP_COMMAND_DC_READ,
    GY_VMU_TCP_COMMAND_DC_WRITE,
    GY_VMU_TCP_COMMAND_DC_LCD,
    GY_VMU_TCP_COMMAND_DC_BUZZER
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


typedef struct GYVisualMemoryDevice {
    GYVisualMemoryTCPSettings*  tcpSettings;
    GYTcpSocket*                 tcpSocket;
    unsigned                    port;
    void*                       _physicalDevHandle;
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

int gyVisualMemoryMapleFrame(uint8_t cmd, uint8_t destAddr, uint8_t srcAddr, void*data, uint32_t dataBytes, uint32_t* totalBytes);


#ifdef __cplusplus
}
#endif

#endif // GYRO_VISUAL_MEMORY_API_H
