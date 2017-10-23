#include <limits.h>
#include "Interlocked.h"

////////////////////////////////////////////////////////////////////////////

#if ( __WORDSIZE == 64)
uint8_t CASPtr(volatile void **ptr,
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
#elif ( __WORDSIZE == 32)

uint8_t CASPtr(volatile void **ptr,
               void **oldVal,
               void *newVal) {

  register uint8_t ret;
  __asm__ __volatile__("lock cmpxchgl %2, %1 \n\t"
                       "sete %0 \n\t"
                       :"=a"(ret), "+m" (*ptr)
                       :"r"(newVal), "0"(*oldVal)
                       :"memory"
                       );
  return ret;
}

#endif
