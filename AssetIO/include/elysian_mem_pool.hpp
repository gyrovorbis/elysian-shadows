#ifndef ELYSIAN_MEM_POOL_HPP
#define ELYSIAN_MEM_POOL_HPP

#include <cassert>
#include <cstring>

namespace elysian {

#define ELYSIAN_MEM_POOL_DEFAULT_FREE_INDEX_CAPACITY    10
#define ELYSIAN_MEM_POOL_DEFAULT_MAX_CAPACITY           100

/* elysian::Pool
 *
 * Simple vector-like data-structure using contiguous
 * static data pool for internal storage. Does not
 * do any automatic dynamic resizing, return index 0
 * on overflow. Maintains an internal state to reduce
 * time complexity of insertions.
 *
 * Time Complexity:
 * Indexing     - O(1)
 * Insertion    - O(1) - O(n)
 * Deletion     - O(1) - O(ELYSIAN_MEM_POOL_FREE_INDEX_ARRAY_SIZE)
 *
 * Iterations must jump over inactive indices, since the container
 * is not sparse. Uses internal state data to be slightly more
 * efficient in certain (hopefully most) cases.
 */

template <typename T>
class Pool {
private:

    //internal state to reduce time complexities
    unsigned    _usedCount          = 0;
    bool        _sequential         = true;
    unsigned    _endIndex           = 0;
    unsigned    _freeCacheCount     = 0;
    unsigned    _startSearchIndex   = 0;

    //actual storage
    int         *_freeIndices       = nullptr;
    T           *_entries           = nullptr;
    bool        *_used              = nullptr;
    unsigned    _maxSize            = ELYSIAN_MEM_POOL_DEFAULT_MAX_CAPACITY;
    unsigned    _freeIndicesSize    = ELYSIAN_MEM_POOL_DEFAULT_FREE_INDEX_CAPACITY;


    unsigned _claimFreeIndex(void) {
        if(_usedCount == _maxSize) return 0;

		if(_sequential) {
            //grab next index
            _used[_endIndex] = true;

			if(++_endIndex == _maxSize) {
                _sequential = false;
            }

			++_usedCount;

            _used[_endIndex] = true;
            return _endIndex-1;

        } else {
            //search free list
            if(_freeCacheCount) {
                for(unsigned i = 0; i < _freeIndicesSize; ++i) {
                    if(_freeIndices[i] != -1) {
                        ++_usedCount;
                        unsigned index = _freeIndices[i];
                        _used[index] = true;
                        _freeIndices[i] = -1;
                        --_freeCacheCount;
                        return index;
                    }
                }
                assert(0);
			} else {

                auto searchFreeIndex = [&](unsigned& index, const unsigned start, const unsigned end) {
                    for(index = start; index < end; ++index) {
                        if(!_used[index]) {
                            //++_usedCount;
							//_used[index] = true;
                            return true;
                        }
                    }
                    return false;
                };

                //fuck, search everything!!
                unsigned pivot = _startSearchIndex;
                if(searchFreeIndex(_startSearchIndex, pivot, _maxSize)) {
                    ++_usedCount;
                    _used[_startSearchIndex] = true;
                    return _startSearchIndex;
                }
                //Loop back around!
                else if(searchFreeIndex(_startSearchIndex, 0, pivot)) {
                    ++_usedCount;
                    _used[_startSearchIndex] = true;
                    return _startSearchIndex;
                }

                assert(0);

			}
		}

        assert(0);
        return 0;
    }


public:

    class iterator {
        friend class Pool;
    private:

        Pool<T>& _parent;
        unsigned _index;

    public:

        iterator(Pool<T>*const parent, const unsigned index):
            _parent(*parent), _index(index) {}

    /*    bool operator != (const iterator& other) const {
            return (_index != other._index);
        }*/

        bool operator < (const iterator& other) const {
            return (_index < other._index);
        }

	   // T* operator->(void) const;
		T& operator*(void) const;

        const iterator& operator++(void) {
            while(!_parent._used[++_index]);

            return *this;
        }
    };

    Pool(const unsigned maxCapacity = ELYSIAN_MEM_POOL_DEFAULT_MAX_CAPACITY,
         const unsigned freeIndexSize = ELYSIAN_MEM_POOL_DEFAULT_FREE_INDEX_CAPACITY):
            _maxSize(maxCapacity), _freeIndicesSize(freeIndexSize)
    {
        _entries        = (T*)malloc(sizeof(T) * maxCapacity);
        _used           = (bool*)malloc(sizeof(bool) * (maxCapacity+1));
        _freeIndices    = (int*)malloc(sizeof(int) * freeIndexSize);

		//_endIndex = maxCapacity;

        memset(_used, 0, sizeof(bool) * maxCapacity);
        _used[maxCapacity] = true;
        for(unsigned i = 0; i < freeIndexSize; ++i) _freeIndices[i] = -1;
    }

    ~Pool(void) {
        free(_freeIndices);
        free(_used);
        free(_entries);
    }

    iterator begin(void) {
        for(unsigned i = 0; i < _endIndex; ++i) {
            if(_used[i]) return iterator(this, i);
        }
        return iterator(this, _endIndex);
    }

    iterator end(void) {
        return iterator(this, _endIndex);
    }

    T& claimFreeEntry(void) {
        return _entries[_claimFreeIndex()];

    }

    void insertEntry(const T& entry) {
        _entries[_claimFreeIndex()] = entry;

    }

	//Way slower than doing it with the iterator... but k.
	void removeEntry(const T& entry) {
		for(auto it = begin(); it < end(); ++it) {
			if(_entries[it._index] == entry) {
				remove(it);
				return;
			}
		}
	}

    void remove(const iterator it) {
        if(_freeCacheCount < _freeIndicesSize) {
            for(unsigned i = 0; i < _freeIndicesSize; ++i) {
                if(_freeIndices[i] == -1) {
                    _freeIndices[i] = it._index;
                    ++_freeCacheCount;
                    break;
                }
            }
        }

        _used[it._index] = false;

        if(--_usedCount == 0) {
            _startSearchIndex = 0;
            _sequential = true;
            if(_endIndex != _maxSize) _used[_endIndex] = 0;
            _endIndex = 0;
            _freeCacheCount = 0;
            for(unsigned i = 0; i < _freeIndicesSize; ++i) _freeIndices[i] = -1;
        }
    }

    void resize(const unsigned size) {
        _entries = (T*)realloc(_entries, sizeof(T)*size);
        _maxSize = size;
        _used    = (bool*)realloc(_used, sizeof(bool) * (size+1));
        clear();
    }

    void clear(void) {
         memset(_used, 0, sizeof(bool) * _maxSize);
         _usedCount = 0;
         _sequential = true;
         _endIndex = 0;
         _used[_maxSize] = true;
         _startSearchIndex = 0;
         for(unsigned i = 0; i < _freeIndicesSize; ++i) _freeIndices[i] = -1;
    }

    unsigned size(void) const { return _usedCount; }

    bool isFull(void) const { return _usedCount == _maxSize; }

    unsigned bytesUsed(void) const {
        return (sizeof(Pool<T>) +
                sizeof(T) * _maxSize +
                sizeof(bool) * (_maxSize+1) +
                sizeof(unsigned) * _freeIndicesSize);
    }

};

/*
template<typename T>
inline T* Pool<T>::iterator::operator ->(void) const {
    return &_parent._entries[_index];
}*/

template<typename T>
inline T& Pool<T>::iterator::operator *(void) const {
	return _parent._entries[_index];
}



}


#endif // ELYSIAN_MEM_POOL_HPP
