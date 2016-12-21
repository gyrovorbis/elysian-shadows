#include "gyro_vmu_serial.h"
#include "gyro_vmu_sfr.h"
#include "gyro_vmu_memory.h"
#include "gyro_vmu_device.h"
#include "gyro_vmu_port1.h"
#include "gyro_vmu_isr.h"
#include "gyro_vmu_osc.h"
#include <gyro_system_api.h>
#include <assert.h>

static void _vmuSerialTransferStart(VMUDevice* dev, int ifaceIndex) {
    VMUSerialIface* iface = &dev->serial.ifaces[ifaceIndex];

    if(gyVmuSerialIfaceConfigMask(dev, ifaceIndex)&VMU_SERIAL_IFACE_SEND_MASK) {
        dev->serial.timeElapsed = 0;
        dev->serial.tsbr = (256.0f - dev->sfr[SFR_OFFSET(SFR_ADDR_SBR)]) * 2.0f * gyVmuOscSecPerCycle(dev);
        iface->sbufCurPos =
                (dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_MSB_MASK)? 7 : 0;

        //Set clock starting position
        gyVmuPort1PinsWriteBegin(dev);
        //Only SIO0 supports configurable polarity.
        if(!ifaceIndex &&
                dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_POL_MASK) {
            gyVmuPort1PinSerialWrite(dev, iface->sckPin, 1);
            iface->sentFirstBit = 0;
        } else {
            gyVmuPort1PinSerialWrite(dev, iface->sckPin, 0);
            gyVmuPort1PinSerialWrite(dev, iface->soPin,
                                     (dev->sfr[SFR_OFFSET(iface->sbufReg)]>>iface->sbufCurPos)&0x1);
            iface->sentFirstBit = 1;
        }
        gyVmuPort1PinsWriteEnd(dev);
    }

}

static int _advanceSerialBuffTransferPos(struct VMUDevice* dev, int ifaceIndex, int* val) {
    VMUSerialIface* iface = &dev->serial.ifaces[ifaceIndex];
    if(dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_MSB_MASK) {
        if(iface->sbufCurPos == 0) {
            if(!ifaceIndex && dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_POL_MASK) {
                *val = (dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]>>iface->sbufCurPos)&0x1;
            } else { //still the same fucking thing without polarity!
                *val = (dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]>>iface->sbufCurPos)&0x1;
            }
            iface->sbufCurPos = 7; //Reset for continuous transfers
            return 0;
        } else {
            if(iface->sentFirstBit) --iface->sbufCurPos;
            else iface->sentFirstBit = 1;
            *val = (dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]>>iface->sbufCurPos)&0x1;
        }
    } else {
        if(iface->sbufCurPos == 7) {
            if(!ifaceIndex && dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_POL_MASK) {
                iface->sbufCurPos = 0;
                *val = (dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]>>iface->sbufCurPos)&0x1;
            } else { //still the same fucking thing without polarity!
                *val = (dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]>>iface->sbufCurPos)&0x1;
            }
            iface->sbufCurPos = 0; //Reset for continuous transfers
            return 0;
        } else {
            ++iface->sbufCurPos;
            *val = (dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]>>iface->sbufCurPos)&0x1;
        }
    }
    return 1;
}

static void _serialTransferComplete(VMUDevice* dev, int ifaceIndex) {
    const VMUSerialIface* iface = &dev->serial.ifaces[ifaceIndex];

    //signal transfer complete
    dev->sfr[SFR_OFFSET(iface->sconReg)] |= SFR_SCON0_END_MASK;

    //Raise interrupt after byte has been received, if IE bit is set
    if(dev->sfr[SFR_OFFSET(iface->sconReg)] & SFR_SCON0_IE_MASK) {
        dev->intReq |= 0x01 << iface->intReq;
    }

    //Stop transfer if it's only 8-bit.
    if(!(dev->sfr[SFR_OFFSET(iface->sconReg)] & SFR_SCON0_LEN_MASK)) {
        dev->sfr[SFR_OFFSET(iface->sconReg)] &= ~SFR_SCON0_CTRL_MASK;
    } else {
        dev->sfr[SFR_OFFSET(iface->sconReg)] |= SFR_SCON0_OV_MASK;
    }
}

