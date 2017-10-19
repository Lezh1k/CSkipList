#ifndef INTERLOCKED_H
#define INTERLOCKED_H

#include <stdint.h>

uint8_t CompareAndSwapX64Ptr(volatile void **ptr,
                             void **oldVal,
                             void *newVal);


#endif // INTERLOCKED_H