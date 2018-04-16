#include "elysian_sort_sweep.hpp"

namespace elysian {

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

}

/*
template<>
unsigned elysian::SortSweepAABB<elysian::Collidable>::curIndex = 0;
*/
