#if 1

#include "gyro_vmu_tcp.h"
#include "gyro_vmu_device.h"
#include "gyro_vmu_sfr.h"
#include "gyro_vmu_isr.h"
#include "gyro_vmu_maple.h"
#include "gyro_vmu_lcd.h"
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
#if 1
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
        dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] |= (SFR_P7_P70_MASK|SFR_P7_P72_MASK|SFR_P7_P73_MASK); //set external voltage and DC controller pins
        dev->intReq |=  1 << VMU_INT_EXT_INT0;
        break;
    }
#endif
}

static int _handleIncomingTcpConnection(VMUDevice* dev) {
#if 1
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
#if 1
    int newConnection = 0;

    //Incoming TCP client connection request
    if(gyNetTcpSocketReady(dev->serialPort._tcpServer)) {
         newConnection = _handleIncomingTcpConnection(dev);
    }

    return newConnection;
#endif
}

static void _mapleProcessRecv(VMUDevice* dev, unsigned char* packetBuff, int recvBytes) {

        GYMapleFrame    respFrame;
        uint32_t        respBytes = 0;

        //Parse Maple Header
        char cmd                        = packetBuff[MAPLE_FRAME_CMD_OFFSET];
        uint8_t       dstAddr           = packetBuff[MAPLE_FRAME_DST_ADDR_OFFSET];
        uint8_t       srcAddr           = packetBuff[MAPLE_FRAME_SRC_ADDR_OFFSET];
        uint8_t       dataWords         = packetBuff[MAPLE_FRAME_LENGTH_OFFSET];
        uint8_t*      data              = &packetBuff[MAPLE_FRAME_DATA_OFFSET];

        //Extract Maple Data
        unsigned char func              = data[MAPLE_FRAME_DATA_FUNC_OFFSET];
        unsigned char partitionNumber   = data[MAPLE_FRAME_DATA_PARTITION_OFFSET];      //should be 0?
        unsigned char phase             = data[MAPLE_FRAME_DATA_PHASE_OFFSET];          //sequence number for piecewise block access
        unsigned char block             = data[MAPLE_FRAME_DATA_BLOCK_OFFSET];          //0 for LCD screens (when cmd is WRITE)
        unsigned char* blockWriteData   = &data[MAPLE_FRAME_DATA_WRITE_OFFSET];

        uint32_t      dataBytes         = dataWords*MAPLE_WORD_SIZE;
        uint32_t      mapleFrameSize    = dataBytes + MAPLE_HEADER_SIZE;
        uint32_t      dataWriteBytes    = dataBytes - MAPLE_FRAME_DATA_WRITE_OFFSET;



        if(recvBytes != mapleFrameSize) {
            _gyLog(GY_DEBUG_WARNING, "[MAPLE/TCP RECV]: TCP Packet size [%d] does not match Maple frame size [%d]!", recvBytes, mapleFrameSize);



        }
        if(!(func == MAPLE_FUNC_CLOCK || func == MAPLE_FUNC_MEMCARD || func == MAPLE_FUNC_LCD)) {
            _gyLog(GY_DEBUG_WARNING, "[MAPLE/TCP RECV]: Command [%d] sent for unsupported Function Type [%d]", cmd, func);
        }


        switch(cmd) {

        case MAPLE_CMD_DEV_INFO_REQ:
            memcpy(&respFrame[MAPLE_FRAME_DATA_OFFSET], VMUMapleDeviceInfoGet(dev), sizeof(VMUMapleDeviceInfo));
            if(gyVisualMemoryMapleFrame(MAPLE_CMD_DEV_INFO_RESP, srcAddr, dstAddr, respFrame, sizeof(VMUMapleDeviceInfo), &respBytes))
                gyVmuSerialTcpSend(dev, respFrame, respBytes);
            break;

        case MAPLE_CMD_BLOCK_READ: {
            unsigned char* flashBlock = gyVmuFlashBlock(dev, block);
            if(flashBlock) {
                _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: Reading block %d from flash.", block);
                memcpy(&respFrame[MAPLE_FRAME_DATA_OFFSET], flashBlock, VMU_FLASH_BLOCK_SIZE);
                if(gyVisualMemoryMapleFrame(MAPLE_CMD_DATA_TRANSFER, srcAddr, dstAddr, respFrame, sizeof(VMUMapleDeviceInfo), &respBytes))
                    gyVmuSerialTcpSend(dev, respFrame, respBytes);
                break;
            } else {
                _gyLog(GY_DEBUG_ERROR, "[MAPLE/TCP RECV]: Read from Flash block %d returned NULL!", block);
            }
        } break;

        case MAPLE_CMD_BLOCK_WRITE: {
            switch(func) {
                case MAPLE_FUNC_MEMCARD: {
                    unsigned char* flashBlock = gyVmuFlashBlock(dev, block);
                    if(dataWriteBytes != VMU_FLASH_BLOCK_SIZE) {
                        _gyLog(GY_DEBUG_WARNING, "[MAPLE/TCP RECV]: Write to flash requested, but data size does not match block size: %d", dataBytes);
                    }
                    if(flashBlock)
                        memcpy(flashBlock, blockWriteData, dataWriteBytes);
                } break;

            case MAPLE_FUNC_LCD:
                _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: Writing to VMU LCD Screen!");
                if(dataWriteBytes != 192) {
                    _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: Write issued to VMU LCD with incorrect data size: %d", dataBytes);
                }

                for(unsigned byte = 0; byte < 192; ++byte) {
                    for(unsigned bit = 0; bit < 8; ++bit) {
                        uint8_t val = (blockWriteData[byte]>>bit)&0x1;
                        unsigned y = byte / 6;
                        unsigned x = (byte % 6)*8 + bit;
                        gyVmuDisplayPixelSet(dev, x, y, val);
                    }

                }
                break;

            case MAPLE_FUNC_CLOCK:
                _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: Writing to VMU Clock!");
                //0-1: year (lower order byte in 0, upper order byte in 1
                //2 : month
                //3 : day
                //4 : hour
                //5 : minute
                //6 : second
                //7 : fixed value (0x00)


                break;
            default: break;
            }

            if(gyVisualMemoryMapleFrame(MAPLE_CMD_ACK, srcAddr, dstAddr, respFrame, 0, &respBytes))
                gyVmuSerialTcpSend(dev, respFrame, respBytes);

            break;
        case MAPLE_CMD_SET_CONDITION:

                if(func == MAPLE_FUNC_CLOCK) {
                    _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: Playing Sound [%x]!", *(uint32_t*)blockWriteData);
                    uint16_t pulse = *(uint16_t*)&blockWriteData[0];
                    uint16_t width = *(uint16_t*)&blockWriteData[2];

                    gyVmuBuzzerSoundPlay(dev, pulse, width, 0);

                } else {
                    _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: (Virtual) Dreamcast device sent SETCOND command on unsupported function: %d (ignoring)", func);
                }

                if(gyVisualMemoryMapleFrame(MAPLE_CMD_ACK, srcAddr, dstAddr, respFrame, 0, &respBytes))
                    gyVmuSerialTcpSend(dev, respFrame, respBytes);
                break;

        case MAPLE_CMD_NO_RESP:
            _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: (Virtual) Dreamcast device sent NO RESPONSE command! (ignoring)");
            break;
        case MAPLE_CMD_EXT_DEV_INFO_REQ:
            _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: (Virtual) Dreamcast device sent EXTENDED_DEVICE_INFO_REQUEST command! (ignoring)");
            break;

        case MAPLE_CMD_SHUTDOWN_DEV:
            _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: (Virtual) Dreamcast device sent SHUTDOWN command! (ignoring)");
            break;
        case MAPLE_CMD_RESET_DEVICE:
            _gyLog(GY_DEBUG_VERBOSE, "[MAPLE/TCP RECV]: (Virtual) Dreamcast device sent RESET command! (ignoring)");
            break;


        default:
            _gyLog(GY_DEBUG_WARNING, "[MAPLE/TCP RECV]: Unknown Maple command received: %d (Ignoring)", cmd);
        }

    }
}

