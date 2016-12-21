#include "gyro_vmu_tcp.h"
#include "gyro_vmu_device.h"
#include "gyro_vmu_sfr.h"
#include "gyro_vmu_isr.h"
#include <gyro_net_api.h>
#include <gyro_system_api.h>
#include <assert.h>


typedef enum TCP_MSG_TYPE {
    TCP_MSG_CONNECTION_TYPE,
    TCP_MSG_SIO0_BUFFER,
    TCP_MSG_SIO0_READY,
    TCP_MSG_SIO0_INACTIVE,
    TCP_MSG_SIO0_ACK,
    TCP_MSG_SIO1_BUFFER,
    TCP_MSG_SIO1_READY,
    TCP_MSG_SIO1_INACTIVE,
    TCP_MSG_SIO1_ACK
} TCP_MSG_TYPE;

static void _setSerialConnectionPins(VMUDevice* dev, VMU_SERIAL_CONNECTION_TYPE type) {
#if 0
    switch(type) {
    case VMU_SERIAL_CONNECTION_NONE:
        _gyLog(GY_DEBUG_VERBOSE, "Disconnecting VMU external serial pins.");
        dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] &= ~(SFR_P7_P70_MASK|SFR_P7_P72_MASK|SFR_P7_P73_MASK); //Clear all external pins
        break;
    case VMU_SERIAL_CONNECTION_VMU:
        _gyLog(GY_DEBUG_VERBOSE, "Configuring VMU external serial pins for VMU to VMU connection.");
        dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] &= ~(SFR_P7_P70_MASK|SFR_P7_P72_MASK); //Clear DC/PC pins
        dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] |= SFR_P7_P73_MASK;   //Set VMU pin
        dev->serialPort._txState = VMU_SERIAL_PORT_INACTIVE; //reset remote RX port state
        dev->serialPort._rxState = VMU_SERIAL_PORT_INACTIVE;
        break;

    case VMU_SERIAL_CONNECTION_DC:
        _gyLog(GY_DEBUG_VERBOSE, "Configuring VMU external serial pins for DC to VMU connection.");
        dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] &= ~SFR_P7_P73_MASK; //Clear VMU/VMU connection pin
        dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] |= (SFR_P7_P70_MASK|SFR_P7_P72_MASK); //set external voltage and DC controller pins
        break;
    }
#endif
}

static int _handleIncomingTcpConnection(VMUDevice* dev) {
#if 0
    int newConnection = 0;

    _gyLog(GY_DEBUG_VERBOSE, "Receiving incoming TCP connection request.");
    _gyPush();

    if(dev->serialPort._tcpSocket) {
        if(!dev->serialPort._useMostRecentClient) {
            _gyLog(GY_DEBUG_WARNING, "Already connected to a client. Refusing request.");
            _gyPop(1);
            return 0;
        } else {
            _gyLog(GY_DEBUG_VERBOSE, "Disconnecting previous client and destroying socket.");
            gyNetTcpSocketClose(dev->serialPort._tcpSocket);
        }
    }

    dev->serialPort._tcpSocket = gyNetTcpSocketServerCheckIncomingConnections(dev->serialPort._tcpServer);
    if(dev->serialPort._tcpSocket) {
        newConnection = gyNetMonitoredConnectionsAdd(dev->serialPort._tcpSocket);
        if(newConnection) {
            _gyLog(GY_DEBUG_VERBOSE, "Connection established successfully.");
        } else {
            _gyLog(GY_DEBUG_ERROR, "Problem adding new connection to monitored connection list.");
        }
    } else {
        _gyLog(GY_DEBUG_ERROR, "Problem establishing socket.");
        newConnection = 0;
    }

    _gyPop(1);

    return newConnection;
#endif
}

static int _updateTcpServer(VMUDevice* dev) {
#if 0
    int newConnection = 0;

    //Incoming TCP client connection request
    if(gyNetTcpSocketReady(dev->serialPort._tcpServer)) {
         newConnection = _handleIncomingTcpConnection(dev);
    }

    return newConnection;
#endif
}

static void _updateTcpSocketSend(VMUDevice* dev) {
#if 0
    //Send SBUF0 to remote if it's ready and SCON0.CTRL is set.
    if(dev->serialPort._txState == VMU_SERIAL_PORT_READY //remote SCON1 is ready to recieve
    &&
#if 0
            (
                (dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] & SFR_SCON0_CTRL_MASK)
             || (dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] & SFR_SCON0_LEN_MASK)
            )
