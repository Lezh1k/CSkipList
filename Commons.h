#ifndef COMMONS_H
#define COMMONS_H

#include <stdint.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define UNUSED_ARG(x) ((void)x)

static inline uint8_t isMarkedPtr(const void *ptr) {
  return ((uint64_t)ptr & 0x01);
}

static inline void* unmarkedPtr(const void *ref) {
  return (void*)((uint64_t)ref & ~0x01);
}

static inline void markPtr(void *ref) {
  ref = (void*)((uint64_t)ref | 0x01);
}

static inline void* markedPtr(const void *ref) {
  return (void*)((uint64_t)ref | 0x01);
}


#endif // COMMONS_H
