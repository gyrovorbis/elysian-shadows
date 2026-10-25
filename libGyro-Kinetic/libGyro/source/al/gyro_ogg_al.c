/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_platform.h>
#if GYRO_PLATFORM != GYRO_PLATFORM_iOS && GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
/* Defined so dickface vorbisfile.h doesn't create static data that
 * we don't give a shit about in this compilation unit. */
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#else
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
#include <ogg.h>

#include <codec.h>
#include <vorbisenc.h>
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbisfile.h>
#endif
#endif

#include <libGyro/gyro_audio_api.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_ogg_api.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_thread_api.h>
#include <stdlib.h>
#include <string.h>

#define GYRO_OGG_STREAM_BUFF_SIZE	(4096 * 20)
#define GYRO_OGG_STREAM_BUFF_COUNT	2
#define GYRO_OGG_THREAD_PRIORITY    GY_THREAD_PRIORITY_HIGH

typedef struct _GYOggPayload {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
	ALuint			buffers[2];
    GYFile*         oggFile;
    OggVorbis_File  oggStream;
    vorbis_info*    vorbisInfo;
    vorbis_comment* vorbisComment;
	ALenum			format;
#endif
} GYOggPayload;
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
static inline const char* _vorbisErrorString(const int code) {
    switch(code) {
        case OV_EREAD:
            return "Read from media";
        case OV_ENOTVORBIS:
            return "Not Vorbis data";
        case OV_EVERSION:
            return "Vorbis version mismatch";
        case OV_EBADHEADER:
            return "Invalid Vorbis header";
        case OV_EFAULT:
            return "Internal logic fault (bug or heap/stack corruption";
        default:
            return "Unknown Ogg error";
    }
}

static size_t __ov_read_fn(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    size_t bytes_read = 0;
    gyFileRead(datasource, ptr, size * nmemb, &bytes_read);
    return bytes_read;
}

static int __ov_seek_fn(void *datasource, ogg_int64_t offset, int whence)
{
    return !gyFileSeek(datasource, offset, whence);
}

static int __ov_close_fn(void *datasource)
{
    return !gyFileClose((GYFile**)&datasource);
}

static long int __ov_tell_fn(void *datasource)
{
    long int pos = 0;
    gyFileTell(datasource, &pos);
    return pos;
}
#endif
GYOgg* gyAudOggCreate(const char*const path) {
    int result;
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID

    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_AUDIO] Loading OGG file: %s", path);
    _gyPush();

    GYOgg* ogg = malloc(sizeof(GYOgg));
    GYOggPayload* payload = ogg->_data = malloc(sizeof(GYOggPayload));

    ov_callbacks callbacks;
    callbacks.read_func = &__ov_read_fn;
    callbacks.seek_func = &__ov_seek_fn;
    callbacks.close_func = &__ov_close_fn;
    callbacks.tell_func = &__ov_tell_fn;

    strcpy(ogg->path, path);
    ogg->looping = 0;

    if(!gyFileOpen(path, "rb", &payload->oggFile)) {
        _gyLog(GY_DEBUG_WARNING, "Could not open ogg file %s", path);
        free(payload);
        free(ogg);
        _gyPop(1);
        return NULL;
    }

    if((result = ov_open_callbacks(payload->oggFile, &payload->oggStream, NULL, 0, callbacks)) < 0) {
        _gyLog(GY_DEBUG_WARNING, "Could not open OGG stream: %s", _vorbisErrorString(result));
        gyFileClose(&payload->oggFile);
        free(payload);
        free(ogg);
        _gyPop(1);
        return NULL;
    }

    payload->vorbisInfo = ov_info(&payload->oggStream, -1);
    payload->vorbisComment = ov_comment(&payload->oggStream, -1);

    payload->format = (payload->vorbisInfo->channels == 1)?
        AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    GY_AL_CLEAR_ERROR();
    alGenBuffers(2, payload->buffers);
    GY_AL_CHECK_ERROR();

    /* Debug stuff. */

    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "OGG Version", payload->vorbisInfo->version);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "Channels", payload->vorbisInfo->channels);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "Rate (Hz)", payload->vorbisInfo->rate);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "Bitrate Upper", payload->vorbisInfo->bitrate_upper);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "Bitrate Nominal", payload->vorbisInfo->bitrate_nominal);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "Bitrate Lower", payload->vorbisInfo->bitrate_lower);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %20d", "Bitrate Window", payload->vorbisInfo->bitrate_window);

    _gyPop(1);
    return ogg;
