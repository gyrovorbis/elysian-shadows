#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gyro_audio_api.h>
#include <gyro_wav_api.h>
#include <gyro_matrix_api.h>
#include <gyro_system_api.h>

static float		_zScale		= 1.0f;
static int			_refDist	= 1;
static ALCdevice*	_alDevice	= NULL;
static ALCcontext*	_alContext	= NULL;

//==== PUBLIC API IMPLEMENTATION =====

int gyAudInit(void) {
    _alDevice = alcOpenDevice(NULL);                                               //Open the device
    if(!_alDevice) {
        _gyLog(GY_DEBUG_WARNING, "Could not open OpenAL Device!");
        return 0;
    }
    _alContext = alcCreateContext(_alDevice, NULL);                                   //Give the device a context
    alcMakeContextCurrent(_alContext);                                             //Make the context the current
    if(!_alContext) {
        _gyLog(GY_DEBUG_WARNING, "Could not make OpenAL Context!");
        return 0;
    }

    return 1;
}

int gyAudUninit(void) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(_alContext);
    alcCloseDevice(_alDevice);
    return 1;
}

void gyAudSetReferenceDistance(const int dist) {
    _refDist = dist;
}

void gyAudSetZScale(const float scale) {
    _zScale = scale;
}
void glalAudSourceAlloc(GYAudioSource*const src) {

    alGenSources(1, (ALuint*)src);
    alSourcei(*(ALuint*)src, AL_REFERENCE_DISTANCE, _refDist);
    _checkALError();
}

struct GYAudioSource* gyAudSourceCreate(void) {
    ALuint* src = malloc(sizeof(ALuint));
    glalAudSourceAlloc((void*)src);
    return (GYAudioSource*)src;
}

void gyAudSourceDestroy(struct GYAudioSource*const src) {
    if(!src) return;
    alDeleteSources(1, (ALuint*)src);
    _checkALError();
    free(src);
}

void gyAudSourceSetfv(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const GYVector3*const vec) {
    int alIndex;
    GYVector3 newVec = *vec;

    switch(attrib) {
    case GY_SOURCE_POS:
        alIndex = AL_POSITION;
        break;
    case GY_SOURCE_VEL:
        alIndex = AL_VELOCITY;
        break;
    default: return;
    }

    newVec.z *= _zScale;
    alSourcefv(*(ALuint*)src, alIndex, (void*)&newVec);

}

void gyAudSourceSetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const float val) {
    int alIndex;
    switch(attrib) {
    case GY_SOURCE_GAIN:
        alIndex = AL_GAIN;
        break;
    case GY_SOURCE_PITCH:
        alIndex = AL_PITCH;
        break;
    case GY_SOURCE_ROLLOFF_FACTOR:
        alIndex = AL_ROLLOFF_FACTOR;
        break;
    default: return;
    }

    alSourcef(*(ALuint*)src, alIndex, val);

}

float gyAudSourceGetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib) {
    int alIndex;
    switch(attrib) {
    case GY_SOURCE_GAIN:
        alIndex = AL_GAIN;
        break;
    case GY_SOURCE_PITCH:
        alIndex = AL_PITCH;
        break;
    case GY_SOURCE_ROLLOFF_FACTOR:
        alIndex = AL_ROLLOFF_FACTOR;
        break;
    default: return 0.0f;
    }

    float value;

    alGetSourcef(*(ALuint*)src, alIndex, &value);
    return value;
}

void gyAudSourceSeti(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const int val) {
    int alIndex;
    switch(attrib) {
    case GY_SOURCE_LOOPING:
        alIndex = AL_LOOPING;
        break;
    case GY_SOURCE_RELATIVE:
        alIndex = AL_SOURCE_RELATIVE;
        break;
    default: return;
    }

    alSourcei(*(ALuint*)src, alIndex, val);
}

void gyAudSourceStop(struct GYAudioSource*const src) {
    alSourceStop(*(ALuint*)src);
}

int gyAudSourceIsPlaying(struct GYAudioSource*const src) {
    ALenum state;
    alGetSourcei(*(ALuint*)src, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos) {
    GYVector3 veccy = *pos;
    veccy.z *= _zScale;
    alListenerfv(AL_POSITION, (void*)&veccy);
}