void gyVmuSerialUpdate(struct VMUDevice* dev, float deltaTime) {
    const float halfPeriod = dev->serial.tsbr/2.0f;

    do {
        int stateChange = 0;
        if(deltaTime > halfPeriod) {
            dev->serial.timeElapsed += halfPeriod;
            deltaTime -= halfPeriod;
        } else {
            dev->serial.timeElapsed += deltaTime;
            deltaTime = 0;
        }

        if(dev->serial.sbrState == VMU_SERIAL_CLK_STATE1) {
            if(dev->serial.timeElapsed >= halfPeriod) {
                dev->serial.sbrState = VMU_SERIAL_CLK_STATE2;
                stateChange = 1;
            }
        } else {    //VMU_SERIAL_CLK_STATE2
            if(dev->serial.timeElapsed >= dev->serial.tsbr) {
                dev->serial.sbrState = VMU_SERIAL_CLK_STATE1;
                stateChange = 1;
                //Carry over remainder time!
                dev->serial.timeElapsed -= dev->serial.tsbr;
            }
        }

        //Update serial output if clock changed states
        if(stateChange) {
            gyVmuPort1PinsWriteBegin(dev);
            for(unsigned i = 0; i < VMU_SERIAL_IFACE_COUNT; ++i) {
                 const VMUSerialIface* iface = &dev->serial.ifaces[i];

                 //Check if interface is currently transmitting
                if(gyVmuSerialIfaceConfigMask(dev, i)&VMU_SERIAL_IFACE_SEND_MASK &&
                        dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_CTRL_MASK) {

                    //Iface 0 with inverted polarity
                    if(!i && dev->sfr[SFR_OFFSET(iface->sconReg)]&SFR_SCON0_POL_MASK) {
                        gyVmuPort1PinSerialWrite(dev, iface->sckPin,
                                                 dev->serial.sbrState == VMU_SERIAL_CLK_STATE1? 1 : 0);

                        if(dev->serial.sbrState == VMU_SERIAL_CLK_STATE2) {

                            int pinVal;
                            int done = !_advanceSerialBuffTransferPos(dev, 0, &pinVal);
                            gyVmuPort1PinSerialWrite(dev, iface->soPin, pinVal);

                            if(done) {
                                _serialTransferComplete(dev, 0);
                            }
                        }

                    } else { //Iface 0 or 1 with default polarity
                        gyVmuPort1PinSerialWrite(dev, iface->sckPin,
                                                 dev->serial.sbrState == VMU_SERIAL_CLK_STATE1? 0 : 1);

                        if(dev->serial.sbrState == VMU_SERIAL_CLK_STATE1) {

                            int pinVal;
                            int done = !_advanceSerialBuffTransferPos(dev, i, &pinVal);
                            gyVmuPort1PinSerialWrite(dev, iface->soPin, pinVal);

                            if(done) {
                                _serialTransferComplete(dev, i);
                            }
                        }
                    }
                }
            }
            gyVmuPort1PinsWriteEnd(dev);
        }

    } while(deltaTime >= 0.000001f); //account for floating-point inaccuracy

}

void _vmuSerialTransferStop(const VMUDevice* dev, int ifaceIndex) {
    /*POLARITY SHIT WITH MAINTAIning the last bit should NOT be done
    after every byte, becuase continuous transfer shouldn't have it.
            It should only be done when the transfer stop bit is set!!!!!*/
//maintain data pin here
}

