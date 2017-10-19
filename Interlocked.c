#include "Interlocked.h"


////////////////////////////////////////////////////////////////////////////

uint8_t CompareAndSwapX64Ptr(volatile void **ptr,
                             void **oldVal,
                             void *newVal) {

  register uint8_t ret;
  __asm__ __volatile__("lock cmpxchgq %2, %1 \n\t"
                       "sete %0 \n\t"
                       :"=a"(ret), "+m" (*ptr)
                       :"r"(newVal), "0"(*oldVal)
                       :"memory"
                       );
  return ret;
}

