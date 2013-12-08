#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
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
		_gyLog(GY_DEBUG_CRITICAL, "OpenAL Error raised: %s", _openALError(error));
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
	return (GYWavSource*)src;
}

void gyAudSourceDestroy(GYWavSource*const src) {
	if(!src) return;
	alDeleteSources(1, (ALuint*)src);
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
	default: return;
	}

	newVec.z *= _zScale;
	alSourcefv(*(ALuint*)src, alIndex, (void*)&newVec);

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
	default: return;
	}

	alSourcef(*(ALuint*)src, alIndex, val);

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
	default: return 0.0f;
	}

	float value;

	alGetSourcef(*(ALuint*)src, alIndex, &value);
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
		break;
	default: return;
	}

	alSourcei(*(ALuint*)src, alIndex, val);
}

void gyAudSourceStop(GYWavSource*const src) {
	alSourceStop(*(ALuint*)src);
}

int gyAudSourceIsPlaying(GYWavSource*const src) {
	ALenum state;
	alGetSourcei(*(ALuint*)src, AL_SOURCE_STATE, &state);
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
	alSourcei(*(ALuint*)src, AL_BUFFER, payload->buffer);
	alSourcePlay(*(ALuint*)src);
}

//audio listener
void gyAudListenerSetPos(const struct _GYVector3* const pos) {
	GYVector3 veccy = *pos;
	veccy.z *= _zScale;
	alListenerfv(AL_POSITION, (void*)&veccy);
}

//===== PRIVATE =======


int _gyAudWavFree(GYWav *const wav) {
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
/* Defined so dickface vorbisfile.h doesn't create static data that
 * we don't give a shit about in this compilation unit. */
#define OV_EXCLUDE_STATIC_CALLBACKS
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

	printf("Version - %d\n", payload->vorbisInfo->version);
	printf("Channels - %d\n", payload->vorbisInfo->channels);
	printf("Rate (hz) - %ld\n", payload->vorbisInfo->rate);
	printf("bitrate upper - %ld\n", payload->vorbisInfo->bitrate_upper);
	printf("bitrate nominal - %ld\n", payload->vorbisInfo->bitrate_nominal);
	printf("bitrate lower - %ld\n", payload->vorbisInfo->bitrate_lower);
	printf("bitrate window - %ld\n", payload->vorbisInfo->bitrate_window);
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
		alBufferData(buffer, payload->format, pcm, size, payload->vorbisInfo->rate);
		_checkALError();
		return 1;
	}

	return 0;
}

//======= OGG STREAM PTHREADICISM =======

#include <pthread.h>

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
		if(gyAudSourceIsPlaying((GYWavSource*)stream)) {

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

GYWavSource* gyAudOggStreamCreate(void) {
	_OggStreamSource*const stream = malloc(sizeof(_OggStreamSource));

	//Init source
	alGenSources(1, &stream->src);
	alSourcei(stream->src, AL_REFERENCE_DISTANCE, _refDist);
	_checkALError();

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
	return (GYWavSource*)stream;
}

void gyAudOggStreamDestroy(GYWavSource*const src) {
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


void gyAudOggStreamPlay(GYWavSource*src, GYOgg*ogg) {
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

void gyAudOggStreamPause(GYWavSource*src) {
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

void gyAudOggStreamStop(GYWavSource*src) {
	_OggStreamSource* stream = (void*)src;
	if(!stream || !stream->ogg) return;
	GYOggPayload* payload = stream->ogg->_data;

	pthread_mutex_lock(&stream->streamMutex);

	gyAudOggStreamPause(src);
	ov_pcm_seek(&payload->oggStream, 0);

	pthread_mutex_unlock(&stream->streamMutex);
}

void gyAudOggStreamSync(GYWavSource*const src1, GYWavSource*const src2) {
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

void gyAudOggStreamSetLooping(GYWavSource*const src, const unsigned looping) {
	_OggStreamSource* stream = (_OggStreamSource*)src;
	GYOgg *ogg = stream->ogg;
	//lock?
	stream->looping = looping;
	if(ogg) ogg->looping = looping;
}

double gyAudOggStreamTime(GYWavSource*const src) {
	_OggStreamSource* stream = (_OggStreamSource*)src;
	GYOggPayload *ogg = stream->ogg->_data;

	//lock? Not sure if this does any writing internally... better fucking not...
	return ov_time_tell(&ogg->oggStream);
}


