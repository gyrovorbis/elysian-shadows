#include <glal/gyro_al.h>
#include <gyro_audio_api.h>
#include <gyro_file_api.h>
#include <gyro_wav_api.h>
#include <gyro_system_api.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct _GYAudWavPayload {
    ALsizei size, freq;
    ALenum 	format;
    ALuint buffer;
    char *data;
} GYAudWavPayload;

void gyAudSourcePlayWav(GYAudioSource*const src, const GYWav* wav) {
    if(!wav) return;
    const GYAudWavPayload* payload = wav->_data;
    alSourcei(*(ALuint*)src, AL_BUFFER, payload->buffer);
    alSourcePlay(*(ALuint*)src);
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
    GYFile* soundFile = NULL;
    _WAVE_Format wave_format;
    _WAVE_RIFF_Header riff_header;
    _WAVE_Data wave_data;
    unsigned char* data;
    int alErrorCode;

    if (!gyFileOpen(filename, "rb", &soundFile)) {
        _gyLog(GY_DEBUG_CRITICAL, "! - Could not open WAV file - %s", filename);
        return 0;
    }

    // Read in the first chunk into the struct
    gyFileRead(soundFile, &riff_header, sizeof(_WAVE_RIFF_Header), NULL);

    //check for RIFF and WAVE tag in memeory
    if (riff_header.chunkID[0] != 'R' ||
         riff_header.chunkID[1] != 'I' ||
         riff_header.chunkID[2] != 'F' ||
         riff_header.chunkID[3] != 'F' ||
         riff_header.format[0] != 'W' ||
         riff_header.format[1] != 'A' ||
         riff_header.format[2] != 'V' ||
         riff_header.format[3] != 'E') {
        gyFileClose(&soundFile);
        _gyLog(GY_DEBUG_CRITICAL, "! - Invalid RIFF/WAVE tag - %s", filename);
        return 0;
    }

    //Read in the 2nd chunk for the wave info
    gyFileRead(soundFile, &wave_format, sizeof(_WAVE_Format), NULL);
    //check for fmt tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ') {
        gyFileClose(&soundFile);
        _gyLog(GY_DEBUG_CRITICAL, "! - Invalid fmt tag - %s", filename);
        return 0;
    }

    //check for extra parameters;
    if (wave_format.subChunkSize > 16)
        gyFileSeek(soundFile, /*sizeof(uint16_t)*/ wave_format.subChunkSize, SEEK_CUR);

    //Read in the the last byte of data before the sound file
    gyFileRead(soundFile, &wave_data, sizeof(_WAVE_Data), NULL);
    //check for data tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a') {
        gyFileClose(&soundFile);
        _gyLog(GY_DEBUG_CRITICAL, "! - Invalid data tag - %s", filename);
        return 0;
    }

    //Allocate memory for data
    data = malloc(wave_data.subChunk2Size);

    // Read in the sound data into the soundData variable
    if (!gyFileRead(soundFile, data, wave_data.subChunk2Size, NULL)) {
        _gyLog(GY_DEBUG_CRITICAL, "! - Could not read data - %s", filename);
        gyFileClose(&soundFile);
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
        gyFileClose(&soundFile);
        return 0;
    }

    //now we put our data into the openAL buffer and
    //check for success
    alBufferData(*buffer, *format, (void*)data,
                 *size, *frequency);
    if((alErrorCode = alGetError()) != AL_NO_ERROR) {
        _gyLog(GY_DEBUG_CRITICAL, "! - alBufferData() failure - %s", _openALError(alErrorCode));
        gyFileClose(&soundFile);
        return 0;
    }

    free(data);
    gyFileClose(&soundFile);
    return 1;
}

int _gyAudWavAlloc(GYWav *const wav) {
    GYAudWavPayload *payload = malloc(sizeof(GYAudWavPayload));

    _loadWavFile(wav->filename, &payload->buffer, &payload->size, &payload->freq, &payload->format);
    wav->_data = payload;
    return 1;
}


int _gyAudWavFree(GYWav *const wav) {
    GYAudWavPayload *payload = wav->_data;

    alDeleteBuffers(1, &payload->buffer);
    _checkALError();
    free(payload);
    return 1;
}

