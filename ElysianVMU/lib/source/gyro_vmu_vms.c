#include "gyro_vmu_vms.h"
#include <gyro_system_api.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>


int gyVmuVMSFileInfoCrcCalc(const unsigned char *buf, int size) {
    int i, c, n = 0;
     for (i = 0; i < size; i++)
     {
       n ^= (buf[i]<<8);
       for (c = 0; c < 8; c++)
         if (n & 0x8000)
       n = (n << 1) ^ 4129;
         else
       n = (n << 1);
     }
     return n & 0xffff;
}

void gyVmuPrintVMSFileInfo(const VMSFileInfo* vms) {
    char string[33];

    assert(sizeof(VMSFileInfo) == VMU_VMS_FILE_INFO_SIZE);

    _gyLog(GY_DEBUG_VERBOSE, "VMS File Info");
    _gyPush();

    memcpy(string, vms->vmuDesc, sizeof(vms->vmuDesc));
    string[sizeof(vms->vmuDesc)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "VMU Description",          string);
    memcpy(string, vms->dcDesc, sizeof(vms->dcDesc));
    string[sizeof(vms->dcDesc)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "DC Description",           string);
    memcpy(string, vms->creatorApp, sizeof(vms->creatorApp));
    string[sizeof(vms->creatorApp)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Creator Application",      string);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Icon Count",               vms->iconCount);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Animation Speed",          vms->animSpeed);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Eyecatch Type",            vms->eyecatchType);
    //Valid/invalid check!
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "CRC",                      vms->crc);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Data Size",                vms->dataBytes);
    memcpy(string, vms->reserved, sizeof(vms->reserved));
    string[sizeof(vms->reserved)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Reserved",                 string);

    _gyPop(1);
}

uint16_t** gyVmuVMSFileInfoCreateIconsARGB444(const VMSFileInfo* vms) {
    if(vms->iconCount == 0) return NULL;

    uint16_t **icons = malloc(sizeof(uint16_t*)*vms->iconCount);

    for(unsigned i = 0 ; i < vms->iconCount; ++i) {
        icons[i] = malloc(sizeof(uint16_t)*VMU_VMS_ICON_BITMAP_WIDTH*VMU_VMS_ICON_BITMAP_HEIGHT);
        unsigned char *data = ((unsigned char*)vms)+sizeof(VMSFileInfo);
        data += i*VMU_VMS_ICON_BITMAP_SIZE;

        for(unsigned b = 0; b < VMU_VMS_ICON_BITMAP_SIZE*2; ++b) {
            const unsigned char palIndex = b%2? data[b/2]&0xf : (data[b/2]>>4)&0xf;
            //assert(palIndex < VMU_VMS_ICON_PALETTE_SIZE);
            icons[i][b] = vms->palette[palIndex];
        }
    }

    return icons;
}

void gyVmuVmsHeaderVmuDescGet(const VMSFileInfo* vms, char* string) {
    memcpy(string, vms->vmuDesc, sizeof(vms->vmuDesc));
    string[sizeof(vms->vmuDesc)] = 0;
}
