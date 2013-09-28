#include "gyro_audio.h"
#include "gyro_wav.h"
#include "gyro_matrix.h"
#include "glal/gyro_glal.h"

static float _zScale = 1.0f;
static int _refDist = 1;

int gyAudInit(void) {
	alutInit(0, 0);
    return 1;
}

int gyAudUninit(void) {
	alutExit();
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


typedef struct _GYAudWavPayload {
	ALsizei size, freq;
	ALenum 	format;
	ALuint buffer;
	ALvoid 	*data;
} GYAudWavPayload;


int _gyAudWavAlloc(GYWav *const wav) {
	GYAudWavPayload *payload = malloc(sizeof(GYAudWavPayload));

	alGenBuffers(1, &payload->buffer);

	alutLoadWAVFile(wav->filename, &payload->format, &payload->data, &payload->size, &payload->freq);

	alBufferData(payload->buffer, payload->format, payload->data, payload->size, payload->freq);
	alutUnloadWAV(payload->format, payload->data, payload->size, payload->freq);

	wav->_data = payload;
	return 1;
}

int _gyAudWavFree(GYWav *const wav) {
	GYAudWavPayload *payload = wav->_data;

	alDeleteBuffers(1, &payload->buffer);

	free(payload);
	return 1;
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
