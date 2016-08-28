#include "gyro_vmu_display.h"
#include "gyro_vmu_device.h"
#include <assert.h>

/* THIS IS WHERE LIBGYRO-BASED RENDERING SHIT SHOULD BE!!!! */

int gyVmuDisplayPixelGet(const VMUDevice* dev, int x, int y) {
    assert(x >= 0 && x < VMU_DISP_PIXEL_WIDTH && y >= 0 && y < VMU_DISP_PIXEL_HEIGHT);

#if 0


    int row     = y;
    int offset  = y/2*4;

    int byte    = row*6 + x/8 + offset;
    int bit     = x%8;

    if(row < 16) {
        return ((dev->xram[VMU_XRAM_BANK_LCD_TOP][byte]>>bit)&0x1);
    } else {
        return ((dev->xram[VMU_XRAM_BANK_LCD_BOT][byte-16]>>bit)&0x1);
    }

#else

    int bank = y/16;
    int row = y%16;
    int offset = row/2*4;

    int byte = row*6 + offset + x/8;
    int bit = x%8;

    return ((dev->xram[bank][byte]>>bit)&0x1);


#endif

   // return ((((unsigned char*)dev->xram)[byte]>>bit)&0x1);
}

int gyVmuDisplayIconGet(const VMUDevice *dev, VMU_DISP_ICN icn) {
    assert(icn >= 0 && icn < VMU_DISP_ICN_COUNT);
    //0x181-184, icons at bit 6-0
    return (dev->xram[VMU_XRAM_BANK_ICN][XRAM_OFFSET(SFR_ADDR_XRAM_BASE)+icn+1]>>(VMU_DISP_ICN_COUNT+1-icn)&0x1);
}


#if 0

static void _DrawASCIIDisplay(VMUDevice* dev) {
    for(unsigned y = 0; y < VMU_DISP_PIXEL_HEIGHT; ++y) {
        for(unsigned x = 0; x < VMU_DISP_PIXEL_WIDTH; ++x) {
            if(gyVmuDisplayPixelGet(dev, x, y)) {
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    unsigned spaces = (32-VMU_DISP_ICN_COUNT)/4;
    for(unsigned i = 0; i < VMU_DISP_ICN_COUNT; ++i) {
        if(gyVmuDisplayIconGet(dev, (VMU_DISP_ICN)i)) {
            switch(i) {
            case VMU_DISP_ICN_CLOCK: printf("C"); break;
            case VMU_DISP_ICN_FILE: printf("F"); break;
            case VMU_DISP_ICN_FLASH: printf("M"); break;
            case VMU_DISP_ICN_GAME: printf("G"); break;
            default: break;
            }
        } else {
            for(unsigned j = 0; j < spaces-1; ++j) printf(" ");
        }
        printf("\n");
    }
}


#endif
