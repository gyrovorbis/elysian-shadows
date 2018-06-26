#ifndef GYRO_COMPUTE_API_H
#define GYRO_COMPUTE_API_H

#include <stdlib.h>
#include <gyro_defines.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GY_COMPUTE_DEBUG
#define GY_COMPUTE_DEVICE_INFO_EXTENSIONS_SIZE          2000
#define GY_COMPUTE_PLATFORM_INFO_EXTENSIONS_SIZE        500
#define GY_COMPUTE_KERNEL_NAME_SIZE                     100
#define GY_COMPUTE_KERNEL_ARG_NAME_SIZE                 50
#define GY_COMPUTE_KERNEL_ARG_TYPE_NAME_SIZE            100
#define GY_COMPUTE_DEVICE_NAME_SIZE                     100
#define GY_COMPUTE_PLATFORM_NAME_SIZE                   100

struct GYComputeDevice;

typedef enum GY_COMPUTE_DEVICE_TYPE {
    GY_COMPUTE_DEVICE_CPU   = 0x1,
    GY_COMPUTE_DEVICE_GPU   = 0x2,
    GY_COMPUTE_DEVICE_DSP   = 0x4,
    GY_COMPUTE_DEVICE_FPGA  = 0x8,
    GY_COMPUTE_DEVICE_OTHER = 0x10,
    GY_COMPUTE_DEVICE_ANY   = 0xff
} GY_COMPUTE_DEVICE_TYPE;

typedef struct GYComputeInitParams {
    int rendererInterop;
} GYComputeInitParams;

typedef struct GYComputeDeviceInitParams {
    uint32_t deviceTypes;
    uint32_t deviceFlags;
} GYComputeDeviceInitParams;

typedef enum GYComputeContextInitFlags {
    GY_COMPUTE_CONTEXT_FLAG_RENDERER_SHARED = 0x1
} GYComputeContextInitFlags;

typedef struct GYComputeContextInitParams {
    int                         flags;
    struct GYComputeDevice*     devices;
    unsigned                    deviceCount;
} GYComputeContextInitParams;

typedef enum GYComputeCommandQueueInitFlags {
    GY_COMPUTE_COMMAND_QUEUE_FLAG_OUT_OF_ORDER_EXECUTION = 0x1,
    GY_COMPUTE_COMMAND_QUEUE_FLAG_ENABLE_PROFILING       = 0x2
} GYComputeCommandQueueInitFlags;

typedef struct GYComputePlatformInfo {
    char        platformName[GY_COMPUTE_PLATFORM_NAME_SIZE];
    char        extensions[GY_COMPUTE_PLATFORM_INFO_EXTENSIONS_SIZE];
    int         rendererContextSharing;
    int         isRenderPlatform;
    unsigned    deviceCount;
} GYComputePlatformInfo;

typedef struct GYComputeDeviceInfo {
    char                    extensions[GY_COMPUTE_DEVICE_INFO_EXTENSIONS_SIZE];
    char                    deviceName[GY_COMPUTE_DEVICE_NAME_SIZE];
    unsigned long           maxMemAllocSize;
    unsigned long           globalMemSize;
    unsigned long           maxConstantMemSize;
    unsigned long           localMemSize;
    size_t                  image2DMaxSize[2];
    size_t                  image3DMaxSize[3];
    size_t                  maxWorkGroupSize;
    size_t                  maxParameterSize;
    size_t                  profilingTimerResolution;
    unsigned                maxComputeUnits;
    unsigned                maxWorkItemDimensions;
    unsigned                maxWorkItemSizes[3];
    unsigned                preferredVectorWidthFloat;
    unsigned                preferredVectorWidthInt;
    unsigned                maxSamplers;
    unsigned                minDataTypeAlignSize;
    unsigned                globalMemCacheLineSize;
    unsigned                baseAddrAlign;
    unsigned                maxConstantArgs;
    int                     rendererSharedContext;
    int                     globalInt32BaseAtomics;
    int                     localInt32BaseAtomics;
    int                     isRenderDevice;
    int                     imageSupport;
    int                     depthTextureSupport;
    GY_COMPUTE_DEVICE_TYPE  devType;
} GYComputeDeviceInfo;

typedef struct GYComputeDevice {
    int platformIdx;
    int deviceIdx;
} GYComputeDevice;
typedef struct GYComputeContext {

} GYComputeContext;

