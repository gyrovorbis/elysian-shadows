#include <pthread.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
/* Defined so dickface vorbisfile.h doesn't create static data that
 * we don't give a shit about in this compilation unit. */
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>
#include <gyro_audio_api.h>
#include <gyro_file_api.h>
#include <gyro_ogg_api.h>
#include <gyro_system_api.h>
#include <stdlib.h>
#include <string.h>

#define GYRO_OGG_STREAM_BUFF_SIZE	(4096 * 20)
#define GYRO_OGG_STREAM_BUFF_COUNT	2

typedef struct _GYOggPayload {
	ALuint			buffers[2];
	GYFile*           oggFile;
	OggVorbis_File  oggStream;
	vorbis_info*    vorbisInfo;
	vorbis_comment* vorbisComment;
	ALenum			format;
} GYOggPayload;

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

GYOgg* gyAudOggCreate(const char*const path) {
	int result;
	int alErrorCode;
	char* device = (char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

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
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not open ogg file %s", path);
		free(payload);
		free(ogg);
		return NULL;
	}

	if((result = ov_open_callbacks(payload->oggFile, &payload->oggStream, NULL, 0, callbacks)) < 0) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not open OGG stream: %s", _vorbisErrorString(result));
        gyFileClose(&payload->oggFile);
		free(payload);
		free(ogg);
		return NULL;
	}

	payload->vorbisInfo = ov_info(&payload->oggStream, -1);
	payload->vorbisComment = ov_comment(&payload->oggStream, -1);

	payload->format = (payload->vorbisInfo->channels == 1)?
		AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	alGenBuffers(2, payload->buffers);
	if((alErrorCode = alGetError()) != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not generate OpenAL buffers for OGG stream: %s", _openALError(alErrorCode));
	}

	/* Debug stuff. */

	printf("Device			- %s\n", device);
	printf("Version			- %d\n", payload->vorbisInfo->version);
	printf("Channels		- %d\n", payload->vorbisInfo->channels);
	printf("Rate (hz)		- %ld\n", payload->vorbisInfo->rate);
	printf("bitrate upper	- %ld\n", payload->vorbisInfo->bitrate_upper);
	printf("bitrate nominal - %ld\n", payload->vorbisInfo->bitrate_nominal);
	printf("bitrate lower	- %ld\n", payload->vorbisInfo->bitrate_lower);
	printf("bitrate window	- %ld\n", payload->vorbisInfo->bitrate_window);
	return ogg;
}

void gyAudOggDestroy(GYOgg*const ogg) {
	GYOggPayload* payload = ogg->_data;
	int errorCode;

	//Homeboy only deleted one, but I think that's a memory leak!
	alDeleteBuffers(2, payload->buffers);
	if((errorCode = alGetError()) != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not delete OpenAL buffers from OGG stream: %s", _openALError(errorCode));
	}

	ov_clear(&payload->oggStream);

	free(payload);
	free(ogg);

}

// Streams data from open OGG file into given buffer
static inline int _streamOgg(const ALuint buffer, GYOgg*const ogg) {
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
				_gyLog(GY_DEBUG_CRITICAL, "! - Error streaming OGG file: %s", _vorbisErrorString(result));
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
		alBufferData(buffer, payload->format, pcm, size, payload->vorbisInfo->rate);
		_checkALError();
		return 1;
	}

	return 0;
}

//======= OGG STREAM PTHREADICISM =======

typedef struct __OggStreamSource {
	ALuint				src;				//polymorphically compatible with GYAudioSource
	GYOgg*				ogg;
	pthread_t			thread;
	unsigned			stillStreaming;		//boolean for whether we have work or not
	pthread_mutex_t		streamMutex;
	pthread_mutexattr_t	streamMutexAttr;
	pthread_cond_t		tryStreamCond;
	unsigned			killMe;
	unsigned			looping;
} _OggStreamSource;

static void* _gyAudOggStreamUpdate(void* data) {
	_OggStreamSource*const stream = data;
	int processed;
	int done;

	while(1) {
		done = 0;

		//Protect against ANY other stream-related operations while updating
		pthread_mutex_lock(&stream->streamMutex);
		while(!stream->stillStreaming) {
			pthread_cond_wait(&stream->tryStreamCond, &stream->streamMutex);	//wait for master to signal to resume stream
		}

		//parent wants us dead (cleaning up stream)
		if(stream->killMe) {
			pthread_mutex_unlock(&stream->streamMutex);
			break; //He had a good run...
		}

		//Update stream if we're playing
		if(gyAudSourceIsPlaying((GYAudioSource*)stream)) {

			alGetSourcei(stream->src, AL_BUFFERS_PROCESSED, &processed);
			_checkALError();

			while(processed--) {
				ALuint buffer;

				alSourceUnqueueBuffers(stream->src, 1, &buffer);
				if(!_streamOgg(buffer, stream->ogg)) {
					done = 1;
					break;
				}

				alSourceQueueBuffers(stream->src, 1, &buffer);
				_checkALError();
			}
		} else done = 1;	//otherwise we're done

		if(done) stream->stillStreaming = 0;

		pthread_mutex_unlock(&stream->streamMutex);
	}

	return NULL;
}