#else
            dev->serialPort._txStart
#endif
            ) //local SCON0 is ready to send
    {   //check pin directions
        if((dev->sfr[SFR_OFFSET(SFR_ADDR_P1DDR)]&(SFR_P1DDR_P10DDR_MASK|SFR_P1DDR_P13DDR_MASK)) && //P1.SO0 and P1.SCK0 set as OUT pins
           (dev->sfr[SFR_OFFSET(SFR_ADDR_P1FCR)]&(SFR_P1FCR_P10FCR_MASK|SFR_P1FCR_P13FCR_MASK))) //SO0 and SCK0 sourced from serial interface
        {
            //send SBUF0 to remote
            unsigned char sbuf0[2] = {
                TCP_MSG_SIO0_BUFFER,
                dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]
            };
            //update txStat if send is successful
            if(gyVmuSerialTcpSend(dev, sbuf0, 2)) {
                _gyLog(GY_DEBUG_VERBOSE, "SENT BYTE - %x", dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]);
                dev->serialPort._txState = VMU_SERIAL_PORT_WAITING;
            }

            dev->serialPort._txStart = 0;

            if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] & SFR_SCON0_OV_MASK) {

            }
        }
    }

    //Tell remote that we're ready to receive and change our state to ready.
    if(dev->serialPort._rxState == VMU_SERIAL_PORT_INACTIVE &&
        (dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_CTRL_MASK))
    {
        unsigned char ready = TCP_MSG_SIO1_READY;
        if(gyVmuSerialTcpSend(dev, &ready, 1)) {
            dev->serialPort._rxState = VMU_SERIAL_PORT_READY;
        }
    }
#endif
}

static void _updateTcpSocketRecv(VMUDevice* dev) {
#if 0
    unsigned char packetBuff[VMU_SERIAL_TCP_PACKET_SIZE_MAX];
    int recvBytes;

    //Check for activity on socket
    if(gyNetTcpSocketReady(dev->serialPort._tcpSocket)) {

        recvBytes = gyNetTcpSocketRecv(dev->serialPort._tcpSocket, packetBuff, VMU_SERIAL_TCP_PACKET_SIZE_MAX);

        //0 byte TCP packet means the client disconnected!
        if(recvBytes <= 0) {
            _gyLog(GY_DEBUG_VERBOSE, "TCP VMU client has disconnected.");
            _gyPush();
            _gyLog(GY_DEBUG_VERBOSE, "Closing TCP socket.");
            gyNetMonitoredConnectionsRemove(dev->serialPort._tcpSocket);
            gyNetTcpSocketClose(dev->serialPort._tcpSocket);
            dev->serialPort._tcpSocket = 0;
            _setSerialConnectionPins(dev, VMU_SERIAL_CONNECTION_NONE);
            _gyPop(1);


        } else { //Decode message

            switch(packetBuff[0]) {
            case TCP_MSG_CONNECTION_TYPE:
                _setSerialConnectionPins(dev, packetBuff[1]);
                break;
            case TCP_MSG_SIO0_BUFFER: //receiving from SBUF0 on other device, goes to SBUF1 on this device
               // if(!(dev->sfr[SFR_OFFSET(SFR_ADDR_P1DDR)]&(SFR_P1DDR_P14DDR_MASK)) &&   //P14/SI11/SB1 as receive
               //    !(dev->sfr[SFR_OFFSET(SFR_ADDR_P1FCR)]&(SFR_P1FCR_P13FCR_MASK)))     //P13/SO1 as general input
               // {
                    //Check if receiving on SBUF1
                    if(dev->serialPort._rxState == VMU_SERIAL_PORT_READY &&
                            (dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_CTRL_MASK))
                    {
                        _gyLog(GY_DEBUG_VERBOSE, "RECEIVED BYTE - %x", packetBuff[1]);
                        _gyPush();
                        //receive data
                        dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF1)] = packetBuff[1];

                        char ack;

                        if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_LEN_MASK) {
#if 1
                            ack = TCP_MSG_SIO1_READY;
#else
                            ack = TCP_MSG_SIO1_ACK;
#endif

                            _gyLog(GY_DEBUG_VERBOSE, "CONTINUOUS RECEIVE SET - setting SCON1_OV_MASK overflow mask");
                        } else {
                        ack = TCP_MSG_SIO1_ACK;
                            _gyLog(GY_DEBUG_VERBOSE, "CONTINUOUS RECEIVE NOT SET - not setting SCON1_OV_MASK");
                                                     dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] &= ~SFR_SCON1_CTRL_MASK;

                        }

                                                    dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] |= SFR_SCON1_OV_MASK;

                        //signal transfer complete
                        dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] |= SFR_SCON1_END_MASK;

                        //Raise interrupt after byte has been received, if IE bit is set
                        if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_IE_MASK) {
                            _gyLog(GY_DEBUG_VERBOSE, "RAISING SCON1 INTERRUPT!");
                            dev->intReq |= 1 << VMU_INT_SI01;
                        }

                        _gyPop(1);

                        //Send ACK back to TX
                        if(gyVmuSerialTcpSend(dev, &ack, 1)) {
                           // if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_LEN_MASK) {
#if 1
                            if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_LEN_MASK) {
                                dev->serialPort._rxState = VMU_SERIAL_PORT_READY;
                            } else {
                                dev->serialPort._rxState = VMU_SERIAL_PORT_INACTIVE;
                            }
