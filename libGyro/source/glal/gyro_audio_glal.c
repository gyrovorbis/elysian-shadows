#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "gyro_audio.h"
#include "gyro_wav.h"
#include "gyro_matrix.h"
#include "glal/gyro_glal.h"
#include "gyro_system.h"

static float _zScale = 1.0f;
static int _refDist = 1;
static ALCdevice *_alDevice;
static ALCcontext *_alContext;

//==== PUBLIC API IMPLEMENTATION =====

int gyAudInit(void) {
	_alDevice = alcOpenDevice(NULL);                                               //Open the device
	if(!_alDevice) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not open OpenAL Device!");
		return 0;
	}
	_alContext = alcCreateContext(_alDevice, NULL);                                   //Give the device a context
	alcMakeContextCurrent(_alContext);                                             //Make the context the current
	if(!_alContext) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not make OpenAL Context!");
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

static inline const char* _openALError(const int errorCode) {
	switch (errorCode) {
	case AL_NO_ERROR:
		return "AL_NO_ERROR";
	case AL_INVALID_NAME:
		return "AL_INVALID_NAME";
	case AL_INVALID_ENUM:
		return "AL_INVALID_ENUM";
	case AL_INVALID_VALUE:
		return "AL_INVALID_VALUE";
	case AL_INVALID_OPERATION:
		return "AL_INVALID_OPERATION";
	case AL_OUT_OF_MEMORY:
		return "AL_OUT_OF_MEMORY";
	default:
		return "No such error code";
	}
}

static inline int _checkALError(void) {
	int error = alGetError();

	if(error != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - OpenAL Error raised: %s", _openALError(error));
		return 1;
	}
	return 0;
}

//audio source
GYWavSource* gyAudSourceCreate(void) {
	ALuint* src = malloc(sizeof(ALuint));
	alGenSources(1, src);
	alSourcei(*src, AL_REFERENCE_DISTANCE, _refDist);
	_checkALError();
	return src;
}

void gyAudSourceDestroy(GYWavSource*const src) {
	int errorCode;
	if(!src) return;
	alDeleteSources(1, src);
	_checkALError();
	free(src);
}

void gyAudSourceSetfv(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const GYVector3*const vec) {
	int alIndex;
	GYVector3 newVec = *vec;

	switch(attrib) {
	case GY_SOURCE_POS:
		alIndex = AL_POSITION;
		break;
	case GY_SOURCE_VEL:
		alIndex = AL_VELOCITY;
		break;
	}

	newVec.z *= _zScale;
	alSourcefv(*src, alIndex, &newVec);

}

void gyAudSourceSetf(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const float val) {
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
	}

	alSourcef(*src, alIndex, val);

}

float gyAudSourceGetf(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib) {
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
	}

	float value;

	alGetSourcef(*src, alIndex, &value);
	return value;
}

void gyAudSourceSeti(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const int val) {
	int alIndex;
	switch(attrib) {
	case GY_SOURCE_LOOPING:
		alIndex = AL_LOOPING;
		break;
	case GY_SOURCE_RELATIVE:
		alIndex = AL_SOURCE_RELATIVE;
	}

	alSourcei(*src, alIndex, val);
}

void gyAudSourceStop(GYWavSource*const src) {
	alSourceStop(*src);
}

int gyAudSourceIsPlaying(GYWavSource*const src) {
	ALenum state;
	alGetSourcei(*src, AL_SOURCE_STATE, &state);

	return (state == AL_PLAYING);
}

typedef struct _GYAudWavPayload {
	ALsizei size, freq;
	ALenum 	format;
	ALuint buffer;
	char *data;
} GYAudWavPayload;

void gyAudSourcePlayWav(GYWavSource*const src, const GYWav* wav) {
	const GYAudWavPayload* payload = wav->_data;
	alSourcei(*src, AL_BUFFER, payload->buffer);
	alSourcePlay(*src);
}

//audio listener
void gyAudListenerSetPos(const struct _GYVector3* const pos) {
	GYVector3 veccy = *pos;
	veccy.z *= _zScale;
	alListenerfv(AL_POSITION, &veccy);
}

//===== PRIVATE =======


int _gyAudWavFree(GYWav *const wav) {
	int errorCode;
	GYAudWavPayload *payload = wav->_data;

	alDeleteBuffers(1, &payload->buffer);
	_checkALError();
	free(payload);
	return 1;
}

