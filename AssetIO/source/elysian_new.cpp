#ifdef __WIN32

#include <stdlib.h>
//#include <malloc.h>
//#include <windows.h>

void* operator new (size_t size)
{
#ifdef __WIN32
   void *p = __mingw_aligned_malloc(size, 16);
#else
    void *p;
    posix_memalign(&p, size, 16);
#endif
    return p;
}

void operator delete (void *p) {
#ifdef __WIN32
    __mingw_aligned_free(p);
#else
    free(p);
#endif
}


#endif
