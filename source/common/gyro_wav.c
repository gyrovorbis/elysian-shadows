#include <string.h>
#include <stdlib.h>
#include <libGyro/gyro_wav_api.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_audio_api.h>
#include "../common/gyro_asset_bank.h"

static int _WavComparator(const void *const payload, const void *const data) {
    return (strcmp(((const GYWav*)payload)->filename, ((const char *)data)) == 0);
}

static void *_WavConstructor(void *const data) {
    //Allocate wav payload
    GYWav *wav = malloc(sizeof(GYWav));
    if(!wav) return 0;
    wav->bytes = 0;
    strcpy(wav->filename, (const char*)data);
    //Allocate platform-specific wav data
    if(!_gyAudWavAlloc(wav)) {
        /*free(wav);
         * Needs to be here to prevent memory leak,
         * but we're always succeeding with empty wavs for protection
         * at the higher layers.*/
        return 0;
    }
    return wav;
}

static int _WavDestructor(void *const payload) {
    int retVal = _gyAudWavFree(payload);
    free(payload);
    return retVal;
}

static GYAssetBank _wavBank = { 0, 0, 0, &_WavComparator, &_WavConstructor, &_WavDestructor };

struct GYWav *gyAudWavCreate(const char *const filename) {
    return gyAssetBankInsert(&_wavBank, (char *)filename);
}

int gyAudWavDestroy(struct GYWav *const wav) {
    return (wav)? gyAssetBankRemove(&_wavBank, wav) : 0;
}

int gyAudWavReload(struct GYWav * const wav) {
    unsigned int success = 1;
    //unsigned int width, height;
    if(!wav) return 0;

    success &= _gyAudWavAlloc(wav);
    success &= _gyAudWavFree(wav);
    return success;
}

int gyAudWavReloadPath(const char*const path) {
    const GYWav* wav = gyAssetBankFind(&_wavBank, path);
    return wav? gyAudWavReload(wav) : 0;
}

int gyAudWavClear(void) {
    return gyAssetBankClear(&_wavBank);
}

unsigned gyAudWavAmount(void) {
    return gyAssetBankSize(&_wavBank);
}

unsigned int gyAudWavRefCount(const char*const filename) {
    GYAssetBankEntry* entry = gyAssetBankFindEntry(&_wavBank, filename, NULL);
    return entry? entry->_refCount : 0;
}

void gyAudWavWalkthrough(void) {
    unsigned int count = 0;
    unsigned int totalBytes = 0;
    GYAssetBankEntry *entry = _wavBank._head;
    GYWav *wav;

    //grab the total byteCount first
    for(; entry != 0; entry = entry->_next) {
        wav = entry->_payload;
        totalBytes += wav->bytes;
    }

    _gyLog(GY_DEBUG_VERBOSE, "WAV Bank (%d items, %fMB total)", gyAudWavAmount(), (float)totalBytes/(1024.0f*1024.0f));
    _gyPush();
    for(entry = _wavBank._head; entry != 0; entry = entry->_next) {
        wav = entry->_payload;
        _gyLog(GY_DEBUG_VERBOSE, "%d) %s", count, wav->filename);
        _gyPush();
        _gyLog(GY_DEBUG_VERBOSE, "size - %d bytes", wav->bytes);
        _gyLog(GY_DEBUG_VERBOSE, "referenceCount - %d", entry->_refCount);
        if(!wav->bytes) {
            _gyLog(GY_DEBUG_WARNING, "Error loading file! Falling back to empty WAV.");
        }
        _gyPop(1);
        ++count;
    }
    _gyPop(1);
}
