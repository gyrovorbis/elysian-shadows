#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libGyro/gyro_audio_api.h>
#include <libGyro/gyro_wav_api.h>
// #include <libGyro/gyro_matrix_api.h>
#include <libGyro/gyro_system_api.h>

//==== PUBLIC API IMPLEMENTATION =====

int gyAudInit(void) {
    return 1;
}

int gyAudUninit(void) {
    return 1;
}

void gyAudSetReferenceDistance(const int dist) {

}

void gyAudSetZScale(const float scale) {

}
void glalAudSourceAlloc(GYAudioSource*const src) {

}

struct GYAudioSource* gyAudSourceCreate(void) {
  return NULL;
}

void gyAudSourceDestroy(struct GYAudioSource*const src) {

}

void gyAudSourceSetfv(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const GYVector3*const vec) {

}

void gyAudSourceSetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const float val) {

}

float gyAudSourceGetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib) {
  return 0.0f;
}

void gyAudSourceSeti(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const int val) {

}

void gyAudSourceStop(struct GYAudioSource*const src) {

}

int gyAudSourceIsPlaying(struct GYAudioSource*const src) {
  return 0;
}

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos) {

}

void gyAudListenerSetGain(const float gain) {

}


GYAudBuffer* gyAudBufferCreate(void) {
  return nullptr;
}

void gyAudBufferDestroy(GYAudBuffer* buff) {

}

//void gyAudSourceBindBuffer(struct GYAudioSource* src, const struct GYAudBuffer* buff);
//void gyAudSourceUnbindBuffer(struct GYAudSource* src);

void gyAudBufferData(GYAudBuffer* buff, unsigned char* data, int format, size_t size, int freq) {

}

void gyAudSourcePlayBuffer(struct GYAudioSource* src, const GYAudBuffer* buff) {

}