/*
 * WAV Loader stolen and anti-C++-itized from Oliver Plunkett:
 * http://www.dunsanyinteractive.com/blogs/oliver/?p=72
 */

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
typedef struct __WAVE_RIFF_Header {
	char chunkID[4];
	uint32_t chunkSize;//size not including chunkSize or chunkID
	char format[4];
} _WAVE_RIFF_Header;

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
typedef struct __WAVE_Format {
	char subChunkID[4];
	uint32_t subChunkSize;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
} _WAVE_Format;

/*
* Struct to hold the data of the wave file
*/
typedef struct __WAVE_Data {
	char subChunkID[4]; //should contain the word data
	uint32_t subChunk2Size; //Stores the size of the data block
} _WAVE_Data;

/*
 * Load wave file function. No need for ALUT with this
 */
static int _loadWavFile(const char* filename, ALuint* buffer, ALsizei* size,
					   ALsizei* frequency, ALenum* format) {
	FILE* soundFile = NULL;
	_WAVE_Format wave_format;
	_WAVE_RIFF_Header riff_header;
	_WAVE_Data wave_data;
	unsigned char* data;
	int alErrorCode;

	soundFile = fopen(filename, "rb");
	if (!soundFile) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not open WAV file - %s", filename);
		return 0;
	}

	// Read in the first chunk into the struct
	fread(&riff_header, sizeof(_WAVE_RIFF_Header), 1, soundFile);

	//check for RIFF and WAVE tag in memeory
	if (riff_header.chunkID[0] != 'R' ||
		 riff_header.chunkID[1] != 'I' ||
		 riff_header.chunkID[2] != 'F' ||
		 riff_header.chunkID[3] != 'F' ||
		 riff_header.format[0] != 'W' ||
		 riff_header.format[1] != 'A' ||
		 riff_header.format[2] != 'V' ||
		 riff_header.format[3] != 'E') {
		fclose(soundFile);
		_gyLog(GY_DEBUG_CRITICAL, "! - Invalid RIFF/WAVE tag - %s", filename);
		return 0;
	}

	//Read in the 2nd chunk for the wave info
	fread(&wave_format, sizeof(_WAVE_Format), 1, soundFile);
	//check for fmt tag in memory
	if (wave_format.subChunkID[0] != 'f' ||
		wave_format.subChunkID[1] != 'm' ||
		wave_format.subChunkID[2] != 't' ||
		wave_format.subChunkID[3] != ' ') {
		fclose(soundFile);
		_gyLog(GY_DEBUG_CRITICAL, "! - Invalid fmt tag - %s", filename);
		return 0;
	}

	//check for extra parameters;
	if (wave_format.subChunkSize > 16)
		fseek(soundFile, sizeof(uint16_t), SEEK_CUR);

	//Read in the the last byte of data before the sound file
	fread(&wave_data, sizeof(_WAVE_Data), 1, soundFile);
	//check for data tag in memory
	if (wave_data.subChunkID[0] != 'd' ||
		wave_data.subChunkID[1] != 'a' ||
		wave_data.subChunkID[2] != 't' ||
		wave_data.subChunkID[3] != 'a') {
		fclose(soundFile);
		_gyLog(GY_DEBUG_CRITICAL, "! - Invalid data tag - %s", filename);
		return 0;
	}

	//Allocate memory for data
	data = malloc(wave_data.subChunk2Size);

	// Read in the sound data into the soundData variable
	if (!fread(data, wave_data.subChunk2Size, 1, soundFile)) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not read data - %s", filename);
		fclose(soundFile);
		return 0;
	}

	//Now we set the variables that we passed in with the
	//data from the structs
	*size = wave_data.subChunk2Size;
	*frequency = wave_format.sampleRate;
	//The format is worked out by looking at the number of
	//channels and the bits per sample.
	if (wave_format.numChannels == 1) {
		if (wave_format.bitsPerSample == 8 )
			*format = AL_FORMAT_MONO8;
		else if (wave_format.bitsPerSample == 16)
			*format = AL_FORMAT_MONO16;
	} else if (wave_format.numChannels == 2) {
		if (wave_format.bitsPerSample == 8 )
			*format = AL_FORMAT_STEREO8;
		else if (wave_format.bitsPerSample == 16)
			*format = AL_FORMAT_STEREO16;
	}

	//create our openAL buffer and check for success
	alGenBuffers(1, buffer);
	if((alErrorCode = alGetError()) != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - alGenBuffers() failure - %s", _openALError(alErrorCode));
		fclose(soundFile);
		return 0;
	}

	//now we put our data into the openAL buffer and
	//check for success
	alBufferData(*buffer, *format, (void*)data,
				 *size, *frequency);
	if((alErrorCode = alGetError()) != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - alBufferData() failure - %s", _openALError(alErrorCode));
		fclose(soundFile);
		return 0;
	}

	free(data);
	fclose(soundFile);
	return 1;
}

