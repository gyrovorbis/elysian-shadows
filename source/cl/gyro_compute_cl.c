#include <cl/gyro_cl.h>
#include <gyro_system_api.h>
#include <gyro_compute_api.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <gyro_file_api.h>

#define GY_CL_BUILD_FLAGS                           "-cl-kernel-arg-info"
#define GY_CL_ENQUEUE_ACQUIRE_RENDERER_OBJECTS_MAX  10
#define CL_QUERY_INFO_STRING_BUFF_SIZE              200
#define CL_MAX_NUMBER_OF_DEVICES_PER_CONTEXT        20
#define GY_CL_ENUM_STR(a) \
    case a: return #a;

//==== PLATFORM-SPECIFIC INTERNAL STRUCTURES ====
typedef struct _PlatformInfo {
    GYComputePlatformInfo   gyInfo;
    char                    platformName[CL_QUERY_INFO_STRING_BUFF_SIZE];
    unsigned                platIdx;
} _PlatformInfo;

typedef struct _DeviceInfo {
    GYComputeDeviceInfo     gyInfo;
    GYComputeDevice*        gyDevice;
} _DeviceInfo;

typedef struct _ComputeDevice {
    GYComputeDevice         gyDevice;
    _DeviceInfo             info;
    cl_device_id            deviceId;
} _ComputeDevice;

typedef struct _Context {
    GYComputeContext        gyContext;
    cl_context              ctx;
} _Context;

typedef struct _CommandQueue {
    GYComputeCommandQueue   gyQueue;
    cl_command_queue        queue;
} _CommandQueue;

typedef struct _Program {
    GYComputeProgram        gyProgram;
    cl_program              program;
    //erorro, build info
} _Program;

typedef struct _Kernel {
    GYComputeKernel         gyKernel;
    GYComputeKernelInfo     gyKernelInfo;
    cl_kernel               kernel;
} _Kernel;

typedef struct _Buffer {
    GYComputeBuffer         gyBuffer;
    cl_mem                  clBuffer;
    cl_GLuint               glBuffer;
} _Buffer;

//===== OPENCL INTERNAL DATA ====
static cl_context*             _clContexts         = NULL;
static cl_platform_id*         _clPlatforms        = NULL;
static cl_uint                 _clPlatformCount    = 0;
static cl_device_id**          _clDevices          = NULL;
static cl_uint*                _clDeviceCount      = NULL;

//==== API INTERNAL DATA ====
static GYComputeInitParams     _initParams;
static _ComputeDevice**        _devices            = NULL;
static _PlatformInfo*          _platformInfo       = NULL;
static int                     _initialized        = 0;
#ifdef GY_COMPUTE_DEBUG
static GYComputeDebugInfo      _debugInfo;
#endif

#ifdef GY_CL_OPENGL_INTEROP
typedef struct _CLGLInteropInfo {
    int                 glQueryDataValid;
    cl_device_id        rendererDevice;
    cl_platform_id      rendererPlatform;
#   ifdef __APPLE__
    CGLContextObj       cglContextObj;
    CGLShareGroupObj    cglShareGroupObj;
#   elif defined(__WIN32)
    HGLRC               wglContext;
    HDC                 wglDeviceContext;
#   else
    GLXContext          glXContext;
    Display*            glXDisplay;
#   endif
} _CLGLInteropInfo;

#ifndef __APPLE__
static clGetGLContextInfoKHR_fn clGetGLContextInfo = NULL;
#endif


static _CLGLInteropInfo         _clGlInteropInfo;
#endif

//========== INTERNAL OPENCL <=> LIBGYRO TYPE CONVERSTION UTILITY FUNCTIONS ===========

static inline GYComputeDevice _gyComputeDeviceFromClDeviceId(cl_device_id clDevice) {
    GYComputeDevice dev = { -1, -1 };
    for(unsigned p = 0; p < _clPlatformCount; ++p) {
        for(unsigned d = 0; d < _clDeviceCount[p]; ++d) {
            if(_clDevices[p][d] == clDevice) {
                dev.platformIdx = p;
                dev.deviceIdx = d;
                goto done;
            }
        }
    }
done:
    return dev;
}

static inline int _gyComputePlatformIndexFromClPlatformId(cl_platform_id clPlatform) {
    for(unsigned p = 0; p < _clPlatformCount; ++p) {
        if(_clPlatforms[p] == clPlatform) {
            return p;
        }
    }
    return -1;
}

static inline _ComputeDevice* _clDeviceFromGyComputeDevice(const GYComputeDevice* dev) {
    GY_ASSERT(dev && dev->platformIdx >= 0 && dev->platformIdx < (int)_clPlatformCount && dev->deviceIdx >= 0 && dev->deviceIdx < (int)_clDeviceCount[dev->platformIdx]);
    return &_devices[dev->platformIdx][dev->deviceIdx];
}


inline static GY_COMPUTE_DEVICE_TYPE _clDeviceTypeToGyroDeviceType(cl_device_type type) {
    switch(type) {
        case CL_DEVICE_TYPE_CPU:    return GY_COMPUTE_DEVICE_CPU;
        case CL_DEVICE_TYPE_GPU:    return GY_COMPUTE_DEVICE_GPU;
        default:                    return GY_COMPUTE_DEVICE_OTHER;
    }
}

inline static GY_COMPUTE_KERNEL_ARG_ACCESS_QUALIFIER _clKernelArgAccessQualifierToGyro(cl_kernel_arg_access_qualifier qualifier) {
    switch(qualifier) {
    case CL_KERNEL_ARG_ACCESS_NONE:         return GY_COMPUTE_KERNEL_ARG_ACCESS_NONE;
    case CL_KERNEL_ARG_ACCESS_WRITE_ONLY:   return GY_COMPUTE_KERNEL_ARG_ACCESS_WRITE_ONLY;
    case CL_KERNEL_ARG_ACCESS_READ_ONLY:    return GY_COMPUTE_KERNEL_ARG_ACCESS_READ_ONLY;
    case CL_KERNEL_ARG_ACCESS_READ_WRITE:   return GY_COMPUTE_KERNEL_ARG_ACCESS_READ_WRITE;
    default:                                return GY_COMPUTE_KERNEL_ARG_ACCESS_UNKNOWN;
    }
}

inline static const char* _gyKernelArgAccessQualifierStr(GY_COMPUTE_KERNEL_ARG_ACCESS_QUALIFIER qualifier) {
    switch(qualifier) {
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ACCESS_NONE)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ACCESS_WRITE_ONLY)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ACCESS_READ_ONLY)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ACCESS_READ_WRITE)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ACCESS_UNKNOWN)
    }
}

inline static GY_COMPUTE_KERNEL_ARG_ADDRESS_QUALIFIER _clKernelArgAddressQualifierToGyro(cl_kernel_arg_address_qualifier qualifier) {
    switch(qualifier) {
    case CL_KERNEL_ARG_ADDRESS_CONSTANT:    return GY_COMPUTE_KERNEL_ARG_ADDRESS_CONSTANT;
    case CL_KERNEL_ARG_ADDRESS_GLOBAL:      return GY_COMPUTE_KERNEL_ARG_ADDRESS_GLOBAL;
    case CL_KERNEL_ARG_ADDRESS_LOCAL:       return GY_COMPUTE_KERNEL_ARG_ADDRESS_LOCAL;
    case CL_KERNEL_ARG_ADDRESS_PRIVATE:     return GY_COMPUTE_KERNEL_ARG_ADDRESS_PRIVATE;
    default: GY_ASSERT(0);                  return GY_COMPUTE_KERNEL_ARG_ADDRESS_UNKNOWN;
    }
}

inline static const char* _gyKernelArgAddressQualifierStr(GY_COMPUTE_KERNEL_ARG_ADDRESS_QUALIFIER qualifier) {
    switch(qualifier) {
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ADDRESS_CONSTANT)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ADDRESS_GLOBAL)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ADDRESS_LOCAL)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ADDRESS_PRIVATE)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_ADDRESS_UNKNOWN)
    }
}


inline static int _clKernelArgTypeQualifierFlagToGyro(cl_kernel_arg_type_qualifier flag) {
    switch(flag) {
    case CL_KERNEL_ARG_TYPE_CONST:          return GY_COMPUTE_KERNEL_ARG_TYPE_CONST;
    case CL_KERNEL_ARG_TYPE_RESTRICT:       return GY_COMPUTE_KERNEL_ARG_TYPE_NONE;
    case CL_KERNEL_ARG_TYPE_VOLATILE:       return GY_COMPUTE_KERNEL_ARG_TYPE_VOLATILE;
    default:                                return GY_COMPUTE_KERNEL_ARG_TYPE_NONE;
    }
}

inline static int _clKernelArgTypeQualifierToGyro(cl_kernel_arg_type_qualifier qualifier) {
    int gyQualifier = 0;
    for(unsigned i = 0; i < sizeof(cl_kernel_arg_type_qualifier)*8; ++i) {
        gyQualifier |= _clKernelArgTypeQualifierFlagToGyro(qualifier&(0x1<<i));
    }
    return gyQualifier;
}

inline static const char* _gyKernelArgTypeQualifierFlagStr(int flag) {
    switch(flag) {
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_TYPE_CONST)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_TYPE_RESTRICT)
        GY_CL_ENUM_STR(GY_COMPUTE_KERNEL_ARG_TYPE_VOLATILE)
        default: return "GY_COMPUTE_KERNEL_ARG_TYPE_NONE";
    }
}

