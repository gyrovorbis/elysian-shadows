#ifndef ELYSIAN_SORT_SWEEP_HPP
#define ELYSIAN_SORT_SWEEP_HPP

#include <AssetIO/utility/elysian_mem_pool.hpp>
#include <AssetIO/utility/elysian_asset_typedefs.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <AssetIO/utility/elysian_generic.hpp>


#ifdef ELYSIAN_OPENCL_ACCELERATED
#	ifdef __APPLE__
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLDevice.h>
#include <OpenGL/CGLCurrent.h>
#		include <OpenCL/opencl.h>
#	else
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#       include <CL/cl_gl.h>
#       include <CL/cl_gl_ext.h>
#	endif
#endif

#define ELYSIAN_SORT_SWEEP_DIMENSIONS               2
#define ELYSIAN_SORT_SWEEP_POOL_SIZE                500
#define ELYSIAN_SORT_SWEEP_AABB_CONTACT_LIST_SIZE   10000
#define ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_PATH        "/assets/opencl/ubo_write_test_kernel.cl"
#define ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_NAME        "drawUBO"
#define ELYSIAN_SORT_SWEEP_CL_STRING_BUFF_SIZE      100
#define ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_FLAGS       "-D ELYSIAN_SORT_SWEEP_DIMENSIONS=" ELYSIAN_EXPAND_STRINGIFY_MACRO(ELYSIAN_SORT_SWEEP_DIMENSIONS)

ELYSIAN_ASSET_TYPEDEF(Collidable)

namespace elysian {

struct SortSweepAABBContact {
    int bodyAIndex = -1;
    int bodyBIndex = -1;
};

template<typename T>
struct SortSweepAABB {
    static unsigned curIndex;
    //subclass is in charge of updating its position and bounding box
    float min[ELYSIAN_SORT_SWEEP_DIMENSIONS] = { 0.0f };
    float max[ELYSIAN_SORT_SWEEP_DIMENSIONS] = { 0.0f };
    //pointer to custom userdata object, ignored on the GPU
    T* collidable   = nullptr;

    bool operator > (const SortSweepAABB& rhs);
    void updateCollidable(void);
};

typedef SortSweepAABB<Collidable> SortSweepCollidable;

#ifdef ELYSIAN_OPENCL_ACCELERATED
 extern  cl_platform_id*     _platforms;
extern   cl_uint             _platformCount;
 extern  cl_device_id**      _devices;
extern   cl_uint*            _deviceCount;
extern     cl_uint             _curPlatformIndex;
extern     cl_uint             _curDeviceIndex;
 extern    cl_context          _context;
 extern    cl_command_queue    _cmdQueue;
  extern   cl_program          _program;
extern     cl_kernel           _kernel;
 extern    cl_mem              _aabbListBuff;
 extern    cl_mem              _aabbListSizeBuff;
 extern    cl_mem              _aabbContactListBuff;
 extern    cl_mem              _aabbContactListSizeBuff;

#endif



template<typename T>
class SortSweepAABBPool: public Pool<SortSweepAABB<T>> {
            typedef Pool<SortSweepAABB<T>> Parent;

private:

#ifdef ELYSIAN_OPENCL_ACCELERATED



#endif





    SortSweepAABBContact _aabbContacts[ELYSIAN_SORT_SWEEP_AABB_CONTACT_LIST_SIZE];
    unsigned            _aabbContactsSize = 0;

#ifdef ELYSIAN_OPENCL_ACCELERATED
    bool        _useGPU = false;
#endif

    void        _sort(void);
    void        _sweep(void);

public:
                SortSweepAABBPool(void);

    void        setUseGPU(const bool val=true);
    bool        getUseGPU(void) const;

    void        initialize(void);
    void        uninitialize(void);

    void        removeAABB(const SortSweepAABB<T>& body);
    void        sortAndSweep(void);