#endif
}

void gyAudOggDestroy(GYOgg*const ogg) {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    if(!ogg) return;
    GYOggPayload* payload = ogg->_data;

    //Homeboy only deleted one, but I think that's a memory leak!
    GY_AL_CLEAR_ERROR();
    alDeleteBuffers(2, payload->buffers);
    GY_AL_CHECK_ERROR();

    ov_clear(&payload->oggStream);

    free(payload);
    free(ogg);
#endif
}

// Streams data from open OGG file into given buffer
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
static inline int _streamOgg(const ALuint buffer, GYOgg*const ogg) {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    char pcm[GYRO_OGG_STREAM_BUFF_SIZE];
    int  size = 0;
    int  section;
    int  result;
    GYOggPayload* payload = ogg->_data;

    while(size < GYRO_OGG_STREAM_BUFF_SIZE)
    {
        result = ov_read(&payload->oggStream, pcm + size, GYRO_OGG_STREAM_BUFF_SIZE - size, 0, 2, 1, &section);

        if(result > 0) {
            size += result;
        } else {
            if(result < 0) {
                _gyLog(GY_DEBUG_WARNING, "Error streaming OGG file: %s", _vorbisErrorString(result));
                return 0;
             } else {
                if(ogg->looping)
                    ov_pcm_seek(&payload->oggStream, 0);
                else
                    break;
            }
        }
    }

    if(size != 0) {
        GY_AL_CLEAR_ERROR();
        alBufferData(buffer, payload->format, pcm, size, payload->vorbisInfo->rate);
        GY_AL_CHECK_ERROR();
        return 1;
    }
#endif
    return 0;
}
#endif

//======= OGG STREAM THREADICISM =======

typedef struct _OggStreamSource {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    ALuint				src;				//polymorphically compatible with GYAudioSource
    GYOgg*				ogg;
    GYThread*           thread;
    unsigned			stillStreaming;		//boolean for whether we have work or not
    GYMutex*            streamMutex;
    GYCondition*        tryStreamCond;
    unsigned			killMe;
    unsigned			looping;
#endif
} _OggStreamSource;

static int _gyAudOggStreamUpdate(void* data) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource*const stream = data;
    int processed;
    int done;
    int success = 1;

    success &= gyThreadPriority(GYRO_OGG_THREAD_PRIORITY);

    while(1) {
        done = 0;

        //Protect against ANY other stream-related operations while updating
        success &= gyMutexLock(stream->streamMutex);
        while(!stream->stillStreaming) {
            success &= gyConditionWait(stream->tryStreamCond, stream->streamMutex); /* wait for master to signal to resume stream */
        }

        //parent wants us dead (cleaning up stream)
        if(stream->killMe) {
            success &= gyMutexUnlock(stream->streamMutex);
            break; //He had a good run...
        }

        //Update stream if we're playing
        if(gyAudSourceIsPlaying((GYAudioSource*)stream)) {
            GY_AL_CLEAR_ERROR();
            alGetSourcei(stream->src, AL_BUFFERS_PROCESSED, &processed);
            GY_AL_CHECK_ERROR();

            while(processed--) {
                ALuint buffer;

                alSourceUnqueueBuffers(stream->src, 1, &buffer);
                if(!_streamOgg(buffer, stream->ogg)) {
                    done = 1;
                    break;
                }
                GY_AL_CLEAR_ERROR();
                alSourceQueueBuffers(stream->src, 1, &buffer);
                GY_AL_CHECK_ERROR();
            }
        } else done = 1;	//otherwise we're done

        if(done) stream->stillStreaming = 0;

        success &= gyMutexUnlock(stream->streamMutex);
    }

    return success;
#endif
}

struct GYAudioSource* gyAudOggStreamCreate(const char* threadName) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource*const stream = malloc(sizeof(_OggStreamSource));

    //Init source
    glalAudSourceAlloc((void*)stream);

    stream->killMe = 0;
    stream->ogg = NULL;
    stream->stillStreaming = 0;
    stream->looping = 1;

    /* Initialize thread and synchronization primitives */
    stream->streamMutex     = gyMutexCreate();
    stream->tryStreamCond   = gyConditionCreate();

    //spawn the thread
    if(!threadName) threadName = "Ogg Decoder Thread";
    stream->thread          = gyThreadCreate(threadName, &_gyAudOggStreamUpdate, stream);
    return (GYAudioSource*)stream;
