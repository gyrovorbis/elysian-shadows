#ifndef GYRO_VMU_SERIAL_H
#define GYRO_VMU_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

//#define VMU_SERIAL_TCP_SERVER_USE_LATEST_CLIENT IMPLEMENT OPTION FOR ME
#define VMU_SERIAL_TCP_POLL_TIMEOUT           0
#define VMU_SERIAL_TCP_PACKET_SIZE_MAX        10000

struct          VMUDevice;
typedef void    GYTcpSocket;
typedef void    GYTcpServer;

typedef enum VMU_SERIAL_CONNECTION_TYPE {
    VMU_SERIAL_CONNECTION_NONE,
    VMU_SERIAL_CONNECTION_VMU,
    VMU_SERIAL_CONNECTION_DC
} VMU_SERIAL_CONNECTION_TYPE;

typedef enum VMU_SERIAL_PORT_REMOTE_STATE {
    VMU_SERIAL_PORT_REMOTE_INACTIVE,
    VMU_SERIAL_PORT_REMOTE_READY,
    VMU_SERIAL_PORT_REMOTE_WAITING
} VMU_SERIAL_PORT_REMOTE_STATE;

typedef struct VMUSerialPort {
    GYTcpServer*                    _tcpServer;
    GYTcpSocket*                    _tcpSocket;
    VMU_SERIAL_PORT_REMOTE_STATE    _sio0State;
    VMU_SERIAL_PORT_REMOTE_STATE    _sio1State;
    int                             _useMostRecentClient;
} VMUSerialPort;

void gyVmuSerialUpdate(struct VMUDevice* dev);

int gyVmuSerialTcpServerStart(struct VMUDevice* dev, unsigned port);
int gyVmuSerialTcpServerStop(struct VMUDevice* dev);
void gyVmuSerialTcpServerUseMostRecentClientSet(struct VMUDevice* dev, int val);

int gyVmuSerialTcpOpen(struct VMUDevice* dev, const char* host, unsigned port);
int gyVmuSerialTcpClose(struct VMUDevice* dev);
int gyVmuSerialTcpSend(struct VMUDevice* dev, unsigned char byte);

void gyVmuSerialTcpSconUpdate(struct VMUDevice* dev, int sfr, unsigned char val);

VMU_SERIAL_CONNECTION_TYPE gyVmuSerialConnectionType(const struct VMUDevice* dev);


#ifdef __cplusplus
}
#endif

#endif // GYRO_VMU_SERIAL_H