inline static const char* _gyKernelArgTypeQualifierStr(uint32_t qualifier) {
    static char buffer[CL_QUERY_INFO_STRING_BUFF_SIZE] = { '\0' };
    int count = 0;
    for(unsigned i = 0; i < sizeof(qualifier)*8; ++i) {
        int q = _clKernelArgTypeQualifierFlagToGyro(qualifier & (0x1<<i));
        if(q != GY_COMPUTE_KERNEL_ARG_TYPE_NONE) {
            if(count) strcat(buffer, "|");
            strcat(buffer, _gyKernelArgTypeQualifierFlagStr(q));
            ++count;
        }
    }
    if(!count) strcpy(buffer, "GY_COMPUTE_KERNEL_ARG_TYPE_NONE");
    return buffer;
}

inline static const char* _clDeviceTypeStr(cl_device_type type) {
    switch(type) {
    case CL_DEVICE_TYPE_ACCELERATOR: return "CL_DEVICE_TYPE_ACCELERATOR";
    case CL_DEVICE_TYPE_CPU:         return "CL_DEVICE_TYPE_CPU";
    case CL_DEVICE_TYPE_GPU:         return "CL_DEVICE_TYPE_GPU";
    case CL_DEVICE_TYPE_DEFAULT:     return "CL_DEVICE_TYPE_DEFAULT";
    case CL_DEVICE_TYPE_CUSTOM:      return "CL_DEVICE_TYPE_CUSTOM";
    case CL_DEVICE_TYPE_ALL:         return "CL_DEVICE_TYPE_ALL";
#ifdef __APPLE__
    case CL_DEVICE_TYPE_USE_ID:      return "CL_DEVICE_TYPE_USE_ID"; //Nonstandardized Apple BULLSHIT
#endif
    default:                         return "UNKOWN";
    }
}

static inline const char* _clDeviceFpConfigBitStr(cl_device_fp_config config) {
    switch(config) {
    case CL_FP_DENORM:                          return "CL_FP_DENORM";              //denorms are supported
    case CL_FP_INF_NAN:                         return "CL_FP_INF_NAN";             //INF and quiet NaNs are supported
    case CL_FP_ROUND_TO_NEAREST:                return "CL_FP_ROUND_TO_NEAREST";    //round to nearest even rounding mode supported
    case CL_FP_ROUND_TO_ZERO:                   return "CL_FP_ROUND_TO_ZERO";       //round to zero rounding mode supported
    case CL_FP_ROUND_TO_INF:                    return "CL_FP_ROUND_TO_INF";        //round to +ve and -ve infinity rounding modes supported
    case CL_FP_FMA:                             return "CL_FP_FMA";                 //IEEE754-2008 fused multiply-add is supported
    case CL_FP_SOFT_FLOAT:                      return "CL_FP_SOFT_FLOAT";
    case CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT:   return "CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT";
    default:                                    return "UNKNOWN";
    }
}

static inline const char* _clDeviceMemCacheTypeStr(cl_device_mem_cache_type cacheType) {
    switch(cacheType) {
    case CL_NONE:               return "CL_NONE";
    case CL_READ_ONLY_CACHE:    return "CL_READ_ONLY_CACHE";
    case CL_READ_WRITE_CACHE:   return "CL_READ_WRITE_CACHE";
    default:                    return "UNKNOWN";
    }
}

static inline int _clDeviceExtensionSupported(const GYComputeDeviceInfo* info, const char* extension) {
   return strstr(info->extensions, extension) != NULL;
}

static inline const char* _clProgramBuildStatusStr(cl_build_status status) {
    switch(status) {
    case CL_BUILD_ERROR:        return "ERROR";
    case CL_BUILD_SUCCESS:      return "SUCCESS";
    case CL_BUILD_IN_PROGRESS:  return "BUILD_IN_PROGRESS";
    case CL_BUILD_NONE:         return "BUILD_NONE";
    default:                    return "UNKNOWN";
    }
}

inline static int _clMemTypeFromGyro(GY_COMPUTE_MEMORY_TYPE type) {
    switch(type) {
    case GY_COMPUTE_MEMORY_TYPE_READ_WRITE:             return CL_MEM_READ_WRITE;
    case GY_COMPUTE_MEMORY_TYPE_WRITE_ONLY:             return CL_MEM_WRITE_ONLY;
    case GY_COMPUTE_MEMORY_TYPE_READ_ONLY:              return CL_MEM_READ_ONLY;
    case GY_COMPUTE_MEMORY_TYPE_USE_HOST_POINTER:       return CL_MEM_USE_HOST_PTR;
    case GY_COMPUTE_MEMORY_TYPE_ALLOC_HOST_POINTER:     return CL_MEM_ALLOC_HOST_PTR;
    case GY_COMPUTE_MEMORY_TYPE_COPY_HOST_POINTER:      return CL_MEM_COPY_HOST_PTR;
    default: GY_ASSERT(0);
    }
}

static int _clQueryDeviceInfo(cl_device_id device, _DeviceInfo* info) {
    char                        buffer[GY_COMPUTE_DEVICE_INFO_EXTENSIONS_SIZE];
    size_t                      vec3[3] = { 0, 0, 0 };
    cl_ulong                    buf_ulong;
    size_t                      buf_sizet;
    size_t                      retSize;
    cl_bool                     buf_bool;
    cl_uint                     buf_uint;
    cl_device_type              dev_type;
    cl_device_fp_config         dev_fpConfig;
    cl_device_mem_cache_type    dev_memCacheType;
    int                         success = 1;

    //======= LOCAL QUERY UTILITY MACROS ========

#define QUERY_DEV_INFO(dev_info, devStr, size, buff, fmt)                               \
    if(!GY_CL_CHECK_ERROR(clGetDeviceInfo(device, dev_info, size, &buff, &retSize))) {  \
        buffer[retSize] = '\0';                                                         \
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20"fmt, devStr, buff);                        \
    } else {                                                                            \
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", devStr, "QUERY FAILED");                  \
        success = 0;                                                                    \
    }

