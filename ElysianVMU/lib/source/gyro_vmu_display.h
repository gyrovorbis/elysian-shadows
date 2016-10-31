#ifndef GYRO_VMU_DISPLAY_H
#define GYRO_VMU_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define VMU_DISP_PIXEL_WIDTH     48
#define VMU_DISP_PIXEL_HEIGHT    32

struct VMUDevice;

typedef enum VMU_XRAM_BANK {
    VMU_XRAM_BANK_LCD_TOP = 0,
    VMU_XRAM_BANK_LCD_BOT,
    VMU_XRAM_BANK_ICN
} VMU_XRAM_BANK;

typedef enum VMU_DISP_ICN {
    VMU_DISP_ICN_FILE,
    VMU_DISP_ICN_GAME,
    VMU_DISP_ICN_CLOCK,
    VMU_DISP_ICN_FLASH,
    VMU_DISP_ICN_COUNT
} VMU_DISP_ICN;

int     gyVmuDisplayPixelGet(const struct VMUDevice* dev, int x, int y);
void    gyVmuDisplayPixelSet(struct VMUDevice* dev, int x, int y, int on);

int     gyVmuDisplayIconGet(const struct VMUDevice* dev, VMU_DISP_ICN icn);
void    gyVmuDisplayIconSet(struct VMUDevice* dev, VMU_DISP_ICN icn, int on);



#ifdef __cplusplus
}
#endif



#endif // GYRO_VMU_DISPLAY_H

