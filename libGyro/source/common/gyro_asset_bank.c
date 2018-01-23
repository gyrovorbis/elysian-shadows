#include <stdlib.h>
#include "gyro_asset_bank.h"

//returns the entry, rather than the payload (with previous entry (for removal))
GYAssetBankEntry* gyAssetBankFindEntry(const GYAssetBank *const bank, const void *const data, GYAssetBankEntry **const prev) {
	GYAssetBankEntry *entry = bank->_head;
    if(prev) *prev = 0;

    if(!entry || !entry->_payload) return NULL;

	if(bank->_comparator) { //user-specified comparator
		for(; entry; entry = entry->_next) {
			if(bank->_comparator(entry->_payload, data)) return entry;
            if(prev) *prev = entry;
		}
	} else { //default pointer comparison
		for(; entry; entry = entry->_next) {
			if(entry->_payload == data) return entry;
            if(prev) *prev = entry;
		}
	}
	return 0;
}

void *gyAssetBankFind(const GYAssetBank *const bank, void *const data) {
	GYAssetBankEntry *entry = bank->_head;

	if(bank->_comparator) { //user-specified comparator
		for(; entry; entry = entry->_next) {
			if(bank->_comparator(entry->_payload, data)) return entry->_payload;
		}
	} else { //default pointer comparison
		for(; entry; entry = entry->_next) {
			if(entry->_payload == data) return data;
		}
	}
	return 0;
}

void gyAssetBankInit(GYAssetBank *const bank, int (*comparator)(const void *const payload, const void *const data), void *(*constructor)(void *const data), int (*destructor)(void *const payload)) {
	bank->_size = 0;
	bank->_head = bank->_tail = 0;
	bank->_comparator = comparator;
	bank->_constructor = constructor;
	bank->_destructor = destructor;
}

void *gyAssetBankInsert(GYAssetBank *const bank, void *const data) {
	GYAssetBankEntry *entry, *prev;
	//Check if entry already exists
    if((entry = gyAssetBankFindEntry(bank, data, &prev))) {
		++entry->_refCount;
		return entry->_payload;
	}

	//Allocate entry
	entry = malloc(sizeof(GYAssetBankEntry));
    if(!entry) return NULL;
	entry->_next = 0;
	entry->_refCount = 1;
	//Invoke allocator to allocate actual payload
	entry->_payload = (bank->_constructor)? bank->_constructor(data) : data;

    if(entry->_payload) {

        //Insert entry
        if(bank->_head) {
            bank->_tail->_next = entry;
            bank->_tail = entry;
        } else { //First entry
            bank->_head = bank->_tail = entry;
        }

        ++bank->_size;
        return entry->_payload;
    } else {
        free(entry);
        return NULL;
    }
}

int gyAssetBankRemove(GYAssetBank *const bank, const void *const payload) {
	GYAssetBankEntry *entry, *prev;

    if((entry = gyAssetBankFindEntry(bank, payload, &prev))) {
		if(--entry->_refCount == 0) {
			if(prev) {
				prev->_next = entry->_next;
			} else { //head entry
				bank->_head = entry->_next;
			}
			if(entry == bank->_tail) { //tail entry
				bank->_tail = prev;
			}
			//Invoke destructor on payload if specified
			if(bank->_destructor) bank->_destructor(entry->_payload);
			free(entry);
			--bank->_size;
		}
		return 1;
	}
	return 0;
}

int gyAssetBankClear(GYAssetBank *const bank) {
	GYAssetBankEntry *entry = bank->_head;
	GYAssetBankEntry *next;

	for(; entry; entry = next) {
		next = entry->_next;
		if(bank->_destructor) bank->_destructor(entry);
		free(entry);
	}

	bank->_head = bank->_tail = 0;
	bank->_size = 0;
	return 1;
}


