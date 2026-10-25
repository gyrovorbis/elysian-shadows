/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <al/gyro_al.h>
#include <libGyro/gyro_audio_api.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_wav_api.h>
#include <libGyro/gyro_system_api.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef struct _GYAudWavPayload {
    #if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    ALsizei size, freq;
    ALenum 	format;
    ALuint buffer;
    char *data;
#endif
} GYAudWavPayload;

void gyAudSourcePlayWav(GYAudioSource*const src, const struct GYWav* wav) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    assert(src);
    if(!wav) return;
    const GYAudWavPayload* payload = wav->_data;
    if(payload->buffer == AL_NONE) return;
    GY_AL_CLEAR_ERROR();
        alSourceStop(*(ALuint*)src);
    GY_AL_CHECK_ERROR();

    GY_AL_CLEAR_ERROR();
        alSourcei(*(ALuint*)src, AL_BUFFER, payload->buffer);
    GY_AL_CHECK_ERROR();

    GY_AL_CLEAR_ERROR();
        alSourcePlay(*(ALuint*)src);
    GY_AL_CHECK_ERROR();
#endif
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
    char        chunkID[4];
    uint32_t    chunkSize;//size not including chunkSize or chunkID
    char        format[4];
} _WAVE_RIFF_Header;

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
typedef struct __WAVE_Format {
    char        subChunkID[4];
    uint32_t    subChunkSize;
    uint16_t    audioFormat;
    uint16_t    numChannels;
    uint32_t    sampleRate;
    uint32_t    byteRate;
    uint16_t    blockAlign;
    uint16_t    bitsPerSample;
} _WAVE_Format;

/*
* Struct to hold the data of the wave file
*/
typedef struct _WAVE_Chunk {
    char        subChunkID[4]; //should contain the word data
    uint32_t    subChunk2Size; //Stores the size of the data block
} _WAVE_Chunk;

/*
 * Load wave file function. No need for ALUT with this
 */
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
static int _loadWavFile(const char* filename, ALuint* buffer, ALsizei* size,
                       ALsizei* frequency, ALenum* format) {
    int success = 0;
    GYFile* soundFile = NULL;
    _WAVE_Format wave_format;
    _WAVE_RIFF_Header riff_header;
    _WAVE_Chunk wave_chunk;
    *size = 0;
    *buffer = 0;
    unsigned char* data;

    if (!gyFileOpen(filename, "rb", &soundFile)) {
        _gyLog(GY_DEBUG_ERROR, "Could not open WAV file [%s]", filename);
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
        _gyLog(GY_DEBUG_WARNING, "Invalid RIFF/WAVE tag [%s]", filename);
        goto error_post_file;
    }

    //Read in the 2nd chunk for the wave info
    gyFileRead(soundFile, &wave_format, sizeof(_WAVE_Format), NULL);
    //check for fmt tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ') {
        _gyLog(GY_DEBUG_WARNING, "Invalid fmt tag [%s]", filename);
        goto error_post_file;
    }

    //check for extra parameters;
    if (wave_format.subChunkSize > 16)
        gyFileSeek(soundFile, /*sizeof(uint16_t)*/ wave_format.subChunkSize, SEEK_CUR);

    size_t bytesRead = 0;
    int status;
    for(;;) {
        status = gyFileRead(soundFile, &wave_chunk, sizeof(_WAVE_Chunk), &bytesRead);

        if(!status || bytesRead != sizeof(_WAVE_Chunk)) {
            _gyLog(GY_DEBUG_ERROR, "'data' chunk not found in WAV header [%s]!", filename);
            goto error_post_file;
        }

        if(wave_chunk.subChunkID[0] == 'd' &&
            wave_chunk.subChunkID[1] == 'a' &&
            wave_chunk.subChunkID[2] == 't' &&
            wave_chunk.subChunkID[3] == 'a') {
            break;
        }

        gyFileSeek(soundFile, wave_chunk.subChunk2Size, SEEK_CUR);
    }

    //Allocate memory for data
    data = malloc(wave_chunk.subChunk2Size);

    // Read in the sound data into the soundData variable
    if (!gyFileRead(soundFile, data, wave_chunk.subChunk2Size, NULL)) {
        _gyLog(GY_DEBUG_WARNING, "Could not read data chunk [%s]", filename);
        goto error_post_malloc;
    }

    //Now we set the variables that we passed in with the
    //data from the structs
    *size = wave_chunk.subChunk2Size;
    *frequency = wave_format.sampleRate;
    //The format is worked out by looking at the number of
    //channels and the bits per sample.
    if (wave_format.numChannels == 1) {
        if (wave_format.bitsPerSample == 8 )
            *format = AL_FORMAT_MONO8;
        else if (wave_format.bitsPerSample == 16)
            *format = AL_FORMAT_MONO16;
        else if(wave_format.bitsPerSample == 32) {
            if(_alExtFloat32) {
                *format = _alFmtMono32;
            } else {
                _gyLog(GY_DEBUG_ERROR, "Cannot buffer 32-bit WAV file without 32-bit OpenAL extensions! [%s]", filename);
                goto error_post_malloc;
            }
        }
    } else if (wave_format.numChannels == 2) {
        if (wave_format.bitsPerSample == 8 )
            *format = AL_FORMAT_STEREO8;
        else if (wave_format.bitsPerSample == 16)
            *format = AL_FORMAT_STEREO16;
        else if(wave_format.bitsPerSample == 32) {
            if(_alExtFloat32) {
                *format = _alFmtStereo32;
            } else {
                _gyLog(GY_DEBUG_ERROR, "Cannot buffer 32-bit WAV file without 32-bit OpenAL extensions! [%s]", filename);
                gyFileClose(&soundFile);
                goto error_post_malloc;
            }
        }

    }

    //create our openAL buffer and check for success
    GY_AL_CLEAR_ERROR();
    alGenBuffers(1, buffer);
    if(GY_AL_CHECK_ERROR()) {
        goto error_post_malloc;
    }

    //now we put our data into the openAL buffer and
    //check for success
    GY_AL_CLEAR_ERROR();
    alBufferData(*buffer, *format, (void*)data,
                 *size, *frequency);
    if(GY_AL_CHECK_ERROR()) {
        goto error_post_malloc;
    }

    success = 1;

error_post_malloc:
    free(data);
error_post_file:
    gyFileClose(&soundFile);
    return success;
}
#endif

int _gyAudWavAlloc(GYWav *const wav) {
    GYAudWavPayload *payload = malloc(sizeof(GYAudWavPayload));

#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    _loadWavFile(wav->filename, &payload->buffer, &payload->size, &payload->freq, &payload->format);
#endif
    wav->_data = payload;
    wav->bytes = payload->size;
    return 1;
}


int _gyAudWavFree(GYWav *const wav) {
    GYAudWavPayload *payload = wav->_data;
    if(payload->buffer != AL_NONE) {
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
        GY_AL_CLEAR_ERROR();
        alDeleteBuffers(1, &payload->buffer);
        GY_AL_CHECK_ERROR();
#endif
    }
    free(payload);
    return 1;
}