typedef struct GYComputeCommandQueue {

} GYComputeCommandQueue;

typedef enum GY_COMPUTE_MEMORY_TYPE {
    GY_COMPUTE_MEMORY_TYPE_READ_WRITE,
    GY_COMPUTE_MEMORY_TYPE_WRITE_ONLY,
    GY_COMPUTE_MEMORY_TYPE_READ_ONLY,
    GY_COMPUTE_MEMORY_TYPE_USE_HOST_POINTER,
    GY_COMPUTE_MEMORY_TYPE_ALLOC_HOST_POINTER,
    GY_COMPUTE_MEMORY_TYPE_COPY_HOST_POINTER
} GY_COMPUTE_MEMORY_TYPE;

typedef struct GYComputeBuffer {

} GYComputeBuffer;

typedef void (*GYComputeKernelNative)(void*);

typedef struct GYComputeProgram {
    char    path[GYRO_PATH_MAX_SIZE];
    int     error;
} GYComputeProgram;

typedef struct GYComputeProgramBuildOptions {

} GYComputeProgramBuildOptions;

typedef struct GYComputeKernelWorkgroupInfo {
    size_t      workGroupSize;
    size_t      compileWorkGroupSize[3];
    uint64_t    localMemSize;
    size_t      preferredWorkGroupSizeMultiple;
    uint64_t    privateMemSize;
} GYComputeKernelWorkgroupInfo;

typedef enum GY_COMPUTE_KERNEL_ARG_ADDRESS_QUALIFIER {
    GY_COMPUTE_KERNEL_ARG_ADDRESS_GLOBAL,
    GY_COMPUTE_KERNEL_ARG_ADDRESS_LOCAL,
    GY_COMPUTE_KERNEL_ARG_ADDRESS_CONSTANT,
    GY_COMPUTE_KERNEL_ARG_ADDRESS_PRIVATE,
    GY_COMPUTE_KERNEL_ARG_ADDRESS_UNKNOWN
} GY_COMPUTE_KERNEL_ARG_ADDRESS_QUALIFIER;

typedef enum GY_COMPUTE_KERNEL_ARG_ACCESS_QUALIFIER {
    GY_COMPUTE_KERNEL_ARG_ACCESS_READ_ONLY,
    GY_COMPUTE_KERNEL_ARG_ACCESS_WRITE_ONLY,
    GY_COMPUTE_KERNEL_ARG_ACCESS_READ_WRITE,
    GY_COMPUTE_KERNEL_ARG_ACCESS_NONE,
    GY_COMPUTE_KERNEL_ARG_ACCESS_UNKNOWN
} GY_COMPUTE_KERNEL_ARG_ACCESS_QUALIFIER;

typedef enum GY_COMPUTE_KERNEL_ARG_TYPE_QUALIFIER {
    GY_COMPUTE_KERNEL_ARG_TYPE_NONE             = 0x0,
    GY_COMPUTE_KERNEL_ARG_TYPE_CONST            = 0x1,
    GY_COMPUTE_KERNEL_ARG_TYPE_RESTRICT         = 0x2,
    GY_COMPUTE_KERNEL_ARG_TYPE_VOLATILE         = 0x4
} GY_COMPUTE_KERNEL_ARG_TYPE_QUALIFIER;

typedef struct GYComputeKernelArgInfo {
    char                                    typeName[GY_COMPUTE_KERNEL_ARG_NAME_SIZE];
    char                                    name[GY_COMPUTE_KERNEL_ARG_NAME_SIZE];
    GY_COMPUTE_KERNEL_ARG_ADDRESS_QUALIFIER addressQualifier;
    GY_COMPUTE_KERNEL_ARG_ACCESS_QUALIFIER  accessQualifier;
    int                                     typeQualifier;
} GYComputeKernelArgInfo;

typedef struct GYComputeKernelInfo {
    uint32_t                numArgs;
    GYComputeKernelArgInfo* argInfo;
} GYComputeKernelInfo;

typedef struct GYComputeKernel {
    char                    name[GY_COMPUTE_KERNEL_NAME_SIZE];
    GYComputeProgram*       program;
    GYComputeKernelNative   nativeFunc;
} GYComputeKernel;

#ifdef GY_COMPUTE_DEBUG