#define QUERY_DEV_INFO_STR(dev_info)    \
    buffer[0] = '\0';                   \
    QUERY_DEV_INFO(dev_info, #dev_info, sizeof(buffer), buffer, "s")

#define QUERY_DEV_INFO_UINT(dev_info)   \
    buf_uint = 0;                       \
    QUERY_DEV_INFO(dev_info, #dev_info, sizeof(cl_uint), buf_uint, "u")

#define QUERY_DEV_INFO_ULONG(dev_info)  \
    buf_ulong = 0;                      \
    QUERY_DEV_INFO(dev_info, #dev_info, sizeof(cl_ulong), buf_ulong, "lu")

#define QUERY_DEV_INFO_SIZET(dev_info)  \
    buf_sizet = 0;                      \
    QUERY_DEV_INFO(dev_info, #dev_info, sizeof(size_t), buf_sizet, "zu")

#define QUERY_DEV_INFO_BOOL(dev_info)  \
    buf_sizet = 0;                      \
    QUERY_DEV_INFO(dev_info, #dev_info, sizeof(cl_bool), buf_bool, "u")

    //======== BEGIN QUERIES ========

    if(info) memset(info, 0, sizeof(GYComputeDeviceInfo));

    QUERY_DEV_INFO_STR(CL_DEVICE_NAME)

    // Query CL_DEVICE_TYPE
    if(!GY_CL_CHECK_ERROR(clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &dev_type, NULL))) {
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20s", "CL_DEVICE_TYPE", _clDeviceTypeStr(dev_type));
    } else {
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", "CL_DEVICE_TYPE", "QUERY FAILED");
        success = 0;
    }
    if(info) info->gyInfo.devType = _clDeviceTypeToGyroDeviceType(dev_type);

    QUERY_DEV_INFO_STR(CL_DEVICE_VENDOR)
    QUERY_DEV_INFO_UINT(CL_DEVICE_VENDOR_ID)
    QUERY_DEV_INFO_STR(CL_DEVICE_VERSION)
    QUERY_DEV_INFO_STR(CL_DEVICE_PROFILE)
    QUERY_DEV_INFO_STR(CL_DRIVER_VERSION)
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_COMPUTE_UNITS)
    if(info) info->gyInfo.maxComputeUnits = buf_uint;
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_CLOCK_FREQUENCY)
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)
    if(info) info->gyInfo.maxWorkItemDimensions = buf_uint;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_MAX_WORK_GROUP_SIZE)
    if(info) info->gyInfo.maxWorkGroupSize = buf_sizet;

    // Query CL_DEVICE_MAX_WORK_ITEM_SIZES
    if(!GY_CL_CHECK_ERROR(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(vec3), &vec3, NULL))) {
        if(info) {
            info->gyInfo.maxWorkItemSizes[0] = vec3[0]; info->gyInfo.maxWorkItemSizes[1] = vec3[1]; info->gyInfo.maxWorkItemSizes[2] = vec3[2];
        }
        char vecBuff[50];
        sprintf(vecBuff, "<%zu, %zu, %zu>", vec3[0], vec3[1], vec3[2]);
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20s", "CL_DEVICE_MAX_WORK_ITEM_SIZES", vecBuff);
    } else {
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", "CL_DEVICE_MAX_WORK_ITEM_SIZES", "QUERY FAILED");
        success = 0;
    }

    QUERY_DEV_INFO_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR)
    QUERY_DEV_INFO_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT)
    QUERY_DEV_INFO_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT)
    if(info) info->gyInfo.preferredVectorWidthInt = buf_uint;
    QUERY_DEV_INFO_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG)
    QUERY_DEV_INFO_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT)
    if(info) info->gyInfo.preferredVectorWidthFloat = buf_uint;
    QUERY_DEV_INFO_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE)
    QUERY_DEV_INFO_UINT(CL_DEVICE_ADDRESS_BITS)
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_READ_IMAGE_ARGS)
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_WRITE_IMAGE_ARGS)
    QUERY_DEV_INFO_UINT(CL_DEVICE_ADDRESS_BITS)
    QUERY_DEV_INFO_ULONG(CL_DEVICE_MAX_MEM_ALLOC_SIZE)
    if(info) info->gyInfo.maxMemAllocSize = buf_ulong;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_IMAGE2D_MAX_WIDTH)
    if(info) info->gyInfo.image2DMaxSize[0] = buf_sizet;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_IMAGE2D_MAX_HEIGHT)
    if(info) info->gyInfo.image2DMaxSize[1] = buf_sizet;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_IMAGE3D_MAX_WIDTH)
    if(info) info->gyInfo.image3DMaxSize[0] = buf_sizet;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_IMAGE3D_MAX_HEIGHT)
    if(info) info->gyInfo.image3DMaxSize[1] = buf_sizet;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_IMAGE3D_MAX_DEPTH)
    if(info) info->gyInfo.image3DMaxSize[2] = buf_sizet;
    QUERY_DEV_INFO_UINT(CL_DEVICE_IMAGE_SUPPORT)
    QUERY_DEV_INFO_SIZET(CL_DEVICE_MAX_PARAMETER_SIZE)
    if(info) info->gyInfo.maxParameterSize = buf_sizet;
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_SAMPLERS)
    if(info) info->gyInfo.maxSamplers = buf_uint;
    QUERY_DEV_INFO_UINT(CL_DEVICE_MEM_BASE_ADDR_ALIGN)
    if(info) info->gyInfo.baseAddrAlign = buf_uint;
    QUERY_DEV_INFO_UINT(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE)
    if(info) info->gyInfo.minDataTypeAlignSize = buf_uint;
    QUERY_DEV_INFO_SIZET(CL_DEVICE_PROFILING_TIMER_RESOLUTION)
    if(info) info->gyInfo.profilingTimerResolution = buf_sizet;
    QUERY_DEV_INFO_BOOL(CL_DEVICE_IMAGE_SUPPORT)
    if(info) info->gyInfo.imageSupport = buf_bool;

    //Query CL_DEVICE_SINGLE_FP_CONFIG
    if(!GY_CL_CHECK_ERROR(clGetDeviceInfo(device, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(cl_device_fp_config), &dev_fpConfig, NULL))) {
        char maskBuff[300] = { '\0' };
        int first = 1;
        cl_device_fp_config bit;

        for(unsigned i = 0; i < sizeof(cl_device_fp_config)/8; ++i) {
            if((bit = (dev_fpConfig & (0x1<<i)))) {
                if(!first) strcat(maskBuff, "|");
                else first = 0;
                strcat(maskBuff, _clDeviceFpConfigBitStr(bit));
            }
        }

        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20s", "CL_DEVICE_SINGLE_FP_CONFIG", maskBuff);
    } else {
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", "CL_DEVICE_SINGLE_FP_CONFIG", "QUERY FAILED");
        success = 0;
    }

    //Query CL_DEVICE_GLOBAL_MEM_CACHE_TYPE
    if(!GY_CL_CHECK_ERROR(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), &dev_memCacheType, NULL))) {
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20s", "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", _clDeviceMemCacheTypeStr(dev_memCacheType));
    } else {
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", "QUERY FAILED");
        success = 0;
    }

    QUERY_DEV_INFO_UINT(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE)
    if(info) info->gyInfo.globalMemCacheLineSize = buf_uint;
    QUERY_DEV_INFO_ULONG(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE)
    QUERY_DEV_INFO_ULONG(CL_DEVICE_GLOBAL_MEM_SIZE)
    if(info) info->gyInfo.globalMemSize = buf_ulong;
    QUERY_DEV_INFO_ULONG(CL_DEVICE_LOCAL_MEM_SIZE)
    if(info) info->gyInfo.localMemSize = buf_ulong;
    QUERY_DEV_INFO_ULONG(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE)
    if(info) info->gyInfo.maxConstantMemSize = buf_ulong;
    QUERY_DEV_INFO_UINT(CL_DEVICE_MAX_CONSTANT_ARGS)
    if(info) info->gyInfo.maxConstantArgs = buf_ulong;
    QUERY_DEV_INFO_STR(CL_DEVICE_EXTENSIONS);
    if(info) strncpy(info->gyInfo.extensions, buffer, GY_COMPUTE_DEVICE_INFO_EXTENSIONS_SIZE);

    //Check for OpenGL shared context extensions
    if(info && _clDeviceExtensionSupported(&info->gyInfo, GY_CL_GL_DEVICE_SHARED_CONTEXT_EXTENSION)) info->gyInfo.rendererSharedContext = 1;
    if(info && _clDeviceExtensionSupported(&info->gyInfo, "cl_khr_global_int32_base_atomics")) info->gyInfo.globalInt32BaseAtomics = 1;
    if(info && _clDeviceExtensionSupported(&info->gyInfo, "cl_khr_local_int32_base_atomics")) info->gyInfo.localInt32BaseAtomics = 1;

    //Check OpenGL interop-related info (if enabled)
#ifdef GY_CL_OPENGL_INTEROP
    if(info && _clGlInteropInfo.glQueryDataValid &&
            _clDevices[info->gyDevice->platformIdx][info->gyDevice->deviceIdx] == _clGlInteropInfo.rendererDevice)
        info->gyInfo.isRenderDevice = 1;

    if(info && _clGlInteropInfo.glQueryDataValid &&
            _clDevices[info->gyDevice->platformIdx][info->gyDevice->deviceIdx] == _clGlInteropInfo.rendererDevice)
        info->gyInfo.isRenderDevice = 1;
#endif

#undef QUERY_DEV_INFO_SIZET
#undef QUERY_DEV_INFO_ULONG
#undef QUERY_DEV_INFO_UINT
#undef QUERY_DEV_INFO

    return success;
}

static inline int _clPlatformExtensionSupported(const GYComputePlatformInfo* info, const char* extension) {
   return strstr(info->extensions, extension) != NULL;
}

static int _clQueryPlatformInfo(const cl_platform_id platform, _PlatformInfo* info) {
    int success = 1;
    char stringBuff[CL_QUERY_INFO_STRING_BUFF_SIZE];

    if(GY_CL_CHECK_ERROR(clGetPlatformInfo(platform, CL_PLATFORM_NAME, CL_QUERY_INFO_STRING_BUFF_SIZE, stringBuff, NULL))) {
        success = 0;
        _gyLog(GY_DEBUG_WARNING, "CL_PLATFORM_NAME platform query failed!");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "%25s : %20s", "CL_PLATFORM_NAME", stringBuff);
    }

    if(GY_CL_CHECK_ERROR(clGetPlatformInfo(platform, CL_PLATFORM_VERSION, CL_QUERY_INFO_STRING_BUFF_SIZE, stringBuff, NULL))) {
        success = 0;
        _gyLog(GY_DEBUG_WARNING, "CL_PLATFORM_VERSION platform query failed!");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "%25s : %20s", "CL_PLATFORM_VERSION", stringBuff);
    }

    if(GY_CL_CHECK_ERROR(clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, CL_QUERY_INFO_STRING_BUFF_SIZE, stringBuff, NULL))) {
        success = 0;
        _gyLog(GY_DEBUG_WARNING, "CL_PLATFORM_PROFILE platform query failed!");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "%25s : %20s", "CL_PLATFORM_PROFILE", stringBuff);
    }

    if(GY_CL_CHECK_ERROR(clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, CL_QUERY_INFO_STRING_BUFF_SIZE, stringBuff, NULL))) {
        success = 0;
        _gyLog(GY_DEBUG_WARNING, "CL_PLATFORM_EXTENSIONS platform query failed!");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "%25s : %20s", "CL_PLATFORM_EXTENSIONS", stringBuff);
    }

    if(info && _clPlatformExtensionSupported(&info->gyInfo, GY_CL_GL_PLATFORM_SHARED_CONTEXT_EXTENSION))  info->gyInfo.rendererContextSharing = 1;

#ifdef GY_CL_OPENGL_INTEROP
    if(info && _clGlInteropInfo.glQueryDataValid && _clPlatforms[info->platIdx] == _clGlInteropInfo.rendererPlatform) info->gyInfo.isRenderPlatform = 1;
#endif

    return success;
}

