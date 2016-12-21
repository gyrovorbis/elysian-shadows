#include "gyro_vmu_display.h"
#include "gyro_vmu_device.h"
#include <gyro_vmu_sfr.h>
#include <assert.h>

/* THIS IS WHERE LIBGYRO-BASED RENDERING SHIT SHOULD BE!!!! */

void _updateLcdBuffer(VMUDevice* dev) {

    unsigned char *sfr =     dev->sfr;
    unsigned char (*xram)[0x80] =     dev->xram;
      int y, x, i, a, b=0, p=0;

    const int pixelDelta = dev->display.ghostingEnabled? 1 : VMU_DISP_GHOSTING_FRAMES;

    p = sfr[0x22];
    if(p>=0x83)
        p -= 0x83;
    b = (p>>6);
    p = (p&0x3f)*2;
    for(y=0; y<32; y++) {
        for(x=0; x<48; ) {
            unsigned value = xram[b][p++];
            //if(!lcdon)
             //   value = 0;


            for(int i = 7; i >= 0; --i) {
                if((value>>(unsigned)i)&0x1) {
                    dev->display.lcdBuffer[y][x] += pixelDelta;
                    if(dev->display.lcdBuffer[y][x] > VMU_DISP_GHOSTING_FRAMES)
                        dev->display.lcdBuffer[y][x] = VMU_DISP_GHOSTING_FRAMES;


                } else {
                    dev->display.lcdBuffer[y][x] -= pixelDelta;
                    if(dev->display.lcdBuffer[y][x] < 0)
                        dev->display.lcdBuffer[y][x] = 0;
                }


                x++;
            }

            if((p&0xf)>=12)
                p+=4;
            if(p>=128) {
                b++;
                p-=128;
            }
            if(b==2 && p>=6) {
                b = 0;
                p -= 6;
            }

        }
        }

}

int gyVmuDisplayPixelGet(const VMUDevice* dev, int x, int y) {
    assert(x >= 0 && x < VMU_DISP_PIXEL_WIDTH && y >= 0 && y < VMU_DISP_PIXEL_HEIGHT);

    int bank = y/16;
    int row = y%16;
    int offset = row/2*4;

    int byte = row*6 + offset + x/8;
    int bit = x%8;

    return ((dev->xram[bank][byte]>>bit)&0x1);

   // return ((((unsigned char*)dev->xram)[byte]>>bit)&0x1);
}

int gyVmuDisplayPixelGhostValue(const struct VMUDevice* dev, int x, int y) {
    assert(x >= 0 && x < VMU_DISP_PIXEL_WIDTH && y >= 0 && y < VMU_DISP_PIXEL_HEIGHT);
    return 255 - ((float)dev->display.lcdBuffer[y][x]/(float)VMU_DISP_GHOSTING_FRAMES)*255.0f;
}

int gyVmuDisplayIconGet(const VMUDevice *dev, VMU_DISP_ICN icn) {
    assert(icn >= 0 && icn < VMU_DISP_ICN_COUNT);
    uint8_t bit;

    //0x181-184, icons at bit 6-0
    switch(icn) {
    case VMU_DISP_ICN_FILE:
        bit = 6;
        break;
    case VMU_DISP_ICN_GAME:
        bit = 4;
        break;
    case VMU_DISP_ICN_CLOCK:
        bit = 2;
        break;
    default:
    case VMU_DISP_ICN_FLASH:
        bit = 0;
        break;
    }

    return (dev->xram[VMU_XRAM_BANK_ICN][XRAM_OFFSET(SFR_ADDR_XRAM_BASE)+icn+1]>>bit)&0x1;
}

int gyVmuDisplayEnabled(const VMUDevice* dev) {
    return (dev->sfr[SFR_OFFSET(SFR_ADDR_VCCR)]>>SFR_VCCR_VCCR7_POS);
}

void gyVmuDisplayEnabledSet(VMUDevice* dev, int enabled) {
    if(enabled) {
        dev->sfr[SFR_OFFSET(SFR_ADDR_VCCR)] |= SFR_VCCR_VCCR7_MASK;
    } else {
        dev->sfr[SFR_OFFSET(SFR_ADDR_VCCR)] &= ~SFR_VCCR_VCCR7_MASK;
    }
}

int gyVmuDisplayUpdateEnabled(const VMUDevice* dev) {
    return (dev->sfr[SFR_OFFSET(SFR_ADDR_MCR)]>>SFR_MCR_MCR3_POS);
}

void gyVmuDisplayUpdateEnabledSet(VMUDevice* dev, int enabled) {
    if(enabled) {
        dev->sfr[SFR_OFFSET(SFR_ADDR_MCR)] |= SFR_MCR_MCR3_MASK;
    } else {
        dev->sfr[SFR_OFFSET(SFR_ADDR_MCR)] &= ~SFR_MCR_MCR3_MASK;
    }
}

VMU_DISP_REFRESH_RATE gyVmuDisplayRefreshRate(const struct VMUDevice* dev) {
    return (dev->sfr[SFR_OFFSET(SFR_ADDR_MCR)]>>SFR_MCR_MCR4_POS);
}

void gyVmuDisplaySetRefreshRate(struct VMUDevice* dev, VMU_DISP_REFRESH_RATE rate) {
    if(rate) {
        dev->sfr[SFR_OFFSET(SFR_ADDR_MCR)] |= SFR_MCR_MCR4_MASK;
    } else {
        dev->sfr[SFR_OFFSET(SFR_ADDR_MCR)] &= ~SFR_MCR_MCR4_MASK;
    }
}

int gyVmuDisplayUpdate(struct VMUDevice* dev, float deltaTime) {
    float refreshTime = (1.0f/(gyVmuDisplayRefreshRate(dev) == VMU_DISP_REFRESH_83HZ?
                82.7f : 165.5f));

    dev->display.refreshElapsed += deltaTime;

    if(dev->display.refreshElapsed >= refreshTime) {
        dev->display.refreshElapsed  = 0.0f;
            _updateLcdBuffer(dev);
            return 1;
    }

    return 0;

}

int gyVmuDisplayGhostingEnabledGet(const struct VMUDevice* dev) {
    return dev->display.ghostingEnabled;
}

void gyVmuDisplayGhostingEnabledSet(struct VMUDevice* dev, int enable) {
    dev->display.ghostingEnabled = enable;
}

