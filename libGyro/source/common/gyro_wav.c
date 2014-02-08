#include <string.h>
#include <stdlib.h>
#include <gyro_wav_api.h>
#include <gyro_system_api.h>
#include <gyro_audio_api.h>
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
	if(!_gyAudWavAlloc(wav)) return 0;
	return wav;
}

static int _WavDestructor(void *const payload) {
	int retVal = _gyAudWavFree(payload);
	free(payload);
	return retVal;
}

static GYAssetBank _wavBank = { 0, 0, 0, &_WavComparator, &_WavConstructor, &_WavDestructor };

GYWav *gyAudWavCreate(const char *const filename) {
	return gyAssetBankInsert(&_wavBank, (char *)filename);
}

int gyAudWavDestroy(GYWav *const wav) {
	return gyAssetBankRemove(&_wavBank, wav);
}

int gyAudWavClear(void) {
	return gyAssetBankClear(&_wavBank);
}

unsigned gyAudWavAmount(void) {
	return gyAssetBankSize(&_wavBank);
}

void gyAudWavWalkthrough(void) {
    unsigned int count = 0;
	GYAssetBankEntry *entry = _wavBank._head;
	GYWav *wav;

	gyPrintf("====WAV WALKTHROUGH====\n");
	for(; entry != 0; entry = entry->_next) {
		wav = entry->_payload;
		gyPrintf("%d) %s", count, wav->filename);
        gyPrintf("\tsize - %d, _refCount - %d\n", wav->bytes, entry->_refCount);
        ++count;
    }
    gyPrintf("====/WAV WALKTHROUGH====\n");
}