static int _clQueryKernelArgInfo(const cl_kernel kernel, unsigned index, GYComputeKernelArgInfo* info) {
    char nameBuff[GY_COMPUTE_KERNEL_ARG_NAME_SIZE];
    char typeNameBuff[GY_COMPUTE_KERNEL_ARG_TYPE_NAME_SIZE];
    cl_kernel_arg_access_qualifier access;
    int gyAccess;
    cl_kernel_arg_type_qualifier type;
    int gyType;
    cl_kernel_arg_address_qualifier address;
    GY_DEBUG_SEVERITY severity;
    int gyAddress;
    size_t retSize;
    int success = 0;

    //Query name
    if(!GY_CL_CHECK_ERROR(clGetKernelArgInfo(kernel, index, CL_KERNEL_ARG_NAME, GY_COMPUTE_KERNEL_ARG_NAME_SIZE-1, &nameBuff, &retSize))) {
        nameBuff[retSize] = '\0';
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20s", "CL_KERNEL_ARG_NAME", nameBuff);
    } else {
        nameBuff[0] = '\0';
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", "CL_KERNEL_ARG_NAME", "QUERY FAILED");
        success = 0;
    }
    if(info) strcpy(info->name, nameBuff);


    //Query type name
    if(!GY_CL_CHECK_ERROR(clGetKernelArgInfo(kernel, index, CL_KERNEL_ARG_TYPE_NAME, GY_COMPUTE_KERNEL_ARG_TYPE_NAME_SIZE-1, &typeNameBuff, &retSize))) {
        typeNameBuff[retSize] = '\0';
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20s", "CL_KERNEL_ARG_TYPE_NAME", typeNameBuff);
    } else {
        typeNameBuff[0] = '\0';
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", "CL_KERNEL_ARG_TYPE_NAME", "QUERY FAILED");
        success = 0;
    }
    if(info) strcpy(info->typeName, typeNameBuff);

    //Query Addressess Qualifier
    if(!GY_CL_CHECK_ERROR(clGetKernelArgInfo(kernel, index, CL_KERNEL_ARG_ADDRESS_QUALIFIER, sizeof(cl_kernel_arg_address_qualifier), &address, NULL))) {
        gyAddress = _clKernelArgAddressQualifierToGyro(address);
        severity = GY_DEBUG_VERBOSE;
    } else {
        gyAddress = GY_COMPUTE_KERNEL_ARG_ADDRESS_UNKNOWN;
        severity = GY_DEBUG_ERROR;
        success = 0;
    }
    _gyLog(severity, "%40s : %20s", "CL_KERNEL_ARG_ADDRESS_QUALIFIER", _gyKernelArgAddressQualifierStr(gyAddress));
    if(info) info->addressQualifier = gyAddress;


    //Query Access Qualifier
    if(!GY_CL_CHECK_ERROR(clGetKernelArgInfo(kernel, index, CL_KERNEL_ARG_ACCESS_QUALIFIER, sizeof(cl_kernel_arg_access_qualifier), &access, NULL))) {
        gyAccess = _clKernelArgAccessQualifierToGyro(access);
        severity = GY_DEBUG_VERBOSE;
    } else {
        gyAccess = GY_COMPUTE_KERNEL_ARG_ACCESS_UNKNOWN;
        severity = GY_DEBUG_ERROR;
        success = 0;
    }
    _gyLog(severity, "%40s : %20s", "CL_KERNEL_ARG_ACCESS_QUALIFIER", _gyKernelArgAccessQualifierStr(gyAccess));
    if(info) info->accessQualifier = gyAccess;

    //Query Type Qualifier
    if(!GY_CL_CHECK_ERROR(clGetKernelArgInfo(kernel, index, CL_KERNEL_ARG_TYPE_QUALIFIER, sizeof(cl_kernel_arg_type_qualifier), &type, NULL))) {
        severity = GY_DEBUG_VERBOSE;
        gyType = _clKernelArgTypeQualifierToGyro(type);
    } else {
        gyType = GY_COMPUTE_KERNEL_ARG_TYPE_NONE;
        severity = GY_DEBUG_ERROR;
        success = 0;
    }
    _gyLog(severity, "%40s : %20s", "CL_KERNEL_ARG_TYPE_QUALIFIER", _gyKernelArgTypeQualifierStr(gyType));
    if(info) info->typeQualifier = gyType;

    return success;
}

static int _clQueryKernelInfo(const cl_kernel kernel, GYComputeKernelInfo* info) {
    int success = 1;
    cl_uint numArgs;
    if(GY_CL_CHECK_ERROR(clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &numArgs, NULL))) {
        success = 0;
        _gyLog(GY_DEBUG_WARNING, "CL_KERNEL_NUM_ARGS kernel query failed!");
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "%25s : %20u", "CL_KERNEL_NUM_ARGS", numArgs);
        if(info) info->numArgs = numArgs;
    }

    if(info && numArgs) {
        info->argInfo = malloc(sizeof(GYComputeKernelArgInfo)*numArgs);
        memset(info->argInfo, 0, sizeof(GYComputeKernelArgInfo)*numArgs);
    }

    for(unsigned a = 0; a < numArgs; ++a) {
        _gyLog(GY_DEBUG_VERBOSE, "Argument [%u] Querying OpenCL Kernel Arg Info", a);
        _gyPush();
        success &= _clQueryKernelArgInfo(kernel, a, info? &info->argInfo[a] : NULL);
        _gyPop(1);
    }

    return success;
}


static int _clQueryKernelWorkgroupInfo(const cl_kernel kernel, const cl_device_id device, GYComputeKernelWorkgroupInfo* info) {
    int success = 1;
    cl_ulong buf_ulong;
    size_t buf_sizet;
    size_t compileWgSize[3];
    size_t retSize;

#define QUERY_WG_INFO(info, devStr, size, buff, fmt)                                                        \
    if(!GY_CL_CHECK_ERROR(clGetKernelWorkGroupInfo(kernel, device, info, size, &buff, &retSize))) {         \
        GY_ASSERT(retSize == size);                                                                         \
        _gyLog(GY_DEBUG_VERBOSE, "%40s : %20"fmt, devStr, buff);                                            \
    } else {                                                                                                 \
        _gyLog(GY_DEBUG_ERROR, "%40s : %20s", devStr, "QUERY FAILED");                                      \
        success = 0;                                                                                         \
    }

#define QUERY_WG_INFO_ULONG(info)  \
    buf_ulong = 0;                      \
    QUERY_WG_INFO(info, #info, sizeof(cl_ulong), buf_ulong, "lu")

#define QUERY_WG_INFO_SIZET(info)  \
    buf_sizet = 0;                      \
    QUERY_WG_INFO(info, #info, sizeof(size_t), buf_sizet, "zu")

    QUERY_WG_INFO_SIZET(CL_KERNEL_WORK_GROUP_SIZE)
    if(info) info->workGroupSize = buf_sizet;

    QUERY_WG_INFO_ULONG(CL_KERNEL_LOCAL_MEM_SIZE)
    if(info) info->localMemSize = buf_ulong;

    QUERY_WG_INFO_SIZET(CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE)
    if(info) info->preferredWorkGroupSizeMultiple = buf_sizet;

    QUERY_WG_INFO_ULONG(CL_KERNEL_PRIVATE_MEM_SIZE)
    if(info) info->privateMemSize = buf_ulong;

    if(!GY_CL_CHECK_ERROR(clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(compileWgSize), &compileWgSize, &retSize))) {
        char strbuff[GY_COMPUTE_DEVICE_INFO_EXTENSIONS_SIZE];
        GY_ASSERT(sizeof(compileWgSize) == retSize);
        if(info) memcpy(info->compileWorkGroupSize, compileWgSize, sizeof(compileWgSize));
        sprintf(strbuff, "<%zu, %zu, %zu>", compileWgSize[0], compileWgSize[1], compileWgSize[2]);
        _gyLog(GY_DEBUG_VERBOSE, "%25s : %20s", "CL_KERNEL_COMPILE_WORK_GROUP_SIZE", strbuff);
    } else {
        _gyLog(GY_DEBUG_ERROR, "CL_KERNEL_COMPILE_WORK_GROUP_SIZE query failed!");
        success = 0;
    }

#undef QUERY_WG_INFO_SIZET
#undef QUERY_WG_INFO_ULONG
#undef QUERY_WG_INFO

    return success;
}


//===================== INTERNAL OPENCL UTILITY FUNCTIONS ============================
#ifdef GY_CL_OPENGL_INTEROP
static int _clGLQueryInteropInfo(void) {
    memset(&_clGlInteropInfo, 0, sizeof(_CLGLInteropInfo));
    if(!_initParams.rendererInterop) return 0;

    int success = 1;
    _gyLog(GY_DEBUG_VERBOSE, "[GY_COMPUTE_API] Querying for OpenGL/OpenCL Interop Support.");
    _gyPush();

#ifdef GY_COMPUTE_DEBUG
    memset(&_debugInfo, 0, sizeof(GYComputeDebugInfo));
#endif


#   ifdef __APPLE__
    //Retrieve sharegroup CL/GL interop bullshit from Apple.
    _clGlInteropInfo.cglContextObj = CGLGetCurrentContext();
    _clGlInteropInfo.cglShareGroupObj = CGLGetShareGroup(_clGlInteropInfo.cglContextObj);
    if(!_clGlInteropInfo.cglContextObj) {
        _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (MacOS, CGL): Failed to retrive current CGL Context object!");
        success = 0;
        goto done;
    }

    if(!_clGlInteropInfo.cglShareGroupObj) {
        _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (MacOS, CGL): Failed to retrieve CGL Share Group object!");
        success = 0;
        goto done;
    }

    cl_context_properties properties[] = {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (intptr_t)_clGlInteropInfo.cglShareGroupObj,
        0
    };

    cl_int errorCode;
    cl_context context = clCreateContext(properties, 0, NULL, 0, 0, &errorCode);
    if(GY_CL_CHECK_ERROR(errorCode)) {
        _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (MacOS, CGL): Failed to create a CL context from GL sharegroup when querying for renderer info!");
        success = 0;
        goto done;
    }

    //Poll for the OpenCL Device associated with the GL/CL context used by the renderer
    if(GY_CL_CHECK_ERROR(clGetGLContextInfoAPPLE(context, _clGlInteropInfo.cglContextObj, CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE, sizeof(cl_device_id), &_clGlInteropInfo.rendererDevice, NULL))) {
        _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (MacOS, CGL): Failed to retrieve OpenCL Device associated with OpenCL context created from OpenGL context!");
        success = 0;
        goto done;
    }

    if(GY_CL_CHECK_ERROR(clGetDeviceInfo(_clGlInteropInfo.rendererDevice, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &_clGlInteropInfo.rendererPlatform, NULL))) {
        _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (MacOS, CGL) Failed to query OpenCL platform corresponding to active OpenGL device!");
        success = 0;
    }

#   else
    clGetGLContextInfo = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
    if(!clGetGLContextInfo) {
        _gyLog(GY_DEBUG_ERROR, "Failed to load extension function address for 'clGetGLContextInfoKHR'!");
        success = 0;
        goto done;
    }

    for(unsigned p = 0; p < _clPlatformCount; ++p) {

        cl_context_properties properties[7] = {
            CL_GL_CONTEXT_KHR,
            0,
            0,
            0,
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)p,
            0
        };

#       ifdef __WIN32
        _clGlInteropInfo.wglContext         = wglGetCurrentContext();
        _clGlInteropInfo.wglDeviceContext   = wglGetCurrentDC();

        if(!_clGlInteropInfo.wglContext) {
            _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (Windows, WGL): Failed to retrieve handle to current OpenGL rendering context!");
            success = 0;
            goto done;
        }

        if(!_clGlInteropInfo.wglDeviceContext) {
            _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (Windows, WGL): Failed to retrieve handle to device context associated with current OpenGL rendering context!");
            success = 0;
            goto done;
        }

        properties[1] = (cl_context_properties)_clGlInteropInfo.wglContext;
        properties[2] = CL_WGL_HDC_KHR;
        properties[3] = (cl_context_properties)_clGlInteropInfo.wglDeviceContext;

#       else
        _clGlInteropInfo.glXContext = glXGetCurrentContext();
        _clGlInteropInfo.glXDisplay = glXGetCurrentDisplay();

        if(!_clGlInteropInfo.glXContext) {
            _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (Linux, GLX): Failed to retrieve current OpenGL context associated with X Window session.");
            success = 0;
            goto done;
        }

        if(!_clGlInteropInfo.glXDisplay) {
            _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (Linux, GLX): Failed to retrieve current display associated with current OpenGL context!");
        }

        properties[1] = (cl_context_properties)_clGlInteropInfo.glXContext;
        properties[2] = CL_GLX_DISPLAY_KHR;
        properties[3] = (cl_context_properties)_clGlInteropInfo.glXDisplay;

#       endif

        cl_device_id devices[CL_MAX_NUMBER_OF_DEVICES_PER_CONTEXT];
        size_t size = 0;

        if(GY_CL_CHECK_ERROR(clGetGLContextInfo(properties, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id)*CL_MAX_NUMBER_OF_DEVICES_PER_CONTEXT, devices, &size)) {
            _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] (KHR): clGetGLContextInfo KHR extension failed! OpenGL interop not available!");
            success = 0;
            goto done;
        }

        if(!size) {
            _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (KHR): Platform %u has no associated GL devices!", p);
            continue;
        }

        int count = size / sizeof(cl_device_id);
        if(count > 1) {
            _gyLog(GY_DEBUG_WARNING, "[GY_COMPUTE_API] (KHR): Returned multiple devices associated with CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR! WTF! Choosing first!");
        }

        _clGlInteropInfo.rendererPlatform = (cl_platform_id)_clPlatforms[p];
        _clGlInteropInfo.rendererDevice = devices[0];
        _clGlInteropInfo.glQueryDataValid = 1;
        break;
    }
    success = _clGlInteropInfo.glQueryDataValid;

