#include <stdlib.h>

#ifdef _MSC_VER
#  define NEWDEL_CALL __cdecl
#else
#  define NEWDEL_CALL
#endif

extern "C" void __cxa_pure_virtual ()
{
  abort();
}

void *NEWDEL_CALL operator new (size_t size)
{
  return malloc(size);
}

void *NEWDEL_CALL operator new [] (size_t size)
{
  return malloc(size);
}

void NEWDEL_CALL operator delete (void *p)
{
  free(p);
}

void NEWDEL_CALL operator delete [] (void *p)
{
  free(p);
}