void gyVmuSerialMemorySink(struct VMUDevice* dev, uint8_t addr, uint8_t value) {
    uint8_t oldVal = dev->memMap[addr];
    for(unsigned i = 0; i < VMU_SERIAL_IFACE_COUNT; ++i) {
        if(addr == dev->serial.ifaces[i].sconReg) {
            if(oldVal != value) _gyLog(GY_DEBUG_VERBOSE, "SCON%d Register Set");
            _gyPush();
            for(uint8_t j = 0; j < 8; ++j) {
                const uint8_t active = (value>>j)&0x1;
                const int changed = ((oldVal>>j)&0x1 != active);
                switch(i) {
                    case SFR_SCON0_IE_POS:
                    if(changed) _gyLog(GY_DEBUG_VERBOSE, "Interrupts: %s", active? "ENABLED" : "DISABLED");
                    break;
                case SFR_SCON0_END_POS:
                    if(changed) _gyLog(GY_DEBUG_VERBOSE, "Status: %s", active? "END" : "IN PROGRESS");
                    break;
                case SFR_SCON0_MSB_POS:
                    if(changed) _gyLog(GY_DEBUG_VERBOSE, "Endianness: %s", active? "MSB" : "LSB");
                    break;
                case SFR_SCON0_CTRL_POS:
                    if(changed) _gyLog(GY_DEBUG_VERBOSE, "Control: %s", active? "START" : "STOP");
                    if(active) _vmuSerialTransferStart(dev, i);
                    else _vmuSerialTransferStop(dev, i);
                    break;
                case SFR_SCON0_LEN_POS:
                    if(changed) _gyLog(GY_DEBUG_VERBOSE, "Length: %s", active? "8-BIT" : "CONTINUOUS");
                    break;
                case SFR_SCON0_OV_POS:
                    if(changed) _gyLog(GY_DEBUG_VERBOSE, "Overrun: %d", active);
                    break;
                case SFR_SCON0_POL_POS:
                    if(i == 0)
                        if(changed)_gyLog(GY_DEBUG_VERBOSE, "Polarity: %d", active);
                    break;
                }
            }
            _gyPop(1);
            break;
        } else if(addr == dev->serial.ifaces[i].sbufReg) {

            break;
        }
        //CHECK FOR CLOCK SPEED CHANGE TO ADJUST SBR!!1111
    }

}


void gyVmuSerialInit(struct VMUDevice* dev) {
    dev->serial.sbrState = VMU_SERIAL_CLK_STATE1;

    for(unsigned i = 0; i < VMU_SERIAL_IFACE_COUNT; ++i) {
        VMUSerialIface* iface
                            = &dev->serial.ifaces[i];
        const int offset    = i? 3 : 0;
        iface->sbufCurPos  = -1;
        iface->sbufReg      = i? SFR_ADDR_SBUF1 : SFR_ADDR_SBUF0;
        iface->sconReg      = i? SFR_ADDR_SCON1 : SFR_ADDR_SCON0;
        iface->intReq       = i? VMU_INT_SIO1 : VMU_INT_SIO0;
        iface->soPin        = 0 + offset;
        iface->siPin        = 1 + offset;
        iface->sckPin       = 2 + offset;
    }
}

void gyVmuSerialP1Recv(struct VMUDevice* dev, uint8_t pins, uint8_t changedMask) {
    /*don't forget to check for trailig clock or whatever-the-fuck for
    overflow on the receive side!!!!*/
}

static inline int _pinConfiguredAsSerialOut(const VMUDevice* dev, uint8_t pin) {
    return gyVmuPort1PinDir(dev, pin) == PORT1_PIN_OUT &&
            gyVmuPort1PinSrc(dev, pin) == PORT1_PIN_SERIAL;
}

int gyVmuSerialIfaceConfigMask(const VMUDevice* dev, int ifaceIndex) {
    assert(ifaceIndex >= 0 && ifaceIndex < VMU_SERIAL_IFACE_COUNT); //There are only two serial ifaces, dipshit...
    const VMUSerialIface* iface = &dev->serial.ifaces[ifaceIndex];
    int mask = 0;

    if(_pinConfiguredAsSerialOut(dev, iface->sckPin) ||
            _pinConfiguredAsSerialOut(dev, iface->soPin)
           // ||_pinConfiguredAsSerialOut(dev, iface->siPin)
            )
        mask |= VMU_SERIAL_IFACE_SEND_MASK;

    if(gyVmuPort1PinDir(dev, iface->sckPin) == PORT1_PIN_IN &&
            gyVmuPort1PinDir(dev, iface->siPin == PORT1_PIN_IN))
        mask |= VMU_SERIAL_IFACE_RECV_MASK;

    return mask;
}


