#ifndef ELYSIAN_SORT_SWEEP_HPP
#define ELYSIAN_SORT_SWEEP_HPP

#include "elysian_mem_pool.hpp"
#include <elysian_asset_typedefs.hpp>

#define ELYSIAN_SORT_SWEEP_DIMENSIONS       3
#define ELYSIAN_SORT_SWEEP_POOL_SIZE        500

ELYSIAN_ASSET_TYPEDEF(Collidable)

namespace elysian {

template<typename T>
struct SortSweepAABB {
    static unsigned curIndex;

    float min[ELYSIAN_SORT_SWEEP_DIMENSIONS];
    float max[ELYSIAN_SORT_SWEEP_DIMENSIONS];
    T* collidable;

    bool operator > (const SortSweepAABB& rhs);
};

typedef SortSweepAABB<Collidable> SortSweepCollidable;

template<typename T>
class SortSweepAABBPool:
        public Pool<SortSweepAABB<T>> {
            typedef Pool<SortSweepAABB<T>> Parent;

public:
            SortSweepAABBPool(void);
    void    removeAABB(const SortSweepAABB<T>& body);
    void    sortAndSweep(void);
};

typedef SortSweepAABBPool<Collidable> SortSweepPool;

template<typename T>
inline SortSweepAABBPool<T>::SortSweepAABBPool(void): Pool<SortSweepAABB<T> >(ELYSIAN_SORT_SWEEP_POOL_SIZE) {}

template<typename T>
inline void SortSweepAABBPool<T>::removeAABB(const SortSweepAABB<T>& body) {
    //resync pointers!
    Parent::removeEntryContiguous(body);
    for(unsigned i = 0; i < Pool<SortSweepAABB<T>>::_endIndex; ++i) {
        Pool<SortSweepAABB<T>>::_entries[i].collidable->setSortSweepAABB(&Pool<SortSweepAABB<T>>::_entries[i]);
    }
}

template<typename T>
inline bool SortSweepAABB<T>::operator > (const SortSweepAABB& rhs) { return (min[curIndex] > rhs.min[curIndex]); }

template<typename T>
inline void SortSweepAABBPool<T>::sortAndSweep(void) {
    SortSweepAABB<T> tempAABB;
    const unsigned curIndex = SortSweepAABB<T>::curIndex;

#if 0

    //Sort entity z entries per z coord (ascending order)
    //IMPLEMENT ME MORE EFFICIENTLY! BUBBLE SORT IS FUCKING GAY
    for(unsigned j = 0; j < _depthBuffMaxHeight; ++j) {
        for(unsigned i = 0; i < _entityCount[j]; ++i) {
            for(unsigned k = i+1; k < _entityCount[j]; ++k) {
                if(_entityBuff[j][i].zCoordRelative > _entityBuff[j][k].zCoordRelative) {
                    E temp = _entityBuff[j][i];
                    _entityBuff[j][i] = _entityBuff[j][k];
                    _entityBuff[j][k] = temp;
                }
            }
        }
    }

#endif

    for(unsigned i = 0; i < Parent::_endIndex; ++i) {
        for(unsigned j = i+1; j < Parent::_endIndex; ++j) {
            if(Parent::_entries[i] > Parent::_entries[j]) {
                tempAABB = Parent::_entries[i];
                Parent::_entries[i] = Parent::_entries[j];
                Parent::_entries[j] = tempAABB;
            }
        }
    }

    float s[3] = { 0.0f, 0.0f, 0.0f };
    float s2[3] = { 0.0f, 0.0f, 0.0f};
    float v[3];

    for(unsigned i = 0; i < Parent::_endIndex; ++i) {

        float p[ELYSIAN_SORT_SWEEP_DIMENSIONS];
        for(unsigned k = 0; k < ELYSIAN_SORT_SWEEP_DIMENSIONS; ++k) p[k] = 0.5f* (Parent::_entries[i].min[k] + Parent::_entries[i].max[k]);

        for(unsigned c = 0; c < 3; ++c) {
            s[c] += p[c];
            s2[c] += p[c] * p[c];
        }

        for(unsigned j = i+1; j < Parent::_endIndex; ++j) {
            if(Parent::_entries[j].min[curIndex] > Parent::_entries[i].min[curIndex])
                break;
            if(Parent::_entries[i].collidable->checkAABBCollision(Parent::_entries[j].collidable))
                Parent::_entries[i].collidable->checkFullCollision(Parent::_entries[j].collidable);
        }
    }

    for(int c = 0; c < 3; ++c) v[c] = s2[c] - s[c] * s[c] / (Parent::_endIndex);

    SortSweepAABB<T>::curIndex = 0;
    if(v[1] > v[0]) SortSweepAABB<T>::curIndex = 1;
    if(v[2] > v[SortSweepAABB<T>::curIndex]) SortSweepAABB<T>::curIndex = 2;

}


}


#endif // ELYSIAN_SORT_SWEEP_HPP
