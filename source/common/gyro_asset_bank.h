/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_ASSET_BANK_H
#define GYRO_ASSET_BANK_H
/*
	Generic bank/manager for assets. Uses a singly-linked list
	for storage mechanism. Uses reference counting for trying
	to reallocate the same asset.

	_constructor - used for searching. Compares payload of current
	entry to data search entry
	_allocator - used to create payload. Allocates payload object
	from data object
	_destructor - used for deleting payload of current entry

	NOTE: When the comparator and allocator are not specified,
	the algorithms default to pointer comparison and assignment.
	This class can be used as a generic linked list of heap data
	in this fashion.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYAssetBankEntry {
	struct _GYAssetBankEntry *_next;
	void *_payload;
	unsigned _refCount;
} GYAssetBankEntry;

typedef struct _GYAssetBank {
	GYAssetBankEntry *_head;
	GYAssetBankEntry *_tail;
	unsigned _size;
	int (*_comparator)(const void *const payload, const void *const data);
	void *(*_constructor)(void *const data);
	int (*_destructor)(void *const payload);
} GYAssetBank;

inline static unsigned gyAssetBankSize(const GYAssetBank *const bank) { return bank->_size; }

void gyAssetBankInit(GYAssetBank *const bank, int (*comparator)(const void *const payload, const void *const data), void *(*constructor)(void *const data), int (*destructor)(void *const payload));
/*O(n) - Not constant time, because it first checks to see if an entry
		 with the same data already exists (using comparator) before allocating
		 a new entry (with the allocator) */
void *gyAssetBankInsert(GYAssetBank *const bank, void *const data);
/*O(n) - We sacrificed constant deletion time to allow us to remove by
		 payload pointer rather than node pointer. Because of this,
		 making a double link is pointless. */
int gyAssetBankRemove(GYAssetBank *const bank, const void *const payload);

/*O(n) - Searches for entry based on VALUE by invoking user-registered
		 COMPARATOR function (or using pointer comparison if NULL)*/
void *gyAssetBankFind(const GYAssetBank *const bank, void *const data);
GYAssetBankEntry* gyAssetBankFindEntry(const GYAssetBank *const bank, const void *const data, GYAssetBankEntry **const prev);
//O(n)
int gyAssetBankClear(GYAssetBank *const bank);

#ifdef __cplusplus
}
#endif

#endif // GYRO_LINKED_LIST_H