int _gyAudWavAlloc(GYWav *const wav) {
	GYAudWavPayload *payload = malloc(sizeof(GYAudWavPayload));

	_loadWavFile(wav->filename, &payload->buffer, &payload->size, &payload->freq, &payload->format);
/*
	alGenBuffers(1, &payload->buffer);

	alutLoadWAVFile(wav->filename, &payload->format, &payload->data, &payload->size, &payload->freq);

	alBufferData(payload->buffer, payload->format, payload->data, payload->size, payload->freq);
	alutUnloadWAV(payload->format, payload->data, payload->size, payload->freq);
*/

	wav->_data = payload;
	return 1;
}

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#define GYRO_OGG_STREAM_BUFF_SIZE	(4096 * 20)
#define GYRO_OGG_STREAM_BUFF_COUNT	2

typedef struct _GYOggPayload {
	ALuint			buffers[2];
	FILE*           oggFile;
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

GYOgg* gyAudCreateOgg(const char*const path) {
	int result;
	int alErrorCode;
	char* device = (char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	GYOgg* ogg = malloc(sizeof(GYOgg));
	GYOggPayload* payload = ogg->_data = malloc(sizeof(GYOggPayload));

	strcpy(ogg->path, path);
	ogg->looping = 0;

	if(!(payload->oggFile = fopen(path, "rb"))) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not open ogg file %s", path);
		free(payload);
		free(ogg);
		return NULL;
	}

	if((result = ov_open(payload->oggFile, &payload->oggStream, NULL, 0)) < 0) {
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not open OGG stream: %s", _vorbisErrorString(result));
		fclose(payload->oggFile);
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


void gyAudDestroyOgg(GYOgg*const ogg) {
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
		//int errorCode;
		alBufferData(buffer, payload->format, pcm, size, payload->vorbisInfo->rate);
		_checkALError();
		return 1;
	}

	return 0;
}

void gyAudSourcePlayOgg(GYWavSource*const src, const struct _GYOgg*const ogg) {
	GYOggPayload* payload = ogg->_data;

	if(gyAudSourceIsPlaying(src))
		return;

	if(!_streamOgg(payload->buffers[0], ogg))
		return;

	if(!_streamOgg(payload->buffers[1], ogg))
		return;

	alSourceQueueBuffers(*src, 2, payload->buffers);
	alSourcePlay(*src);
	_checkALError();
}

void gyAudPauseOggStream(GYWavSource*const src, GYOgg*const ogg) {
	int queued;

	alSourceStop(*src);
	alGetSourcei(*src, AL_BUFFERS_QUEUED, &queued);

	while(queued--) {
		ALuint buffer;

		alSourceUnqueueBuffers(*src, 1, &buffer);
		_checkALError();
	}
}

void gyAudStopOggStream(GYWavSource*const src, GYOgg*const ogg) {
	gyAudPauseOggStream(src, ogg);
	GYOggPayload* payload = ogg->_data;
	ov_pcm_seek(&payload->oggStream, 0);
}

//I think this can be invoked even when an OGG is not currently streaming or is done?
void gyAudOggUpdate(GYWavSource* const src, struct _GYOgg* const ogg) {
	int processed;
	int errorCode;

	alGetSourcei(*src, AL_BUFFERS_PROCESSED, &processed);
	_checkALError();

	while(processed--) {
		ALuint buffer;

		alSourceUnqueueBuffers(*src, 1, &buffer);
		if(!_streamOgg(buffer, ogg)) break;

		alSourceQueueBuffers(*src, 1, &buffer);
		_checkALError();
	}

}


void gyAudOggSync(GYOgg*const ogg1, GYOgg*const ogg2) {
	GYOggPayload* payload1 = ogg1->_data;
	GYOggPayload* payload2 = ogg2->_data;
	double time;

	time = ov_time_tell(&payload1->oggStream);
	ov_time_seek_lap(&payload2->oggStream, time);

}

void gyAudOggSetLooping(GYOgg*const ogg, const unsigned looping) {
	ogg->looping = looping;
}

double gyAudOggTime(GYOgg*const ogg) {
	GYOggPayload* payload = ogg->_data;
	return ov_time_tell(&payload->oggStream);
}

//======= OGG STREAM PTHREADICISM =======

#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

typedef struct __AudioStream {
	ALuint			src;
	GYOgg*			ogg;
	pthread_t		thread;
	sem_t*			semaphore;
	pthread_mutex_t mutex;
	pthread_cond_t	condition;
	unsigned		looping;
	unsigned        killme;
} _AudioStream;

static void* _gyAudOggStreamUpdate(void* data) {
	_AudioStream*const stream = data;
	int processed;
	int errorCode;
	int done;

	while(1) {
		done = 0;

		if(sem_trywait(stream->semaphore) == -1) {
			pthread_mutex_lock(&stream->mutex);
			pthread_cond_wait(&stream->condition, &stream->mutex);
			pthread_mutex_unlock(&stream->mutex);
		}

		if(stream->killme) break; //It had a good run.

		if(!gyAudSourceIsPlaying(stream)) {
			continue;
		}

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

		if(!done)
			sem_post(stream->semaphore);
	}

	return NULL;
}

GYWavSource* gyAudSourceOggStreamCreate(void) {
	char buff[20];
	_AudioStream*const stream = malloc(sizeof(_AudioStream));
	alGenSources(1, &stream->src);
	alSourcei(stream->src, AL_REFERENCE_DISTANCE, _refDist);
	_checkALError();
	stream->looping = 1;
	stream->ogg = NULL;
	stream->killme = 0;
#ifdef __WIN32  //GOD FUCKING DAMNIT!!!!!
	stream->semaphore = malloc(sizeof(sem_t));
	sem_init(stream->semaphore, 0, 0);
#else
	sprintf(buff, "%d", stream->src);
	sem_unlink(buff);
	stream->semaphore = sem_open(buff, O_CREAT, 0777, 0);
#endif
	pthread_mutex_init(&stream->mutex, NULL);
	pthread_cond_init(&stream->condition, NULL);
	pthread_create(&stream->thread, NULL, &_gyAudOggStreamUpdate, stream);
	return stream;
}

void gyAudSourceOggStreamDestroy(GYWavSource*const src) {
	char buff[20];
	_AudioStream*const stream = src;
	//Wake the fucker up (if asleep) and tell it to commit suicide
	stream->killme = 1;
	pthread_mutex_lock(&stream->mutex);
	pthread_cond_signal(&stream->condition);
	pthread_mutex_unlock(&stream->mutex);
	pthread_join(stream->thread, NULL);
#ifdef __WIN32
	sem_destroy(stream->semaphore);
	free(stream->semaphore);
#else
	sem_close(stream->semaphore);
#endif
	sem_unlink(buff);
	pthread_mutex_destroy(&stream->mutex);
	pthread_cond_destroy(&stream->condition);
	gyAudSourceDestroy(src);
}


void gyAudOggStreamPlay(GYWavSource*src, GYOgg*ogg) {
	_AudioStream* stream = src;
	GYOggPayload* payload = ogg->_data;

	if(gyAudSourceIsPlaying(src))
		return;

	if(!_streamOgg(payload->buffers[0], ogg))
		return;

	if(!_streamOgg(payload->buffers[1], ogg))
		return;

	alSourceQueueBuffers(*src, 2, payload->buffers);
	alSourcePlay(*src);
	_checkALError();

	stream->ogg = ogg;

	pthread_mutex_lock(&stream->mutex);
	pthread_cond_signal(&stream->condition);
	pthread_mutex_unlock(&stream->mutex);
}

void gyAudOggStreamPause(GYWavSource*src) {
	_AudioStream* stream = src;
	int queued;

	alSourceStop(stream->src);
	alGetSourcei(stream->src, AL_BUFFERS_QUEUED, &queued);

	while(queued--) {
		ALuint buffer;

		alSourceUnqueueBuffers(stream->src, 1, &buffer);
		_checkALError();
	}
}

void gyAudOggStreamStop(GYWavSource*src) {
	GYOggPayload* payload = ((_AudioStream*)src)->ogg->_data;

	gyAudOggStreamPause(src);
	ov_pcm_seek(&payload->oggStream, 0);
}