#   endif

done:
    _clGlInteropInfo.glQueryDataValid = success;
    if(_clGlInteropInfo.glQueryDataValid) {
        GYComputeDevice dev = _gyComputeDeviceFromClDeviceId(_clGlInteropInfo.rendererDevice);
        GY_ASSERT(dev.platformIdx != -1 && dev.deviceIdx != -1);
        _gyLog(GY_DEBUG_VERBOSE, "OpenCL Device associated with current OpenGL Context/Screen: [Platform: %u, Device: %u]", dev.platformIdx, dev.deviceIdx);
    }
    _gyPop(1);
    return success;
}

#endif

#if 0
void initializeOpenCL(void) {

   dbgprintf("OpenCL Initialization");
   dbgpush();

   cl_int status = CL_SUCCESS;

   //Initialize platforms
#ifdef __APPLE__

#elif defined(__WIN32)


#else
   /*
   cl_context_properties props[] = {
       CL_GL_CONTEXT_KHR,
       (cl_context_properties)glXGetCurrentContext()
   };
   clGetGL*/
#endif


#if 0
   //Initialize devices
 for(unsigned i = 0; i < _platformCount; ++i) {
   status = clGetDeviceIDs(_platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &_deviceCount[i]);
   _checkClError(status, "Could not get device count!");
   _devices[i] = (cl_device_id*)malloc(_deviceCount[i]*sizeof(cl_device_id));
   status = clGetDeviceIDs(_platforms[i], CL_DEVICE_TYPE_ALL, _deviceCount[i], _devices[i], nullptr);
   _checkClError(status, "Could not populate device array!");

 }

#ifdef __APPLE__
  CGLContextObj kCGLContext = CGLGetCurrentContext(); // GL Context
  CGLShareGroupObj kCGLShareGroup =
  CGLGetShareGroup(kCGLContext); // Share Group
  cl_context_properties props[] =
  {
  CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
  (cl_context_properties) kCGLShareGroup, 0
      /*
      ,

      CL_CONTEXT_PLATFORM,
  (cl_context_properties) cpPlatform, 0*/
  };
#elif defined(__WIN32)
 cl_context_properties props[] =
 {
 CL_GL_CONTEXT_KHR,
 (cl_context_properties) wglGetCurrentContext(), // HGLRC handle
 CL_WGL_HDC_KHR,
 (cl_context_properties) wglGetCurrentDC(), // HDC handle
 CL_CONTEXT_PLATFORM,
 (cl_context_properties)_platforms[0], 0
 };
#else
  cl_context_properties props[] = {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)_platforms[i],
      CL_GLX_DISPLAY_KHR,
      (intptr_t)glXGetCurrentDisplay(),
      CL_GL_CONTEXT_KHR,
      (intptr_t)glXGetCurrentContext(),
      0
  };

#endif




 _clPrintPlatformInfo();
 //Create context
 _context = clCreateContext(props, _deviceCount[0], _devices[0], nullptr, nullptr, &status);
 _checkClError(status, "Could not create context!");

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
#if 0
   //Load program source from file
   GYFile* fp;
   size_t fileLen;
   char* programSource;
   dbgprintf("Compiling program: %s [%s]", ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_PATH, ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_FLAGS);
   dbgpush();
   if(!gyFileOpen(ELYSIAN_SORT_SWEEP_SWEEP_KERNEL_PATH, "r", &fp)) {
       Debug::printf(Debug::LOG_WARNING, "Could not open source file!");
   }
   gyFileLength(fp, &fileLen);
   programSource = (char*)malloc(sizeof(char)*(fileLen+1));
   gyFileRead(fp, (void*)programSource, fileLen, nullptr);
   programSource[fileLen] = '\0';^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^[OS^
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
#endif
/*
   cl_mem ubo_cl = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, glalLightingUBO, nullptr);

   cl_gl_object_type objType;
   GLuint objName;
   cl_int stats = clGetGLObjectInfo(ubo_cl, &objType, &objName);

   dbgprintf("Status: %d, GL ID - %d, CL ID - %d, CL TYPE - %d", stats, ubo_cl, objName, objType);
  // CL_GL_OBJECT_BUFFER

*/
   dbgpop();
#endif
}
#endif

