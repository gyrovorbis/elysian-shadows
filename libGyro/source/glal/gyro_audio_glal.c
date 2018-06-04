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
    GY_AL_CLEAR_ERROR();
    alGetError(); //defaults to fucking error on Windows for whatever-the-fuck reason!
    _alDevice = alcOpenDevice(NULL);                                               //Open the device
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_AUDIO_API: Initializing OpenAL Subsystem");
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "OpenAL Vendor",        alGetString(AL_VENDOR));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "OpenAL Renderer",      alGetString(AL_RENDERER));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "OpenAL Version",       alGetString(AL_VERSION));
    const char* devStr = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Device", devStr? devStr : "ERROR");
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "OpenAL Extensions",    alGetString(AL_EXTENSIONS));
        char* device = (char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

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
        _alFmtMono32 = alGetEnumValue("AL_FORMAT_MONO_FLOAT32");
        _alFmtStereo32 = alGetEnumValue("AL_FORMAT_STEREO_FLOAT32");
    } else {
        _gyLog(GY_DEBUG_WARNING, "'AL_EXT_float32' not supported by device.");
    }
    GY_AL_CHECK_ERROR();
    _gyPop(1);

#endif
    return 1;
}

int gyAudUninit(void) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_AUDIO_API: Uninitializing OpenAL!");
    GY_AL_CLEAR_ERROR();
    alcMakeContextCurrent(NULL);
    alcDestroyContext(_alContext);
    alcCloseDevice(_alDevice);
    GY_AL_CHECK_ERROR();
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
    GY_AL_CLEAR_ERROR();
    alGenSources(1, (ALuint*)src);
    alSourcei(*(ALuint*)src, AL_REFERENCE_DISTANCE, _refDist);
    GY_AL_CHECK_ERROR();
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
    GY_AL_CLEAR_ERROR();
    alDeleteSources(1, (ALuint*)src);
    GY_AL_CHECK_ERROR();
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
    GY_AL_CLEAR_ERROR();
    alSourcefv(*(ALuint*)src, alIndex, (void*)&newVec);
    GY_AL_CHECK_ERROR();
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
    GY_AL_CLEAR_ERROR();
    alSourcef(*(ALuint*)src, alIndex, val);
    GY_AL_CHECK_ERROR();
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
    GY_AL_CLEAR_ERROR();
    alGetSourcef(*(ALuint*)src, alIndex, &value);
    GY_AL_CHECK_ERROR();
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
    GY_AL_CLEAR_ERROR();
    alSourcei(*(ALuint*)src, alIndex, val);
    GY_AL_CHECK_ERROR();
#endif
}

void gyAudSourceStop(struct GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    GY_AL_CLEAR_ERROR();
    alSourceStop(*(ALuint*)src);
    alSourcei(*(ALuint*)src, AL_BUFFER, NULL);
    GY_AL_CHECK_ERROR();
#endif
}

int gyAudSourceIsPlaying(struct GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    ALenum state;
#if 0
    GY_AL_CLEAR_ERROR();
    ALuint buf;
    alGetSourcei(*(ALuint*)src, AL_BUFFER, &buf);
    GY_AL_CHECK_ERROR();
    if(buf == 0) return 0;
#endif
    GY_AL_CLEAR_ERROR();
    alGetSourcei(*(ALuint*)src, AL_SOURCE_STATE, &state);
    GY_AL_CHECK_ERROR();
    return (state == AL_PLAYING);
#endif
}

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    GYVector3 veccy = *pos;
    veccy.z *= _zScale;
    GY_AL_CLEAR_ERROR();
    alListenerfv(AL_POSITION, (void*)&veccy);
    GY_AL_CHECK_ERROR();
#endif
}

void gyAudListenerSetGain(const float gain) {
    GY_AL_CLEAR_ERROR();
    alListenerf(AL_GAIN, gain);
    GY_AL_CHECK_ERROR();
}


GYAudBuffer* gyAudBufferCreate(void) {
    ALuint id;

    _gyLog(GY_DEBUG_VERBOSE, "Creating audio buffer.");
    _gyPush();

    GY_AL_CLEAR_ERROR();
    alGenBuffers(1, &id);
    if(GY_AL_CHECK_ERROR()) {
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
    GY_AL_CLEAR_ERROR();
    alDeleteBuffers(1, &id);
    GY_AL_CHECK_ERROR();
    free(buff);

    _gyPop(1);
}

//void gyAudSourceBindBuffer(struct GYAudioSource* src, const struct GYAudBuffer* buff);
//void gyAudSourceUnbindBuffer(struct GYAudSource* src);

void gyAudBufferData(GYAudBuffer* buff, unsigned char* data, int format, size_t size, int freq) {
    GY_AL_CLEAR_ERROR();
    alBufferData(buff->_data, format, (void*)data, size, freq);
    GY_AL_CHECK_ERROR();
}

void gyAudSourcePlayBuffer(struct GYAudioSource* src, const GYAudBuffer* buff) {
    //now we put our data into the openAL buffer and
    //check for success
    GY_AL_CLEAR_ERROR();
    alSourcei(*(ALuint*)src, AL_BUFFER, (ALuint)buff->_data);
    alSourcePlay(*(ALuint*)src);
    GY_AL_CHECK_ERROR();
}

