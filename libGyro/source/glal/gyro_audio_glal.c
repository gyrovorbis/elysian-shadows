#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
	//alutInit(0, 0);                                                      //And an OpenAL Context

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
	//alutExit();

	alcMakeContextCurrent(NULL);
	alcDestroyContext(_alContext);
	alcCloseDevice(_alDevice);
    return 1;
}

int gyAudPlayOgg(const char *const filename) {
    return 1;
}


int gyAudStopOgg(void) {
    return 1;
}


void gyAudPlayWav(const struct _GYWav *const wav) {

}


void gyAudSetReferenceDistance(const int dist) {
	_refDist = dist;
}

void gyAudSetZScale(const float scale) {
	_zScale = scale;
}


//audio source
GYWavSource* gyAudSourceCreate(void) {
	ALuint* src = malloc(sizeof(ALuint));
	alGenSources(1, src);
	alSourcei(*src, AL_REFERENCE_DISTANCE, _refDist);
	return src;
}

void gyAudSourceDestroy(GYWavSource*const src) {
	if(!src) return;
	alDeleteSources(1, src);
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
	}

	alSourcef(*src, alIndex, val);

}

void gyAudSourceSeti(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const int val) {
	int alIndex;
	switch(attrib) {
	case GY_SOURCE_LOOPING:
		alIndex = AL_LOOPING;
	}

	alSourcei(*src, alIndex, val);
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
	GYAudWavPayload *payload = wav->_data;

	alDeleteBuffers(1, &payload->buffer);

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
	if(alGetError() != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - alGenBuffers() failure - %s", filename);
		fclose(soundFile);
		return 0;
	}

	//now we put our data into the openAL buffer and
	//check for success
	alBufferData(*buffer, *format, (void*)data,
				 *size, *frequency);
	if(alGetError() != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "! - alBufferData() failure - %s", filename);
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





