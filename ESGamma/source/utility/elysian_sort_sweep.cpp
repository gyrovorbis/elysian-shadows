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


}

/*
template<>
unsigned elysian::SortSweepAABB<elysian::Collidable>::curIndex = 0;
*/