#endif
}

void gyAudOggStreamDestroy(struct GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    struct _OggStreamSource*const stream = (void*)src;

    //Kill the stream thread
    stream->killMe = 1;							//flag thread to commit suicide
    gyMutexLock(stream->streamMutex);
    gyConditionSignal(stream->tryStreamCond);
    stream->stillStreaming = 1;					//wake its bitch ass up
    gyMutexUnlock(stream->streamMutex);
    gyThreadWait(stream->thread);            /* await its death */

    //free threading resources
    gyMutexDestroy(stream->streamMutex);
    gyConditionDestroy(stream->tryStreamCond);

    //kill alSource
    gyAudSourceDestroy(src);
#endif
}


void gyAudOggStreamPlay(struct GYAudioSource*src, GYOgg*ogg) {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource* stream = (void*)src;

    if(!ogg) {
        _gyLog(GY_DEBUG_ERROR, "gyAudStreamPlay() - Could not play OGG file! (NULL pointer)");
        return;
    }

    GYOggPayload* payload = ogg->_data;
    int validBuffers = 0;

    if(gyAudSourceIsPlaying(src))
        gyAudOggStreamPause(src);

    gyMutexLock(stream->streamMutex);

    if(_streamOgg(payload->buffers[0], ogg))
        ++validBuffers;

    if(_streamOgg(payload->buffers[1], ogg))
        ++validBuffers;

    if(validBuffers) {
        GY_AL_CLEAR_ERROR();
        alSourceQueueBuffers(*(ALuint*)src, validBuffers, payload->buffers);
        alSourcePlay(*(ALuint*)src);
        GY_AL_CHECK_ERROR();

        stream->ogg = ogg;
        stream->ogg->looping = stream->looping;

        stream->stillStreaming = 1;
        gyConditionSignal(stream->tryStreamCond);
    }

    gyMutexUnlock(stream->streamMutex);
#endif
}

void gyAudOggStreamPause(struct GYAudioSource*src) {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource* stream = (void*)src;
    int queued;

    gyMutexLock(stream->streamMutex);

    alSourceStop(stream->src);
    alGetSourcei(stream->src, AL_BUFFERS_QUEUED, &queued);

    while(queued--) {
        ALuint buffer;
        GY_AL_CLEAR_ERROR();
        alSourceUnqueueBuffers(stream->src, 1, &buffer);
        GY_AL_CHECK_ERROR();
    }
    alSourcei(*(ALuint*)src, AL_BUFFER, NULL);

    stream->stillStreaming = 0;

    gyMutexUnlock(stream->streamMutex);
#endif
}

void gyAudOggStreamStop(struct GYAudioSource*src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource* stream = (void*)src;
    if(!stream || !stream->ogg) return;
    GYOggPayload* payload = stream->ogg->_data;

    gyMutexLock(stream->streamMutex);

    gyAudOggStreamPause(src);
    ov_pcm_seek(&payload->oggStream, 0);
    stream->ogg = NULL;

    gyMutexUnlock(stream->streamMutex);
#endif
}

void gyAudOggStreamSync(GYAudioSource*const src1, GYAudioSource*const src2) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource* stream1 = (_OggStreamSource*)src1;
    _OggStreamSource* stream2 = (_OggStreamSource*)src2;
    GYOggPayload* payload1 = stream1->ogg->_data;
    GYOggPayload* payload2 = stream2->ogg->_data;

    double time;
    gyMutexLock(stream1->streamMutex);
    gyMutexLock(stream2->streamMutex);

    time = ov_time_tell(&payload1->oggStream);
    ov_time_seek_lap(&payload2->oggStream, time);

    gyMutexUnlock(stream1->streamMutex);
    gyMutexUnlock(stream2->streamMutex);
#endif
}

void gyAudOggStreamSetLooping(GYAudioSource*const src, const unsigned looping) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource* stream = (_OggStreamSource*)src;
    GYOgg *ogg = stream->ogg;
    //lock?
    stream->looping = looping;
    if(ogg) ogg->looping = looping;
#endif
}

double gyAudOggStreamTime(GYAudioSource*const src) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _OggStreamSource* stream = (_OggStreamSource*)src;
    GYOggPayload *ogg = stream->ogg->_data;

    //lock? Not sure if this does any writing internally... better fucking not...
    return ov_time_tell(&ogg->oggStream);
#endif
}


