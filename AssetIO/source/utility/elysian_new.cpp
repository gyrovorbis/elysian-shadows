#ifdef __WIN32

#include <stdlib.h>
//#include <malloc.h>
//#include <windows.h>

#define ELYSIAN_HEAP_ALIGN_SIZE     16

void* operator new[] (size_t size) {
#ifdef __WIN32


    void *ptr;
    void *p = malloc(size + ELYSIAN_HEAP_ALIGN_SIZE - 1 + sizeof(void*));

       /* Address of the aligned memory according to the align parameter*/
       ptr = (void*) (((unsigned int)p + sizeof(void*) + ELYSIAN_HEAP_ALIGN_SIZE -1) & ~(ELYSIAN_HEAP_ALIGN_SIZE-1));
       /* store the address of the malloc() above
        * at the beginning of our total memory area.
        * You can also use *((void **)ptr-1) = p
        * instead of the one below.
        */
       *((void**)((unsigned int)ptr - sizeof(void*))) = p;
       /* Return the address of aligned memory */
       return ptr;
#else
    void *p;
    posix_memalign(&p, size, 16);
#endif
    return p;
}

void operator delete[] (void* p) {
#ifdef __WIN32
    void *ptr = *((void**)((unsigned int)p - sizeof(void*)));
      free(ptr);
#else
    free(p);
#endif

}

void* operator new (size_t size)
{
#ifdef __WIN32


    void *ptr;
    void *p = malloc(size + ELYSIAN_HEAP_ALIGN_SIZE - 1 + sizeof(void*));

       /* Address of the aligned memory according to the align parameter*/
       ptr = (void*) (((unsigned int)p + sizeof(void*) + ELYSIAN_HEAP_ALIGN_SIZE -1) & ~(ELYSIAN_HEAP_ALIGN_SIZE-1));
       /* store the address of the malloc() above
        * at the beginning of our total memory area.
        * You can also use *((void **)ptr-1) = p
        * instead of the one below.
        */
       *((void**)((unsigned int)ptr - sizeof(void*))) = p;
       /* Return the address of aligned memory */
       return ptr;

   //void *p = __mingw_aligned_malloc(size, 16);
#else
    void *p;
    posix_memalign(&p, size, 16);
#endif
    return p;
}

void operator delete (void *p) {
#ifdef __WIN32
    void *ptr = *((void**)((unsigned int)p - sizeof(void*)));
      free(ptr);
#else
    free(p);
#endif
}


#endif
