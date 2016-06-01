#include "gyro_vmu_buzzer.h"

void playSoundAL(int period, int pulseWidth, int db);


static unsigned char _freqResponse[0xff] = {
    [0xe0 ... 0xe6] = 62,
    [0xe7 ... 0xe8] = 63,
    [0xe9]          = 64,
    [0xea ... 0xec] = 63,
    [0xed ... 0xef] = 64,
    [0xf0]          = 66,
    [0xf1]          = 65,
    [0xf2]          = 66,
    [0xf3 ... 0xf6] = 64,
    [0xf7]          = 65,
    [0xf8]          = 66,
    [0xf9]          = 69,
    [0xfa]          = 71,
    [0xfb]          = 72,
    [0xfc]          = 70,
    [0xfd]          = 69,
    [0xfe]          = 66
};

void gyVmuBuzzerPlaySound(int period, int pulseWidth, int db) {
 //   printf("\a");
    playSoundAL(period, pulseWidth, (pulseWidth == period*0.5f)? _freqResponse[256-period] : 0);
}