//===================== GYRO COMPUTE SUBSYSTEM API ========================
int gyComputeInit(const GYComputeInitParams* params) {
    int success = 1;
    GY_ASSERT(params && !_initialized);

    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_COMPUTE] Initializing OpenCL");
    _gyPush();

    if(params) memcpy(&_initParams, params, sizeof(GYComputeInitParams));

    //Initialize OpenCL Platform(s)
    if(!GY_CL_CHECK_ERROR(clGetPlatformIDs(0, NULL, &_clPlatformCount))) {
        _gyLog(GY_DEBUG_VERBOSE, "Found %d OpenCL Platforms", _clPlatformCount);

        _clPlatforms = malloc(_clPlatformCount*sizeof(cl_platform_id));

        if(!GY_CL_CHECK_ERROR(clGetPlatformIDs(_clPlatformCount, _clPlatforms, NULL))) {

            _clDevices = malloc(_clPlatformCount*sizeof(cl_device_id*));
            _devices = malloc(_clPlatformCount*sizeof(_ComputeDevice*));
            _platformInfo = malloc(sizeof(_PlatformInfo) * _clPlatformCount);
            memset(_devices, 0, sizeof(_ComputeDevice*)*_clPlatformCount);
            memset(_clDevices, 0, sizeof(cl_device_id*)*_clPlatformCount);
            memset(_platformInfo, 0, sizeof(_PlatformInfo)*_clPlatformCount);

            _clDeviceCount = malloc(_clPlatformCount*sizeof(cl_uint));
            memset(_clDeviceCount, 0, _clPlatformCount * sizeof(cl_uint));

            _clContexts = malloc(sizeof(cl_context)*_clPlatformCount);
            memset(_clContexts, 0, sizeof(cl_context)*_clPlatformCount);

            //Initialize OpenCL Device(s)
            for(unsigned i = 0; i < _clPlatformCount; ++i) {
                _platformInfo[i].platIdx = i;
                if(!GY_CL_CHECK_ERROR(clGetDeviceIDs(_clPlatforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &_clDeviceCount[i]))) {
                    _gyLog(GY_DEBUG_VERBOSE, "Found %u OpenCL Devices for Platform [%u].", _clDeviceCount[i], i);
                    _clDevices[i] = malloc(_clDeviceCount[i]*sizeof(cl_device_id));
                    memset(_clDevices[i], 0, sizeof(cl_device_id)*_clDeviceCount[i]);

                    if(!GY_CL_CHECK_ERROR(clGetDeviceIDs(_clPlatforms[i], CL_DEVICE_TYPE_ALL, _clDeviceCount[i], _clDevices[i], NULL))) {
                        _devices[i] = malloc(_clDeviceCount[i]*sizeof(_ComputeDevice));
                        _platformInfo[i].gyInfo.deviceCount = _clDeviceCount[i];
                        memset(_devices[i], 0, sizeof(_ComputeDevice)*_clDeviceCount[i]);
                        for(unsigned j = 0; j < _clDeviceCount[i]; ++j) {
                            _devices[i][j].gyDevice.platformIdx = i;
                            _devices[i][j].gyDevice.deviceIdx   = j;
                            _devices[i][j].info.gyDevice = &_devices[i][j].gyDevice;
                            _devices[i][j].deviceId = _clDevices[i][j];
                        }

                    } else {
                        _gyLog(GY_DEBUG_ERROR, "Failed to retrieve OpenCL Device IDs for Platform [%u]", i);
                        free(_clDevices[i]);
                        _clDevices[i] = NULL;
                        _clDeviceCount[i] = 0;
                        success = 0;
                    }


                } else {
                    _gyLog(GY_DEBUG_ERROR, "Failed to retrieve OpenCL Device count for Platform [%u]", i);
                    _clDeviceCount[i] = 0;
                    success = 0;
                }

            }

        } else {
            _gyLog(GY_DEBUG_ERROR, "Failed to retrieve OpenCL Platforms IDs!");
            _clPlatformCount = 0;
            goto fail_platforms;
        }


    } else {
        _gyLog(GY_DEBUG_ERROR, "Failed to retrieve OpenCL Platform count!");
        _clPlatformCount = 0;
        goto fail;
    }

    gyComputeQueryInfo();

    goto done;


    fail_platforms:
        free(_clPlatforms);
        _clPlatforms = NULL;
        _clPlatformCount = 0;
    fail:
        success = 0;
    done:
        _gyPop(1);
        _initialized = success;
        return success;
}

int gyComputeUninit(void) {
    int success = 1;

    _gyLog(GY_DEBUG_VERBOSE, "[GY_COMPUTE_API] Uninitializing OpenCL");
    _gyPush();

    free(_clContexts);

    if(_devices) {
        for(unsigned p = 0; p < _clPlatformCount; ++p) {
            free(_devices[p]);
        }
        free(_devices);
        _devices = NULL;
    }
    if(_clDevices) {
        for(unsigned p = 0; p < _clPlatformCount; ++p) {
            free(_clDevices[p]);
        }
        free(_clDevices);
        _clDevices = NULL;
    }

    free(_clPlatforms);
    free(_clDeviceCount);

    _clContexts = NULL;
    _clPlatforms = NULL;
    _clDeviceCount= NULL;
    _clPlatformCount = 0;
    _initialized = 0;

#ifdef GY_COMPUTE_DEBUG
    if(_debugInfo.activeContextCount != 0) {
        _gyLog(GY_DEBUG_ERROR, "%u GYComputeContext objects were never destroyed!",         _debugInfo.activeContextCount);
        success = 0;
    }
    if(_debugInfo.activeCommandQueueCount != 0) {
        _gyLog(GY_DEBUG_ERROR, "%u GYComputeCommandQueue objects were never destroyed!",    _debugInfo.activeContextCount);
        success = 0;
    }
    if(_debugInfo.activeCommandQueueCount != 0) {
        _gyLog(GY_DEBUG_ERROR, "%u GYComputeProgram objects were never unloaded!",          _debugInfo.activeProgramCount);
    }
    if(_debugInfo.activeKernelCount != 0) {
        _gyLog(GY_DEBUG_ERROR, "%u GYComputeKernel objects were never destroyed!",          _debugInfo.activeKernelCount);
    }
    if(_debugInfo.activeBufferCount != 0) {
        _gyLog(GY_DEBUG_ERROR, "%u GYComputeBuffer objects were never destroyed!",          _debugInfo.activeBufferCount);
    }
#endif

    _gyPop(1);
    return success;
}
int gyComputeQueryInfo(void) {
    int success = 1;

    _gyLog(GY_DEBUG_VERBOSE, "[GY_COMPUTE_API] Querying OpenCL Info");
    _gyPush();

#ifdef GY_CL_OPENGL_INTEROP
    success &= _clGLQueryInteropInfo();
#endif

    for(unsigned i = 0; i < _clPlatformCount; ++i) {
        _gyLog(GY_DEBUG_VERBOSE, "Querying Platform[%u]", i);
        _gyPush();

        success &= _clQueryPlatformInfo(_clPlatforms[i], &_platformInfo[i]);

        for(unsigned j = 0; j < _clDeviceCount[i]; ++j) {
            _gyLog(GY_DEBUG_VERBOSE, "Querying Device[%u]", j);
            _gyPush();
            success &= _clQueryDeviceInfo(_clDevices[i][j], &_devices[i][j].info);

            _gyPop(1);
        }
        _gyPop(1);
    }
    _gyPop(1);

    return success;
}

//================= GYRO COMPUTE PLATFORM API =================

int gyComputePlatformCount(void) {
    return _clPlatformCount;
}

const GYComputePlatformInfo* gyComputePlatformQueryInfo(unsigned platformIdx) {
    GY_ASSERT(_initialized && platformIdx >= 0 && platformIdx < _clPlatformCount);
    return &_platformInfo->gyInfo;
}


//=================== GYRO COMPUTE DEVICE API ======================
GYComputeDevice gyComputeDeviceHandle(unsigned platformIdx, unsigned deviceIdx) {
    GYComputeDevice device = { platformIdx, deviceIdx };
    return device;
}


const GYComputeDeviceInfo* gyComputeDeviceQueryInfo(const GYComputeDevice* device) {
    GY_ASSERT(_initialized);
    _ComputeDevice* dev = _clDeviceFromGyComputeDevice(device);
    return &dev->info.gyInfo;
}


//================= GYRO COMPUTE CONTEXT API ======================
GYComputeContext* gyComputeContextCreate(unsigned platformIdx, const GYComputeContextInitParams* params) {
    GY_ASSERT(_initialized && params);
    GY_ASSERT(platformIdx >= 0 && platformIdx < _clPlatformCount);
    GY_ASSERT(params->deviceCount > 0 && params->deviceCount < CL_MAX_NUMBER_OF_DEVICES_PER_CONTEXT);

    cl_device_id devices[CL_MAX_NUMBER_OF_DEVICES_PER_CONTEXT];
    cl_context_properties properties[7] = {
        (cl_context_properties)CL_CONTEXT_PLATFORM,
        (cl_context_properties)_clPlatforms[platformIdx],
        (cl_context_properties)0,
        (cl_context_properties)0,
        (cl_context_properties)0,
        (cl_context_properties)0,
        (cl_context_properties)0
    };

#ifdef GY_CL_OPENGL_INTEROP
    if(params->flags & GY_COMPUTE_CONTEXT_FLAG_RENDERER_SHARED) {
        if(!_clGlInteropInfo.glQueryDataValid) {
            _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API]: Attempting to create an OpenGL/OpenCL shared context, when interop query failed!");
            return NULL;
        }
        int rendererPlatformIdx = _gyComputePlatformIndexFromClPlatformId(_clGlInteropInfo.rendererPlatform);
        GY_ASSERT(rendererPlatformIdx != -1);
        if((int)platformIdx != rendererPlatformIdx) {
            _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API]: Attempting to create an OpenGL/OpenCL shared context with platform [%u], when GL context is associated with platform [%d]!", platformIdx, rendererPlatformIdx);
            return NULL;
        }

#   ifdef __APPLE__
        properties[2] = (cl_context_properties)CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
        properties[3] = (cl_context_properties)_clGlInteropInfo.cglShareGroupObj;
#   else
        properties[2] = CL_GL_CONTEXT_KHR;

#       ifdef __WIN32

        properties[3] = (cl_context_properties)_clGlInteropInfo.wglContext;
        properties[4] = CL_WGL_HDC_KHR;
        properties[5] = (cl_context_properties)_clGlInteropInfo.wglDeviceContext;

#       else

        properties[1] = (cl_context_properties)_clGlInteropInfo.glXContext;
        properties[2] = CL_GLX_DISPLAY_KHR;
        properties[3] = (cl_context_properties)_clGlInteropInfo.glXDisplay;

