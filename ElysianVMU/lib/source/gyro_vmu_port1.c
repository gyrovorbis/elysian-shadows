#include "gyro_vmu_port1.h"
#include "gyro_vmu_sfr.h"
#include "gyro_vmu_device.h"
#include <gyro_system_api.h>
#include <assert.h>

static void _writePin(VMUDevice* dev, uint8_t pin, uint8_t value, PORT1_PIN_SRC src) {
    assert(0); //YOU NEED TO HAVE CALLED BEGIN WRITE FIRST, DICKWEASEL!

    //check if we're even modifying anything
    if(((dev->port1.pins>>pin)&0x1) != value) {
        //check if the pin is even configured as an output
        if(gyVmuPort1PinDir(dev, pin) == PORT1_PIN_OUT) {
            //Check whether pin is configured to receive data from the source
            int pinSrc = gyVmuPort1PinSrc(dev, pin);
            if(pinSrc == src) {
                //add pin to pendingPins
                if(value) dev->port1.pendingPins |= (0x1<<pin);
                else dev->port1.pendingPins &= ~(0x1<<pin);
                dev->port1.pendingMask |= (0x1<<pin);
            }
        }
    }
}

static void _writePins(VMUDevice* dev, uint8_t pins, PORT1_PIN_SRC src) {
    gyVmuPort1PinsWriteBegin(dev);
    for(uint8_t i = 0; i < PORT1_SIZE; ++i) {
        _writePin(dev, i, (pins>>i)&0x1, src);
    }
    gyVmuPort1PinsWriteEnd(dev);
}

void gyVmuPort1PinsWriteBegin(struct VMUDevice* dev) {
    if(dev->port1.writing) {
        _gyLog(GY_DEBUG_WARNING, "Attempting write to P1 before last write ended.");
    }
    dev->port1.writing = true;
    dev->port1.pendingPins = 0;
    dev->port1.pendingMask = 0;
}

void gyVmuPort1PinsWriteEnd(struct VMUDevice* dev) {
    if(!dev->port1.writing) {
        _gyLog(GY_DEBUG_WARNING, "Attempting to end writing to P1 before a write even began.");
    }

    //Flush the pins if they have been modified
    if(dev->port1.pendingMask) {
        const uint8_t newPins = dev->port1.pendingPins & dev->port1.pendingMask;
        dev->port1.pins = newPins;
        //POLYRMOPHIC SERIAL/GPIO/TCP TRANSMIT(newPins)
    }
    dev->port1.writing = 0;
}

void gyVmuPort1MemorySink(VMUDevice *dev, int addr, uint8_t val) {
    switch(addr) {
    case SFR_ADDR_P1DDR:
        _gyLog(GY_DEBUG_VERBOSE, "P1DDR Register Set");
        _gyPush();
        for(unsigned i = 0; i < PORT1_SIZE; ++i) {
            int active = val&(0x1<<i);
            if(i == SFR_P1DDR_P10DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.0 (SO0): %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P11DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.1 (S10): %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P12DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.2 (SCK0): %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P13DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.3 (SO1): %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P14DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.4 (S11): %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P15DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.5 (SCK1): %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P16DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.6: %s", active? "out" : "in");
            } else if(i == SFR_P1DDR_P17DDR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "P1.7: %s", active? "out" : "in");
            }
        }
        _gyPop(1);
        break;
    case SFR_ADDR_P1FCR:
        _gyLog(GY_DEBUG_VERBOSE, "P1FCR Register Set");
        _gyPush();
        for(unsigned i = 0; i < PORT1_SIZE; ++i) {
            int active = val&(0x1<<i);
            if(i == SFR_P1FCR_P10FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.0 Output: %s", active? "SO1" : "P1.0 Data");
            } else if(i == SFR_P1FCR_P11FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.1 Output: %s", active? "SI1" : "P1.1 Data");
            } else if(i == SFR_P1FCR_P12FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.2 Output: %s", active? "SCK0" : "P1.2 Data");
            } else if(i == SFR_P1FCR_P13FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.3 Output: %s", active? "SO1" : "P1.3 Data");
            } else if(i == SFR_P1FCR_P14FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.4 Output: %s", active? "SI1" : "P1.4 Data");
            } else if(i == SFR_P1FCR_P15FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.5 Output: %s", active? "SCK1" : "P1.5 Data");
            } else if(i == SFR_P1FCR_P16FCR_POS) {
                if(active) _gyLog(GY_DEBUG_WARNING, "Port1.6 is fixed to P1.6 Data! Cannot be reassigned!");
            } else if(i == SFR_P1FCR_P17FCR_POS) {
                _gyLog(GY_DEBUG_VERBOSE, "Port1.7 Output: %s", active? "PWM" : "P1.7 Data");
            }

        }
        _gyPop(1);
        break;
    case SFR_ADDR_P1: {
        int tx = 0;
        for(uint8_t i = 0; i < PORT1_SIZE; ++i) {
            if(gyVmuPort1PinSrc(dev, i) == PORT1_PIN_DATA &&
                    gyVmuPort1PinDir(dev, i) == PORT1_PIN_OUT) {
                tx = 1;
                _writePins(dev, val, PORT1_PIN_DATA);
                break;
            }
        }
        if(!tx) {
            gyPrintf(GY_DEBUG_WARNING, "P1 data write [%d] with no output pins sourced from data!", val);
        }
    }
        break;
    }
}

PORT1_PIN_SRC gyVmuPort1PinSrc(const struct VMUDevice* dev, uint8_t pin) {
    return (uint8_t)dev->memMap[SFR_OFFSET(SFR_ADDR_P1FCR)]&(0x1<<pin);
}

PORT1_PIN_DIR gyVmuPort1PinDir(const struct VMUDevice* dev, uint8_t pin) {
    return (uint8_t)dev->memMap[SFR_OFFSET(SFR_ADDR_P1DDR)]&(0x1<<pin);
}

void gyVmuPort1PinSerialWrite(struct VMUDevice* dev, uint8_t pin, uint8_t value) {
    _writePin(dev, pin, value, PORT1_PIN_SERIAL);
}

void gyVmuPort1PinsSerialWrite(VMUDevice *dev, uint8_t value) {
    _writePins(dev, value, PORT1_PIN_SERIAL);
}

void gyVmuPort1PollRecv(struct VMUDevice* dev) {
    uint8_t recvPinMask = 0;
    for(uint8_t i = 0; i < PORT1_SIZE; ++i) {
        if(gyVmuPort1PinDir(dev, i) == PORT1_PIN_OUT) {
            recvPinMask |= (0x1<<i);
        }
    }

    //Don't bother polling anything if we don't have any input pins
    if(recvPinMask) {
        if(/*PHYSICAL_LAYER_READY_RECV(dev)*/1) {
            int newPinsState    = 0;//PHYSICAL_LAYER_RECV();
            int changedMask     = 0;

            for(uint8_t i = 0; i < PORT1_SIZE; ++i) {
                const uint8_t curMask = 0x1<<i;
                //Check if the pin is set to receive data
                if(recvPinMask&curMask) {
                    const newHigh = newPinsState&curMask;
                    //Check if the state of the pin changed
                    if(dev->port1.pins&curMask != newHigh) {
                        changedMask |= curMask;
                        if(newHigh) dev->port1.pins |= newHigh;
                        else dev->port1.pins &= ~newHigh;
                    }
                }
            }
            //Notify the upper layer of any state change on relevant pins
            if(changedMask) {
                gyVmuSerialP1Recv(dev, newPinsState, changedMask);
            }
        }
    }
}