GYAudioSource* gyAudOggStreamCreate(void) {
	_OggStreamSource*const stream = malloc(sizeof(_OggStreamSource));

	//Init source
	glalAudSourceAlloc((void*)stream);

	stream->killMe = 0;
	stream->ogg = NULL;
	stream->stillStreaming = 0;
	stream->looping = 1;

	//Initialize thread and synchronization primitives
	pthread_mutexattr_init(&stream->streamMutexAttr);
	pthread_mutexattr_settype(&stream->streamMutexAttr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&stream->streamMutex, &stream->streamMutexAttr);
	pthread_cond_init(&stream->tryStreamCond, NULL);

	//spawn the thread
	pthread_create(&stream->thread, NULL, &_gyAudOggStreamUpdate, stream);
	return (GYAudioSource*)stream;
}

void gyAudOggStreamDestroy(GYAudioSource*const src) {
	_OggStreamSource*const stream = (void*)src;

	//Kill the stream thread
	stream->killMe = 1;										//flag thread to commit suicide
	pthread_mutex_lock(&stream->streamMutex);
	pthread_cond_signal(&stream->tryStreamCond);
	stream->stillStreaming = 1;								//wake its bitch ass up
	pthread_mutex_unlock(&stream->streamMutex);
	pthread_join(stream->thread, NULL);						//await its death

	//free threading resources
	pthread_mutex_destroy(&stream->streamMutex);
	pthread_mutexattr_destroy(&stream->streamMutexAttr);
	pthread_cond_destroy(&stream->tryStreamCond);

	//kill alSource
	gyAudSourceDestroy(src);
}


void gyAudOggStreamPlay(GYAudioSource*src, GYOgg*ogg) {
	_OggStreamSource* stream = (void*)src;
	GYOggPayload* payload = ogg->_data;
	int validBuffers = 0;

	if(gyAudSourceIsPlaying(src))
		gyAudOggStreamPause(src);

	pthread_mutex_lock(&stream->streamMutex);

	if(_streamOgg(payload->buffers[0], ogg))
		++validBuffers;

	if(_streamOgg(payload->buffers[1], ogg))
		++validBuffers;

	if(validBuffers) {
		alSourceQueueBuffers(*(ALuint*)src, validBuffers, payload->buffers);
		alSourcePlay(*(ALuint*)src);
		_checkALError();

		stream->ogg = ogg;
		stream->ogg->looping = stream->looping;

		stream->stillStreaming = 1;
		pthread_cond_signal(&stream->tryStreamCond);
	}

	pthread_mutex_unlock(&stream->streamMutex);
}

void gyAudOggStreamPause(GYAudioSource*src) {
	_OggStreamSource* stream = (void*)src;
	int queued;

	pthread_mutex_lock(&stream->streamMutex);

	alSourceStop(stream->src);
	alGetSourcei(stream->src, AL_BUFFERS_QUEUED, &queued);

	while(queued--) {
		ALuint buffer;

		alSourceUnqueueBuffers(stream->src, 1, &buffer);
		_checkALError();
	}

	stream->stillStreaming = 0;

	pthread_mutex_unlock(&stream->streamMutex);
}

void gyAudOggStreamStop(GYAudioSource*src) {
	_OggStreamSource* stream = (void*)src;
	if(!stream || !stream->ogg) return;
	GYOggPayload* payload = stream->ogg->_data;

	pthread_mutex_lock(&stream->streamMutex);

	gyAudOggStreamPause(src);
	ov_pcm_seek(&payload->oggStream, 0);

	pthread_mutex_unlock(&stream->streamMutex);
}

void gyAudOggStreamSync(GYAudioSource*const src1, GYAudioSource*const src2) {
	_OggStreamSource* stream1 = (_OggStreamSource*)src1;
	_OggStreamSource* stream2 = (_OggStreamSource*)src2;
	GYOggPayload* payload1 = stream1->ogg->_data;
	GYOggPayload* payload2 = stream2->ogg->_data;

	double time;
	pthread_mutex_lock(&stream1->streamMutex);
	pthread_mutex_lock(&stream2->streamMutex);

	time = ov_time_tell(&payload1->oggStream);
	ov_time_seek_lap(&payload2->oggStream, time);

	pthread_mutex_unlock(&stream1->streamMutex);
	pthread_mutex_unlock(&stream2->streamMutex);

}

void gyAudOggStreamSetLooping(GYAudioSource*const src, const unsigned looping) {
	_OggStreamSource* stream = (_OggStreamSource*)src;
	GYOgg *ogg = stream->ogg;
	//lock?
	stream->looping = looping;
	if(ogg) ogg->looping = looping;
}

double gyAudOggStreamTime(GYAudioSource*const src) {
	_OggStreamSource* stream = (_OggStreamSource*)src;
	GYOggPayload *ogg = stream->ogg->_data;

	//lock? Not sure if this does any writing internally... better fucking not...
	return ov_time_tell(&ogg->oggStream);
}