#       endif
#   endif
    }
#endif

#ifdef GY_COMPUTE_DEBUG
#   ifdef GY_CL_OPENGL_INTEROP
    if(params->flags & GY_COMPUTE_CONTEXT_FLAG_RENDERER_SHARED) {
        for(unsigned i = 0; i < params->deviceCount; ++i) {
            _ComputeDevice* dev = (_ComputeDevice*)&params->devices[i];
            if(!dev->info.gyInfo.rendererSharedContext) {
                _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] Failed to create renderer-shared context: device [%u] does not support it!", i);
                return NULL;
            }
        }
    }
#   endif

    for(unsigned i = 0; i < params->deviceCount-1; ++i) {
        for(unsigned j = i+1; j < params->deviceCount; ++j) {
            if(params->devices[i].platformIdx != params->devices[j].platformIdx) {
                _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] Failed to create context: Compute devices [%u] and [%u] are in different platforms!", i, j);
                return NULL;
            }
        }
    }
#endif

    for(unsigned i = 0; i < params->deviceCount; ++i) {
        devices[i] = _clDevices[params->devices[i].platformIdx][params->devices[i].deviceIdx];
    }

    cl_int retVal;
    cl_context context = clCreateContext(properties, params->deviceCount, devices, 0, 0, &retVal);

    if(GY_CL_CHECK_ERROR(retVal)) {
        _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] Failed to create OpenCL context!");
        return NULL;
    }

    _Context* ctx = malloc(sizeof(_Context));
    ctx->ctx = context;

#ifdef GY_COMPUTE_DEBUG
    ++_debugInfo.activeContextCount;
#endif

    return (GYComputeContext*)ctx;
}

int gyComputeContextDestroy(GYComputeContext* context) {
    int success = 1;
    GY_ASSERT(_initialized);
    if(context) {
        _Context* clCtx = (_Context*)context;
        if(GY_CL_CHECK_ERROR(clReleaseContext(clCtx->ctx))) {
            _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] Failed to destroy OpenCL context!");
            success = 0;
        }
        free(clCtx);
#ifdef GY_COMPUTE_DEBUG
        --_debugInfo.activeContextCount;
#endif
        return 1;
    } else success = 0;
    return success;
}

//================= GYRO COMPUTE COMMAND QUEUE API ======================
GYComputeCommandQueue* gyComputeCommandQueueCreate(const GYComputeContext* context, const GYComputeDevice* device, int flags) {
    GY_ASSERT(_initialized && context && device);
    cl_int retCode;

    cl_command_queue_properties properties = 0;
    if(flags & GY_COMPUTE_COMMAND_QUEUE_FLAG_ENABLE_PROFILING)          properties |= CL_QUEUE_PROFILING_ENABLE;
    if(flags & GY_COMPUTE_COMMAND_QUEUE_FLAG_OUT_OF_ORDER_EXECUTION)    properties |= (intptr_t)CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;

    cl_command_queue clQueue = clCreateCommandQueue(((_Context*)context)->ctx, _clDeviceFromGyComputeDevice(device)->deviceId, properties, &retCode);

    if(GY_CL_CHECK_ERROR(retCode)) {
        _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] Failed to create OpenCL command queue!");
        return NULL;
    }

    _CommandQueue* queue = malloc(sizeof(_CommandQueue));
    queue->queue = clQueue;

#ifdef GY_COMPUTE_DEBUG
    ++_debugInfo.activeCommandQueueCount;
#endif

    return &queue->gyQueue;
}

int gyComputeCommandQueueDestroy(GYComputeCommandQueue* queue) {
   GY_ASSERT(_initialized);

    int success = 1;
    if(queue) {
        _CommandQueue* cmd = (_CommandQueue*)queue;
        if(GY_CL_CHECK_ERROR(clReleaseCommandQueue(cmd->queue))) {
            _gyLog(GY_DEBUG_ERROR, "[GY_COMPUTE_API] Failed to create OpenCL command queue!");
            success = 0;
        }
        free(cmd);

#ifdef GY_COMPUTE_DEBUG
        --_debugInfo.activeCommandQueueCount;
#endif
    } else success = 0;
    return success;
}

int gyComputeCommandQueueEnqueueKernel(GYComputeCommandQueue* queue, GYComputeKernel* kernel, unsigned workDimensions, const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize, unsigned eventCount, void* eventList, void* event) {
    _Kernel* kern = ((_Kernel*)kernel);
    _CommandQueue* cmd = ((_CommandQueue*)queue);

    GY_ASSERT(queue && kernel && workDimensions && globalWorkOffset && globalWorkSize && localWorkSize);

    if(!kern->kernel) return 0;

    if(GY_CL_CHECK_ERROR(clEnqueueNDRangeKernel(cmd->queue, kern->kernel, workDimensions, globalWorkOffset, globalWorkSize, localWorkSize, eventCount, eventList, event))) return 0;
    return 1;
}

int gyComputeCommandQueueEnqueueBufferWrite(GYComputeCommandQueue *queue, const GYComputeBuffer *buffer, int blocking, size_t offset, size_t cb, const void *data, unsigned eventCount, void *eventList, void *event) {
    GY_ASSERT(queue && buffer && data && cb);

    return !GY_CL_CHECK_ERROR(clEnqueueWriteBuffer(((_CommandQueue*)queue)->queue, ((_Buffer*)buffer)->clBuffer, blocking, offset, cb, data, eventCount, eventList, event));
}

int gyComputeCommandQueueEnqueueBufferRead(GYComputeCommandQueue *queue, GYComputeBuffer *buffer, int blocking, size_t offset, size_t cb, void *ptr, unsigned eventCount, void *eventList, void *event) {
    GY_ASSERT(queue && buffer && ptr && cb);

    return !GY_CL_CHECK_ERROR(clEnqueueReadBuffer(((_CommandQueue*)queue)->queue, ((_Buffer*)buffer)->clBuffer, blocking, offset, cb, ptr, eventCount, eventList, event));
}

int gyComputeCommandQueueEnqueueAcquireRendererObject(GYComputeCommandQueue *queue, unsigned count, const GYComputeBuffer** buffers) {
    GY_ASSERT(queue && buffers && count > 0 && count < GY_CL_ENQUEUE_ACQUIRE_RENDERER_OBJECTS_MAX);
    cl_mem clBuffers[GY_CL_ENQUEUE_ACQUIRE_RENDERER_OBJECTS_MAX];
    for(unsigned i = 0; i < count; ++i) clBuffers[i] = ((_Buffer*)buffers[i])->clBuffer;

    return !GY_CL_CHECK_ERROR(clEnqueueAcquireGLObjects(((_CommandQueue*)queue)->queue, count, clBuffers, 0, NULL, NULL));
}

int gyComputeCommandQueueEnqueueReleaseRendererObject(GYComputeCommandQueue *queue, unsigned count, const GYComputeBuffer** buffers) {
    GY_ASSERT(queue && buffers && count > 0 && count < GY_CL_ENQUEUE_ACQUIRE_RENDERER_OBJECTS_MAX);
    cl_mem clBuffers[GY_CL_ENQUEUE_ACQUIRE_RENDERER_OBJECTS_MAX];
    for(unsigned i = 0; i < count; ++i) clBuffers[i] = ((_Buffer*)buffers[i])->clBuffer;

    return !GY_CL_CHECK_ERROR(clEnqueueReleaseGLObjects(((_CommandQueue*)queue)->queue, count, clBuffers, 0, NULL, NULL));
}

int gyComputeCommandQueueFinish(GYComputeCommandQueue* queue) {
    GY_ASSERT(queue);
    return !GY_CL_CHECK_ERROR(clFinish(((_CommandQueue*)queue)->queue));
}

//================== GYRO COMPUTE BUFFER API ===================

GYComputeBuffer* gyComputeBufferCreate(GYComputeContext* context, GY_COMPUTE_MEMORY_TYPE type, size_t size, void* data) {
    GY_ASSERT(context && size);

    cl_int success;
    cl_mem clBuff = clCreateBuffer(((_Context*)context)->ctx, _clMemTypeFromGyro(type), size, data, &success);

    if(!GY_CL_CHECK_ERROR(success)) {
        _Buffer* buff = malloc(sizeof(_Buffer));
        memset(buff, 0, sizeof(_Buffer));
        buff->clBuffer = clBuff;
#ifdef GY_COMPUTE_DEBUG
        ++_debugInfo.activeBufferCount;
#endif
        return &buff->gyBuffer;
    } else {
        _gyLog(GY_DEBUG_ERROR, "Could not create OpenCL buffer object!");
        return NULL;
    }
}

GYComputeBuffer* gyComputeBufferCreateFromRendererBuffer(GYComputeContext* context, GY_COMPUTE_MEMORY_TYPE type, intptr_t renderBuff) {
    cl_int retVal;
    cl_mem clBuff = clCreateFromGLBuffer(((_Context*)context)->ctx, _clMemTypeFromGyro(type), (cl_GLuint)renderBuff, &retVal);

    if(GY_CL_CHECK_ERROR(retVal)) {
        _gyLog(GY_DEBUG_ERROR, "Failed to create OpenCL buffer from OpenGL buffer!");
        return NULL;
    }

    _Buffer* buffer = malloc(sizeof(_Buffer));
    memset(buffer, 0, sizeof(_Buffer));
    buffer->clBuffer = clBuff;
    buffer->glBuffer = (cl_GLuint)renderBuff;
#ifdef GY_COMPUTE_DEBUG
    ++_debugInfo.activeBufferCount;
#endif
    return &buffer->gyBuffer;
}