#else
                            dev->serialPort._rxState = VMU_SERIAL_PORT_INACTIVE;
#endif
                           // }
                        }
                    } else {
                        _gyLog(GY_DEBUG_ERROR, "Remote device sent a byte [%x] while local VMU's was not ready to receive!", packetBuff[1]);
                    }
                break;
            case TCP_MSG_SIO1_BUFFER: //Normally SBUF1 should not be a sending buffer, but check config to support it
                _gyLog(GY_DEBUG_ERROR, "JUST RECIEVED SOME SHIT ON SI01 MSG!!! - %x", packetBuff[1]);
                break;
            case TCP_MSG_SIO1_ACK:

                if(dev->serialPort._txState == VMU_SERIAL_PORT_WAITING) {
                    _gyLog(GY_DEBUG_VERBOSE, "ACK RECEIVED!");

                    if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] & SFR_SCON0_LEN_MASK) {

                        _gyLog(GY_DEBUG_VERBOSE, "CONTINUOUS SEND SET");
#if 0
                        dev->serialPort._txState = VMU_SERIAL_PORT_READY;
#else
                        dev->serialPort._txState = VMU_SERIAL_PORT_INACTIVE;
#endif
                    } else {
                        _gyLog(GY_DEBUG_VERBOSE, "CONTINUOUS SEND NOT SET");
                        //signal transfer done
                        dev->serialPort._txState = VMU_SERIAL_PORT_INACTIVE;
                        dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] &= ~SFR_SCON0_CTRL_MASK;
                    }
                        dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] |= SFR_SCON0_OV_MASK;
                    dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] |= SFR_SCON0_END_MASK;
                    //Raise interrupt if interrupt bit is set
                    if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] & SFR_SCON0_IE_MASK) {
                        dev->intReq |= 1 << VMU_INT_SIO0;
                        _gyLog(GY_DEBUG_VERBOSE, "RAISING SCON0 INTERRUPT!");
                    }


                } else {
                    _gyLog(GY_DEBUG_ERROR, "RECEIVED AN UNEXPECTED ACK!");
                }

                break;

            case TCP_MSG_SIO1_READY:
                _gyLog(GY_DEBUG_ERROR, "READY RECV!!!");
                dev->serialPort._txState = VMU_SERIAL_PORT_READY;
                break;
            default:
                _gyLog(GY_DEBUG_ERROR, "Received unknown VMU TCP message type [%d].", packetBuff[0]);
            }

        }
    }
#endif
}

#if 0
void _gyVmuSerialUpdateTcp(VMUDevice* dev) {

    int newConnection = 0;

    //Check if we're using serial over TCP
    if(dev->serialPort._tcpServer || dev->serialPort._tcpSocket) {

        //Check the status of our active connections
        gyNetMonitoredConnectionsPoll(VMU_SERIAL_TCP_POLL_TIMEOUT);

        //update TCP Server
        if(dev->serialPort._tcpServer) {
            newConnection = _updateTcpServer(dev);
        }

        //Update TCP Socket (only if it existed when we polled the sockets)
        if(!newConnection && dev->serialPort._tcpSocket) {
            _updateTcpSocketSend(dev);
            _updateTcpSocketRecv(dev);
        }

    }

}

