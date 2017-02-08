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
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
static ALCdevice*	_alDevice	= NULL;
static ALCcontext*	_alContext	= NULL;
#endif
ALenum              _alFmtStereo32  = -1;
ALenum              _alFmtMono32    = -1;
ALboolean           _alExtFloat32   = 0;

//==== PUBLIC API IMPLEMENTATION =====

int gyAudInit(void) {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _alDevice = alcOpenDevice(NULL);                                               //Open the device
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_AUDIO_API: Initializing OpenAL Subsystem");
    _gyPush();

    if(!_alDevice) {
        _gyLog(GY_DEBUG_WARNING, "Could not open OpenAL Device!");
        _gyPop(1);
        return 0;
    }

    _alContext = alcCreateContext(_alDevice, NULL);                                   //Give the device a context
    alcMakeContextCurrent(_alContext);                                             //Make the context the current
    if(!_alContext) {
        _gyLog(GY_DEBUG_WARNING, "Could not make OpenAL Context!");
        _gyPop(1);
        return 0;
    }

    _alExtFloat32 = alIsExtensionPresent("AL_EXT_float32");
    if(_alExtFloat32) {
        _gyLog(GY_DEBUG_VERBOSE, "'AL_EXT_float32' extension supported.");
        _alFmtMono32 = alGetEnumValue("AL_FORMAT_MONO_FLOAT32");
        _alFmtStereo32 = alGetEnumValue("AL_FORMAT_STEREO_FLOAT32");
    } else {
        _gyLog(GY_DEBUG_WARNING, "'AL_EXT_float32' not supported by device.");
    }

    _gyPop(1);
#endif
    return 1;
}

int gyAudUninit(void) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_AUDIO_API: Uninitializing OpenAL!");
    alcMakeContextCurrent(NULL);
    alcDestroyContext(_alContext);
    alcCloseDevice(_alDevice);
#endif
    return 1;
}

void gyAudSetReferenceDistance(const int dist) {
    _refDist = dist;
}

void gyAudSetZScale(const float scale) {
    _zScale = scale;
}
void glalAudSourceAlloc(GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    alGenSources(1, (ALuint*)src);
    alSourcei(*(ALuint*)src, AL_REFERENCE_DISTANCE, _refDist);
    _checkALError("glalAudSourceAlloc(): alSourcei(AL_REFERENCE_DISTANCE)");
#endif
}

struct GYAudioSource* gyAudSourceCreate(void) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    ALuint* src = malloc(sizeof(ALuint));
    glalAudSourceAlloc((void*)src);
    return (GYAudioSource*)src;
#endif
}

void gyAudSourceDestroy(struct GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    if(!src) return;
    alDeleteSources(1, (ALuint*)src);
    _checkALError("gyAudSourceDestroy(): alDeleteSources()");
    free(src);
#endif
}

void gyAudSourceSetfv(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const GYVector3*const vec) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
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
#endif
}

void gyAudSourceSetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const float val) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
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
#endif
}

float gyAudSourceGetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
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
#endif
}

void gyAudSourceSeti(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const int val) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
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
#endif
}

void gyAudSourceStop(struct GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    alSourceStop(*(ALuint*)src);
#endif
}

int gyAudSourceIsPlaying(struct GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    ALenum state;
    alGetSourcei(*(ALuint*)src, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
#endif
}

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    GYVector3 veccy = *pos;
    veccy.z *= _zScale;
    alListenerfv(AL_POSITION, (void*)&veccy);
#endif
}

void gyAudListenerSetGain(const float gain) {
    alListenerf(AL_GAIN, gain);
}


GYAudBuffer* gyAudBufferCreate(void) {
    ALuint id;
    int alErrorCode;

    _gyLog(GY_DEBUG_VERBOSE, "Creating audio buffer.");
    _gyPush();

    alGenBuffers(1, &id);
    if((alErrorCode = alGetError()) != AL_NO_ERROR) {
        _gyLog(GY_DEBUG_WARNING, "! - alGenBuffers() failure - %s", _openALError(alErrorCode));
        return NULL;
    }

    GYAudBuffer* buffer = malloc(sizeof(GYAudBuffer));
    buffer->_data = (ALuint)id;
    buffer->bytes = 0;

    _gyPop(1);

    return buffer;
}

void gyAudBufferDestroy(GYAudBuffer* buff) {
    if(!buff) return;

    _gyLog(GY_DEBUG_VERBOSE, "Destroying audio buffer.");
    _gyPush();

    ALuint id = (ALuint)buff->_data;
    alDeleteBuffers(1, &id);
    _checkALError("gyAudBufferDestroy(): alDeleteBuffers()");
    free(buff);

    _gyPop(1);
}

//void gyAudSourceBindBuffer(struct GYAudioSource* src, const struct GYAudBuffer* buff);
//void gyAudSourceUnbindBuffer(struct GYAudSource* src);

void gyAudBufferData(GYAudBuffer* buff, unsigned char* data, int format, size_t size, int freq) {
    int alErrorCode;
    alBufferData(buff->_data, format, (void*)data, size, freq);
    if((alErrorCode = alGetError()) != AL_NO_ERROR) {
        _gyLog(GY_DEBUG_WARNING, "! - alBufferData() failure - %s", _openALError(alErrorCode));
    }
}

void gyAudSourcePlayBuffer(struct GYAudioSource* src, const GYAudBuffer* buff) {
    //now we put our data into the openAL buffer and
    //check for success
    alSourcei(*(ALuint*)src, AL_BUFFER, (ALuint)buff->_data);
    alSourcePlay(*(ALuint*)src);
}

