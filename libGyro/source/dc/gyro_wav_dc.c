#include <gyro_wav_api.h>
#include <gyro_audio_api.h>
#include <gyro_matrix_api.h>

int _gyAudWavAlloc(GYWav *const wav) {
    return 1;
}

int _gyAudWavFree(GYWav *const wav) {
    return 1;
}

void gyAudSourcePlayWav(GYAudioSource*const src, const struct GYWav* wav) {
    if(!wav) return;

}


void gyAudSetReferenceDistance(const int dist) {
}

void gyAudSetZScale(const float scale) {
}
void glalAudSourceAlloc(GYAudioSource*const src) {

}

struct GYAudioSource* gyAudSourceCreate(void) {

}

void gyAudSourceDestroy(struct GYAudioSource*const src) {

}

void gyAudSourceSetfv(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const GYVector3*const vec) {


}

void gyAudSourceSetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const float val) {


}

float gyAudSourceGetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib) {

}

void gyAudSourceSeti(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const int val) {

}

void gyAudSourceStop(struct GYAudioSource*const src) {
}

int gyAudSourceIsPlaying(struct GYAudioSource*const src) {

}

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos) {

}