void gyVmuSerialUpdate(VMUDevice* dev) {
    _gyVmuSerialUpdateTcp(dev);
    //update other shit like GPIO pins or physical connection here
}

VMU_SERIAL_CONNECTION_TYPE gyVmuSerialConnectionType(const struct VMUDevice* dev) {
    if(dev->serialPort._tcpSocket) {
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] & SFR_P7_P73_MASK) {
            return VMU_SERIAL_CONNECTION_VMU;
        } else if(dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] & SFR_P7_P70_MASK) {
            return VMU_SERIAL_CONNECTION_DC;
        }
    }

    return VMU_SERIAL_CONNECTION_NONE;
}

int gyVmuSerialTcpServerStart(struct VMUDevice* dev, unsigned port) {
    dev->serialPort._tcpServer = gyNetTcpSocketServerCreate(port);
    return dev->serialPort._tcpServer? 1 : 0;
}

int gyVmuSerialTcpServerStop(struct VMUDevice* dev) {
    int success = 1;

    success &= gyVmuSerialTcpClose(dev);

    if(dev->serialPort._tcpServer) {
        success &= gyNetTcpSocketServerDestroy(dev->serialPort._tcpServer);
        dev->serialPort._tcpServer = NULL;
    }

    return success;
}

int gyVmuSerialTcpOpen(VMUDevice* dev, const char* host, unsigned port) {
    int success = 0;
    dev->serialPort._tcpSocket = gyNetTcpSocketOpen(host, port);
    if(dev->serialPort._tcpSocket) {
        if(!gyNetMonitoredConnectionsAdd(dev->serialPort._tcpSocket)) {
            _gyLog(GY_DEBUG_VERBOSE, "Could not add socket to monitored connection list!");

        } else {

            //Send connection type to remote immediately.
            char bytes[2] = {
                TCP_MSG_CONNECTION_TYPE,
                VMU_SERIAL_CONNECTION_VMU
            };
            if(gyNetTcpSocketSend(dev->serialPort._tcpSocket, bytes, 2)) {
                _gyLog(GY_DEBUG_VERBOSE, "Connection type sent to remote.");
                _setSerialConnectionPins(dev, VMU_SERIAL_CONNECTION_VMU);
                success = 1;
            } else {
                _gyLog(GY_DEBUG_ERROR, "Unable to send connection type over TCP!");
            }
        }

    }
    return success;
}

int gyVmuSerialTcpClose(VMUDevice *dev) {
    int success = 1;
    if(dev->serialPort._tcpSocket) {
        _setSerialConnectionPins(dev, VMU_SERIAL_CONNECTION_NONE);
        success &= gyNetTcpSocketClose(dev->serialPort._tcpSocket);
        dev->serialPort._tcpSocket = NULL;
    }

    return success;
}

void gyVmuSerialTcpServerUseMostRecentClientSet(struct VMUDevice* dev, int val) {
    dev->serialPort._useMostRecentClient = val;
}

int gyVmuSerialTcpSend(struct VMUDevice* dev, const unsigned char* bytes, int size) {
    int bytesSent = 0;
    int success = 0;

    if(dev->serialPort._tcpSocket) {
        if((bytesSent = gyNetTcpSocketSend(dev->serialPort._tcpSocket, bytes, size)) != size) {
            _gyLog(GY_DEBUG_ERROR, "Could not send packet over TCP! [Bytes Sent: %d]", bytesSent);
        } else success = 1;
    } else {
        _gyLog(GY_DEBUG_ERROR, "No open TCP socket! Cannot send packet!");
    }

    return success;
}

void gyVmuSerialTcpSconUpdate(struct VMUDevice* dev, int sfr, unsigned char val) {
    if(!dev->serialPort._tcpSocket) return;

    switch(sfr) {
    case SFR_ADDR_SCON0:
        if(val & (SFR_SCON0_LEN_MASK|SFR_SCON0_CTRL_MASK)) {
            dev->serialPort._txStart = 1;
            _gyLog(GY_DEBUG_ERROR, "TXSTART! STATE - %d", dev->serialPort._txState);
        }
        break;
    case SFR_ADDR_SCON1:

        break;
    default: assert(0); //What the fuck, you should not have passed a non-SCON sfr here, fuckface!!!
    }

}
#endif
