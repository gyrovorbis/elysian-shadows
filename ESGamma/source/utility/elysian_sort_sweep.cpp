#include "elysian_sort_sweep.hpp"

#ifdef ELYSIAN_OPENCL_ACCELERATED
#	ifdef __APPLE__
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLDevice.h>
#include <OpenGL/CGLCurrent.h>
#		include <OpenCL/opencl.h>
#	else
    #ifdef __WIN32
        #include <windows.h>
    #endif

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <glal/gyro_gl.h>
//#       include <GL/glx.h>
#       include <GL/glext.h>
#		include <CL/cl.h>
#       include <CL/cl_gl.h>
#       include <CL/cl_gl_ext.h>
#	endif
#endif
 extern GLuint                       glalLightingUBO;
namespace elysian {

cl_platform_id*     _platforms				= nullptr;
cl_uint             _platformCount			= 0;
cl_device_id**      _devices				= nullptr;
cl_uint*            _deviceCount			= nullptr;
  cl_uint             _curPlatformIndex       = 0;
  cl_uint             _curDeviceIndex         = 0;
  cl_context          _context;
  cl_command_queue    _cmdQueue;
  cl_program          _program;
  cl_kernel           _kernel;
  cl_mem              _aabbListBuff;
  cl_mem              _aabbListSizeBuff;
  cl_mem              _aabbContactListBuff;
  cl_mem              _aabbContactListSizeBuff;


  void initializeOpenCL(void) {
 #ifdef ELYSIAN_OPENCL_ACCELERATED
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
   }
   cl_context_properties props[] =
   {
   CL_GL_CONTEXT_KHR,
   (cl_context_properties) wglGetCurrentContext(), // HGLRC handle
   CL_WGL_HDC_KHR,
   (cl_context_properties) wglGetCurrentDC(), // HDC handle
   CL_CONTEXT_PLATFORM,
   (cl_context_properties)_platforms[0], 0
   };

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
 #else


 #endif

 }

}

/*
template<>
unsigned elysian::SortSweepAABB<elysian::Collidable>::curIndex = 0;
*/