typedef struct GYComputeDebugInfo {
    int activeContextCount;
    int activeCommandQueueCount;
    int activeProgramCount;
    int activeKernelCount;
    int activeBufferCount;
} GYComputeDebugInfo;

#endif

typedef struct GYComputeEvent {

} GYComputeEvent;


int                             gyComputeInit(const GYComputeInitParams* params);
int                             gyComputeUninit(void);
int                             gyComputeQueryInfo(void);
int                             gyComputeFinish(void);

int                             gyComputePlatformCount(void);
const GYComputePlatformInfo*    gyComputePlatformQueryInfo(unsigned platformIdx);


GYComputeDevice                 gyComputeDeviceHandle(unsigned platformIdx, unsigned deviceIdx);
const GYComputeDeviceInfo*      gyComputeDeviceQueryInfo(const GYComputeDevice* device);


GYComputeContext*               gyComputeContextCreate(unsigned platformIdx, const GYComputeContextInitParams* params);
int                             gyComputeContextDestroy(GYComputeContext* context);

GYComputeProgram*               gyComputeProgramLoad(const char* filePath, const GYComputeContext* context);
int                             gyComputeProgramUnload(GYComputeProgram* program);
int                             gyComputeProgramCompile(GYComputeProgram* program, const GYComputeDevice* devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options);

GYComputeBuffer*                gyComputeBufferCreate(GYComputeContext* context, GY_COMPUTE_MEMORY_TYPE type, size_t size, void* data);
GYComputeBuffer*                gyComputeBufferCreateFromRendererBuffer(GYComputeContext* context, GY_COMPUTE_MEMORY_TYPE type, intptr_t renderBuff);
GYComputeBuffer*                gyComputeBufferCreateFromRendererTexture(GYComputeContext* context, GY_COMPUTE_MEMORY_TYPE type, intptr_t renderTex);
int                             gyComputeBufferDestroy(GYComputeBuffer* buffer);

GYComputeKernel*                gyComputeKernelCreate(const GYComputeProgram* program, const char* kernelName, void* softwareFallbackFuncPtr);
int                             gyComputeKernelDestroy(GYComputeKernel* kernel);
const GYComputeKernelInfo*      gyComputeKernelQueryInfo(const GYComputeKernel* kernel);
int                             gyComputeKernelQueryWorkgroupInfo(const GYComputeKernel* kernel, const GYComputeDevice* device, GYComputeKernelWorkgroupInfo* info);
int                             gyComputeKernelArg(GYComputeKernel* kernel, unsigned index, size_t size, void* arg);
int                             gyComputeKernelArgBuffer(GYComputeKernel* kernel, unsigned index, GYComputeBuffer* buffer);

GYComputeCommandQueue*          gyComputeCommandQueueCreate(const GYComputeContext* context, const GYComputeDevice* device, int flags);
int                             gyComputeCommandQueueDestroy(GYComputeCommandQueue* queue);
int                             gyComputeCommandQueueFinish(GYComputeCommandQueue* queue);
int                             gyComputeCommandQueueEnqueueKernel(GYComputeCommandQueue* queue, GYComputeKernel* kernel, unsigned workDimensions, const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize, unsigned eventCount, void* eventList, void* event);
int                             gyComputeCommandQueueEnqueueBufferWrite(GYComputeCommandQueue* queue, const GYComputeBuffer* buffer, int blocking, size_t offset, size_t cb, const void* data, unsigned eventCount, void* eventList, void* event);
int                             gyComputeCommandQueueEnqueueBufferRead(GYComputeCommandQueue* queue, GYComputeBuffer* buffer, int blocking, size_t offset, size_t cb, void* ptr, unsigned eventCount, void* eventList, void* event);
int                             gyComputeCommandQueueEnqueueAcquireRendererObject(GYComputeCommandQueue* queue, unsigned count, const GYComputeBuffer** buffers);
int                             gyComputeCommandQueueEnqueueReleaseRendererObject(GYComputeCommandQueue* queue, unsigned count, const GYComputeBuffer** buffers);

#ifdef GY_COMPUTE_DEBUG
const GYComputeDebugInfo*       gyComputeDebugQueryInfo(void);
#endif





#ifdef __cplusplus
}
#endif

#endif // GYRO_COMPUTE_H
