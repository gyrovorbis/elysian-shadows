#ifndef GYRO_AUDIO_H
#define GYRO_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

struct _GYWav;
//typedef struct _GYWav GYWav;

int gyAudInit(void);
int gyAudUninit(void);

int gyAudPlayOgg(const char *const filename);
int gyAudStopOgg(void);
void gyAudPlayWav(const struct _GYWav *const wav);


#ifdef __cplusplus
}
#endif

#endif // GYRO_AUDIO_H