static void _updateTcpSocketSend(VMUDevice* dev) {
#if 1 //TEMPORARY BULLSHIT REESTED TO ALL FUCK


#else
            if(gyVmuSerialConnectionType(dev) == VMU_SERIAL_CONNECTION_DC) {
                if(dev->serialPort._txStart) {
                    _gyLog(GY_DEBUG_VERBOSE, "[EVMU] - Sending 256 bytes from WRAM!!!");
                    gyVmuSerialTcpSend(dev, dev->wram, 256);
                    dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] |= SFR_SCON0_OV_MASK;
                dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] |= SFR_SCON0_END_MASK;
                //Raise interrupt if interrupt bit is set
                if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] & SFR_SCON0_IE_MASK) {
                    dev->intReq |= 1 << VMU_INT_SIO0;
                    _gyLog(GY_DEBUG_VERBOSE, "RAISING SCON0 INTERRUPT!");
                }


                }

            } else {
#if 1
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
            }
#endif
#endif
}

static void _updateTcpSocketRecv(VMUDevice* dev) {
#if 1
    unsigned char packetBuff[VMU_SERIAL_TCP_PACKET_SIZE_MAX];
    int recvBytes;

    //Check for activity on socket
    if(gyNetTcpSocketReady(dev->serialPort._tcpSocket)) {

        recvBytes = gyNetTcpSocketRecv(dev->serialPort._tcpSocket, packetBuff, VMU_SERIAL_TCP_PACKET_SIZE_MAX);

        if(recvBytes <= 0) {
            gyVmuSerialTcpClose(dev);

        } else

        if(gyVmuSerialConnectionType(dev) == VMU_SERIAL_CONNECTION_DC) {
#if 1 //temporary bullshit -- reested to all fuck!!!
    _mapleProcessRecv(dev, packetBuff, recvBytes);
#else
                _gyLog(GY_DEBUG_VERBOSE, "EVMU - Copying %d bytes to wram!", recvBytes);
                memcpy(dev->wram, packetBuff, recvBytes);

                dev->intReq |= 1 << VMU_INT_SIO1;
                dev->intReq |= 1 << VMU_INT_RFB;
                dev->sfr[SFR_OFFSET(SFR_ADDR_VTRBF)] = packetBuff[0];

                //signal transfer complete
                dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] |= SFR_SCON1_END_MASK;

                //Raise interrupt after byte has been received, if IE bit is set
                if(dev->sfr[SFR_OFFSET(SFR_ADDR_SCON1)] & SFR_SCON1_IE_MASK) {
                    _gyLog(GY_DEBUG_VERBOSE, "RAISING SCON1 INTERRUPT!");

                }
#endif
        } else {
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
                            dev->intReq |= 1 << VMU_INT_SIO1;
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

#if 1
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

void gyVmuSerialPortUpdate(VMUDevice* dev) {
    _gyVmuSerialUpdateTcp(dev);
    //update other shit like GPIO pins or physical connection here
}

VMU_SERIAL_CONNECTION_TYPE gyVmuSerialConnectionType(const struct VMUDevice* dev) {
    if(dev->serialPort._tcpSocket) {
       // if(dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] & SFR_P7_P73_MASK) {
         //   return VMU_SERIAL_CONNECTION_VMU;
        //} else
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_P7)] & SFR_P7_P70_MASK) {
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
#endif