    T*          getContactBodyA(const unsigned index) const;
    T*          getContactBodyB(const unsigned index) const;
    unsigned    getContactCount(void) const;
};

typedef SortSweepAABBPool<Collidable> SortSweepPool;


template<typename T>
unsigned SortSweepAABB<T>::curIndex = 0;

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
inline void SortSweepAABB<T>::updateCollidable(void) { collidable->setSortSweepAABB(this); }

template<typename T>
inline T* SortSweepAABBPool<T>::getContactBodyA(const unsigned index) const { return Parent::_entries[_aabbContacts[index].bodyAIndex].collidable; }

template<typename T>
inline T* SortSweepAABBPool<T>::getContactBodyB(const unsigned index) const { return Parent::_entries[_aabbContacts[index].bodyBIndex].collidable; }

template<typename T>
inline unsigned SortSweepAABBPool<T>::getContactCount(void) const { return _aabbContactsSize; }

template<typename T>
inline void SortSweepAABBPool<T>::setUseGPU(const bool val) { _useGPU = val; }

template<typename T>
inline bool SortSweepAABBPool<T>::getUseGPU(void) const { return _useGPU; }

template<typename T>
inline void SortSweepAABBPool<T>::_sort(void) {
    SortSweepAABB<T> tempAABB;

    //TERRIBLE BUBBLE SORT
    for(unsigned i = 0; i < Parent::_endIndex; ++i) {
        for(unsigned j = i+1; j < Parent::_endIndex; ++j) {
            if(Parent::_entries[i] > Parent::_entries[j]) {
                tempAABB = Parent::_entries[i];
                Parent::_entries[i] = Parent::_entries[j];
                Parent::_entries[i].updateCollidable();
                Parent::_entries[j] = tempAABB;
                Parent::_entries[j].updateCollidable();
            }
        }
    }

}

template<typename T>
inline void SortSweepAABBPool<T>::sortAndSweep(void) {
    _sort();
    _sweep();
}


static inline void _checkClError(const cl_int status, const char*const str) {
  if(status != CL_SUCCESS) {
    dbgprintf("! - OpenCL Error: %s\n", str);
    exit(1);
  }
}


template<typename T>
inline void SortSweepAABBPool<T>::_sweep(void) {
        if(!_useGPU) {
#if 1

        const unsigned curIndex = SortSweepAABB<T>::curIndex;

        float s[3] = { 0.0f, 0.0f, 0.0f };
        float s2[3] = { 0.0f, 0.0f, 0.0f};
        //float v[3];

        for(unsigned i = 0; i < Parent::_endIndex; ++i) {

            float p[ELYSIAN_SORT_SWEEP_DIMENSIONS];
            //store midpoints of each AABB
            for(unsigned k = 0; k < ELYSIAN_SORT_SWEEP_DIMENSIONS; ++k) p[k] = 0.5f* (Parent::_entries[i].min[k] + Parent::_entries[i].max[k]);

            for(unsigned c = 0; c < ELYSIAN_SORT_SWEEP_DIMENSIONS; ++c) {
                s[c] += p[c];           //update running sum of midpoints
                s2[c] += p[c] * p[c];   //update running sum of square midpoints
            }

            for(unsigned j = i+1; j < Parent::_endIndex; ++j) { //compare against every other object
                if(Parent::_entries[j].min[curIndex] > Parent::_entries[i].max[curIndex])  //break early if not overlapping with next object
                    break;
                if(Parent::_entries[i].collidable->checkAABBCollision(Parent::_entries[j].collidable)) //cheap, inaccurate collision check
                    Parent::_entries[i].collidable->checkFullCollision(Parent::_entries[j].collidable); //expensive, accurate collision check
            }
        }

        //find variance per axis?
        //for(int c = 0; c < ELYSIAN_SORT_SWEEP_DIMENSIONS; ++c) v[c] = s2[c] - (s[c] * s[c]) / (float)(Parent::_endIndex);
        //SortSweepAABB<T>::curIndex = 0;
        //if(v[1] > v[0]) SortSweepAABB<T>::curIndex = 1;
        //if(v[2] > v[SortSweepAABB<T>::curIndex]) SortSweepAABB<T>::curIndex = 2;
  } else {
#else

    cl_int status = clEnqueueWriteBuffer(_cmdQueue,
                                  _aabbListBuff,
                                  CL_FALSE,
                                  0,
                                  Parent::_endIndex*sizeof(SortSweepCollidable),
                                  &Parent::_entries[0],
                                  0,
                                  nullptr,
                                  nullptr);
    _checkClError(status, "Could not write aabbListBuffer to device!");

    const cl_uint listSize = Parent::size();
    status = clEnqueueWriteBuffer(_cmdQueue,
                                  _aabbListSizeBuff,
                                  CL_FALSE,
                                  0,
                                  sizeof(cl_uint),
                                  &listSize,
                                  0,
                                  nullptr,
                                  nullptr);
    _checkClError(status, "Could not write aabbListSizeBuffer to device!");

    const cl_uint zero = 0;
    status = clEnqueueWriteBuffer(_cmdQueue,
                                  _aabbContactListSizeBuff,
                                  CL_FALSE,
                                  0,
                                  sizeof(cl_uint),
                                  &zero,
                                  0,
                                  nullptr,
                                  nullptr);
    _checkClError(status, "Could not clear contact list buffer on device!");


    size_t globalWorkSize[1];
    globalWorkSize[0] = Parent::_endIndex;

    status = clEnqueueNDRangeKernel(_cmdQueue,
                                    _kernel,
                                    1,              //work dim
                                    nullptr,        //global work offset
                                    globalWorkSize,
                                    nullptr,        //local work size
                                    0,
                                    nullptr,
                                    nullptr);

    _checkClError(status, "Could not enqueue kernel on device!");

    cl_uint contactListSize;
    status = clEnqueueReadBuffer(_cmdQueue,
                                 _aabbContactListSizeBuff,
                                 CL_TRUE,
                                 0,
                                 sizeof(cl_uint),
                                 &contactListSize,
                                 0,
                                 nullptr,
                                 nullptr);
     _checkClError(status, "Could not transfer contact list size back to host!");
     _aabbContactsSize = contactListSize;

   if(_aabbContactsSize) {
    status = clEnqueueReadBuffer(_cmdQueue,
                                  _aabbContactListBuff,
                                  CL_TRUE,
                                  0,
                                  sizeof(SortSweepAABBContact)*_aabbContactsSize,
                                  &_aabbContacts[0],
                                  0,
                                  nullptr,
                                  nullptr);
    _checkClError(status, "Could not transfer contacts back to host!");
  }
#endif
    }
}

  void initializeOpenCL(void);

template<typename T>
inline void SortSweepAABBPool<T>::uninitialize(void) {
#ifdef ELYSIAN_OPENCL_ACCELERATED
    clReleaseMemObject(_aabbListBuff);
    clReleaseMemObject(_aabbContactListBuff);
    free(_devices);

    free(_platforms);
#else

#endif
}


}


#endif // ELYSIAN_SORT_SWEEP_HPP
