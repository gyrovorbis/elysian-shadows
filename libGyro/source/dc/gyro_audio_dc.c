#include <gyro_audio_api.h>
#include <gyro_wav_api.h>
#include <gyro_ogg_api.h>

int gyAudInit() {
    return 1;
}

int gyAudUninit() {
    return 1;
}

int gyAudPlayOgg(const char *const filename) {
    return 1;
}


int gyAudStopOgg() {
    return 1;
}


void gyAudPlayWav(const GYWav *const wav) {
//    return 1;
}



GYOgg* gyAudOggCreate(const char*const path) {


}

void gyAudOggDestroy(GYOgg*const ogg) {}
struct GYAudioSource* gyAudOggStreamCreate(void) {

}

void gyAudOggStreamDestroy(struct GYAudioSource*const src) {

}


void gyAudOggStreamPlay(struct GYAudioSource*src, GYOgg*ogg) {

}

void gyAudOggStreamPause(struct GYAudioSource*src) {

}

void gyAudOggStreamStop(struct GYAudioSource*src) {

}

void gyAudOggStreamSync(GYAudioSource*const src1, GYAudioSource*const src2) {

}

void gyAudOggStreamSetLooping(GYAudioSource*const src, const unsigned looping) {

}

double gyAudOggStreamTime(GYAudioSource*const src) {

}