GYComputeBuffer* gyComputeBufferCreateFromRendererTexture(GYComputeContext* context, GY_COMPUTE_MEMORY_TYPE type, intptr_t renderTex) {
    cl_int retVal;
    cl_mem clBuff = clCreateFromGLTexture2D(((_Context*)context)->ctx, _clMemTypeFromGyro(type), GL_TEXTURE_2D, 0, (GLuint)renderTex, &retVal);

    if(GY_CL_CHECK_ERROR(retVal)) {
        _gyLog(GY_DEBUG_ERROR, "Failed to create OpenCL buffer from OpenGL texture!");
        return NULL;
    }

    _Buffer* buffer = malloc(sizeof(_Buffer));
    memset(buffer, 0, sizeof(_Buffer));
    buffer->clBuffer = clBuff;
    buffer->glBuffer = (GLuint)renderTex;
#ifdef GY_COMPUTE_DEBUG
    ++_debugInfo.activeBufferCount;
#endif
    return &buffer->gyBuffer;

}

int gyComputeBufferDestroy(GYComputeBuffer* buffer) {
    if(buffer) {
        _Buffer* buff = (_Buffer*)buffer;
        GY_CL_CHECK_ERROR(clReleaseMemObject(buff->clBuffer));
#ifdef GY_COMPUTE_DEBUG
        --_debugInfo.activeBufferCount;
#endif
        return 1;
    } else {

        return 0;
    }
}

//================== GYRO COMPUTE PROGRAM API ===================

GYComputeProgram* gyComputeProgramLoad(const char *filePath, const GYComputeContext *context) {
    GY_ASSERT(_initialized && filePath && context);
    cl_int status;

    //Load program source from file
    GYFile* fp;
    size_t fileLen;
    char* programSource;

    _Program* program = malloc(sizeof(_Program));
    memset(program, 0, sizeof(_Program));
    GY_ASSERT(strlen(filePath) < GYRO_PATH_MAX_SIZE);
    strncpy(program->gyProgram.path, filePath, GYRO_PATH_MAX_SIZE);

    _gyLog(GY_DEBUG_VERBOSE, "[GY_COMPUTE_API] Loading OpenCL program [%s]", filePath);

    _gyPush();
    if(!gyFileOpen(filePath, "r", &fp)) {
        _gyLog(GY_DEBUG_ERROR, "Failed to open file!");
        program->gyProgram.error = 1;
        goto done;
    }
    gyFileLength(fp, &fileLen);
    GY_ASSERT(fileLen);
    programSource = malloc(sizeof(char)*fileLen+1);
    gyFileRead(fp, (void*)programSource, fileLen, NULL);
    programSource[fileLen] = '\0';

    //Create and compile program
    _Context* ctx = ((_Context*)context);
    program->program = clCreateProgramWithSource(ctx->ctx, 1, (const char**)&programSource, NULL, &status);
    if(GY_CL_CHECK_ERROR(status)) {
        _gyLog(GY_DEBUG_ERROR, "Failed to create OpenCL program object from source.");
        program->gyProgram.error = 1;
    }

    free(programSource);
done:
    _gyPop(1);
#ifdef GY_COMPUTE_DEBUG
    ++_debugInfo.activeProgramCount;
#endif
    return &program->gyProgram;
}

int gyComputeProgramUnload(GYComputeProgram* program) {
    GY_ASSERT(program);
    if(program) {
#ifdef GY_COMPUTE_DEBUG
        --_debugInfo.activeProgramCount;
#endif
        if(!GY_CL_CHECK_ERROR(clReleaseProgram(((_Program*)program)->program))) {
            return 1;
        }
    }
    return 0;
}

int gyComputeProgramCompile(GYComputeProgram *program, const GYComputeDevice *devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options) {
    (void)options;

    GY_ASSERT(program && devices && deviceCount);
    cl_device_id devIds[CL_MAX_NUMBER_OF_DEVICES_PER_CONTEXT];
    int success = !program->error;
    _Program* pgm = (_Program*)program;
    cl_build_status buildStatus;

    _gyLog(GY_DEBUG_VERBOSE, "[GY_COMPUTE_API] Compiling OpenCL program [%s] for [%u] devices", program->path, deviceCount);
    _gyPush();

    if(program->error) {
        _gyLog(GY_DEBUG_ERROR, "Aborting due to invalid CL program object.");
        goto end;
    }

    for(unsigned d = 0; d < deviceCount; ++d) devIds[d] = _clDeviceFromGyComputeDevice(&devices[d])->deviceId;

    if(GY_CL_CHECK_ERROR(clBuildProgram(pgm->program, deviceCount, devIds, GY_CL_BUILD_FLAGS, NULL, NULL))) {
        success = 0;
        for(unsigned d = 0; d < deviceCount; ++d) {
            GY_CL_CHECK_ERROR(clGetProgramBuildInfo(pgm->program, devIds[d], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &buildStatus, NULL));

            _gyLog((buildStatus != CL_BUILD_SUCCESS)? GY_DEBUG_ERROR : GY_DEBUG_VERBOSE, "Device[%u] Build Status: [%s]", d, _clProgramBuildStatusStr(buildStatus));
            if(buildStatus == CL_BUILD_SUCCESS) continue;
            else success = 0;
            _gyPush();

            size_t logSize;
            if(GY_CL_CHECK_ERROR(clGetProgramBuildInfo(pgm->program, devIds[d], CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize))) {
                _gyLog(GY_DEBUG_ERROR, "Failed to query build log size for Device[%u]", d);
                _gyPop(1);
                continue;
            }

            char*const buildLog = (char*)malloc(sizeof(char)*(logSize+1));
            if(GY_CL_CHECK_ERROR(clGetProgramBuildInfo(pgm->program, devIds[d], CL_PROGRAM_BUILD_LOG, logSize, buildLog, NULL))) {
                _gyLog(GY_DEBUG_ERROR, "Failed to retrieve build log for Device[%u]", d);
            } else {
                buildLog[logSize] = '\0';
                _gyLog(GY_DEBUG_VERBOSE, "%s", buildLog);
            }
            free(buildLog);
            _gyPop(1);
        }

    }

end:
    _gyPop(1);
    return success;
}

//================== GYRO COMPUTE KERNEL API ===================
GYComputeKernel* gyComputeKernelCreate(const GYComputeProgram* program, const char* kernelName, void* softwareFallbackFuncPtr) {
    GY_ASSERT(program && kernelName);

    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_COMPUTE_API] Creating OpenCL Kernel: [%s]", kernelName);
    _gyPush();

    cl_int retCode;
    cl_kernel kern = clCreateKernel(((_Program*)program)->program, kernelName, &retCode);

    if(GY_CL_CHECK_ERROR(retCode)) {
        _gyLog(GY_DEBUG_ERROR, "Failed to create OpenCL Kernel [%s]", kernelName);
        _gyPop(1);
        return NULL;
    }

    _Kernel* kernel = malloc(sizeof(_Kernel));
    memset(kernel, 0, sizeof(_Kernel));
    kernel->kernel = kern;

#ifdef GY_COMPUTE_DEBUG
    ++_debugInfo.activeKernelCount;
#endif

    _clQueryKernelInfo(kern, &kernel->gyKernelInfo);

    _gyPop(1);
    return &kernel->gyKernel;
}

int gyComputeKernelDestroy(GYComputeKernel *kernel) {
    int success = 1;

    if(kernel) {
        success &= GY_CL_CHECK_ERROR(clReleaseKernel(((_Kernel*)kernel)->kernel));
        free(kernel);
        free(((_Kernel*)kernel)->gyKernelInfo.argInfo);
#ifdef GY_COMPUTE_DEBUG
    --_debugInfo.activeKernelCount;
#endif
    } else success = 0;

    return success;
}

const GYComputeKernelInfo* gyComputeKernelQueryInfo(const GYComputeKernel* kernel) {
    GY_ASSERT(kernel);
    return &((_Kernel*)kernel)->gyKernelInfo;
}

int gyComputeKernelQueryWorkgroupInfo(const GYComputeKernel* kernel, const GYComputeDevice* device, GYComputeKernelWorkgroupInfo* info) {
    GY_ASSERT(kernel && device && info);
    return _clQueryKernelWorkgroupInfo(((_Kernel*)kernel)->kernel, _clDeviceFromGyComputeDevice(device)->deviceId, info);
}

int gyComputeKernelArg(GYComputeKernel* kernel, unsigned index, size_t size, void* arg) {
    GY_ASSERT(kernel && size && arg);
    _Kernel* kern = ((_Kernel*)kernel);
    GY_ASSERT(index < kern->gyKernelInfo.numArgs);
    return !GY_CL_CHECK_ERROR(clSetKernelArg(kern->kernel, index, size, arg));
}

int gyComputeKernelArgBuffer(GYComputeKernel *kernel, unsigned index, GYComputeBuffer *buffer) {
    return gyComputeKernelArg(kernel, index, sizeof(cl_mem), &((_Buffer*)buffer)->clBuffer);
}

//================== GYRO COMPUTE DEBUG API ===================


//================== GYRO COMPUTE DEBUG API ===================
const GYComputeDebugInfo* gyComputeDebugQueryInfo(void) {
    return &_debugInfo;
}
