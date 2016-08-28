#ifndef GYRO_VMU_BUZZER_H
#define GYRO_VMU_BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

struct GYAudioSource;
struct GYAudBuffer;
struct VMUDevice;

typedef struct VMUBuzzer {
    struct GYAudioSource*   _audioSrc;
    struct GYAudBuffer*     _audioBuff;
    int                     _enabled;
    int                     _soundPlaying;
    int                     _pulseWidth;
    int                     _period;
} VMUBuzzer;

int     gyVmuBuzzerInit(struct VMUDevice* dev);
int     gyVmuBuzzerUninit(struct VMUDevice* dev);
void    gyVmuBuzzerReset(struct VMUDevice* dev);

void    gyVmuBuzzerSoundPlay(struct VMUDevice* dev, int period, int pulseWidth, int db);
void    gyVmuBuzzerSoundStop(struct VMUDevice* dev);
int     gyVmuBuzzerEnabled(const struct VMUDevice* dev);
void    gyVmuBuzzerEnabledSet(struct VMUDevice* dev, int value);

#ifdef __cplusplus
}
#endif



#endif // GYRO_VMU_SOUND_H

