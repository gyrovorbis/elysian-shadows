#ifndef ELYSIAN_SORT_SWEEP_HPP
#define ELYSIAN_SORT_SWEEP_HPP

#include <elysian_mem_pool.hpp>
#include <elysian_asset_typedefs.hpp>
#include <elysian_debug.hpp>
#include <elysian_generic.hpp>

#ifdef ELYSIAN_OPENCL_ACCELERATED
#	ifdef __APPLE__
#		include <OpenCL/opencl.h>
#	else
#		include <CL/cl.h>
#	endif
#endif

#define ELYSIAN_SORT_SWEEP_DIMENSIONS               3
#define ELYSIAN_SORT_SWEEP_POOL_SIZE                500
#define ELYSIAN_SORT_SWEEP_AABB_CONTACT_LIST_SIZE   10000
#define ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_PATH        "/assets/opencl/sweep_kernel.cl"
#define ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_NAME        "aabb_sweep"
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


template<typename T>
class SortSweepAABBPool: public Pool<SortSweepAABB<T>> {
            typedef Pool<SortSweepAABB<T>> Parent;

private:

#ifdef ELYSIAN_OPENCL_ACCELERATED

static inline void _checkClError(const cl_int status, const char*const str) {
	if(status != CL_SUCCESS) {
		dbgprintf("! - OpenCL Error: %s\n", str);
		exit(1);
	}
}

void _clPrintDeviceInfo(const cl_device_id device) {
	auto CL_CHECK = [](const cl_int status) {
		if(status != CL_SUCCESS) {
            dbgprintf("! - OpenCL Error querying device attribute!");
		}
	};

	char buffer[10240];
		 cl_uint buf_uint;
		 cl_ulong buf_ulong;
         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buffer), buffer, nullptr));
         dbgprintf("Device Name:\t%s", buffer);
		 dbgpush();

         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(buffer), buffer, nullptr));
         dbgprintf("Vendor:\t%s", buffer);
         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(buffer), buffer, nullptr));
         dbgprintf("Device Version:\t%s", buffer);
         CL_CHECK(clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(buffer), buffer, nullptr));
         dbgprintf("Driver Version:\t%s", buffer);
         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, nullptr));
         dbgprintf("Max Compute Units:\t%u", (unsigned int)buf_uint);
         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, nullptr));
         dbgprintf("Max Clock Frequency:\t%u", (unsigned int)buf_uint);
         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, nullptr));
         dbgprintf("Global Memory Size:\t%llu", (unsigned long long)buf_ulong);
         CL_CHECK(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_ulong), &buf_ulong, nullptr));
         dbgprintf("Max Workgroup Size:\t%llu", (unsigned long long)buf_ulong);

		 // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
		 size_t workitem_dims;
         clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, nullptr);
         dbgprintf("Max Work Item Dimensions:\t %u", (unsigned int) workitem_dims);

		 size_t workitem_size[3];
         clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, nullptr);
         dbgprintf("Max Work Item Sizes:\t%u / %u / %u", workitem_size[0], workitem_size[1], workitem_size[2]);

		 dbgpop();
}

 void _clPrintPlatformInfo(void) {
	char stringBuff[ELYSIAN_SORT_SWEEP_CL_STRING_BUFF_SIZE];

	auto queryCLPlat = [&](const unsigned index, const cl_platform_info info, const char* string) {
		cl_int status = clGetPlatformInfo(_platforms[index], info, sizeof(char)*ELYSIAN_SORT_SWEEP_CL_STRING_BUFF_SIZE, stringBuff, nullptr);
		if(status == CL_SUCCESS) {
			dbgprintf("%s:\t%s", string, stringBuff);
		} else {
			dbgprintf("Query failed: %s", string);
		}
	};

	for(unsigned i = 0; i < _platformCount; ++i) {
		queryCLPlat(i, CL_PLATFORM_NAME, "Platform Name");
		dbgpush();

		queryCLPlat(i, CL_PLATFORM_VENDOR,		"Vendor");
		queryCLPlat(i, CL_PLATFORM_VERSION,		"Version");
		queryCLPlat(i, CL_PLATFORM_PROFILE,		"Profile");
		queryCLPlat(i, CL_PLATFORM_EXTENSIONS,	"Extensions");

		for(unsigned j = 0; j < _deviceCount[i]; ++j) {
			_clPrintDeviceInfo(_devices[i][j]);
		}

		dbgpop();
	}
}


