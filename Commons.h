#ifndef COMMONS_H
#define COMMONS_H

#include <stdint.h>
#include <stdlib.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define UNUSED_ARG(x) ((void)x)

static inline uint8_t IsMarkedPtr(const void *ptr) {
  return ((uint64_t)ptr & 0x01);
}

static inline void* UnmarkedPtr(const void *ref) {
  return (void*)((uint64_t)ref & ~0x01);
}

static inline void MarkPtr(void *ref) {
  ref = (void*)((uint64_t)ref | 0x01);
}

static inline void* MarkedPtr(const void *ref) {
  return (void*)((uint64_t)ref | 0x01);
}

static inline int32_t GenRandomBase2(int32_t maxLevel) {
  int32_t lvl = 0;
  while (rand() & 0x1 && ++lvl < maxLevel)
    ;
  return lvl;
}

typedef enum LF_ERROR {
  LFE_SUCCESS = 0,
  LFE_ALREADY_DONE,
  LFE_FAILED
} LF_ERROR_T;


#endif // COMMONS_H