#endif



#ifdef ELYSIAN_OPENCL_ACCELERATED
	cl_platform_id*     _platforms				= nullptr;
	cl_uint             _platformCount			= 0;
	cl_device_id**      _devices				= nullptr;
	cl_uint*            _deviceCount			= nullptr;
    cl_uint             _curPlatformIndex       = 0;
    cl_uint             _curDeviceIndex         = 1;
    cl_context          _context;
    cl_command_queue    _cmdQueue;
    cl_program          _program;
    cl_kernel           _kernel;
    cl_mem              _aabbListBuff;
    cl_mem              _aabbListSizeBuff;
    cl_mem              _aabbContactListBuff;
    cl_mem              _aabbContactListSizeBuff;
#endif

    SortSweepAABBContact _aabbContacts[ELYSIAN_SORT_SWEEP_AABB_CONTACT_LIST_SIZE];
    unsigned            _aabbContactsSize = 0;

#ifdef ELYSIAN_OPENCL_ACCELERATED
    bool        _useGPU = true;
#else
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

template<typename T>
inline void SortSweepAABBPool<T>::_sweep(void) {
        if(!_useGPU) {
#ifndef ELYSIAN_OPENCL_ACCELERATED

        const unsigned curIndex = SortSweepAABB<T>::curIndex;

        float s[3] = { 0.0f, 0.0f, 0.0f };
        float s2[3] = { 0.0f, 0.0f, 0.0f};
        float v[3];

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
        for(int c = 0; c < ELYSIAN_SORT_SWEEP_DIMENSIONS; ++c) v[c] = s2[c] - (s[c] * s[c]) / (float)(Parent::_endIndex);
        SortSweepAABB<T>::curIndex = 0;
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


template<typename T>
inline void SortSweepAABBPool<T>::initialize(void) {
#ifdef ELYSIAN_OPENCL_ACCELERATED
    dbgprintf("OpenCL Initialization");
    dbgpush();

    cl_int status = CL_SUCCESS;

    //Initialize platforms
    status = clGetPlatformIDs(0, nullptr, &_platformCount);
    _checkClError(status, "Could not get platform count!");
	_platforms = (cl_platform_id*)malloc(_platformCount*sizeof(cl_platform_id));
	_devices = (cl_device_id**)malloc(_platformCount*sizeof(cl_device_id*));
	_deviceCount = (cl_uint*)malloc(_platformCount*sizeof(cl_uint));
    status = clGetPlatformIDs(_platformCount, _platforms, nullptr);
    _checkClError(status, "Could not populate platform array!");

    //Initialize devices
	for(unsigned i = 0; i < _platformCount; ++i) {
		status = clGetDeviceIDs(_platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &_deviceCount[i]);
		_checkClError(status, "Could not get device count!");
		_devices[i] = (cl_device_id*)malloc(_deviceCount[i]*sizeof(cl_device_id));
		status = clGetDeviceIDs(_platforms[i], CL_DEVICE_TYPE_ALL, _deviceCount[i], _devices[i], nullptr);
		_checkClError(status, "Could not populate device array!");

		//Create context
		_context = clCreateContext(nullptr, _deviceCount[i], _devices[i], nullptr, nullptr, &status);
		_checkClError(status, "Could not create context!");
	}
	_clPrintPlatformInfo();

    //Create command queue
    _cmdQueue = clCreateCommandQueue(_context, _devices[_curPlatformIndex][_curDeviceIndex], 0, &status);
    _checkClError(status, "Could not create command queue!");

    //Initialize buffers
    _aabbListBuff               = clCreateBuffer(_context, CL_MEM_READ_ONLY, ELYSIAN_SORT_SWEEP_POOL_SIZE*sizeof(SortSweepCollidable), nullptr, &status);
    _checkClError(status, "Could not allocate aabbListBuff");
    _aabbListSizeBuff           = clCreateBuffer(_context, CL_MEM_READ_ONLY, sizeof(cl_uint), nullptr, &status);
    _checkClError(status, "Could not allocate aabbListSizeBuff");
    _aabbContactListBuff        = clCreateBuffer(_context, CL_MEM_WRITE_ONLY, ELYSIAN_SORT_SWEEP_AABB_CONTACT_LIST_SIZE*sizeof(SortSweepAABBContact), nullptr, &status);
    _checkClError(status, "Could not allocate aabbContactListBuff");
    _aabbContactListSizeBuff    = clCreateBuffer(_context, CL_MEM_WRITE_ONLY, sizeof(cl_uint), nullptr, &status);
    _checkClError(status, "Could not allocate aabbContactListSizeBuff");

    //Load program source from file
    GYFile* fp;
    size_t fileLen;
    char* programSource;
    dbgprintf("Compiling program: %s [%s]", ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_PATH, ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_FLAGS);
    dbgpush();
    if(!gyFileOpen(ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_PATH, "r", &fp)) {
        Debug::printf(Debug::DBG_WARNING, "Could not open source file!");
    }
    gyFileLength(fp, &fileLen);
    programSource = (char*)malloc(sizeof(char)*(fileLen+1));
    gyFileRead(fp, (void*)programSource, fileLen, nullptr);
    programSource[fileLen] = '\0';
    gyFileClose(&fp);

    //Create and compile program
    _program = clCreateProgramWithSource(_context, 1, (const char**)&programSource, nullptr, &status);
    _checkClError(status, "Could not create program object!");
    status = clBuildProgram(_program, _deviceCount[_curPlatformIndex], _devices[_curPlatformIndex], ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_FLAGS, nullptr, nullptr);

    //Check for compilation errors
    if(status != CL_SUCCESS) {
        size_t logSize;
        status = clGetProgramBuildInfo(_program, _devices[_curPlatformIndex][_curDeviceIndex], CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        _checkClError(status, "Could not retrieve program build information!");
        char*const buildLog = (char*)malloc(sizeof(char)*(logSize+1));
        status = clGetProgramBuildInfo(_program, _devices[_curPlatformIndex][_curDeviceIndex], CL_PROGRAM_BUILD_LOG, logSize, buildLog, nullptr);
        buildLog[logSize] = '\0';
        dbgprintf("Build failed!");
        dbgprintf("%s", buildLog);
        free(buildLog);
    } else dbgprintf("Compiled successfully!");
    free(programSource);
    dbgpop();

    //Initialize kernel
    dbgprintf("Initiliazing kernel: %s", ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_NAME);
    _kernel = clCreateKernel(_program, ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_NAME, &status);
    _checkClError(status, "Could not create kernel!");

    //Set kernel arguments
    status = clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_aabbListBuff);
    _checkClError(status, "Could not set arg 0!");
    status = clSetKernelArg(_kernel, 1, sizeof(cl_mem), &_aabbListSizeBuff);
    _checkClError(status, "Could not set arg 1!");
    status = clSetKernelArg(_kernel, 2, sizeof(cl_mem), &_aabbContactListBuff);
    _checkClError(status, "Could not set arg 2!");
    status = clSetKernelArg(_kernel, 3, sizeof(cl_mem), &_aabbContactListSizeBuff);
    _checkClError(status, "Could not set arg 3!");

    dbgpop();
#else


#endif

}

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
